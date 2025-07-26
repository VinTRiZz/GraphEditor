#ifndef GSEJ_FORMAT_H
#define GSEJ_FORMAT_H

#include "abstractsaveformat.h"

#include <QJsonObject>

namespace Filework
{

/**
 * @brief The GSEJ_Format class Обновлённый формат сохранения данных, представляет из себя JSON
 */
class GSEJ_Format : public AbstractSaveFormat
{
public:
    GSEJ_Format();
    ~GSEJ_Format();

    bool fromDataJson(const QJsonObject& iJson);
    QJsonObject toDataJson() const;

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;
};

}

#endif // GSEJ_FORMAT_H
