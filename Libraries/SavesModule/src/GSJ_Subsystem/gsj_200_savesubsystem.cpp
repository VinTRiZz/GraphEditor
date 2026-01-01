#include "gsj_200_savesubsystem.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <Components/Logger/Logger.h>

#include <GraphObject/PluginObjectInterface.h>
#include <PluginCoreInterface/Core.h>
#include <PluginModule/PluginMaster.h>

namespace Filework {

GSJ_200_SaveSubsystem::GSJ_200_SaveSubsystem() :
    AbstractSaveSubsystem("2.0.0")
{

}

bool GSJ_200_SaveSubsystem::canProcess(const QString &fileData) const
{
    QJsonParseError err;
    auto parsedJson = QJsonDocument::fromJson(fileData.toUtf8(), &err);
    if (parsedJson.isNull()) {
        setErrorText("Parse error: " + err.errorString());
        return false;
    }
    auto iJson = parsedJson.object();

    // Properties
    if (!iJson.contains("properties")) {
        LOG_ERROR("Not found section: properties");
        return false;
    }
    if (!iJson["properties"].isObject()) {
        LOG_ERROR("Invalid properties section");
        return false;
    }

    // Common properties
    if (!iJson.contains("properties")) {
        LOG_ERROR("Not found section: properties");
        return false;
    }
    if (!iJson["properties"].isObject()) {
        LOG_ERROR("Invalid properties section");
        return false;
    }

    // Vertices
    if (!iJson.contains("objects")) {
        LOG_ERROR("Not found section: objects");
        return false;
    }
    if (!iJson["objects"].isArray()) {
        LOG_ERROR("Invalid objects section");
        return false;
    }

    LOG_OK("Structure check succeed");
    return true;
}

bool GSJ_200_SaveSubsystem::createSavedata(const Graph::GraphObjectManagerPtr &pGraph, QString &savedata) const
{
    QJsonObject root;

    // System section
    root["system"] = createSystemJson();

    // Properties
    QJsonObject propertiesObj;
    {
        auto graphMetadata = pGraph->getObject()->getMetaInfo();
        propertiesObj["name"] = graphMetadata->getName();
        propertiesObj["descr"] = graphMetadata->getDescription();
        propertiesObj["created"] = graphMetadata->getCreateTime().toString(
            Graph::DATE_CONVERSION_FORMAT);
        propertiesObj["edited"] = graphMetadata->getEditTime().toString(
            Graph::DATE_CONVERSION_FORMAT);
    }
    root["properties"] = propertiesObj;

    QJsonArray objectsJsonA;
    for (auto* pVertex : pGraph->getObject()->getAllObjects()) {
        objectsJsonA.push_back(pVertex->toJson());
    }
    for (auto* pObj : pGraph->getObject()->getPluginObjects()) {
        objectsJsonA.push_back(pObj->toJson());
    }
    root["objects"] = objectsJsonA;

    savedata = QJsonDocument(root).toJson(QJsonDocument::Compact);
    return true;
}

bool GSJ_200_SaveSubsystem::parseSavedata(const Graph::GraphObjectManagerPtr &pGraph, const QString &savedata) const
{
    QJsonParseError err;
    auto parsedJson = QJsonDocument::fromJson(savedata.toUtf8(), &err);
    if (parsedJson.isNull()) {
        setErrorText("Parse error: " + err.errorString());
        return false;
    }
    auto iJson = parsedJson.object();

    auto pGraphObj = pGraph->getObject();

    // Ресет для чистоты чтения
    pGraphObj->clearGraphData();

    // Common properties
    auto graphMetadata = pGraphObj->getMetaInfo();
    auto propertiesObj = iJson["properties"].toObject();
    graphMetadata->setName(propertiesObj["name"].toString());
    graphMetadata->setDescription(propertiesObj["descr"].toString());
    graphMetadata->setCreateTime(QDateTime::fromString(propertiesObj["created"].toString(), Graph::DATE_CONVERSION_FORMAT));
    graphMetadata->setEditTime(QDateTime::fromString(propertiesObj["edited"].toString(), Graph::DATE_CONVERSION_FORMAT));

    // Get objects
    auto& pluginMaster = Graph::PluginMaster::getInstance();
    Graph::GObjectItem* pInvalidObject {nullptr};
    QJsonArray objectsJsonA = iJson["objects"].toArray();

    for (auto objectJsonR : objectsJsonA) {
        auto vObj = objectJsonR.toObject();

        auto pluginName = vObj["pluginName"].toString();
        auto pluginObjectName = vObj["pluginObjectName"].toString();

        auto pPluginInterface = pluginMaster.getPlugin(pluginName);
        if (!pPluginInterface) {
            LOG_WARNING("Not found plugin:", pluginName);
            pInvalidObject = new Graph::GObjectItem;
            pInvalidObject->fromJson(vObj);
            pInvalidObject->setItemNotFound();
            pGraphObj->addObject(pInvalidObject);
            pInvalidObject = nullptr;
            continue;
        }
        auto pItem = pPluginInterface->getPluginCore()->createObject(pluginObjectName);
        if (!pItem) {
            LOG_WARNING("Not found plugin item:", QString("%0::%1").arg(pluginName, pluginObjectName));
            pInvalidObject = new Graph::GObjectItem;
            pInvalidObject->fromJson(vObj);
            pInvalidObject->setItemNotFound();
            pGraphObj->addObject(pInvalidObject);
            pInvalidObject = nullptr;
            continue;
        }

        if (auto pObj = dynamic_cast<Graph::GObjectItem*>(pItem); pObj != nullptr) {
            pGraphObj->addObject(pObj);
        } else {
            pGraphObj->addPluginObject(pItem);
        }
    }
    return true;
}

QJsonObject GSJ_200_SaveSubsystem::createSystemJson() const
{
    QJsonObject systemObj;
    systemObj["app_version"] = QString(GRAPH_EDITOR_VERSION);
    systemObj["format_version"] = getVersion();
    return systemObj;

}

} // namespace Filework
