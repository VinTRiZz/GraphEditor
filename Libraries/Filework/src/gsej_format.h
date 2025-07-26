#ifndef GSEJ_FORMAT_H
#define GSEJ_FORMAT_H

#include "abstractsaveformat.h"

#include <QJsonObject>

namespace Filework
{

class GSEJ_Format : public AbstractSaveFormat
{
public:
    GSEJ_Format();
    ~GSEJ_Format();

    bool fromJson(const QJsonObject& iJson);
    QJsonObject toJson() const;

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;
};

}

#endif // GSEJ_FORMAT_H
