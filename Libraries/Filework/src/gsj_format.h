#ifndef GSEJ_FORMAT_H
#define GSEJ_FORMAT_H

#include "abstractsaveformat.h"

#include <QJsonObject>

namespace Filework
{

/**
 * @brief The GSJ_Format class Обновлённый формат сохранения данных, представляет из себя JSON
 */
class GSJ_Format : public AbstractSaveFormat
{
public:
    GSJ_Format();
    ~GSJ_Format();

    QString getExtension() const override;
    QString getDescription() const override;
    void    setEncryptionKey(const QString& keyString) override {};
    QString getEncryptionKey(const QString& keyString) const override { return {}; }

    bool initFromDataJson(const QJsonObject& iJson);
    QJsonObject toDataJson() const;

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;
    bool isStructureValid(const QJsonObject& iJson) const;

protected:
    QJsonObject createSystemJson() const;
    bool config_getIsEncrypted(const QJsonObject& iJson);
    QString config_getVersion(const QJsonObject& iJson);
    QString config_getFormatVersion(const QJsonObject& iJson);
};

}

#endif // GSEJ_FORMAT_H
