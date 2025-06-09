#include "savemaster.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QFileInfo>
#include <QDir>

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

    db.close();
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

    db.close();
    return true;
}
