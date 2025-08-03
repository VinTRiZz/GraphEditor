#include "encryption.h"

#include <openssl/aes.h>  // Для AES_BLOCK_SIZE
#include <openssl/err.h>
#include <openssl/evp.h>

#include <QByteArray>
#include <QDebug>
#include <random>

namespace Encryption {

QByteArray generateSecureIV() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<char> dist(-127, 127);

    QByteArray iv(AES_BLOCK_SIZE, 0);  // 16 байт
    std::generate_n(iv.data(), AES_BLOCK_SIZE - 1,
                    []() -> char { return dist(gen); });
    return iv;
}

QByteArray encryptAes256Cbc(const QByteArray& plainText, QByteArray key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return QByteArray();

    auto iv = generateSecureIV();

    if (key.size() > 32) {
        auto separatedKeyData = key.mid(32);
        key = key.mid(0, 32);
    } else if (key.size() < 32) {
        std::fill_n(std::back_inserter(key), 32 - key.size(), 0);
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) !=
        1) {
        return QByteArray();
    }

    QByteArray cipherText;
    cipherText.resize(plainText.size() + AES_BLOCK_SIZE);
    int len = 0;
    int cipherTextLen = 0;

    if (EVP_EncryptUpdate(
            ctx, reinterpret_cast<unsigned char*>(cipherText.data()), &len,
            reinterpret_cast<const unsigned char*>(plainText.data()),
            plainText.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    cipherTextLen = len;

    if (EVP_EncryptFinal_ex(
            ctx, reinterpret_cast<unsigned char*>(cipherText.data()) + len,
            &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    cipherTextLen += len;
    cipherText.resize(cipherTextLen);

    EVP_CIPHER_CTX_free(ctx);
    return iv + cipherText;
}

QByteArray decryptAes256Cbc(QByteArray cipherText, QByteArray key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return QByteArray();

    // Get IV
    auto iv = cipherText.mid(0, 15);
    cipherText = cipherText.mid(16);

    if (key.size() > 32) {
        auto separatedKeyData = key.mid(32);
        key = key.mid(0, 32);
    } else if (key.size() < 32) {
        std::fill_n(std::back_inserter(key), 32 - key.size(), 0);
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) !=
        1) {
        return QByteArray();
    }

    QByteArray plainText;
    plainText.resize(cipherText.size());
    int len = 0;
    int plainTextLen = 0;

    if (EVP_DecryptUpdate(
            ctx, reinterpret_cast<unsigned char*>(plainText.data()), &len,
            reinterpret_cast<const unsigned char*>(cipherText.data()),
            cipherText.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plainTextLen = len;

    if (EVP_DecryptFinal_ex(
            ctx, reinterpret_cast<unsigned char*>(plainText.data()) + len,
            &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plainTextLen += len;
    plainText.resize(plainTextLen);

    EVP_CIPHER_CTX_free(ctx);
    return plainText;
}

}  // namespace Encryption
