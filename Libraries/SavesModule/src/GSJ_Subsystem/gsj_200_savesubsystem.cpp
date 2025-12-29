#include "gsj_200_savesubsystem.hpp"

#include <QJsonArray>

#include <Components/Logger/Logger.h>

#include <GraphObject/PluginObjectInterface.h>

namespace Filework {

GSJ_200_SaveSubsystem::GSJ_200_SaveSubsystem() :
    AbstractSaveSubsystem("1.0.0")
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
    const QJsonObject props = iJson["properties"].toObject();
    if (!props.contains("common")) {
        LOG_ERROR("Not found section: properties:common");
        return false;
    }
    if (!props["common"].isObject()) {
        LOG_ERROR("Invalid properties:common section");
        return false;
    }

    // Custom properties
    if (!props.contains("custom")) {
        LOG_ERROR("Not found section: properties:custom");
        return false;
    }
    if (!props["custom"].isObject()) {
        LOG_ERROR("Invalid properties:custom section");
        return false;
    }

    // Vertices
    if (!iJson.contains("vertices")) {
        LOG_ERROR("Not found section: vertices");
        return false;
    }
    if (!iJson["vertices"].isObject()) {
        LOG_ERROR("Invalid vertices section");
        return false;
    }

    // Connections
    if (!iJson.contains("connections")) {
        LOG_ERROR("Not found section: connections");
        return false;
    }
    if (!iJson["connections"].isObject()) {
        LOG_ERROR("Not found section: connections");
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

    // Properties section
    QJsonObject propertiesObj;

    // Common properties
    QJsonObject commonObj;
    auto graphMetadata = pGraph->getObject()->getMetaInfo();
    commonObj["name"] = graphMetadata->getName();
    commonObj["descr"] = graphMetadata->getDescription();
    commonObj["created"] = graphMetadata->getCreateTime().toString(
        Graph::DATE_CONVERSION_FORMAT);
    commonObj["edited"] = graphMetadata->getEditTime().toString(
        Graph::DATE_CONVERSION_FORMAT);
    propertiesObj["common"] = commonObj;

    // Vertices section
    QJsonArray verticesObj;
    for (const auto& vertex : pGraph->getObject()->getAllVertices()) {
        QJsonObject vObj;
        vObj["id"] = QString::number(vertex.getId().value());
        vObj["posX"] = vertex.getPos().x();
        vObj["posY"] = vertex.getPos().x();
        vObj["name"] = vertex.getName();
        vObj["commonData"] = vertex.getCommonData();
        vObj["pluginData"] = vertex.getPluginData();
        vObj["extraData"] = vertex.getExtraData();

        QJsonArray cons;
        for (auto& con : vertex.getConnections()) {
            QJsonObject conV;
            conV["targetId"] = con.first.value();
            conV["itemId"] = con.second;
            cons.push_back(conV);
        }
        vObj["connections"] = cons;
        verticesObj.push_back(vObj);
    }
    root["vertices"] = verticesObj;

    QJsonArray objectsObj;
    for (const auto& obj : pGraph->getObject()->getPluginObjects()) {
        if (!obj.objectId.has_value()) {
            continue;
        }
        QJsonObject pluginObj;
        pluginObj["id"] = obj.objectId.value();
        pluginObj["pluginName"] = obj.pluginName;
        pluginObj["pluginObjectName"] = obj.pluginObjectName;
        pluginObj["serializedData"] = obj.serializedData.toHex().data();

        objectsObj.push_back(pluginObj);
    }
    root["objects"] = objectsObj;

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

    // Properties section
    QJsonObject propertiesObj;

    // Common properties
    auto graphMetadata = pGraphObj->getMetaInfo();
    auto commonObj = iJson["common"].toObject();
    graphMetadata->setName(commonObj["name"].toString());
    graphMetadata->setDescription(commonObj["descr"].toString());
    graphMetadata->setCreateTime(QDateTime::fromString(commonObj["created"].toString(), Graph::DATE_CONVERSION_FORMAT));
    graphMetadata->setEditTime(QDateTime::fromString(commonObj["edited"].toString(), Graph::DATE_CONVERSION_FORMAT));

    // Vertices section
    for (auto vertR : iJson["vertices"].toArray()) {
        auto vert = vertR.toObject();
        Graph::GObject vObj;
        vObj.setId(vert["id"].toInt());

        vObj.setPos(QPointF(vert["posX"].toDouble(), vert["posX"].toDouble()));
        vObj.setName(vert["name"].toString());
        vObj.setPluginData(QJsonDocument::fromJson(vert["commonData"].toString().toUtf8()).object());
        vObj.setPluginData(QJsonDocument::fromJson(vert["pluginData"].toString().toUtf8()).object());
        vObj.setExtraData(QJsonDocument::fromJson(vert["extraData"].toString().toUtf8()).object());

        for (auto conR : iJson["connections"].toArray()) {
            auto con = conR.toObject();
            auto conTargetId = con["targetId"].toInt();
            auto conItemId = con["itemId"].toInt();
            vObj.addConnection({conTargetId, conItemId});
        }
        pGraphObj->addVertex(vObj);
    }

    // Objects
    QJsonObject objectsObj;
    for (auto pluginObjR : iJson["objects"].toArray()) {
        auto pluginObj = pluginObjR.toObject();

        Graph::PluginObjectInterface::ObjectMetadata obj;
        obj.objectId            = pluginObj["id"].toInt();
        obj.pluginName          = pluginObj["pluginName"].toString();
        obj.pluginObjectName    = pluginObj["pluginObjectName"].toString();
        obj.serializedData      = QByteArray::fromHex(pluginObj["serializedData"].toString().toUtf8());

        pGraphObj->addPluginObject(obj);
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
