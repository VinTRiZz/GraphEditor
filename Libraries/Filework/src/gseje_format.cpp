#include "gseje_format.h"

#include <QJsonDocument>

#include <Common/Encryption.h>

namespace Filework
{

GSEJE_Format::GSEJE_Format()
{

}

GSEJE_Format::~GSEJE_Format()
{

}

void GSEJE_Format::setKey(const QString &key)
{
    m_key = key;
}

QString GSEJE_Format::getKey() const
{
    return m_key;
}

bool GSEJE_Format::save(const QString &targetPath) const
{
    auto resultData = QJsonDocument(toDataJson()).toJson();
    auto encryptedData = Encryption::encryptAes256Cbc(resultData, getKey().toUtf8());

    // TODO: Write data
    return false;
}

bool GSEJE_Format::load(const QString &targetPath)
{
    // TODO: read data
    return false;
}

bool GSEJE_Format::isFileValid(const QString &targetPath) const
{
    return false;
}

}
