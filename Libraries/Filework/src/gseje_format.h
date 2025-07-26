#ifndef GSEJE_FORMAT_H
#define GSEJE_FORMAT_H

#include "gsej_format.h"

namespace Filework
{

/**
 * @brief The GSEJE_Format class Формат gsej с шифрованием
 */
class GSEJE_Format : public GSEJ_Format
{
public:
    GSEJE_Format();
    ~GSEJE_Format();

    void setKey(const QString& key);
    QString getKey() const;

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;

private:
    QString m_key;
};

}

#endif // GSEJE_FORMAT_H
