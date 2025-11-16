#include "abstractplugin.hpp"

#include "pluginlibrarymanager.hpp"

namespace Graph {

AbstractPlugin::AbstractPlugin()
{

}

AbstractPlugin::~AbstractPlugin()
{

}

bool AbstractPlugin::initPlugin(const std::string &pluginFile)
{
    return m_libraryManager->load(pluginFile);
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
    auto getLibraryItemNames = getLibraryManager()->getFunction<std::list<std::string>(*)()>("getLibraryItemNames");
    return getLibraryItemNames();
}

std::string AbstractPlugin::getItemType(const std::string &itemName) const
{
    auto getItemType = getLibraryManager()->getFunction<std::string(*)(const std::string&)>("getItemType");
    return getItemType(itemName);
}

std::shared_ptr<PluginLibraryManager> AbstractPlugin::getLibraryManager() const
{
    return m_libraryManager;
}

} // namespace Graph
