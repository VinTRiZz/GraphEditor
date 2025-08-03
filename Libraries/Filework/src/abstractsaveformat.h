#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <GraphObject/Maintainer.h>

#include <QString>

namespace Filework {

class AbstractSaveFormat {
public:
    AbstractSaveFormat();
    virtual ~AbstractSaveFormat();

    virtual QString getExtension() const = 0;
    virtual QString getDescription() const = 0;
    virtual void setEncryptionKey(const QString& keyString) = 0;
    virtual QString getEncryptionKey(const QString& keyString) const = 0;

    bool getIsEncrypted() const;
    QString getVersion() const;

    void setGraphMaintaner(Graph::PMaintainer pGraphMaintaner);
    Graph::PMaintainer getGraphMaintaner() const;
    Graph::GraphObject& getGraph() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;

private:
    Graph::PMaintainer m_pGraphMaintaner;

protected:
    bool m_isEncrypted{false};
    QString m_formatVersion;

    bool rewriteFileData(const QString& filePath,
                         const QByteArray& iData) const;
    bool readFromFile(const QString& filePath, QByteArray& oData) const;

    /**
     * @brief getEncoded Транслировать символы, которые могут иметь управляющие
     * символы, в base64
     * @param iStr Входной массив байт
     * @return Конвертированный массив байт
     */
    QByteArray getEncoded(const QByteArray& iStr) const;
    QByteArray getDecoded(const QByteArray& iBytes) const;

    /**
     * @brief getEncodedPixmap  Конвертировать pixmap в base64 PNG кодированный
     * набор байт
     * @param iPxmap            Входное изображение
     * @return                  Массив байт
     */
    QByteArray getEncodedPixmap(const QPixmap& iPxmap) const;
    QPixmap getDecodedPixmap(const QByteArray& iBytes) const;
};

}  // namespace Filework

#endif  // ABSTRACTSAVEFORMAT_H
