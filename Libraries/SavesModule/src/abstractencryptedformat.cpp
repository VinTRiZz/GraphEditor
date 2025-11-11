#include "abstractencryptedformat.h"

void Filework::AbstractEncryptedFormat::setEncryptionKey(const QString &keyString) {
    m_encryptedKey = keyString;
}

QString Filework::AbstractEncryptedFormat::getEncryptionKey() const {
    return m_encryptedKey;
}
