#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>

#include "common.hpp"

namespace Encryption {

/**
 * @brief The Argon2idParams struct Parameters of the Argon2id KDF
 * @note  memoryKiB is the memory cost in 1 KiB blocks; iterations is the time cost
 */
struct Argon2idParams
{
    std::uint32_t memoryKiB    {64 * 1024};
    std::uint32_t iterations   {3};
    std::uint32_t parallelism  {1};
    std::uint32_t hashLength   {32};
    std::string   secret;          ///< Optional Argon2 secret / pepper
    std::string   associatedData;  ///< Optional associated data (domain separation)
};

/**
 * @brief argon2id Derive a key using Argon2id (RFC 9106)
 * @param password  User-supplied password material
 * @param salt      Per-derivation random salt (16+ bytes recommended)
 * @param params    Argon2id cost parameters
 * @return          Raw derived key or std::nullopt on error
 */
std::optional<std::string> argon2id(
    const std::string_view& password,
    const std::string_view& salt,
    const Argon2idParams& params = Argon2idParams {});

/**
 * @brief hmacSha256 Compute HMAC-SHA256 and return the raw 32-byte tag
 * @param key       HMAC key
 * @param data      Data to authenticate
 * @return          Raw tag or std::nullopt on error
 */
std::optional<std::string> hmacSha256(
    const std::string_view& key,
    const std::string_view& data);

/**
 * @brief sha256    Вычислить хеш по функции SHA-256
 * @param input     Входной массив байт (или текст)
 * @return          Хеш (64 байт)
 */
std::optional<std::string> sha256(const std::string_view& input);

/**
 * @brief sha256file    Вычислить хеш данных файла
 * @param filepath
 * @return
 */
std::optional<std::string> sha256file(const std::string_view& filepath);

/**
 * @brief The SensitiveDataHash class Comparable object for sensitive data hashing
 */
struct SensitiveDataHash {
    std::string salt;
    std::string hash;

    bool isValid() const;
};

/**
 * @brief hashSensitiveData Create hash using sensitive data, using PBKDF2-HMAC-SHA256 algorithm
 * @param sensitiveData
 * @return                  Comparable object
 */
SensitiveDataHash hashSensitiveData(std::string&& sensitiveData);
SensitiveDataHash hashSensitiveData(const SafeString& sensitiveData);

}  // namespace Encryption
