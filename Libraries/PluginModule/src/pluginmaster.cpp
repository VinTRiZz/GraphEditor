#include "pluginmaster.hpp"

#include <Components/Logger/Logger.h>

#include <filesystem>

namespace Graph {

PluginMaster::PluginMaster()
{

}

PluginMaster::~PluginMaster()
{

}

PluginMaster &PluginMaster::getInstance()
{
    static PluginMaster inst;
    return inst;
}

void PluginMaster::init(const std::string &pluginRoot)
{
    m_plugins.clear();
    LOG_INFO("Searching for plugins (dir:", pluginRoot, ")");
    for (auto& dirEntr : std::filesystem::directory_iterator(pluginRoot)) {
        auto filePath = std::filesystem::path(dirEntr);
        addPlugin(filePath.generic_string());
    }
    emit pluginListChanged();
    LOG_INFO("Loaded", m_plugins.size());
}

bool PluginMaster::addPlugin(const std::string &pluginFile)
{
    if (!std::filesystem::exists(pluginFile) || !std::filesystem::is_regular_file(pluginFile)) {
        LOG_ERROR("Plugin file not exists or not a file:", pluginFile);
        return false;
    }

    auto filePath = std::filesystem::path(pluginFile);

    // Анализ расширения
    auto fileExt = filePath.extension().string();
    if (fileExt.empty()) {
        LOG_WARNING("Unknown plugin:", filePath.filename().string());
        return false;
    }
    fileExt.erase(0, 1); // Убираем точку
    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](const auto& c){ return std::tolower(c); });

    // Vertices
    if (fileExt == "vpl") {
        m_plugins.push_back(std::make_shared<GraphEditorPlugin>());
        if (!m_plugins.back()->initFromFile(filePath.generic_string())) {
            m_plugins.pop_back();
            return false;
        }
        LOG_OK("Loaded vertex plugin:", m_plugins.back()->getPluginName());
        emit pluginInited(m_plugins.back()->getPluginName().c_str());
        return true;
    }

    LOG_WARNING("Unknown plugin extension:", fileExt);
    return false;
}

std::list<std::string> PluginMaster::getPluginNames() const
{
    std::list<std::string> res;
    for (auto& plugin : m_plugins) {
        res.push_back(plugin->getPluginName());
    }
    return res;
}

std::shared_ptr<GraphEditorPlugin> PluginMaster::getPlugin(const std::string &pluginName) const
{
    for (auto& plugin : m_plugins) {
        if (pluginName == plugin->getPluginName()) {
            return plugin;
        }
    }
    return {};
}

std::list<std::shared_ptr<GraphEditorPlugin> > PluginMaster::getAllPlugins() const
{
    return m_plugins;
}

}
