#pragma once

#include <Components/ExtraClasses/Error.h>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/core_names.h> // OSSL_ defines

#include <cstring>
#include <optional>
#include <string_view>
#include <memory>

#include "common.hpp"
#include "rsa.hpp"

namespace Encryption {

using ContextPtr = std::shared_ptr<EVP_PKEY_CTX>;

// TODO: First arg is context for isolation, use?
ContextPtr rsaCreateKeygenContext();
ContextPtr rsaCreateContext(const RSAKeysPtr& publicKey);

// Returns the maximum size of plaintext (in bytes) that can fit in a single chunk
std::optional<size_t> getDataChunkLength(const RSAKeysPtr& key, int padding);


// For saving into files
inline auto f_fileCloser = [](auto* fp) {
    fclose(fp);
};
using FilePtr = std::unique_ptr<FILE, decltype(f_fileCloser)>;

FilePtr openFileC(const std::string_view& filePath, const char* mode = "rb");

// For passwords
struct KeyPassphrase {
    const char* data {nullptr};
    std::size_t size {0};
};

inline auto f_keyReader = [](char *buf, int size, int rwflag, void *userdata) -> int {
    if (buf == nullptr || size <= 0 || userdata == nullptr) {
        return 0;
    }

    const auto* passphrase = static_cast<const KeyPassphrase*>(userdata);
    if (passphrase->data == nullptr || passphrase->size == 0) {
        return 0;
    }

    const auto copySize = passphrase->size < static_cast<std::size_t>(size)
                              ? passphrase->size
                              : static_cast<std::size_t>(size);
    std::memcpy(buf, passphrase->data, copySize);
    return static_cast<int>(copySize);
};

// Used in BIO's
using BIOPtr =  std::shared_ptr<BIO>;
BIOPtr createBIO(const std::string_view& writableValue);

}