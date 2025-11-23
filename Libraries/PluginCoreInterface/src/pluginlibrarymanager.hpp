#pragma once

#include <string>
#include <functional>

#include <filesystem>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>

namespace Graph {

class PluginLibraryManager
{
    std::string m_pluginName;
    boost::dll::shared_library m_pluginLib;
public:
    PluginLibraryManager();
    ~PluginLibraryManager();

    bool load(const std::string& pluginFilePath);
    bool isLoaded() const;
    std::string getLibraryName() const;

    template<typename Func>
    decltype(auto) getFunction(const std::string& functionName) {
        if (!m_pluginLib.is_loaded()) {
            throw std::runtime_error("PluginLibraryManager: Plugin not loaded");
        }

        if (!m_pluginLib.has(functionName)) {
            throw std::runtime_error("PluginLibraryManager: Invalid function name: " + functionName);
        }

        return std::function<Func>(m_pluginLib.get<Func>(functionName));
    }
};

} // namespace Graph

