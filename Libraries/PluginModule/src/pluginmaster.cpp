#include "pluginmaster.hpp"

#include <Components/Logger/Logger.h>

#include <GraphObject/GraphObject.h>
#include <PluginCoreInterface/Core.h>

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
    LOG_INFO("Total loaded plugins:", m_plugins.size());
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

    if (fileExt == "gepl") {
        m_plugins.push_back(std::make_shared<GraphEditorPlugin>());
        if (!m_plugins.back()->initFromFile(filePath.generic_string().c_str())) {
            m_plugins.pop_back();
            return false;
        }
        LOG_OK("Loaded plugin:", m_plugins.back()->getPluginName());
        emit pluginInited(m_plugins.back()->getPluginName());
        return true;
    }

    LOG_WARNING("Unknown plugin extension:", fileExt);
    return false;
}

std::list<QString> PluginMaster::getPluginNames() const
{
    std::list<QString> res;
    for (auto& plugin : m_plugins) {
        res.push_back(plugin->getPluginName());
    }
    return res;
}

std::shared_ptr<GraphEditorPlugin> PluginMaster::getPlugin(const QString &pluginName) const
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

PluginObjectInterface *PluginMaster::createObject(const QJsonObject &objectData)
{
    auto mainObjectInfo = objectData["PluginObjectInterface"].toObject();
    auto pluginName = mainObjectInfo["pluginName"].toString();
    auto pluginObjectName = mainObjectInfo["pluginObjectName"].toString();
    auto pPluginInterface = getPlugin(pluginName);

    Graph::GInvalidObjectItem* pInvalidObject {nullptr};
    if (!pPluginInterface) {
        LOG_WARNING("Not found plugin:", pluginName);
        pInvalidObject = new Graph::GInvalidObjectItem(pluginName, pluginObjectName);
        pInvalidObject->fromJson(objectData);
        return pInvalidObject;
    }
    auto pItem = pPluginInterface->getPluginCore()->createObject(pluginObjectName);
    if (!pItem) {
        LOG_WARNING("Not found plugin item:", QString("%0::%1").arg(pluginName, pluginObjectName));
        pInvalidObject = new Graph::GInvalidObjectItem(pluginName, pluginObjectName);
        pInvalidObject->fromJson(objectData);
        return pInvalidObject;
    }

    if (!pItem->fromJson(objectData)) {
        LOG_WARNING("Failed to parse object:", QString("%0::%1").arg(pluginName, pluginObjectName));
    }

    return pItem;
}

}
