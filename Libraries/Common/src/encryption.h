#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QByteArray>

namespace Encryption
{

QByteArray encryptAes256Cbc(const QByteArray& plainText, QByteArray key);
QByteArray decryptAes256Cbc(QByteArray cipherText, QByteArray key);

}

#endif // ENCRYPTION_H
