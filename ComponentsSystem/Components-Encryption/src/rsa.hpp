#pragma once

/// **************************************************************************** ///
/// *********************** Rivest-Shamir-Adleman (RSA) ************************ ///
/// **************************************************************************** ///

#include <string>
#include <memory>
#include <optional>

#include "common.hpp"

struct rsa_st;
using RSA = rsa_st;

typedef struct evp_pkey_st EVP_PKEY; // For OpenSSL RSA

namespace Encryption
{

using RSAKeysPtr = std::shared_ptr<EVP_PKEY>; // Shared just to not show delete implementation

// Key generating
enum class RSA_SecurityLevel : uint8_t
{
    SecLevel_standard = 0,
    SecLevel_balanced,
    SecLevel_strong,
};
RSAKeysPtr rsaGenerateKeys(RSA_SecurityLevel secLevel = RSA_SecurityLevel::SecLevel_standard);

// Save key using BIO
bool rsaSaveKey(const RSAKeysPtr& pkey,
                const std::string_view& filename,
                bool isPubkey = true,
                const std::string_view& passphrase = {});

// Read key using BIO
RSAKeysPtr rsaReadKey(const std::string& filename,
                      bool isPubkey = true,
                      const std::string& passphrase = {});

// Encryption
std::optional<std::string> rsaEncryptString(const RSAKeysPtr& publicKey, const std::string& plaintext);
std::optional<std::string> rsaDecryptString(const RSAKeysPtr& privateKey, const std::string& ciphertext);

// PEM format only (empty string mean error)
std::string rsaKeyToString(const RSAKeysPtr& pubKey,
                           bool isPubkey = true,
                           const std::string_view& passphrase = {});

RSAKeysPtr rsaKeyFromString(const std::string& pubKey,
                            bool isPubkey = true,
                            const std::string_view& passphrase = {});

// File encryption
bool rsaEncryptFile(const std::string& targetFile, const RSAKeysPtr& pubkey);
bool rsaEncryptFile(const std::string& targetFile, const std::string& pubkeyPath);

// File decryption
bool rsaDecryptFile(const std::string& targetFile, const RSAKeysPtr& privkey);
bool rsaDecryptFile(const std::string& targetFile, const std::string& privkeyPath, const std::string& pass = {});

/**
 * @brief rsaHybridEncryptFile  Hybrid file encryption (first block with AES-256-GCM key, encrypted by RSA)
 * @param targetFile            File to encrypt
 * @param aes256gcmKey          32 byte AES-256-GCM key (configured before function call)
 * @param privkey               Private RSA key to encrypt AES-256-GCM key
 * @return
 */
ExtraClasses::ErrorObject rsaHybridEncryptFile(
    const std::string_view& targetFile,
    const std::string_view& aes256gcmKey,
    const RSAKeysPtr& privkey);

/**
 * @brief rsaHybridDecryptFile  Hybrid file decryption (first block with AES-256-GCM key, encrypted by RSA)
 * @param targetFile            File to decrypt
 * @param aes256gcmKey          32 byte AES-256-GCM key (configured before function call)
 * @param privkey               Private RSA key to encrypt AES-256-GCM key
 * @return
 */
ExtraClasses::ErrorObject rsaHybridDecryptFile(
    const std::string_view& targetFile,
    const std::string_view& aes256gcmKey,
    const RSAKeysPtr& privkey);

}

