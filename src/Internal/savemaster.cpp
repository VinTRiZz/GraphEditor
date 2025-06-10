#include "savemaster.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QBuffer>
#include <QFileInfo>
#include <QDir>

#include <QJsonDocument>

#include <boost/algorithm/hex.hpp>

#include "graphcommon.h"

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
    queryText = QString("INSERT INTO graph_properties(prop_name, prop_value) VALUES ('%1', '%2')");

    if (!executeQuery(q, queryText.arg("name",          iGraphObject.getName()))) { return false; }
    if (!executeQuery(q, queryText.arg("description",   iGraphObject.getDescription()))) { return false; }
    if (!executeQuery(q, queryText.arg("create date",   iGraphObject.getCreateTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }
    if (!executeQuery(q, queryText.arg("edit time",     iGraphObject.getEditTime().toString(GraphCommon::DATE_CONVERSION_FORMAT)))) { return false; }

    LOG_INFO("Inserting user data as properties...");
    for (auto& userProp : iGraphObject.getCustomValueMap()) {
        if (!executeQuery(q, queryText.arg(userProp.first, userProp.second.toString()))) {
            return false;
        }
    }

    // Загрузка вершин в таблицу
    LOG_INFO("Inserting vertices info...");
    auto queryTextBase = QString("INSERT INTO vertices VALUES (");
    for (auto& vert : iGraphObject.getAllVertices()) {
        queryText = queryTextBase;

        queryText += QString::number(vert.id) + ",";
        queryText += QString::number(vert.posX) + ",";
        queryText += QString::number(vert.posY) + ",";
        queryText += "'" + vert.shortName + "',";
        queryText += "'" + vert.name + "',";
        queryText += "'" + vert.description + "',";
        queryText += "'" + QJsonDocument(vert.customProperties).toJson().toBase64() + "',";
        queryText += "'" + getEncoded(vert.borderColor) + "',";
        queryText += "'" + getEncoded(vert.backgroundColor) + "',";
        queryText += "'" + getEncoded(vert.pxmap) + "'";

        queryText += ")";
        if (!executeQuery(q, queryText)) { return false; }
    }

    LOG_INFO("Inserting connections info...");
    queryTextBase = QString("INSERT INTO connections VALUES (");
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
    LOG_OK("Graph saved");
    return true;
}

bool SaveMaster::load(const QString &iFilePath, Graph::GraphObject &oGraphObject)
{
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
        containProps |= tableName == GraphCommon::DB_GRAPH_PROPS_TABLENAME;
        containVertices |= tableName == GraphCommon::DB_GRAPH_VERTICES_TABLENAME;
        containConnections |= tableName == GraphCommon::DB_GRAPH_CONNECTIONS_TABLENAME;

        if (containProps && containVertices && containConnections) {
            break;
        }
    }

    if (!containProps || !containVertices || !containConnections) {
        LOG_ERROR("Did not found one of required tables in savefile");
        return false;
    }


    db.close();
    LOG_OK("Graph loaded");
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
    return iStr.toBase64();
}

QByteArray SaveMaster::getEncoded(const QColor &iCol)
{
    auto encodedString = QString("%1%2%3%4");

    auto toHex = [](int val) -> QString {
        std::string tmpStr;
        GraphCommon::convertEncodeChar(val, 16, tmpStr);
        auto res = QString::fromStdString(tmpStr);
        if (res.isEmpty()) {
            return "00";
        }
        return res;
    };

    return encodedString.arg(
        toHex(iCol.red()),
        toHex(iCol.green()),
        toHex(iCol.blue()),
        toHex(iCol.alpha())
    ).toUtf8();
}

QByteArray SaveMaster::getEncoded(const QPixmap &iPxmap)
{
    QByteArray bytes;
    QBuffer byteBuff(&bytes);
    if (iPxmap.save(&byteBuff, "PNG")) {
        return bytes.toBase64();
    }
    return {};
}

QByteArray SaveMaster::getDecoded(const QByteArray &iBytes)
{
    return QByteArray::fromBase64(iBytes);
}

QPixmap SaveMaster::getDecodedPixmap(const QByteArray &iBytes)
{
    QPixmap pxmap;
    pxmap.loadFromData(QByteArray::fromBase64(iBytes), "PNG");
    return pxmap;
}

QColor SaveMaster::getDecodedColor(const QByteArray &iBytes)
{
    auto redComponent = std::stoi(std::string({iBytes.at(0), iBytes.at(1)}), 0, 16);
    auto greenComponent = std::stoi(std::string({iBytes.at(2), iBytes.at(3)}), 0, 16);
    auto blueComponent = std::stoi(std::string({iBytes.at(4), iBytes.at(5)}), 0, 16);
    auto alphaComponent = std::stoi(std::string({iBytes.at(6), iBytes.at(7)}), 0, 16);
    return QColor(redComponent, greenComponent, blueComponent, alphaComponent);
}
