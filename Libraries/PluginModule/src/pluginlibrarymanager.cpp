#include "pluginlibrarymanager.hpp"

#include <Components/Logger/Logger.h>

namespace Graph {

PluginLibraryManager::PluginLibraryManager()
{

}

PluginLibraryManager::~PluginLibraryManager()
{
    if (m_pluginLib.is_loaded()) {
        m_pluginLib.unload();
    }
}

bool PluginLibraryManager::load(const std::string &pluginFilePath)
{
    try {
        LOG_INFO("Loading plugin file:", pluginFilePath);
        if (!std::filesystem::exists(pluginFilePath)) {
            throw std::runtime_error("vertex plugin file not exist: " + pluginFilePath);
        }

        auto pluginName = std::filesystem::path(pluginFilePath).filename().string();
        pluginName.erase(pluginName.find_last_of("."));
        m_pluginName = pluginName;

        m_pluginLib = boost::dll::shared_library(
                    pluginFilePath,
                    boost::dll::load_mode::append_decorations
                    );
        LOG_OK("Plugin file loaded:", m_pluginName);

    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load plugin file:", e.what());
        return false;
    }
    return true;
}

bool PluginLibraryManager::isLoaded() const
{
    return m_pluginLib.is_loaded();
}

std::string PluginLibraryManager::getLibraryName() const
{
    return m_pluginName;
}

} // namespace Graph
