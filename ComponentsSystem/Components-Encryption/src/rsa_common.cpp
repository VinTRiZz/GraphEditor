#include "rsa_common.hpp"


namespace Encryption {

ContextPtr rsaCreateKeygenContext() {
    return ContextPtr(EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr),
                      [](EVP_PKEY_CTX* ctx){
                          if (nullptr != ctx) {
                              EVP_PKEY_CTX_free(ctx);
                          }
                          const auto errorCode = ERR_peek_error();
                          if (errorCode != 0) {
                              setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                                                 "[RSA] Context free",
                                                 ERR_error_string(errorCode, nullptr));
                          }
                      });
}

ContextPtr rsaCreateContext(const RSAKeysPtr &publicKey) {
    return ContextPtr(EVP_PKEY_CTX_new_from_pkey(nullptr, publicKey.get(), nullptr),
                      [](EVP_PKEY_CTX* ctx){
                          if (nullptr != ctx) {
                              EVP_PKEY_CTX_free(ctx);
                          }
                          const auto errorCode = ERR_peek_error();
                          if (errorCode != 0) {
                              setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                                                 "[RSA] Context free",
                                                 ERR_error_string(errorCode, nullptr));
                          }
                      });
}


std::optional<size_t> getDataChunkLength(const RSAKeysPtr &key, int padding) {
    if (!key) { return std::nullopt; }

    int keySize = EVP_PKEY_get_size(key.get());
    if (keySize <= 0) { return std::nullopt; }

    switch (padding) {
    case RSA_PKCS1_PADDING:
        // PKCS#1 v1.5 padding requires at least 11 bytes of overhead
        if (keySize <= 11) return std::nullopt;
        return static_cast<size_t>(keySize - 11);

    case RSA_NO_PADDING:
        // No padding: the entire modulus can be used
        return static_cast<size_t>(keySize);

    case RSA_PKCS1_OAEP_PADDING: {
        // RSA OAEP uses SHA-256 for both the digest and MGF1 in this component.
        const int hashSize = 32;
        // OAEP overhead = 2 * hashLen + 2
        if (keySize <= 2 * hashSize + 2) return std::nullopt;
        return static_cast<size_t>(keySize - 2 * hashSize - 2);
    }
    }
    // Unsupported padding mode for encryption
    return std::nullopt;    
}

FilePtr openFileC(const std::string_view &filePath, const char* mode) {
    const std::string path(filePath);
    return FilePtr( fopen(path.c_str(), mode), f_fileCloser );
}

BIOPtr createBIO(const std::string_view &writableValue) {
    return BIOPtr(
        BIO_new_mem_buf(writableValue.data(), static_cast<int>(writableValue.size())),
        [](BIO* bio){
            if (nullptr != bio) {
                BIO_free(bio);
            }
        }
        );
}

}