#include "aes256.hpp"

#include "aes256_common.hpp"

namespace Encryption {

std::optional<std::string> aes256_cbc_encrypt(const std::string &input, const std::string &key)
{
    if (input.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES256-CBC] Encrypt",
                           "Empty input");
        return {};
    }

    if (key.size() != 32) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES256-CBC] key size",
                           "Key size is invalid (expected 32 bytes)");
        return {};
    }

    // Setup cipher
    auto pCipher = aes256_cbc_getContext();
    auto pCtx = ContextPtr(EVP_CIPHER_CTX_new(), deleter);
    if (!pCtx || !pCipher) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES256-CBC] Init context",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Init AES
    std::string iv = generateBytes_CSPRNG(64);
    if (EVP_EncryptInit_ex(pCtx.get(), pCipher.get(), NULL, (const unsigned char*)key.c_str(),
                           (const unsigned char*)iv.c_str()) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES256-CBC] EVP_EncryptInit_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Encrypt data
    const int plaintextLength = input.size();
    const int maxCiphertextLength = plaintextLength + EVP_CIPHER_block_size(pCipher.get());
    unsigned char ciphertextBytes[maxCiphertextLength];
    int ciphertextLength = 0;
    if (EVP_EncryptUpdate(pCtx.get(), ciphertextBytes, &ciphertextLength,
                          (const unsigned char*)input.c_str(),
                          plaintextLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES256-CBC] EVP_EncryptUpdate",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Encrypt final block
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(pCtx.get(), ciphertextBytes + ciphertextLength,
                            &ciphertextFinalLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES256-CBC] EVP_EncryptFinal_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    ciphertextLength += ciphertextFinalLength;
    return (iv + std::string((const char*)ciphertextBytes, ciphertextLength));
}

std::optional<std::string> aes256_cbc_decrypt(const std::string &input, const std::string &key)
{
    if (input.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES256-CBC] Decrypt",
                           "Empty input");
        return {};
    }

    if (key.size() != 32) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES256-CBC] key size",
                           "Key size is invalid (expected 32 bytes)");
        return {};
    }

    // Split input into IV and ciphertext
    auto encryptedPacket = input;
    std::string iv = encryptedPacket.substr(0, 64);
    encryptedPacket.erase(0, 64);

    // Init context
    auto pCipher = aes256_cbc_getContext();
    auto pCtx = ContextPtr(EVP_CIPHER_CTX_new(), deleter);
    if (!pCtx) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES256-CBC] Init",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Init AES
    if (EVP_DecryptInit_ex(pCtx.get(), pCipher.get(), NULL, (const unsigned char*)key.c_str(),
                           (const unsigned char*)iv.c_str()) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES256-CBC] EVP_DecryptInit_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Decrypt data
    const int ciphertextLength = encryptedPacket.size();
    const int maxPlaintextLength = ciphertextLength + EVP_CIPHER_block_size(pCipher.get());
    unsigned char plaintextBytes[maxPlaintextLength];
    int plaintextLength = 0;
    if (EVP_DecryptUpdate(pCtx.get(), plaintextBytes, &plaintextLength,
                          (const unsigned char*)encryptedPacket.c_str(),
                          ciphertextLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES256-CBC] EVP_DecryptUpdate",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Decrypt final block
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(pCtx.get(), plaintextBytes + plaintextLength,
                            &plaintextFinalLength) != 1)
    {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES256-CBC] EVP_DecryptFinal_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    plaintextLength += plaintextFinalLength;
    return std::string((const char*)plaintextBytes, plaintextLength);
}

}  // namespace Encryption
