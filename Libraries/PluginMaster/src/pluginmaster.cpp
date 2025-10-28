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
        LOG_INFO("Checking file:", filePath.generic_string());

        // Анализ расширения
        auto fileExt = filePath.extension().string();
        if (fileExt.empty()) {
            LOG_WARNING("Unknown plugin:", filePath.filename().string());
            continue;
        }
        fileExt.erase(0, 1); // Убираем точку
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](const auto& c){ return std::tolower(c); });

        // Грузим плагин
        if (fileExt == VertexPlugin::getPluginExtension()) {
            LOG_INFO("Is vertex plugin");
            m_vertexPlugins.push_back(VertexPlugin(filePath.generic_string()));
        } else {
            LOG_WARNING("Unknown plugin extension:", fileExt);
        }
    }
    LOG_INFO("Loaded", m_vertexPlugins.size()); // TODO: Абстрактные плагины?
}

}
