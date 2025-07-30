#include "abstractsaveformat.h"

#include <QFile>
#include <QBuffer>

namespace Filework
{

AbstractSaveFormat::AbstractSaveFormat()
{

}

AbstractSaveFormat::~AbstractSaveFormat()
{

}

bool AbstractSaveFormat::getIsEncrypted() const
{
    return m_isEncrypted;
}

void AbstractSaveFormat::setGraphMaintaner(Graph::PMaintainer pGraphMaintaner)
{
    m_pGraphMaintaner = pGraphMaintaner;
}

Graph::PMaintainer AbstractSaveFormat::getGraphMaintaner() const
{
    return m_pGraphMaintaner;
}

Graph::GraphObject& AbstractSaveFormat::getGraph() const
{
    if (!m_pGraphMaintaner) {
        throw std::runtime_error("SaveFormat: Did not set graph maintaner, requested graph (invalid operation)");
    }
    return m_pGraphMaintaner->getObject();
}

bool AbstractSaveFormat::rewriteFileData(const QString &filePath, const QByteArray &iData) const
{
    QFile targetFile(filePath);
    targetFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    if (!targetFile.isOpen()) {
        LOG_ERROR("Error opening file to rewrite:", filePath, "Reason:", targetFile.errorString());
        return false;
    }
    targetFile.write(iData);
    return true;
}

bool AbstractSaveFormat::readFromFile(const QString &filePath, QByteArray &oData) const
{
    QFile targetFile(filePath);
    targetFile.open(QIODevice::ReadOnly);
    if (!targetFile.isOpen()) {
        LOG_ERROR("Error opening file to read:", filePath, "Reason:", targetFile.errorString());
        return false;
    }
    oData = targetFile.readAll();
    return true;
}

QByteArray AbstractSaveFormat::getEncoded(const QByteArray &iStr) const
{
    return iStr.toHex();
}

QByteArray AbstractSaveFormat::getEncodedPixmap(const QPixmap &iPxmap) const
{
    QByteArray bytes;
    QBuffer byteBuff(&bytes);
    if (iPxmap.save(&byteBuff, "PNG")) {
        return bytes.toHex();
    }
    return {};
}

QByteArray AbstractSaveFormat::getDecoded(const QByteArray &iBytes) const
{
    return QByteArray::fromHex(iBytes);
}

QPixmap AbstractSaveFormat::getDecodedPixmap(const QByteArray &iBytes) const
{
    QPixmap pxmap;
    pxmap.loadFromData(QByteArray::fromHex(iBytes), "PNG");
    return pxmap;
}

}
