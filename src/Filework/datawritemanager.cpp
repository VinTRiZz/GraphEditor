#include "datawritemanager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QBuffer>
#include <QFileInfo>
#include <QDir>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "Graph/graphcommon.h"

#include "logging.h"

namespace Filework
{

DataWriteManager::DataWriteManager()
{

}

bool DataWriteManager::save(const QString &oFilePath, const Graph::GraphObject &iGraphObject)
{
    auto targetFileInfo = QFileInfo(oFilePath);
    auto targetFileDirInfo = QFileInfo(targetFileInfo.dir().absolutePath());
    if (!targetFileDirInfo.exists() || (targetFileInfo.exists() && !targetFileInfo.isFile())) {
        LOG_ERROR("SaveMaster::save: Invalid path to object file (directory not exist or target is not a file)");
        return false;
    }
    LOG_INFO("Saving data of graph", iGraphObject.getName(), "to file", oFilePath);

    QJsonObject generalConfig;
    QJsonObject currentConfig;
    QJsonObject currentConfigData;

    auto addConfigData = [&](const QString& configName, const QVariant& configValue){
        currentConfigData[configName] = configValue.toByteArray().toHex().constData();
    };

    auto appendSection = [&](const QString& sectionName) {
        currentConfig[sectionName] = currentConfigData;
        currentConfigData = {};
    };

    auto appendMainConfig = [&](const QString& sectionName){
        currentConfigData[sectionName] = currentConfig;
    };

    addConfigData("name",            iGraphObject.getName().toUtf8().toHex());
    addConfigData("description",     iGraphObject.getDescription().toUtf8().toHex());
    addConfigData("created",         iGraphObject.getCreateTime().toString(GraphCommon::DATE_CONVERSION_FORMAT).toUtf8().toHex());
    addConfigData("changed",         iGraphObject.getEditTime().toString(GraphCommon::DATE_CONVERSION_FORMAT).toUtf8().toHex());
    appendSection("COMMON");

    for (auto& conf : iGraphObject.getCustomValueMap()) {
        addConfigData(conf.first, conf.second);
    }
    appendSection("CUSTOM");

    appendMainConfig("PROPERTIES");

    for (auto& vert : iGraphObject.getAllVertices()) {
        addConfigData("id",                 vert.id);
        addConfigData("posX",               vert.posX);
        addConfigData("posY",               vert.posY);
        addConfigData("shortName",          vert.shortName);
        addConfigData("name",               vert.name);
        addConfigData("description",        vert.description);
        addConfigData("customProperties",   vert.customProperties);
        addConfigData("borderColor",        vert.borderColor.name());
        addConfigData("backgroundColor",    vert.backgroundColor.name());
//        addConfigData("pxmap",              getEncoded(vert.pxmap));
    }

    QFile oFile(oFilePath);
    if (!oFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        LOG_ERROR("Error opening file:", oFile.errorString());
        return false;
    }

    return false;
}

}
