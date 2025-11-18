#include "abstractplugin.hpp"

#include "pluginlibrarymanager.hpp"
#include <boost/tokenizer.hpp>

#include <Components/Logger/Logger.h>

#include <PluginCoreInterface/Core.h>
#include <PluginCoreInterface/ObjectCreating.h>
#include <PluginCoreInterface/WidgetCreating.h>
#include <PluginCoreInterface/TypeProcessing.h>

namespace Graph {

AbstractPlugin::AbstractPlugin()
{

}

AbstractPlugin::~AbstractPlugin()
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

bool AbstractPlugin::initFromFile(const std::string &pluginFile)
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

std::string AbstractPlugin::getPluginName() const
{
    if (m_libraryManager) {
        return m_libraryManager->getLibraryName();
    }
    return {};
}

std::list<std::string> AbstractPlugin::getItemList() const
{
    auto f_getTypeCount = getLibraryManager()->getFunction<decltype(getTypeCount)>("getTypeCount");

    auto f_getName = getLibraryManager()->getFunction<decltype(getItemName)>("getItemName");
    auto f_getTypeString = getLibraryManager()->getFunction<decltype(getTypeString)>("getTypeString");

    std::list<std::string> res;
    for (long long i = 0; i < f_getTypeCount(); ++i) {
        auto itemName = std::shared_ptr<std::string>(f_getName(i));
        auto typeString = std::shared_ptr<std::string>(f_getTypeString(i));
        res.push_back(std::string("[ ") + *typeString + "] " + *itemName);
    }
    return res;
}

std::shared_ptr<PluginLibraryManager> AbstractPlugin::getLibraryManager() const
{
    return m_libraryManager;
}

} // namespace Graph
