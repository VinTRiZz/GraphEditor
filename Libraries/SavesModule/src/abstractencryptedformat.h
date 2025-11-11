#pragma once

#include "abstractsaveformat.h"

namespace Filework {

/**
 * @brief The AbstractEncryptedFormat class База для шифрующего формата
 * шифрования в класс
 */
class AbstractEncryptedFormat : public AbstractSaveFormat {
public:
    using AbstractSaveFormat::AbstractSaveFormat;

    /**
     * @brief setEncryptionKey  Задать ключ шифрования
     * @param keyString         Строка-ключ шифрования
     */
    void setEncryptionKey(const QString& keyString);

    /**
     * @brief getEncryptionKey  Получить ключ шифрования
     * @return                  Строка-ключ шифрования
     */
    QString getEncryptionKey() const;

protected:
    QString m_encryptedKey;
};

}  // namespace Filework
