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
    LOG_INFO("Searching for plugins (dir:", pluginRoot, ")");
    for (auto& dirEntr : std::filesystem::directory_iterator(pluginRoot)) {
        auto filePath = std::filesystem::path(dirEntr);

        // Анализ расширения
        auto fileExt = filePath.extension().string();
        if (fileExt.empty()) {
            LOG_WARNING("Unknown plugin:", filePath.filename().string());
            continue;
        }
        fileExt.erase(0, 1); // Убираем точку
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](const auto& c){ return std::tolower(c); });

        // Vertices
        if (fileExt == "vpl") {
            m_plugins.push_back(std::make_shared<VertexPlugin>());
            if (!m_plugins.back()->initPlugin(filePath.generic_string())) {
                m_plugins.pop_back();
                continue;
            }
            LOG_OK("Loaded vertex plugin:", m_plugins.back()->getPluginName());
            continue;
        }

        // Connections
        if (fileExt == "cpl") {
            LOG_INFO("Loading connection plugin:", filePath.generic_string());
            m_plugins.push_back(std::make_shared<ConnectionPlugin>());
            if (!m_plugins.back()->initPlugin(filePath.generic_string())) {
                m_plugins.pop_back();
                continue;
            }
            LOG_OK("Loaded connection plugin:", m_plugins.back()->getPluginName());
            continue;
        }

        LOG_WARNING("Unknown plugin extension:", fileExt);
    }
    LOG_INFO("Loaded", m_plugins.size());
}

std::list<std::string> PluginMaster::getPluginNames() const
{
    std::list<std::string> res;
    for (auto& plugin : m_plugins) {
        res.push_back(plugin->getPluginName());
    }
    return res;
}

std::shared_ptr<AbstractPlugin> PluginMaster::getPlugin(const std::string &pluginName) const
{
    for (auto& plugin : m_plugins) {
        if (pluginName == plugin->getPluginName()) {
            return plugin;
        }
    }
    return {};
}

std::list<std::shared_ptr<ConnectionPlugin> > PluginMaster::getConnecionPlugins() const
{
    std::list<std::shared_ptr<ConnectionPlugin> > res;
    for (auto& plugin : m_plugins) {
        auto pVertPlugin = std::dynamic_pointer_cast<ConnectionPlugin>(plugin);
        if (pVertPlugin) {
            res.push_back(pVertPlugin);
        }
    }
    return res;
}

std::list<std::shared_ptr<VertexPlugin> > PluginMaster::getVertexPlugins() const
{
    std::list<std::shared_ptr<VertexPlugin> > res;
    for (auto& plugin : m_plugins) {
        auto pVertPlugin = std::dynamic_pointer_cast<VertexPlugin>(plugin);
        if (pVertPlugin) {
            res.push_back(pVertPlugin);
        }
    }
    return res;
}

}
