#include "aes256.hpp"

#include "aes256_common.hpp"

#include <Components/Filework/Common.h>
#include <Components/Filework/TemporaryFile.h>

#include <memory>

namespace Encryption {

// =================================================== //
// ============= Encryption processing =============== //
// =================================================== //
std::pair<ContextPtr, CipherPtr>
    aes256_gcm_initEncrypt(const std::string_view& key,
                        const std::string_view& iv,
                        const std::string_view& aad) {
    if (key.size() != 32 || iv.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] encrypt key/IV",
                           "Invalid key or IV length for AES-256-GCM");
        return {};
    }

    const auto pCipher = aes256_gcm_getContext();
    if (!pCipher) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_get_cipherbyname",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    ContextPtr pCtx { EVP_CIPHER_CTX_new(), deleter };
    if (!pCtx) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_CIPHER_CTX_new",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_EncryptInit_ex(pCtx.get(), pCipher.get(), NULL, NULL, NULL) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_EncryptInit_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(iv.size()), NULL) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_CIPHER_CTX_ctrl",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_EncryptInit_ex(pCtx.get(), NULL, NULL,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_EncryptInit_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (!aad.empty()) {
        int aadLength = 0;
        if (EVP_EncryptUpdate(pCtx.get(), NULL, &aadLength,
                              reinterpret_cast<const unsigned char*>(aad.data()),
                              static_cast<int>(aad.size())) != 1) {
            setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                               "[AES-256-GCM] EVP_EncryptUpdate",
                               ERR_error_string(ERR_get_error(), nullptr));
            return {};
        }
    }
    return {std::move(pCtx), pCipher};
}

// Tag + final block length
std::pair<std::string, int> aes256_gcm_deinitEncrypt(const ContextPtr& pCtx, unsigned char* currentPos) {
    int finalBlockLength = 0;
    if (EVP_EncryptFinal_ex(pCtx.get(), currentPos, &finalBlockLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_EncryptFinal_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    unsigned char tagBytes[BLOB_TAG_LENGTH];
    if (EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_GET_TAG,
                            static_cast<int>(sizeof(tagBytes)), tagBytes) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_CIPHER_CTX_ctrl",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    return {std::string(reinterpret_cast<const char*>(tagBytes), BLOB_TAG_LENGTH),
            finalBlockLength};
}


// =================================================== //
// ============= Decryption processing =============== //
// =================================================== //
std::pair<ContextPtr, CipherPtr>
    aes256_gcm_initDecrypt(const std::string_view& key,
                       const std::string_view& iv,
                       const std::string_view& aad,
                       const std::string_view& tag) {
    if (key.size() != 32 || iv.empty() || tag.size() != BLOB_TAG_LENGTH) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] decrypt key/IV/tag",
                           "Invalid key, IV or tag length for AES-256-GCM");
        return {};
    }

    CipherPtr pCipher = aes256_gcm_getContext();
    if (!pCipher) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[AES-256-GCM] aes256_gcm_getContext",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    ContextPtr pCtx { EVP_CIPHER_CTX_new(), deleter };
    if (!pCtx){
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_CIPHER_CTX_new",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    size_t iv_len = iv.size();
    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_size_t(OSSL_CIPHER_PARAM_IVLEN, &iv_len),
        OSSL_PARAM_construct_end()
    };

    if (EVP_DecryptInit_ex2(pCtx.get(), pCipher.get(),
                            reinterpret_cast<const unsigned char*>(key.data()),
                            reinterpret_cast<const unsigned char*>(iv.data()), params) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_EncryptInit_ex2",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (!aad.empty()) {
        int aadLength = 0;
        if (EVP_DecryptUpdate(pCtx.get(), NULL, &aadLength,
                              reinterpret_cast<const unsigned char*>(aad.data()),
                              static_cast<int>(aad.size())) != 1) {
            setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                               "[AES-256-GCM] EVP_DecryptUpdate",
                               ERR_error_string(ERR_get_error(), nullptr));
            return {};
        }
    }
    return {std::move(pCtx), pCipher};
}

std::optional<int> aes256_gcm_deinitDecrypt(
    const ContextPtr& pCtx,
    const std::string& tag,
    unsigned char* plaintextOutput) {
    if (EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_SET_TAG,
                            static_cast<int>(tag.size()),
                            (void*)tag.data()) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_CIPHER_CTX_ctrl",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Check if tag is valid
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(pCtx.get(), plaintextOutput, &plaintextFinalLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_DecryptFinal_ex",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    return plaintextFinalLength;
}


// =================================================== //
// ============ Interface implementation ============= //
// =================================================== //

std::optional<std::string> aes256_gcm_encrypt(const std::string& input, const std::string_view& key, const std::string_view& aad) {
    auto iv = generateBytes_CSPRNG(BLOB_IV_LENGTH);
    std::string tag;
    std::string ciphertext;

    auto [pCtx, pCipher] = aes256_gcm_initEncrypt(key, iv, aad);
    if (!pCipher) { return {}; }

    const int plaintextLength = input.size();
    const int maxCiphertextLength = plaintextLength + EVP_CIPHER_block_size(pCipher.get());

    unsigned char ciphertextBytes[maxCiphertextLength];
    int ciphertextLength = 0;

    if (EVP_EncryptUpdate(pCtx.get(), ciphertextBytes, &ciphertextLength,
                          reinterpret_cast<const unsigned char*>(input.c_str()),
                          plaintextLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_EncryptUpdate",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    auto deinitPair = aes256_gcm_deinitEncrypt(pCtx, ciphertextBytes + ciphertextLength);
    if (deinitPair.first.empty()) { return {}; }
    tag = deinitPair.first;

    ciphertext.assign(reinterpret_cast<const char*>(ciphertextBytes), ciphertextLength + deinitPair.second);
    auto packedResult = packGcmBlob(iv, tag, ciphertext);
    return packedResult;
}


std::optional<std::string> aes256_gcm_decrypt(const std::string& input, const std::string_view& key, const std::string_view& aad) {
    std::string iv;
    std::string tag;
    std::string ciphertext;
    if (!unpackGcmBlob(input, iv, tag, ciphertext)) { return {}; }

    auto [pCtx, pCipher] = aes256_gcm_initDecrypt(key, iv, aad, tag);
    if (!pCipher) { return {}; }

    const int ciphertextLength = ciphertext.size();
    const int maxPlaintextLength = ciphertextLength + EVP_CIPHER_block_size(pCipher.get());

    unsigned char plaintextBytes[maxPlaintextLength];
    int plaintextLength = 0;

    if (EVP_DecryptUpdate(pCtx.get(), plaintextBytes, &plaintextLength,
                          reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
                          ciphertextLength) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] EVP_DecryptUpdate",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    auto finalBlockSizeOpt = aes256_gcm_deinitDecrypt(pCtx, tag, plaintextBytes + plaintextLength);
    if (!finalBlockSizeOpt.has_value()) { return {}; }
    plaintextLength += finalBlockSizeOpt.value();

    return std::string(reinterpret_cast<const char*>(plaintextBytes), plaintextLength);
}


// ===================================================================== //
// ======================== File encryption ============================ //
// ===================================================================== //


ExtraClasses::ErrorObject aes256_gcm_encryptFile(
    const std::string_view& inputFile,
    const std::string_view& key,
    const std::string_view& aad) {

    if (key.size() != 32) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] file encrypt key",
                           "Key size is invalid (expected 32 bytes)");
        return getEncryptionError();
    }

    std::string plaintext;
    auto readResult = Filework::Common::readFileData(inputFile, plaintext);
    if (!readResult.isOk()) { return readResult; }

    auto encrypted = aes256_gcm_encrypt(plaintext,
                                        std::string(key),
                                        std::string(aad));
    if (!encrypted.has_value()) { return getEncryptionError(); }

    auto writeResult = Filework::Common::saveFileData(inputFile, encrypted.value());
    if (!writeResult.isOk()) { return writeResult; }

    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject aes256_gcm_decryptFile(
    const std::string_view& inputFile,
    const std::string_view& key,
    const std::string_view& aad) {

    if (key.size() != 32) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] file decrypt key",
                           "Key size is invalid (expected 32 bytes)");
        return getEncryptionError();
    }

    std::string ciphertext;
    auto readResult = Filework::Common::readFileData(inputFile, ciphertext);
    if (!readResult.isOk()) { return readResult; }

    auto decrypted = aes256_gcm_decrypt(ciphertext,
                                        std::string(key),
                                        std::string(aad));
    if (!decrypted.has_value()) { return getEncryptionError(); }

    try {
        Filework::TemporaryFile outputFile(std::string(inputFile), false);
        outputFile.setSecureMode(true);
        outputFile.write(decrypted->data(),
                         static_cast<std::streamsize>(decrypted->size()));
        outputFile.flush();

        if (outputFile.bad() || outputFile.fail()) {
            setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                               "[AES-256-GCM] file decrypt output",
                               "I/O error while writing decrypted data");
            return getEncryptionError();
        }

        outputFile.accept();
    } catch (const std::exception& ex) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[AES-256-GCM] file decrypt output",
                           ex.what());
        return getEncryptionError();
    }

    return ExtraClasses::Ok;
}

}
