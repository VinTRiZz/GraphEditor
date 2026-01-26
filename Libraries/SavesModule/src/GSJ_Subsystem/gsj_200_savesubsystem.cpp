#include "gsj_200_savesubsystem.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <Components/Logger/Logger.h>
#include <Components/Common/ApplicationSettings.h>

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

    if (iJson["system"].toObject()["format_version"] != getVersion()) {
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

    auto& settings = Common::ApplicationSettings::getInstance();
    auto savemodeSetting = settings.getSetting(Graph::SettingsNames::DEBUGCONFIG, Graph::SettingsNames::DEBUGCONFIG_JSON_SAVE_MODE);
    if (savemodeSetting->getValue().isNull()) {
        savemodeSetting->setValue(QJsonDocument::Compact);
    }
    savedata = QJsonDocument(root).toJson(QJsonDocument::JsonFormat(savemodeSetting->getValue().toInt()));
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
    auto fileSavepath = pGraphObj->getMetaInfo()->getSavepath(); // Нюанс работы
    pGraphObj->clearGraphData();
    pGraphObj->getMetaInfo()->setSavepath(fileSavepath);

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

        auto pItem = pluginMaster.createObject(vObj);
        if (!pItem) {
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
