#include "savemaster.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QBuffer>
#include <QFileInfo>
#include <QDir>

#include <QJsonDocument>

#include <boost/algorithm/hex.hpp>

#include "Graph/graphcommon.h"

#include  "logging.h"

bool SaveMaster::save(const QString &oFilePath, const Graph::GraphObject &iGraphObject)
{
    auto targetFileInfo = QFileInfo(oFilePath);
    auto targetFileDirInfo = QFileInfo(targetFileInfo.dir().absolutePath());
    if (!targetFileDirInfo.exists() || (targetFileInfo.exists() && !targetFileInfo.isFile())) {
        LOG_ERROR("SaveMaster::save: Invalid path to object file (directory not exist or target is not a file)");
        return false;
    }

    LOG_INFO("Saving data of graph", iGraphObject.getName(), "to file", oFilePath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(oFilePath);

    if (!db.open()) {
        LOG_ERROR("Error opening savefile:", db.lastError().text());
        return false;
    }

    // Настройка таблиц
    QSqlQuery q(db);

    LOG_INFO("Setting up tables...");
    auto queryText = QString("DELETE FROM %0");
    if (!executeQuery(q, GraphCommon::DB_GRAPH_PROPS_CREATEQUERY))                      { return false; }
    if (!executeQuery(q, queryText.arg(GraphCommon::DB_GRAPH_PROPS_TABLENAME)))         { return false; }

    if (!executeQuery(q, GraphCommon::DB_GRAPH_VERTICES_CREATEQUERY))                   { return false; }
    if (!executeQuery(q, queryText.arg(GraphCommon::DB_GRAPH_VERTICES_TABLENAME)))      { return false; }

    if (!executeQuery(q, GraphCommon::DB_GRAPH_CONNECTIONS_CREATEQUERY))                { return false; }
    if (!executeQuery(q, queryText.arg(GraphCommon::DB_GRAPH_CONNECTIONS_TABLENAME)))   { return false; }

    // Загрузка информации о графе
    LOG_INFO("Inserting common data as properties...");
    queryText = QString("INSERT INTO %0%1").arg(GraphCommon::DB_GRAPH_PROPS_TABLENAME, QString("(prop_name, prop_value) VALUES ('%1', '%2')"));

    if (!executeQuery(q, queryText.arg("name",          iGraphObject.getName()))) { return false; }
    if (!executeQuery(q, queryText.arg("description",   iGraphObject.getDescription()))) { return false; }
    if (!executeQuery(q, queryText.arg("create time",   iGraphObject.getCreateTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }
    if (!executeQuery(q, queryText.arg("edit time",     iGraphObject.getEditTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }

    LOG_INFO("Inserting user data as properties...");
    for (auto& userProp : iGraphObject.getCustomValueMap()) {
        if (!executeQuery(q, queryText.arg(userProp.first, userProp.second.toString()))) {
            return false;
        }
    }

    // Загрузка вершин в таблицу
    LOG_INFO("Inserting vertices info...");
    auto queryTextBase = QString("INSERT INTO %0 VALUES (").arg(GraphCommon::DB_GRAPH_VERTICES_TABLENAME);
    for (auto& vert : iGraphObject.getAllVertices()) {
        queryText = queryTextBase;

        queryText += QString::number(vert.id) + ",";
        queryText += QString::number(vert.posX) + ",";
        queryText += QString::number(vert.posY) + ",";
        queryText += "'" + vert.shortName + "',";
        queryText += "'" + vert.name + "',";
        queryText += "'" + vert.description + "',";
        queryText += "'" + QJsonDocument(vert.customProperties).toJson().toHex() + "',";
        queryText += "'" + getEncoded(vert.borderColor) + "',";
        queryText += "'" + getEncoded(vert.backgroundColor) + "',";
        queryText += "'" + getEncoded(QPixmap::fromImage(vert.image)) + "'";

        queryText += ")";
        if (!executeQuery(q, queryText)) { return false; }
    }

    LOG_INFO("Inserting connections info...");
    queryTextBase = QString("INSERT INTO %0 VALUES (").arg(GraphCommon::DB_GRAPH_CONNECTIONS_TABLENAME);
    for (auto& con : iGraphObject.getAllConnections()) {
        queryText = queryTextBase;

        queryText += "NULL,";
        queryText += QString::number(con.idFrom) + ",";
        queryText += QString::number(con.idTo) + ",";
        queryText += QString::number(con.connectionWeight) + ",";
        queryText += "'" + con.name + "',";
        queryText += "'" + getEncoded(con.lineColor) + "',";
        queryText += QString(con.isDirected ? "true" : "false");

        queryText += ")";
        if (!executeQuery(q, queryText)) { return false; }
    }

    db.close();
    db.removeDatabase(db.connectionName());
    LOG_OK("Graph saved");
    return true;
}

bool SaveMaster::load(const QString &iFilePath, Graph::GraphObject &oGraphObject)
{
    oGraphObject = Graph::GraphObject(); // Обнулить перед записью, чтобы не было артефактов

    auto targetFileInfo = QFileInfo(iFilePath);
    if (!targetFileInfo.exists() || !targetFileInfo.isFile()) {
        LOG_ERROR("SaveMaster::load: Invalid path to object file (file not exist or target is not a file)");
        return false;
    }

    LOG_INFO("Loading data of from file", iFilePath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(iFilePath);

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
        containProps        |= tableName == GraphCommon::DB_GRAPH_PROPS_TABLENAME;
        containVertices     |= tableName == GraphCommon::DB_GRAPH_VERTICES_TABLENAME;
        containConnections  |= tableName == GraphCommon::DB_GRAPH_CONNECTIONS_TABLENAME;

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
    auto queryText = QString("SELECT prop_name, prop_value FROM %0 WHERE prop_name IN ('name', 'description', 'create time', 'edit time') ORDER BY id ASC").arg(GraphCommon::DB_GRAPH_PROPS_TABLENAME);
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

    oGraphObject.setName(commonValues["name"].toString());
    oGraphObject.setDescription(commonValues["description"].toString());
    oGraphObject.setCreateTime(QDateTime::fromString(commonValues["create time"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));
    oGraphObject.setEditTime(QDateTime::fromString(commonValues["edit time"].toString(), GraphCommon::DATE_CONVERSION_FORMAT));

    LOG_INFO("Loading user data as properties...");
    queryText = QString("SELECT prop_name, prop_value FROM %0 WHERE prop_name NOT IN ('name', 'description', 'create time', 'edit time') ORDER BY prop_name ASC").arg(GraphCommon::DB_GRAPH_PROPS_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
    while (q.next()) {
        oGraphObject.setCustomValue(q.value(0).toString(), q.value(1));
    }

    // Загрузка вершин
    LOG_INFO("Loading vertices info...");
    queryText = QString("SELECT * FROM %0").arg(GraphCommon::DB_GRAPH_VERTICES_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
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
        vert.borderColor        = getDecodedColor(q.value(valPos++).toByteArray());
        vert.backgroundColor    = getDecodedColor(q.value(valPos++).toByteArray());
        vert.image              = getDecodedPixmap(q.value(valPos++).toByteArray()).toImage();
        oGraphObject.addVertex(vert);
    }

    LOG_INFO("Inserting connections info...");
    queryText = QString("SELECT * FROM %0").arg(GraphCommon::DB_GRAPH_CONNECTIONS_TABLENAME);
    if (!executeQuery(q, queryText)) { return false; }
    while (q.next()) {
        Graph::GConnection con;
        int valPos {0};
        valPos++; // Skip ID

        con.idFrom              = q.value(valPos++).toLongLong();
        con.idTo                = q.value(valPos++).toLongLong();
        con.connectionWeight    = q.value(valPos++).toDouble();
        con.name                = q.value(valPos++).toString();
        con.lineColor           = getDecodedColor(q.value(valPos++).toByteArray());
        con.isDirected          = q.value(valPos++).toBool();

        oGraphObject.addConnection(con);
    }

    db.close();
    db.removeDatabase(db.connectionName());
    LOG_OK("Graph loaded");
    return true;
}

bool SaveMaster::testValidance()
{
    LOG_INFO("Validance test of save master started");

    Graph::GraphObject savedGraph;

    savedGraph.setName("Test example graph");
    savedGraph.setDescription("Example description");
    savedGraph.setCreateTime(QDateTime::currentDateTime());
    savedGraph.setEditTime(QDateTime::currentDateTime());

    Graph::GVertex vert;
    vert.shortName = "Vertex 1";
    vert.backgroundColor = Qt::red;
    vert.posX = 100;
    vert.posY = 100;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 2";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 100;
    vert.posY = 300;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 3";
    vert.backgroundColor = Qt::red;
    vert.posX = 300;
    vert.posY = 100;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 4";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 300;
    vert.posY = 300;
    savedGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "Connection 1";
    con.idFrom = 1;
    con.idTo = 2;
    savedGraph.addConnection(con);

    con.name = "Connection 2";
    con.idFrom = 2;
    con.idTo = 3;
    savedGraph.addConnection(con);

    con.name = "Connection 3";
    con.idFrom = 1;
    con.idTo = 3;
    savedGraph.addConnection(con);

    con.name = "Connection 4";
    con.idFrom = 1;
    con.idTo = 4;
    savedGraph.addConnection(con);

    con.name = "Connection 5";
    con.idFrom = 3;
    con.idTo = 1;
    savedGraph.addConnection(con);

    auto testTargetPath = "/tmp/GraphEditorSaveTest.gse";
    if (!save(testTargetPath, savedGraph)) {
        LOG_ERROR("Validance test: save failed");
        return false;
    }

    Graph::GraphObject loadedGraph;
    if (!load(testTargetPath, loadedGraph)) {
        QFile::remove(testTargetPath);
        LOG_ERROR("Validance test: load failed");
        return false;
    }
    QFile::remove(testTargetPath);

    if (loadedGraph != savedGraph) {
        LOG_ERROR("Validance test: Loaded graph is not equal to saved graph. Some errors during save-load operation or in equality compare");
        return false;
    }

    LOG_OK("Validance test of save master complete with success");
    return true;
}

bool SaveMaster::executeQuery(QSqlQuery &q, const QString &queryText)
{
    if (!q.exec(queryText)) {
        LOG_ERROR("Error executing query with text:");
        LOG_ERROR(q.lastError().text());
        return false;
    }
    return true;
}

QByteArray SaveMaster::getEncoded(const QByteArray &iStr)
{
    return iStr.toHex();
}

QByteArray SaveMaster::getEncoded(const QColor &iCol)
{
    return iCol.name().toUtf8().data();
}

QByteArray SaveMaster::getEncoded(const QPixmap &iPxmap)
{
    QByteArray bytes;
    QBuffer byteBuff(&bytes);
    if (iPxmap.save(&byteBuff, "PNG")) {
        return bytes.toHex();
    }
    return {};
}

QByteArray SaveMaster::getDecoded(const QByteArray &iBytes)
{
    return QByteArray::fromHex(iBytes);
}

QPixmap SaveMaster::getDecodedPixmap(const QByteArray &iBytes)
{
    QPixmap pxmap;
    pxmap.loadFromData(QByteArray::fromHex(iBytes), "PNG");
    return pxmap;
}

QColor SaveMaster::getDecodedColor(const QByteArray &iBytes)
{
    return QColor(iBytes.data()); // From name
}
