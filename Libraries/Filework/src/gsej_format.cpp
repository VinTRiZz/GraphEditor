#include "gsej_format.h"
#include "qfileinfo.h"

#include <QJsonDocument>

#include <Common/Logging.h>
#include <Common/Encryption.h>

namespace Filework
{

GSEJ_Format::GSEJ_Format()
{

}

GSEJ_Format::~GSEJ_Format()
{

}

bool GSEJ_Format::fromDataJson(const QJsonObject &iJson)
{
    return false;
}

QJsonObject GSEJ_Format::toDataJson() const
{
    return {};
}

bool GSEJ_Format::save(const QString &targetPath) const
{
    if (!isFileValid(targetPath)) {
        return false;
    }

    auto resultData = QJsonDocument(toDataJson()).toJson();

    // TODO: Write data
    return false;
}

bool GSEJ_Format::load(const QString &targetPath)
{
    if (!isFileValid(targetPath) || !QFileInfo(targetPath).exists()) {
        return false;
    }

    // TODO: read data
    return false;
}

bool GSEJ_Format::isFileValid(const QString &targetPath) const
{
    auto fileInfo = QFileInfo(targetPath);
    if (fileInfo.exists()) {
        QFile iFile(targetPath);
        auto testJson = QJsonDocument::fromJson(iFile.readAll());

        if (testJson.isNull()) {
            return false;
        }

        auto jsonObj = testJson.object();
        if (jsonObj.contains("")) {

        }
    }
    return false;
}

}
