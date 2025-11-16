#pragma once

#include <string>
#include <memory>
#include <list>

namespace Graph {

class PluginLibraryManager;

class AbstractPlugin
{
    std::shared_ptr<PluginLibraryManager> m_libraryManager;
public:
    AbstractPlugin();
    virtual ~AbstractPlugin();

    bool initPlugin(const std::string& pluginFile);
    std::string getPluginName() const;

    std::list<std::string> getItemList() const;
    std::string getItemType(const std::string& itemName) const;

protected:
    std::shared_ptr<PluginLibraryManager> getLibraryManager() const;
};

} // namespace Graph

