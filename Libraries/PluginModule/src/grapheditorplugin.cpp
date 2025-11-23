#include "grapheditorplugin.hpp"

#include "pluginlibrarymanager.hpp"
#include <boost/tokenizer.hpp>

#include <Components/Logger/Logger.h>

#include <PluginCoreInterface/Core.h>

namespace Graph {

GraphEditorPlugin::GraphEditorPlugin()
{

}

GraphEditorPlugin::~GraphEditorPlugin()
{

}

bool GraphEditorPlugin::initFromFile(const QString &pluginFile)
{
    m_libraryManager = std::make_shared<PluginLibraryManager>();
    auto isLoaded = m_libraryManager->load(pluginFile.toStdString());
    if (!isLoaded) {
        return false;
    }
    auto f_createCore = getLibraryManager()->getFunction<decltype(createPluginCore)>("createPluginCore");
    m_pPluginCore = std::shared_ptr<AbstractPluginCore>(f_createCore());
    return true;
}

QString GraphEditorPlugin::getPluginName() const
{
    if (m_libraryManager) {
        return m_libraryManager->getLibraryName().c_str();
    }
    return {};
}

std::shared_ptr<AbstractPluginCore> GraphEditorPlugin::getPluginCore() const
{
    return m_pPluginCore;
}

std::shared_ptr<PluginLibraryManager> GraphEditorPlugin::getLibraryManager() const
{
    return m_libraryManager;
}

} // namespace Graph
