#include "gsej_100_savesubsystem.hpp"

#include <Components/Encryption/AES-256.h>

namespace Filework {

GSEJ_100_SaveSubsystem::GSEJ_100_SaveSubsystem() :
    AbstractSaveSubsystem("1.0.0")
{

}

void GSEJ_100_SaveSubsystem::setEncryptionKey(const QString &key)
{
    m_key = key;
}

QString GSEJ_100_SaveSubsystem::getEncryptionKey() const
{
    return m_key;
}

bool GSEJ_100_SaveSubsystem::canProcess(const QString &fileData) const
{
    QJsonParseError err;
    auto parsedJson = QJsonDocument::fromJson(fileData.toUtf8(), &err);
    if (parsedJson.isNull()) {
        setErrorText("Parse error: " + err.errorString());
        return false;
    }
    auto iJson = parsedJson.object();
    return (iJson["version"] == getVersion());
}

bool GSEJ_100_SaveSubsystem::createSavedata(const Graph::PMaintainer &pGraph, QString &savedata) const
{
    if (!m_gsjSubsys.createSavedata(pGraph, savedata)) {
        return false;
    }

    QJsonObject resJson;
    resJson["version"] = getVersion();
    resJson["payload"] = QString(Encryption::qtEncryptAes256Cbc(savedata.toUtf8(), m_key.toUtf8()).toHex());
    if (resJson["payload"].isNull()) {
        savedata = QString();
        setErrorText(QString("[GSEJ SUBSYS] Error encrypting: ") + Encryption::getEncryptionErrorText().c_str());
        return false;
    }
    savedata = QJsonDocument(resJson).toJson(QJsonDocument::Compact);
    return true;
}

bool GSEJ_100_SaveSubsystem::parseSavedata(const Graph::PMaintainer &pGraph, const QString &savedata) const
{
    QJsonParseError err;
    auto parsedJson = QJsonDocument::fromJson(savedata.toUtf8(), &err);
    if (parsedJson.isNull()) {
        setErrorText("Parse error: " + err.errorString());
        return false;
    }
    auto iJson = parsedJson.object();
    auto decryptedData = Encryption::qtDecryptAes256Cbc(QByteArray::fromHex(iJson["payload"].toString().toUtf8()), m_key.toUtf8());
    if (decryptedData.isNull()) {
        setErrorText(QString("[GSEJ SUBSYS] Error decrypting: ") + Encryption::getEncryptionErrorText().c_str());
        return false;
    }
    return m_gsjSubsys.parseSavedata(pGraph, decryptedData);
}

} // namespace Filework
