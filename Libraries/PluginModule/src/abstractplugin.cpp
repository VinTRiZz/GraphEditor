#include "abstractplugin.hpp"

#include "pluginlibrarymanager.hpp"
#include <boost/tokenizer.hpp>

#include <Components/Logger/Logger.h>

namespace Graph {

AbstractPlugin::AbstractPlugin()
{

}

AbstractPlugin::~AbstractPlugin()
{

}

bool AbstractPlugin::initPlugin(const std::string &pluginFile)
{
    m_libraryManager = std::make_shared<PluginLibraryManager>();
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
    auto getLibraryItemNames = getLibraryManager()->getFunction<const char*()>("getLibraryItemNames");
    auto libItems = getLibraryItemNames();

    std::list<std::string> items;
    boost::char_separator<char> chrSep(";");
    for (auto& tk : boost::tokenizer(std::string(libItems), chrSep)) {
        items.push_back(tk);
    }
    return items;
}

std::string AbstractPlugin::getItemType(const std::string &itemName) const
{
    auto getItemType = getLibraryManager()->getFunction<const char*(const std::string&)>("getItemType");
    return getItemType(itemName);
}

std::shared_ptr<PluginLibraryManager> AbstractPlugin::getLibraryManager() const
{
    return m_libraryManager;
}

} // namespace Graph
