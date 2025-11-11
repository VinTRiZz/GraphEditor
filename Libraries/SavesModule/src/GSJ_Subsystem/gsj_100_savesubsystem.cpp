#include "gsj_100_savesubsystem.hpp"

namespace Filework {

GSJ_100_SaveSubsystem::GSJ_100_SaveSubsystem() :
    AbstractSaveSubsystem("1.0.0")
{

}

bool GSJ_100_SaveSubsystem::canProcess(const QString &fileData) const
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

bool GSJ_100_SaveSubsystem::createSavedata(const Graph::PMaintainer &pGraph, QString &savedata) const
{
    QJsonObject root;

    // System section
    root["system"] = createSystemJson();

    // Properties section
    QJsonObject propertiesObj;

    // Common properties
    QJsonObject commonObj;
    commonObj["name"] = pGraph->getName();
    commonObj["descr"] = pGraph->getDescription();
    commonObj["created"] = pGraph->getCreateTime().toString(
        GraphCommon::DATE_CONVERSION_FORMAT);
    commonObj["edited"] = pGraph->getEditTime().toString(
        GraphCommon::DATE_CONVERSION_FORMAT);
    propertiesObj["common"] = commonObj;

    // Custom properties
    QJsonObject customObj;
    const auto customMap = pGraph->getCustomValueMap();
    for (const auto& [key, value] : customMap) {
        customObj[key] = QJsonValue::fromVariant(value);
    }
    propertiesObj["custom"] = customObj;
    root["properties"] = propertiesObj;

    // Vertices section
    QJsonObject verticesObj;
    const auto vertices = pGraph->getObject().getAllVertices();
    for (const auto& vertex : vertices) {
        QJsonObject vObj;
        vObj["id"] = QString::number(vertex.id);
        vObj["posX"] = vertex.posX;
        vObj["posY"] = vertex.posY;
        vObj["shortName"] = vertex.displayName;
        vObj["name"] = vertex.name;
        vObj["description"] = vertex.description;
        vObj["borderColor"] =
            CommonFunctions::encodeColor(vertex.lineColor).data();
        vObj["backgroundColor"] =
            CommonFunctions::encodeColor(vertex.backgroundColor).data();
        vObj["image"] = vertex.vertexExtraData;

        verticesObj[QString::number(vertex.id)] = vObj;
    }
    root["vertices"] = verticesObj;

    // Connections section
    QJsonObject connectionsObj;
    const auto connections = pGraph->getObject().getAllConnections();

    // Группировка соединений по исходным вершинам
    std::map<GraphCommon::graphId_t, QJsonObject> connectionsMap;
    for (const auto& conn : connections) {
        QJsonObject conObj;
        conObj["idFrom"] = QString::number(conn.idFrom);
        conObj["idTo"] = QString::number(conn.idTo);
        conObj["name"] = conn.name;
        conObj["color"] = CommonFunctions::encodeColor(conn.color).data();

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

    savedata = QJsonDocument(root).toJson(QJsonDocument::Compact);
    return true;
}

bool GSJ_100_SaveSubsystem::parseSavedata(const Graph::PMaintainer &pGraph, const QString &savedata) const
{
    QJsonParseError err;
    auto parsedJson = QJsonDocument::fromJson(savedata.toUtf8(), &err);
    if (parsedJson.isNull()) {
        setErrorText("Parse error: " + err.errorString());
        return false;
    }
    auto iJson = parsedJson.object();

    // Ресет для чистоты чтения
    pGraph->resetMaintainer();

    // Parse properties
    const QJsonObject props = iJson["properties"].toObject();

    const QJsonObject common = props["common"].toObject();
    pGraph->setToolTip(common["name"].toString());
    pGraph->setDescription(common["descr"].toString());
    pGraph->setCreateTime(QDateTime::fromString(
        common["created"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));
    pGraph->setEditTime(QDateTime::fromString(
        common["edited"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));

    const QJsonObject custom = props["custom"].toObject();
    for (const auto& key : custom.keys()) {
        pGraph->setCustomValue(key, custom[key].toVariant());
    }

    // Parse vertices
    const QJsonObject vertices = iJson["vertices"].toObject();
    for (const auto& key : vertices.keys()) {
        const QJsonObject vObj = vertices[key].toObject();

        Graph::GVertex vertex;
        vertex.id = vObj["id"].toString().toULongLong();
        vertex.posX = vObj["posX"].toDouble();
        vertex.posY = vObj["posY"].toDouble();
        vertex.displayName = vObj["shortName"].toString();
        vertex.name = vObj["name"].toString();
        vertex.description = vObj["description"].toString();
        vertex.lineColor = CommonFunctions::decodeColor(
            vObj["borderColor"].toString().toUtf8());
        vertex.backgroundColor = CommonFunctions::decodeColor(
            vObj["backgroundColor"].toString().toUtf8());
        vertex.vertexExtraData = vObj["vertexExtraData"].toObject();

        if (!pGraph->getObject().addVertex(vertex)) {
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
            conn.color = CommonFunctions::decodeColor(
                conObj["color"].toString().toUtf8());

            if (!pGraph->getObject().addConnection(conn)) {
                LOG_WARNING("Failed to add connection:", conn.idFrom,
                            conn.idTo);
            }
        }
    }
    return true;
}

QJsonObject GSJ_100_SaveSubsystem::createSystemJson() const
{
    QJsonObject systemObj;
    systemObj["app_version"] = QString(GRAPH_EDITOR_VERSION);
    systemObj["format_version"] = getVersion();
    return systemObj;

}

} // namespace Filework
