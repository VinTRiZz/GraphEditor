#include "hash.hpp"

#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/kdf.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include <climits>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <filesystem>

// OpenSSL engine implementation
#define OPENSSL_ENGINE NULL

namespace Encryption {

std::optional<std::string> sha256(const std::string_view &input)
{
    if (input.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[SHA-256] input",
                           "Empty input");
        return {};
    }

    std::shared_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new(), [](auto* ctx) {
        if (ctx) {
            EVP_MD_CTX_free(ctx);
        }
    });
    if (!context) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_MD_CTX_new",
                           std::string("Failed to create context of SHA-256: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestInit_ex",
                           std::string("SHA-256 sum init failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_DigestUpdate(context.get(), input.data(), input.size()) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestUpdate",
                           std::string("SHA-256 sum data adding failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    if (EVP_DigestFinal_ex(context.get(), hash, &lengthOfHash) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestFinal_ex",
                           std::string("SHA-256 sum finalize failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}



std::optional<std::string> sha256file(const std::string_view &filepath)
{
    if (!std::filesystem::exists(filepath)) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[SHA-256] file",
                           std::string("SHA-256 sum failed: No such file or directory: ") + filepath.data());
        return {};
    }

    std::shared_ptr<EVP_MD_CTX> context(EVP_MD_CTX_new(), [](auto* ctx) {
        if (ctx) {
            EVP_MD_CTX_free(ctx);
        }
    });
    if (!context) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_MD_CTX_new",
                           std::string("Failed to create EVP_MD_CTX: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    if (EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestInit_ex",
                           std::string("SHA-256 sum init failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::ifstream inputFile(filepath.data());
    const auto bufsize = 1024;
    char readbuf[bufsize] {};
    auto readCnt = inputFile.readsome(readbuf, bufsize);
    while (readCnt > 0) {
        if (EVP_DigestUpdate(context.get(), readbuf, readCnt) != 1) {
            setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestUpdate",
                           std::string("SHA-256 sum data adding failed: ") + ERR_error_string(ERR_get_error(), nullptr));
            return {};
        }
        readCnt = inputFile.readsome(readbuf, bufsize);
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    if (EVP_DigestFinal_ex(context.get(), hash, &lengthOfHash) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SHA-256] EVP_DigestFinal_ex",
                           std::string("SHA-256 sum finalize failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}



std::optional<std::string> argon2id(
    const std::string_view &password,
    const std::string_view &salt,
    const Argon2idParams &params)
{
    constexpr std::uint32_t MIN_MEMORY_KIB {8};
    constexpr std::uint32_t MAX_MEMORY_KIB {1024 * 1024};
    constexpr std::uint32_t MAX_ITERATIONS {64};
    constexpr std::uint32_t MAX_PARALLELISM {64};
    constexpr std::uint32_t MIN_HASH_LENGTH {4};
    constexpr std::uint32_t MAX_HASH_LENGTH {1024};

    if (password.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] password",
                           "Empty password");
        return {};
    }
    if (salt.size() < 8) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] salt",
                           "Salt is too short (at least 8 bytes are required)");
        return {};
    }
    if (params.memoryKiB < MIN_MEMORY_KIB ||
        params.memoryKiB > MAX_MEMORY_KIB ||
        params.memoryKiB < static_cast<std::uint64_t>(MIN_MEMORY_KIB) * params.parallelism ||
        params.iterations == 0 ||
        params.iterations > MAX_ITERATIONS ||
        params.parallelism == 0 ||
        params.parallelism > MAX_PARALLELISM ||
        params.hashLength < MIN_HASH_LENGTH ||
        params.hashLength > MAX_HASH_LENGTH) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] parameters",
                           "Invalid Argon2id cost parameters");
        return {};
    }

    std::unique_ptr<EVP_KDF, decltype(&EVP_KDF_free)> kdf(
        EVP_KDF_fetch(nullptr, "ARGON2ID", nullptr),
        &EVP_KDF_free);
    if (!kdf) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] EVP_KDF_fetch",
                           std::string("Argon2id is not available: ") +
                               ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::unique_ptr<EVP_KDF_CTX, decltype(&EVP_KDF_CTX_free)> ctx(
        EVP_KDF_CTX_new(kdf.get()),
        &EVP_KDF_CTX_free);
    if (!ctx) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] EVP_KDF_CTX_new",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::uint32_t memoryKiB = params.memoryKiB;
    std::uint32_t iterations = params.iterations;
    std::uint32_t parallelism = params.parallelism;
    std::uint32_t version = 0x13; // Argon2 v1.3

    OSSL_PARAM osslParameters[10];
    std::size_t paramIndex = 0;
    osslParameters[paramIndex++] = OSSL_PARAM_construct_octet_string(
        OSSL_KDF_PARAM_PASSWORD,
        const_cast<char*>(password.data()),
        password.size());
    osslParameters[paramIndex++] = OSSL_PARAM_construct_octet_string(
        OSSL_KDF_PARAM_SALT,
        const_cast<char*>(salt.data()),
        salt.size());
    osslParameters[paramIndex++] = OSSL_PARAM_construct_uint32(
        OSSL_KDF_PARAM_ARGON2_MEMCOST, &memoryKiB);
    osslParameters[paramIndex++] = OSSL_PARAM_construct_uint32(
        OSSL_KDF_PARAM_ITER, &iterations);
    osslParameters[paramIndex++] = OSSL_PARAM_construct_uint32(
        OSSL_KDF_PARAM_ARGON2_LANES, &parallelism);
    osslParameters[paramIndex++] = OSSL_PARAM_construct_uint32(
        OSSL_KDF_PARAM_ARGON2_VERSION, &version);

    if (!params.secret.empty()) {
        osslParameters[paramIndex++] = OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_SECRET,
            const_cast<char*>(params.secret.data()),
            params.secret.size());
    }
    if (!params.associatedData.empty()) {
        osslParameters[paramIndex++] = OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_ARGON2_AD,
            const_cast<char*>(params.associatedData.data()),
            params.associatedData.size());
    }
    osslParameters[paramIndex] = OSSL_PARAM_construct_end();

    std::string derivedKey;
    derivedKey.resize(params.hashLength);
    const auto deriveResult = EVP_KDF_derive(
        ctx.get(),
        reinterpret_cast<unsigned char*>(derivedKey.data()),
        derivedKey.size(),
        osslParameters);
    if (deriveResult <= 0) {
        secureClearMemory(derivedKey.data(), derivedKey.size());
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[Argon2id] EVP_KDF_derive",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    return derivedKey;
}

std::optional<std::string> hmacSha256(
    const std::string_view &key,
    const std::string_view &data)
{
    if (key.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[HMAC-SHA256] key",
                           "Empty key");
        return {};
    }
    if (key.size() > static_cast<std::size_t>(INT_MAX)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[HMAC-SHA256] key",
                           "Key is too long");
        return {};
    }

    unsigned char tag[EVP_MAX_MD_SIZE] {};
    unsigned int tagLength = 0;
    const auto result = HMAC(
        EVP_sha256(),
        key.data(),
        static_cast<int>(key.size()),
        reinterpret_cast<const unsigned char*>(data.data()),
        data.size(),
        tag,
        &tagLength);
    if (result == nullptr || tagLength == 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[HMAC-SHA256] HMAC",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::string output(reinterpret_cast<const char*>(tag), tagLength);
    secureClearMemory(tag, sizeof(tag));
    return output;
}

SensitiveDataHash hashSensitiveData(std::string &&sensitiveData)
{
    SafeString dat;
    return hashSensitiveData(dat);
}

SensitiveDataHash hashSensitiveData(const SafeString &sensitiveData)
{
    if (sensitiveData->empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SensitiveDataHash] Init",
                           "Empty input");
        return {};
    }

    const size_t SDH_SALT_SIZE = 16;
    const size_t SDH_HASH_SIZE = 32; // 256 bit
    const uint32_t SDH_ITERATION_COUNT = 600000; // OWASP recomendation

    // Setup KDF context
    std::unique_ptr<EVP_KDF, decltype(&EVP_KDF_free)> kdf(
        EVP_KDF_fetch(nullptr, "PBKDF2", nullptr),
        &EVP_KDF_free);
    if (!kdf) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SensitiveDataHash] Init",
                           std::string("KDF fetching failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    auto pCtx = std::shared_ptr<EVP_KDF_CTX>{
        EVP_KDF_CTX_new(kdf.get()),
        [](EVP_KDF_CTX* ctx){
            EVP_KDF_CTX_free(ctx);
        }
    };

    if (!pCtx) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SensitiveDataHash] Init",
                           std::string("KDF context setup failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Configure OpenSSL for hashing with PBKDF2
    auto salt = generateBytes_CSPRNG(SDH_SALT_SIZE);
    OSSL_PARAM osslParameters[] = {
        OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, const_cast<char*>("SHA256"), 0),
        OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, const_cast<char*>(sensitiveData->data()), sensitiveData->size()),
        OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, salt.data(), salt.size()),
        OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ITER, const_cast<uint32_t*>(&SDH_ITERATION_COUNT)),
        OSSL_PARAM_construct_end()
    };

    // Get key
    std::string derivedKey;
    derivedKey.resize(SDH_HASH_SIZE);
    auto deriveRes = EVP_KDF_derive(pCtx.get(), reinterpret_cast<uint8_t*>(derivedKey.data()), derivedKey.size(), osslParameters);
    if (deriveRes <= 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalHashFailed,
                           "[SensitiveDataHash] Derive",
                           std::string("PBKDF2 calc failed: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    return { std::move(salt), std::move(derivedKey) };
}

bool SensitiveDataHash::isValid() const {
    return !(hash.empty() | salt.empty());
}

}  // namespace Encryption
