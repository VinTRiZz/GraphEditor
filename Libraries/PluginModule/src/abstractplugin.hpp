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

    bool initFromFile(const std::string& pluginFile);
    std::string getPluginName() const;

    std::list<std::string> getItemList() const;

protected:
    std::shared_ptr<PluginLibraryManager> getLibraryManager() const;
};

} // namespace Graph

