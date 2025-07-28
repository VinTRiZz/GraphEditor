#include "gse_format.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QBuffer>
#include <QFileInfo>
#include <QDir>

#include <QJsonDocument>

#include <boost/algorithm/hex.hpp>

#include <GraphObject/Object.h>
#include <Common/Logging.h>

namespace Filework
{

const QString DB_GRAPH_PROPS_TABLENAME          {"graph_properties"};
const QString DB_GRAPH_PROPS_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
id INTEGER PRIMARY KEY,
prop_name TEXT UNIQUE NOT NULL,
prop_value TEXT
);
)").arg(DB_GRAPH_PROPS_TABLENAME);

const QString DB_GRAPH_VERTICES_TABLENAME       {"vertices"};
const QString DB_GRAPH_VERTICES_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
    id              INTEGER PRIMARY KEY,
    posx            FLOAT NOT NULL,
    posy            FLOAT NOT NULL,
    short_name      TEXT NOT NULL,
    name            TEXT,
    description     TEXT,
    custom_props    TEXT,
    color_rgb       TEXT, -- R-G-B in hex, example: 255 003 166 -> ff 03 a6
    bgr_color_rgb   TEXT, -- R-G-B in hex, example: 255 003 166 -> ff 03 a6
    pxmap           TEXT  -- Pixmap as PNG
);
)").arg(DB_GRAPH_VERTICES_TABLENAME);

const QString DB_GRAPH_CONNECTIONS_TABLENAME    {"connections"};
const QString DB_GRAPH_CONNECTIONS_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
    id          INTEGER PRIMARY KEY,
    idFrom      INTEGER NOT NULL,
    idTo        INTEGER NOT NULL,
    weight      FLOAT DEFAULT 0,
    name        TEXT,
    color_rgb   TEXT, -- R-G-B in hex, example: 255 003 166 -> ff 03 a6

    FOREIGN KEY (idFrom) REFERENCES %1(id) ON DELETE CASCADE,
    FOREIGN KEY (idTo) REFERENCES %1(id) ON DELETE CASCADE
);
)").arg(DB_GRAPH_CONNECTIONS_TABLENAME, DB_GRAPH_VERTICES_TABLENAME);


GSE_Format::GSE_Format()
{

}

GSE_Format::~GSE_Format()
{

}

bool GSE_Format::save(const QString &targetPath) const
{
    auto targetFileInfo = QFileInfo(targetPath);
    auto targetFileDirInfo = QFileInfo(targetFileInfo.dir().absolutePath());
    if (!targetFileDirInfo.exists() || (targetFileInfo.exists() && !targetFileInfo.isFile())) {
        LOG_ERROR("GSE_Format::save: Invalid path to object file (directory not exist or target is not a file)");
        return false;
    }

    auto& iGraphObject = getGraph();
    auto graphMaintaner = getGraphMaintaner();

    LOG_INFO("Saving data of graph", graphMaintaner->getName(), "to file", targetPath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(targetPath);

    if (!db.open()) {
        LOG_ERROR("Error opening savefile:", db.lastError().text());
        return false;
    }

    // Настройка таблиц
    QSqlQuery q(db);

    LOG_INFO("Setting up tables...");
    auto queryText = QString("DELETE FROM %0");
    if (!executeQuery(q, DB_GRAPH_PROPS_CREATEQUERY))                      { return false; }
    if (!executeQuery(q, queryText.arg(DB_GRAPH_PROPS_TABLENAME)))         { return false; }

    if (!executeQuery(q, DB_GRAPH_VERTICES_CREATEQUERY))                   { return false; }
    if (!executeQuery(q, queryText.arg(DB_GRAPH_VERTICES_TABLENAME)))      { return false; }

    if (!executeQuery(q, DB_GRAPH_CONNECTIONS_CREATEQUERY))                { return false; }
    if (!executeQuery(q, queryText.arg(DB_GRAPH_CONNECTIONS_TABLENAME)))   { return false; }

    // Загрузка информации о графе
    LOG_INFO("Inserting common data as properties...");
    queryText = QString("INSERT INTO %0%1").arg(DB_GRAPH_PROPS_TABLENAME, QString("(prop_name, prop_value) VALUES ('%1', '%2')"));

    if (!executeQuery(q, queryText.arg("name",          graphMaintaner->getName()))) { return false; }
    if (!executeQuery(q, queryText.arg("description",   graphMaintaner->getDescription()))) { return false; }
    if (!executeQuery(q, queryText.arg("create time",   graphMaintaner->getCreateTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }
    if (!executeQuery(q, queryText.arg("edit time",     graphMaintaner->getEditTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }

    LOG_INFO("Inserting user data as properties...");
    for (auto& userProp : graphMaintaner->getCustomValueMap()) {
        if (!executeQuery(q, queryText.arg(userProp.first, userProp.second.toString()))) {
            return false;
        }
    }

    // Загрузка вершин в таблицу
    LOG_INFO("Inserting vertices info...");
    auto queryTextBase = QString("INSERT INTO %0 VALUES (").arg(DB_GRAPH_VERTICES_TABLENAME);
    uint totalInsertedVertices {0};
    for (auto& vert : iGraphObject.getAllVertices()) {
        queryText = queryTextBase;

        queryText += QString::number(vert.id) + ",";
        queryText += QString::number(vert.posX) + ",";
        queryText += QString::number(vert.posY) + ",";
        queryText += "'" + vert.shortName + "',";
        queryText += "'" + vert.name + "',";
        queryText += "'" + vert.description + "',";
        queryText += "'" + QJsonDocument(vert.customProperties).toJson().toHex() + "',";
        queryText += "'" + GraphCommon::encodeColorGSE(vert.borderColor) + "',";
        queryText += "'" + GraphCommon::encodeColorGSE(vert.backgroundColor) + "',";
        queryText += "'" + getEncodedPixmap(QPixmap::fromImage(vert.image)) + "'";

        queryText += ")";
        if (!executeQuery(q, queryText)) { return false; }
        totalInsertedVertices++;
    }
    LOG_OK("Added", totalInsertedVertices, "vertices");

    LOG_INFO("Inserting connections info...");
    queryTextBase = QString("INSERT INTO %0 VALUES (").arg(DB_GRAPH_CONNECTIONS_TABLENAME);
    auto totalInsertedConnections {0};
    for (auto& con : iGraphObject.getAllConnections()) {
        queryText = queryTextBase;

        queryText += "NULL,";
        queryText += QString::number(con.idFrom) + ",";
        queryText += QString::number(con.idTo) + ",";
        queryText += QString::number(con.connectionWeight) + ",";
        queryText += "'" + con.name + "',";
        queryText += "'" + GraphCommon::encodeColorGSE(con.lineColor) + "'";

        queryText += ")";
        if (!executeQuery(q, queryText)) { return false; }
        totalInsertedConnections++;
    }
    LOG_OK("Added", totalInsertedConnections, "connections");

    db.close();
    db.removeDatabase(db.connectionName());
    LOG_OK("Graph saved");
    return true;
}

bool GSE_Format::load(const QString &targetPath)
{
    auto& oGraphObject = getGraph();
    oGraphObject = Graph::GraphObject(); // Обнулить перед записью, чтобы не было артефактов

    auto graphMaintaner = getGraphMaintaner();
    graphMaintaner->resetMaintainer();

    if (!isFileValid(targetPath)) {
        LOG_ERROR("GSE_Format::load: Invalid path to object file (file not exist, invalid or target is not a file)");
        return false;
    }

    LOG_INFO("Loading data of from file", targetPath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(targetPath);

    if (!db.open()) {
        LOG_ERROR("Error opening savefile:", db.lastError().text());
        return false;
    }

    QSqlQuery q(db);

    // Проверка, все ли таблицы на месте
    LOG_INFO("Checking required tables...");
    if (!executeQuery(q, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name ASC")) { return false; }

    bool containProps {false};
    bool containVertices {false};
    bool containConnections {false};
    while (q.next()) {
        auto tableName = q.value(0).toString();
        containProps        |= tableName == DB_GRAPH_PROPS_TABLENAME;
        containVertices     |= tableName == DB_GRAPH_VERTICES_TABLENAME;
        containConnections  |= tableName == DB_GRAPH_CONNECTIONS_TABLENAME;

        if (containProps && containVertices && containConnections) {
            break;
        }
    }

    if (!containProps || !containVertices || !containConnections) {
        LOG_ERROR("Did not found one of required tables in savefile");
        return false;
    }

    // Загрузка информации о графе
    LOG_INFO("Loading common data as properties...");
    auto queryText = QString("SELECT prop_name, prop_value FROM %0 WHERE prop_name IN ('name', 'description', 'create time', 'edit time') ORDER BY id ASC").arg(DB_GRAPH_PROPS_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }

    std::map<QString, QVariant> commonValues;
    while (q.next()) {
        commonValues[q.value(0).toString()] = q.value(1);
    }
    bool containName        = (commonValues.find("name") != commonValues.end());
    bool containDescription = (commonValues.find("description") != commonValues.end());
    bool containCreateDate  = (commonValues.find("create time") != commonValues.end());
    bool containEditDate    = (commonValues.find("edit time") != commonValues.end());
    if (!containName || !containDescription || !containCreateDate || !containEditDate) {
        LOG_ERROR("Did not found some of common values in savefile");
        return false;
    }

    graphMaintaner->setName(commonValues["name"].toString());
    graphMaintaner->setDescription(commonValues["description"].toString());
    graphMaintaner->setCreateTime(QDateTime::fromString(commonValues["create time"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));
    graphMaintaner->setEditTime(QDateTime::fromString(commonValues["edit time"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));

    LOG_INFO("Loading user data as properties...");
    queryText = QString("SELECT prop_name, prop_value FROM %0 WHERE prop_name NOT IN ('name', 'description', 'create time', 'edit time') ORDER BY prop_name ASC").arg(DB_GRAPH_PROPS_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
    auto totalLoadedData {0};
    while (q.next()) {
        graphMaintaner->setCustomValue(q.value(0).toString(), q.value(1));
        totalLoadedData++;
    }
    LOG_OK("Loaded", totalLoadedData, "custom graph properties");

    // Загрузка вершин
    LOG_INFO("Loading vertices info...");
    queryText = QString("SELECT * FROM %0").arg(DB_GRAPH_VERTICES_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
    totalLoadedData = 0;
    while (q.next()) {
        Graph::GVertex vert;
        int valPos {0};

        vert.id                 = q.value(valPos++).toLongLong();
        vert.posX               = q.value(valPos++).toLongLong();
        vert.posY               = q.value(valPos++).toLongLong();
        vert.shortName          = q.value(valPos++).toString();
        vert.name               = q.value(valPos++).toString();
        vert.description        = q.value(valPos++).toString();
        vert.customProperties   = QJsonDocument::fromJson(getDecoded(q.value(valPos++).toByteArray())).object();
        vert.borderColor        = GraphCommon::decodeColorGSE(q.value(valPos++).toByteArray());
        vert.backgroundColor    = GraphCommon::decodeColorGSE(q.value(valPos++).toByteArray());
        vert.image              = getDecodedPixmap(q.value(valPos++).toByteArray()).toImage();
        oGraphObject.addVertex(vert);
        totalLoadedData++;
    }
    LOG_OK("Loaded", totalLoadedData, "vertices");

    LOG_INFO("Inserting connections info...");
    queryText = QString("SELECT * FROM %0").arg(DB_GRAPH_CONNECTIONS_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
    totalLoadedData = 0;
    while (q.next()) {
        Graph::GConnection con;
        int valPos {0};
        valPos++; // Skip ID

        con.idFrom              = q.value(valPos++).toLongLong();
        con.idTo                = q.value(valPos++).toLongLong();
        con.connectionWeight    = q.value(valPos++).toDouble();
        con.name                = q.value(valPos++).toString();
        con.lineColor           = GraphCommon::decodeColorGSE(q.value(valPos++).toByteArray());

        oGraphObject.addConnection(con);
        totalLoadedData++;
    }
    LOG_OK("Loaded", totalLoadedData, "connections");

    db.close();
    db.removeDatabase(db.connectionName());
    LOG_OK("Graph loaded");
    return true;
}

bool GSE_Format::isFileValid(const QString &targetPath) const
{
    // Precheck
    auto targetFileInfo = QFileInfo(targetPath);
    auto targetFileDirInfo = QFileInfo(targetFileInfo.dir().absolutePath());
    if (!targetFileDirInfo.exists() || (targetFileInfo.exists() && !targetFileInfo.isFile())) {
        return false;
    }

    // Targeted check
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(targetPath);

    if (!db.open()) {
        LOG_ERROR("Error opening savefile:", db.lastError().text());
        return false;
    }
    return true;
}

bool GSE_Format::executeQuery(QSqlQuery &q, const QString &queryText) const
{
    if (!q.exec(queryText)) {
        LOG_ERROR("Error executing query with text:");
        LOG_ERROR(q.lastError().text());
        return false;
    }
    return true;
}

}
