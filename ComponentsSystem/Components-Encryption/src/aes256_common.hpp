#pragma once

#include <Components/ExtraClasses/Error.h>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/core_names.h> // OSSL_ defines

#include <optional>
#include <string_view>
#include <memory>

#include "common.hpp"

namespace Encryption {

using CipherPtr = std::shared_ptr<EVP_CIPHER>;
CipherPtr aes256_createContext(const std::string& cipherName) noexcept;
CipherPtr aes256_gcm_getContext();
CipherPtr aes256_cbc_getContext();

static const auto BLOB_IV_LENGTH  = 12;  // GCM IV size recommended
static const auto BLOB_TAG_LENGTH = 16;  // GCM tag length
static const auto BLOB_LENGTH = BLOB_IV_LENGTH + BLOB_TAG_LENGTH;

// =================================================== //
// Pack in order: IV || TAG || CIPHERTEXT
std::optional<std::string> packGcmBlob(const std::string& iv,
                                       const std::string& tag,
                                       const std::string& ciphertext);

// =================================================== //
// Pack in order: IV (12) | TAG (16) | CIPHERTEXT
bool unpackGcmBlob(const std::string& blob,
                   std::string& iv,
                   std::string& tag,
                   std::string& ciphertext);


// =================================================== //
// =================================================== //
// =================================================== //
inline auto deleter = [](EVP_CIPHER_CTX* ctx){
    if (nullptr != ctx) {
        EVP_CIPHER_CTX_free(ctx);
    }
    const auto errorCode = ERR_peek_error();
    if (errorCode != 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[AES-256-GCM] Context deleting",
                           ERR_error_string(errorCode, nullptr));
    }
};
using ContextPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(deleter)>;

}