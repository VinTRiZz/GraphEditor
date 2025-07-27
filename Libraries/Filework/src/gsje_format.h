#ifndef GSEJE_FORMAT_H
#define GSEJE_FORMAT_H

#include "gsj_format.h"

namespace Filework
{

/**
 * @brief The GSEJE_Format class Формат gsej с шифрованием
 */
class GSJE_Format : public GSJ_Format
{
public:
    GSJE_Format();
    ~GSJE_Format();

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
