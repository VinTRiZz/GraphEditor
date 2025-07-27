#include "gsje_format.h"

#include <QJsonDocument>

#include <Common/Logging.h>
#include <Common/Encryption.h>

#include <QFile>
#include <QFileInfo>

namespace Filework
{

GSJE_Format::GSJE_Format()
{

}

GSJE_Format::~GSJE_Format()
{

}

void GSJE_Format::setKey(const QString &key)
{
    m_key = key;
}

QString GSJE_Format::getKey() const
{
    return m_key;
}

bool GSJE_Format::save(const QString &targetPath) const
{
    if (!isFileValid(targetPath)) {
        return false;
    }

    auto resultData = QJsonDocument(toDataJson()).toJson();
    auto encryptedData = Encryption::encryptAes256Cbc(resultData, getKey().toUtf8());

    // TODO: Write data
    return false;
}

bool GSJE_Format::load(const QString &targetPath)
{
    if (!isFileValid(targetPath) || !QFileInfo(targetPath).exists()) {
        return false;
    }

    // TODO: read data
    return false;
}

bool GSJE_Format::isFileValid(const QString &targetPath) const
{
    return false;
}

}
