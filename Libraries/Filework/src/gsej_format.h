#ifndef GSEJE_FORMAT_H
#define GSEJE_FORMAT_H

#include "gsj_format.h"

namespace Filework
{

/**
 * @brief The GSEJ_Format class Формат gsej с шифрованием
 */
class GSEJ_Format : public GSJ_Format
{
public:
    GSEJ_Format();
    ~GSEJ_Format();

    QString getExtension() const override;
    QString getDescription() const override;
    void    setEncryptionKey(const QString& keyString) override;;
    QString getEncryptionKey(const QString& keyString) const override;

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;

private:
    QString m_key;
};

}

#endif // GSEJE_FORMAT_H
