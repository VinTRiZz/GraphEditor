#include "savemaster.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QFileInfo>
#include <QDir>

#include  "logging.h"

static const QString dateConversionFormat = "HH:mm_dd.MM.yyyy"; //! Формат конвертирования даты в строку

void SaveMaster::save(const QString &oFilePath, const Graph::GraphObject &iGraphObject)
{
    auto targetFileInfo = QFileInfo(oFilePath);
    auto targetFileDirInfo = QFileInfo(targetFileInfo.dir().absolutePath());
    if (!targetFileDirInfo.exists() || (targetFileInfo.exists() && !targetFileInfo.isFile())) {
        throw std::invalid_argument("SaveMaster::save: Invalid path to object file (directory not exist or target is not a file)");
    }

    LOG_INFO("Saving data of graph", iGraphObject.getName(), "to file", oFilePath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(oFilePath);
}

void SaveMaster::load(const QString &iFilePath, Graph::GraphObject &oGraphObject)
{
    auto targetFileInfo = QFileInfo(iFilePath);
    if (!targetFileInfo.exists() || !targetFileInfo.isFile()) {
        throw std::invalid_argument("SaveMaster::load: Invalid path to object file (file not exist or target is not a file)");
    }

    LOG_INFO("Loading data of from file", iFilePath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(iFilePath);
}
