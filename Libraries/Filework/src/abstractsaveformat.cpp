#include "abstractsaveformat.h"

#include <QBuffer>
#include <QFile>

namespace Filework {

AbstractSaveFormat::AbstractSaveFormat(
        const QString &formatVersion,
        const QString &formatExtension,
        const QString &formatDescription,
        bool backwardCompatible) :
    m_formatVersion(formatVersion),
    m_formatExtension(formatExtension),
    m_formatDescription(formatDescription),
    m_isBackwardCompatible(backwardCompatible) {

}

AbstractSaveFormat::~AbstractSaveFormat() {}

bool AbstractSaveFormat::isBackwardCompatible() const
{
    return m_isBackwardCompatible;
}

QString AbstractSaveFormat::getExtension() const
{
    return m_formatExtension;
}

QString AbstractSaveFormat::getDescription() const
{
    return m_formatDescription;
}

QString AbstractSaveFormat::getVersion() const
{
    return m_formatVersion;
}

void AbstractSaveFormat::setGraphMaintainer(Graph::PMaintainer pGraphMaintaner) {
    m_pGraphMaintaner = pGraphMaintaner;
}

Graph::PMaintainer AbstractSaveFormat::getGraphMaintainer() const {
    return m_pGraphMaintaner;
}

bool AbstractSaveFormat::replaceFileData(const QString& filePath,
                                         const QByteArray& iData) const {
    QFile targetFile(filePath);
    targetFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    if (!targetFile.isOpen()) {
        LOG_ERROR("Error opening file to rewrite:", filePath,
                  "Reason:", targetFile.errorString());
        return false;
    }
    targetFile.write(iData);
    return true;
}

bool AbstractSaveFormat::readFromFile(const QString& filePath,
                                      QByteArray& oData) const {
    QFile targetFile(filePath);
    targetFile.open(QIODevice::ReadOnly);
    if (!targetFile.isOpen()) {
        LOG_ERROR("Error opening file to read:", filePath,
                  "Reason:", targetFile.errorString());
        return false;
    }
    oData = targetFile.readAll();
    return true;
}

QByteArray AbstractSaveFormat::getEncoded(const QByteArray& iStr) const {
    return iStr.toHex();
}

QByteArray AbstractSaveFormat::getEncodedPixmap(const QPixmap& iPxmap) const {
    QByteArray bytes;
    QBuffer byteBuff(&bytes);
    if (iPxmap.save(&byteBuff, "PNG")) {
        return bytes.toHex();
    }
    return {};
}

QByteArray AbstractSaveFormat::getDecoded(const QByteArray& iBytes) const {
    return QByteArray::fromHex(iBytes);
}

QPixmap AbstractSaveFormat::getDecodedPixmap(const QByteArray& iBytes) const {
    QPixmap pxmap;
    pxmap.loadFromData(QByteArray::fromHex(iBytes), "PNG");
    return pxmap;
}

}  // namespace Filework
