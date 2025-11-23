#include "grapheditorplugin.hpp"

#include "pluginlibrarymanager.hpp"
#include <boost/tokenizer.hpp>

#include <Components/Logger/Logger.h>

#include <PluginCoreInterface/Core.h>
#include <PluginCoreInterface/ObjectCreating.h>
#include <PluginCoreInterface/WidgetCreating.h>
#include <PluginCoreInterface/TypeProcessing.h>

namespace Graph {

GraphEditorPlugin::GraphEditorPlugin()
{

}

GraphEditorPlugin::~GraphEditorPlugin()
{
    if (m_libraryManager->isLoaded()) {
        auto f_getErrorText = getLibraryManager()->getFunction<decltype(getErrorText)>("getErrorText");

        auto f_deinitPlugin = getLibraryManager()->getFunction<decltype(deinitPlugin)>("deinitPlugin");
        auto deinitRes = f_deinitPlugin();
        if (deinitRes != 0) {
            LOG_ERROR("Plugin deinit error:", f_getErrorText());
        }
    }
}

bool GraphEditorPlugin::initFromFile(const std::string &pluginFile)
{
    m_libraryManager = std::make_shared<PluginLibraryManager>();
    auto isLoaded = m_libraryManager->load(pluginFile);
    if (!isLoaded) {
        return false;
    }
    auto f_getErrorText = getLibraryManager()->getFunction<decltype(getErrorText)>("getErrorText");

    auto f_initPlugin = getLibraryManager()->getFunction<decltype(initPlugin)>("initPlugin");
    auto initRes = f_initPlugin();
    if (initRes != 0) {
        LOG_ERROR("Plugin init error:", f_getErrorText());
        return false;
    }

    auto f_restoreState = getLibraryManager()->getFunction<decltype(restoreState)>("restoreState");
    auto restoreRes = f_restoreState();
    if (restoreRes != 0) {
        LOG_ERROR("Plugin state restore error:", f_getErrorText());
        return false;
    }

    return true;
}

std::string GraphEditorPlugin::getPluginName() const
{
    if (m_libraryManager) {
        return m_libraryManager->getLibraryName();
    }
    return {};
}

std::list<std::string> GraphEditorPlugin::getObjectNameList() const
{
    auto f_getTypeCount = getLibraryManager()->getFunction<decltype(getTypeCount)>("getTypeCount");
    auto f_getName = getLibraryManager()->getFunction<decltype(getItemName)>("getItemName");

    std::list<std::string> res;
    for (long long i = 0; i < f_getTypeCount(); ++i) {
        auto itemName = std::shared_ptr<std::string>(f_getName(i));
        res.push_back(*itemName);
    }
    return res;
}

std::string GraphEditorPlugin::getObjectType(const std::string &itemName) const
{
    // TODO: Кэшировать
    auto f_getTypeCount = getLibraryManager()->getFunction<decltype(getTypeCount)>("getTypeCount");
    auto f_getName = getLibraryManager()->getFunction<decltype(getItemName)>("getItemName");
    auto f_getTypeString = getLibraryManager()->getFunction<decltype(getTypeString)>("getTypeString");

    for (long long i = 0; i < f_getTypeCount(); ++i) {
        auto pItemName = std::shared_ptr<std::string>(f_getName(i));
        if (*pItemName == itemName) {
            return *std::shared_ptr<std::string>(f_getTypeString(i));
        }
    }
    return {};
}

PluginObjectItnterface *GraphEditorPlugin::createObject(const std::string &name)
{
    auto f_createObject = getLibraryManager()->getFunction<decltype(::createObject)>("createObject");
    return f_createObject(name);
}

PluginConfigurationWidget *GraphEditorPlugin::getConfigurationEditor()
{
    auto f_createConfigEditor = getLibraryManager()->getFunction<decltype(::getConfigurationEditor)>("getConfigurationEditor");
    return f_createConfigEditor();
}

PluginItemPropertyWidget *GraphEditorPlugin::getPropertyEditor(PluginObjectItnterface *pTarget)
{
    auto f_createPropEditor = getLibraryManager()->getFunction<decltype(::getPropertyEditor)>("getPropertyEditor");
    return f_createPropEditor(pTarget);
}

PluginInteractionWidget *GraphEditorPlugin::getInteractor()
{
    auto f_getInteractor = getLibraryManager()->getFunction<decltype(::getInteractor)>("getInteractor");
    return f_getInteractor();
}

std::shared_ptr<PluginLibraryManager> GraphEditorPlugin::getLibraryManager() const
{
    return m_libraryManager;
}

} // namespace Graph
