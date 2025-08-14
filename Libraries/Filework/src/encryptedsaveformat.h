#ifndef ABSTRACTENCRYPTEDFORMAT_H
#define ABSTRACTENCRYPTEDFORMAT_H

#include "abstractsaveformat.h"

namespace Filework
{

/**
 * @brief The AbstractEncryptedFormatMixin class Миксин для добавления функций шифрования в класс
 */
template <typename T>
class EncryptedFormatMixin
{
public:
    /**
     * @brief setEncryptionKey  Задать ключ шифрования
     * @param keyString         Строка-ключ шифрования
     */
    void setEncryptionKey(const QString& keyString) {
        m_encryptedKey = keyString;
    }

    /**
     * @brief getEncryptionKey  Получить ключ шифрования
     * @return                  Строка-ключ шифрования
     */
    QString getEncryptionKey() const {
        return m_encryptedKey;
    }

protected:
    QString m_encryptedKey;
};

using EncryptedSaveFormat = EncryptedFormatMixin<AbstractSaveFormat>;

}

#endif // ABSTRACTENCRYPTEDFORMAT_H
