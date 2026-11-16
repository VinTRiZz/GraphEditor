#include "aes256_common.hpp"

namespace Encryption {

// TODO: First arg is context for isolation, use?
CipherPtr aes256_createContext(const std::string &cipherName) noexcept {
    auto cipher = CipherPtr(
        EVP_CIPHER_fetch(nullptr, cipherName.c_str(), nullptr),
        [](EVP_CIPHER* cipher) { EVP_CIPHER_free(cipher); });
    return cipher;
}

CipherPtr aes256_cbc_getContext() {
    thread_local static auto cipher = aes256_createContext("aes-256-cbc");
    return cipher;
}

CipherPtr aes256_gcm_getContext() {
    thread_local static auto cipher = aes256_createContext("aes-256-gcm");
    return cipher;
}

std::optional<std::string> packGcmBlob(const std::string &iv, const std::string &tag, const std::string &ciphertext)
{
    if (iv.size() != BLOB_IV_LENGTH || tag.size() != BLOB_TAG_LENGTH) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] packGcmBlob",
                           "Invalid size of AES-256-GCM blob part");
        return {};
    }

    std::string blob;
    blob.reserve(iv.size() + tag.size() + ciphertext.size());
    blob.append(iv);
    blob.append(tag);
    blob.append(ciphertext);
    return blob;
}

bool unpackGcmBlob(const std::string &blob, std::string &iv, std::string &tag, std::string &ciphertext)
{
    if (blob.size() < BLOB_IV_LENGTH + BLOB_TAG_LENGTH) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[AES-256-GCM] unpackGcmBlob",
                           "Invalid size of AES-256-GCM blob part");
        return false;
    }
    iv.assign(blob.data(), BLOB_IV_LENGTH);
    tag.assign(blob.data() + BLOB_IV_LENGTH, BLOB_TAG_LENGTH);
    ciphertext.assign(blob.data() + BLOB_IV_LENGTH + BLOB_TAG_LENGTH,
                      blob.size() - BLOB_IV_LENGTH - BLOB_TAG_LENGTH);
    return true;
}

}