#include "gsj_format.h"
#include <QFileInfo>

#include <QJsonDocument>

#include <Common/Logging.h>

#include <QJsonDocument>
#include <QJsonArray>
#include <QColor>

#include <QFileInfo>
#include <QDir>

namespace Filework
{

GSJ_Format::GSJ_Format()
{

}

GSJ_Format::~GSJ_Format()
{

}

bool GSJ_Format::initFromDataJson(const QJsonObject &iJson)
{
    if (!isStructureValid(iJson)) {
        return false;
    }

    auto pMaintainer = getGraphMaintaner();
    pMaintainer->resetMaintainer();

    // Parse properties
    const QJsonObject props = iJson["properties"].toObject();

    const QJsonObject common = props["common"].toObject();
    pMaintainer->setName(common["name"].toString());
    pMaintainer->setDescription(common["descr"].toString());
    pMaintainer->setCreateTime(QDateTime::fromString(common["created"].toString(), Qt::ISODate));
    pMaintainer->setEditTime(QDateTime::fromString(common["edited"].toString(), Qt::ISODate));

    const QJsonObject custom = props["custom"].toObject();
    for (const auto& key : custom.keys()) {
        pMaintainer->setCustomValue(key, custom[key].toVariant());
    }

    // Parse vertices
    const QJsonObject vertices = iJson["vertices"].toObject();
    for (const auto& key : vertices.keys()) {
        const QJsonObject vObj = vertices[key].toObject();

        Graph::GVertex vertex;
        vertex.id = vObj["id"].toString().toULongLong();
        vertex.posX = vObj["posX"].toDouble();
        vertex.posY = vObj["posY"].toDouble();
        vertex.shortName = vObj["shortName"].toString();
        vertex.name = vObj["name"].toString();
        vertex.description = vObj["description"].toString();
        vertex.borderColor = GraphCommon::decodeColor(vObj["borderColor"].toString().toUtf8());
        vertex.backgroundColor = GraphCommon::decodeColor(vObj["backgroundColor"].toString().toUtf8());

        if (vObj.contains("customProperties") && vObj["customProperties"].isObject()) {
            vertex.customProperties = vObj["customProperties"].toObject();
        }

        if (!pMaintainer->getObject().addVertex(vertex)) {
            LOG_WARNING("Failed to add vertex:", vertex.id);
        }
    }

    // Parse connections
    const QJsonObject connections = iJson["connections"].toObject();
    for (const auto& fromKey : connections.keys()) {
        const QJsonObject toObj = connections[fromKey].toObject();

        for (const auto& toKey : toObj.keys()) {
            const QJsonObject conObj = toObj[toKey].toObject();

            Graph::GConnection conn;
            conn.idFrom = conObj["idFrom"].toString().toULongLong();
            conn.idTo = conObj["idTo"].toString().toULongLong();
            conn.name = conObj["name"].toString();
            conn.lineColor = GraphCommon::decodeColor(conObj["color"].toString().toUtf8());
            conn.connectionWeight = conObj["weight"].toDouble();

            if (conObj.contains("customProperties") && conObj["customProperties"].isObject()) {
                conn.customProperties = conObj["customProperties"].toObject();
            }

            if (!pMaintainer->getObject().addConnection(conn)) {
                LOG_WARNING("Failed to add connection:", conn.idFrom, conn.idTo);
            }
        }
    }
    return true;
}

QJsonObject GSJ_Format::toDataJson() const
{
    auto pMaintainer = getGraphMaintaner();
    QJsonObject root;

    // System section
    root["system"] = createSystemJson("1.0.0", false);

    // Properties section
    QJsonObject propertiesObj;

    // Common properties
    QJsonObject commonObj;
    commonObj["name"] = pMaintainer->getName();
    commonObj["descr"] = pMaintainer->getDescription();
    commonObj["created"] = pMaintainer->getCreateTime().toString(Qt::ISODate);
    commonObj["edited"] = pMaintainer->getEditTime().toString(Qt::ISODate);
    propertiesObj["common"] = commonObj;

    // Custom properties
    QJsonObject customObj;
    const auto customMap = pMaintainer->getCustomValueMap();
    for (const auto& [key, value] : customMap) {
        customObj[key] = QJsonValue::fromVariant(value);
    }
    propertiesObj["custom"] = customObj;
    root["properties"] = propertiesObj;

    // Vertices section
    QJsonObject verticesObj;
    const auto vertices = pMaintainer->getObject().getAllVertices();
    for (const auto& vertex : vertices) {
        QJsonObject vObj;
        vObj["id"] = QString::number(vertex.id);
        vObj["posX"] = vertex.posX;
        vObj["posY"] = vertex.posY;
        vObj["shortName"] = vertex.shortName;
        vObj["name"] = vertex.name;
        vObj["description"] = vertex.description;
        vObj["borderColor"] = GraphCommon::encodeColor(vertex.borderColor).data();
        vObj["backgroundColor"] = GraphCommon::encodeColor(vertex.backgroundColor).data();
        vObj["customProperties"] = vertex.customProperties;

        verticesObj[QString::number(vertex.id)] = vObj;
    }
    root["vertices"] = verticesObj;

    // Connections section
    QJsonObject connectionsObj;
    const auto connections = pMaintainer->getObject().getAllConnections();

    // Группировка соединений по исходным вершинам
    std::map<GraphCommon::graphId_t, QJsonObject> connectionsMap;
    for (const auto& conn : connections) {
        QJsonObject conObj;
        conObj["idFrom"] = QString::number(conn.idFrom);
        conObj["idTo"] = QString::number(conn.idTo);
        conObj["name"] = conn.name;
        conObj["color"] = GraphCommon::encodeColor(conn.lineColor).data();
        conObj["weight"] = conn.connectionWeight;
        conObj["customProperties"] = conn.customProperties;

        const QString fromKey = QString::number(conn.idFrom);
        const QString toKey = QString::number(conn.idTo);

        if (!connectionsObj.contains(fromKey)) {
            connectionsObj[fromKey] = QJsonObject();
        }

        QJsonObject fromObj = connectionsObj[fromKey].toObject();
        fromObj[toKey] = conObj;
        connectionsObj[fromKey] = fromObj;
    }
    root["connections"] = connectionsObj;

    // Empty sections for future use
    root["interaction"] = QJsonObject();
    root["static"] = QJsonObject();
    root["etc"] = QJsonObject();

    return root;
}

bool GSJ_Format::save(const QString &targetPath) const
{
    auto targetDir = QFileInfo(targetPath).dir();
    if (!targetDir.exists() || !targetDir.isReadable()) {
        return false;
    }

    auto resultData = QJsonDocument(toDataJson()).toJson();
    return rewriteFileData(targetPath, resultData);
}

bool GSJ_Format::load(const QString &targetPath)
{
    QByteArray tmpData;
    if (!readFromFile(targetPath, tmpData)) {
        return false;
    }
    return initFromDataJson(QJsonDocument::fromJson(tmpData).object());
}

bool GSJ_Format::isFileValid(const QString &targetPath) const
{
    auto fileInfo = QFileInfo(targetPath);
    if (!fileInfo.exists()) {
        return false;
    }

    QFile iFile(targetPath);
    auto testJson = QJsonDocument::fromJson(iFile.readAll());

    if (testJson.isNull()) {
        return false;
    }

    auto jsonObj = testJson.object();
    return isStructureValid(jsonObj);
}

bool GSJ_Format::isStructureValid(const QJsonObject &iJson) const
{
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

    return true;
}

QJsonObject GSJ_Format::createSystemJson(const QString& formatVersion, bool isEncrypted) const
{
    QJsonObject systemObj;
    systemObj["app_version"] = QString(GRAPH_EDITOR_VERSION);
    systemObj["format_version"] = formatVersion;
    systemObj["is_encrypted"] = isEncrypted;
    return systemObj;
}

bool GSJ_Format::config_getIsEncrypted(const QJsonObject &iJson)
{
    return iJson["is_encrypted"].toBool();
}

QString GSJ_Format::config_getVersion(const QJsonObject &iJson)
{
    return iJson["app_version"].toString();
}

QString GSJ_Format::config_getFormatVersion(const QJsonObject &iJson)
{
    return iJson["format_version"].toString();
}

}
