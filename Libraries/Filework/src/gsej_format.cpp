#include "gsej_format.h"

#include <Common/Encryption.h>
#include <Common/Logging.h>

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>

namespace Filework {

GSEJ_Format::GSEJ_Format()
    : AbstractSaveFormat("1.0.0", "gsej", "Зашифрованный граф", true) {}

GSEJ_Format::~GSEJ_Format() {}

void GSEJ_Format::setGraphMaintainer(Graph::PMaintainer pMaintainer) {
  m_rootFormat.setGraphMaintainer(pMaintainer);
  AbstractSaveFormat::setGraphMaintainer(pMaintainer);
}

bool GSEJ_Format::save(const QString &targetPath) const {
  auto targetDir = QFileInfo(targetPath).dir();
  if (!targetDir.exists()) {
    return false;
  }

  auto systemJson = createSystemJson();
  QJsonObject resultJson;
  resultJson["system"] = systemJson;

  auto payloadJson = m_rootFormat.toDataJson();
  resultJson["payload"] =
      Encryption::encryptAes256Cbc(
          QJsonDocument(payloadJson).toJson(QJsonDocument::Compact),
          m_key.toUtf8())
          .toHex()
          .data();

  auto resultData = QJsonDocument(resultJson).toJson();
  return replaceFileData(targetPath, resultData);
}

bool GSEJ_Format::load(const QString &targetPath) {
  if (!isFileValid(targetPath)) {
    LOG_WARNING("Invalid path:", targetPath);
    return false;
  }
  // незачем проверять, что считалось, как и структуру JSON (она проверяется в
  // isFileValid)
  QByteArray dataBytes;
  readFromFile(targetPath, dataBytes);
  auto inputJson = QJsonDocument::fromJson(dataBytes);

  // TODO: Проверка версий?
  auto systemSection = inputJson["system"];
  if (!systemSection["is_encrypted"]
           .toBool()) { // Не работаем с "чистыми" данными
    LOG_WARNING("Not encrypted data in GSJE format found");
    return false;
  }

  // Дешифровка
  auto payloadSection = inputJson["payload"];
  auto payloadHex = payloadSection.toString().toUtf8();
  auto payloadEncrypted = QByteArray::fromHex(payloadHex);
  auto decryptedData =
      Encryption::decryptAes256Cbc(payloadEncrypted, m_key.toUtf8());
  return m_rootFormat.initFromDataJson(
      QJsonDocument::fromJson(decryptedData).object());
}

bool GSEJ_Format::isFileValid(const QString &targetPath) const {
  QByteArray oDataBytes;
  if (!readFromFile(targetPath, oDataBytes)) {
    return false;
  }

  auto testJson = QJsonDocument::fromJson(oDataBytes);
  if (testJson.isNull()) {
    LOG_INFO("GSJE format: Invalid file (NULL data json)", targetPath);
    return false;
  }

  auto jsonObj = testJson.object();
  if (!jsonObj.contains("system") || !jsonObj.contains("payload")) {
    LOG_INFO("GSJE format: Invalid file (do not contain some sections)",
             targetPath);
    return false;
  }
  return jsonObj["system"].isObject();
}

QJsonObject GSEJ_Format::createSystemJson() const {
  QJsonObject systemObj;
  systemObj["app_version"] = QString(GRAPH_EDITOR_VERSION);
  systemObj["format_version"] = getVersion();
  systemObj["is_encrypted"] = true;
  return systemObj;
}

} // namespace Filework
