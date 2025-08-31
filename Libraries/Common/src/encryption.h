#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QByteArray>

namespace Encryption {

/**
 * @brief encryptAes256Cbc  Зашифровать текст с помощью AES-256
 * @param plainText         Текст для шифрования
 * @param key               Ключ для шифрования. Если меньше 32 байт, дополнится нулями, иначе -- излишние символы будут через XOR применены к первым 32 байт
 * @return                  Зашифрованный текст или NULL QByteArray при ошибке
 */
QByteArray encryptAes256Cbc(const QByteArray& plainText, QByteArray key);

/**
 * @brief decryptAes256Cbc  Дешифровать текст с помощью AES-256
 * @param plainText         Текст для дешифрования
 * @param key               Ключ для дешифрования. Если меньше 32 байт, дополнится нулями, иначе -- излишние символы будут через XOR применены к первым 32 байт
 * @return                  Дешифрованный текст или NULL QByteArray при ошибке
 */
QByteArray decryptAes256Cbc(QByteArray cipherText, QByteArray key);

/**
 * @brief sha256    Вычислить хеш по функции SHA-256
 * @param txt       Входной массив байт (или текст)
 * @return          Хеш (64 байт)
 */
QByteArray sha256(QByteArray txt);

}  // namespace Encryption

#endif  // ENCRYPTION_H
