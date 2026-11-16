#include "rsa.hpp"

#include <algorithm>
#include <filesystem>

#include <Components/Filework/Common.h>
#include <Components/Filework/TemporaryFile.h>
#include <Components/ExtraClasses/DataFragmentator.h>

#include "aes256.hpp"
#include "rsa_common.hpp"

namespace Encryption
{

RSAKeysPtr rsaGenerateKeys(RSA_SecurityLevel secLevel) {
    auto pCtx = rsaCreateKeygenContext();
    if (!pCtx) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalKeyGenerationFailed,
                           "[RSA-KEYGEN] EVP_PKEY_CTX_new_id",
                           "Failed to create EVP_PKEY_CTX");
        return {};
    }

    if (EVP_PKEY_keygen_init(pCtx.get()) <= 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalKeyGenerationFailed,
                           "[RSA-KEYGEN] EVP_PKEY_keygen_init",
                           "Failed to initialize keygen");
        return {};
    }

    uint bitCount {};
    switch (secLevel)
    {
    case RSA_SecurityLevel::SecLevel_standard:
        bitCount = 2048;
        break;
    case RSA_SecurityLevel::SecLevel_balanced:
        bitCount = 3072;
        break;
    case RSA_SecurityLevel::SecLevel_strong:
        bitCount = 4096;
        break;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pCtx.get(), bitCount) <= 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalKeyGenerationFailed,
                           "[RSA-KEYGEN] EVP_PKEY_CTX_set_rsa_keygen_bits",
                           "Failed to set key length");
        return {};
    }

    EVP_PKEY* key = nullptr;
    if (EVP_PKEY_keygen(pCtx.get(), &key) <= 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalKeyGenerationFailed,
                           "[RSA-KEYGEN] EVP_PKEY_keygen",
                           "Failed to generate key pair");
        return {};
    }

    return RSAKeysPtr(key, &EVP_PKEY_free);
}


// =========================================================== //


bool rsaSaveKey(const RSAKeysPtr& pkey,
                const std::string_view& filename,
                bool isPubkey,
                const std::string_view& passphrase) {
    auto fp = openFileC(filename, "wb");
    if (!fp) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-SAVEPUBK] file operation",
                           strerror(errno));
        return false;
    }

    int writeByteCount {};
    if (isPubkey) {
        writeByteCount = PEM_write_PUBKEY(fp.get(), pkey.get());
    } else if (passphrase.empty()) {
        writeByteCount = PEM_write_PKCS8PrivateKey(
            fp.get(),
            pkey.get(),
            nullptr, nullptr, 0,
            nullptr, nullptr
            );
    } else {
        writeByteCount = PEM_write_PKCS8PrivateKey(
            fp.get(),
            pkey.get(),
            EVP_aes_256_cbc(), passphrase.data(), passphrase.size(),
            nullptr, nullptr
            );
    }
    if (writeByteCount == 0) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-SAVE-KEY] file operation",
                           strerror(errno));
        return false;
    }
    return true;
}


RSAKeysPtr rsaReadKey(const std::string& filename,
                      bool isPubkey,
                      const std::string& passphrase) {
    auto fp = openFileC(filename);
    if (!fp) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-LOADPUBK] fopen",
                           strerror(errno));
        return nullptr;
    }

    EVP_PKEY* pkey {nullptr};
    if (isPubkey) {
        pkey = PEM_read_PUBKEY(fp.get(), nullptr, nullptr, nullptr);
    } else {
        KeyPassphrase passphraseData {passphrase.data(), passphrase.size()};
        pkey = PEM_read_PrivateKey(fp.get(), nullptr, f_keyReader, &passphraseData);
    }

    if (!pkey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-LOADPUBK] PEM_read_PUBKEY",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }
    return RSAKeysPtr(pkey, &EVP_PKEY_free);
}


// =========================================================== //


std::optional<std::string> rsaEncryptString(const RSAKeysPtr& publicKey, const std::string &plaintext) {
    if (plaintext.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA] input",
                           "Empty text to encrypt");
        return {};
    }

    const auto paddingType = RSA_PKCS1_OAEP_PADDING;
    const auto chunkLength = getDataChunkLength(publicKey, paddingType);
    if (!chunkLength) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-ENCS-SZ] chunk size",
                           std::string("Failed to determine RSA chunk size: ") + ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    // Split data into parts
    if (plaintext.size() > *chunkLength) {
        auto splittedData = ExtraClasses::DataInfo::split(plaintext, chunkLength.value());
        std::string resH;
        for (auto& pt : splittedData) {
            auto res = Encryption::rsaEncryptString(publicKey, pt);
            if (!res.has_value()) { return {}; }
            resH += *res;
        }
        return resH;
    }

    auto pCtx = rsaCreateContext(publicKey);
    if (!pCtx || EVP_PKEY_encrypt_init(pCtx.get()) <= 0) {
        return {};
    }

    // Set padding - OAEP padding is recommended for new applications :cite[1]
    if (EVP_PKEY_CTX_set_rsa_padding(pCtx.get(), paddingType) <= 0) {
        return {};
    }
    if (EVP_PKEY_CTX_set_rsa_oaep_md(pCtx.get(), EVP_sha256()) <= 0 ||
        EVP_PKEY_CTX_set_rsa_mgf1_md(pCtx.get(), EVP_sha256()) <= 0) {
        return {};
    }

    // Determine buffer size
    size_t ciphertext_len {0};
    if (EVP_PKEY_encrypt(pCtx.get(),
                         nullptr,
                         &ciphertext_len,
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()),
                         plaintext.size()) <= 0) {
        return {};
    }

    // Perform encryption
    std::string result;
    result.resize(ciphertext_len);
    if (EVP_PKEY_encrypt(
                pCtx.get(),
                reinterpret_cast<unsigned char*>(result.data()),
                &ciphertext_len,
                reinterpret_cast<const unsigned char*>(plaintext.c_str()),
                plaintext.size()) <= 0) {
        return {};
    }
    return result;
}

std::optional<std::string> rsaDecryptString(const RSAKeysPtr& privateKey, const std::string &ciphertext) {
    if (ciphertext.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA] input",
                           "Empty text to encrypt");
        return {};
    }

    // Split data into parts
    const auto keySize = EVP_PKEY_get_size(privateKey.get());
    if (keySize <= 0) { return {}; }
    if (ciphertext.size() > static_cast<std::size_t>(keySize)) {
        auto splittedData = ExtraClasses::DataInfo::split(
            ciphertext, static_cast<std::size_t>(keySize));
        std::string res;
        for (auto& pt : splittedData) {
            auto decPt = Encryption::rsaDecryptString(privateKey, pt);
            if (!decPt.has_value()) {
                return {};
            }
            res += *decPt;
        }
        return res;
    }

    auto pCtx = rsaCreateContext(privateKey);
    if (!pCtx || EVP_PKEY_decrypt_init(pCtx.get()) <= 0) {
        return {};
    }

    // Set padding - MUST match the padding used during encryption
    const auto paddingType = RSA_PKCS1_OAEP_PADDING;
    if (EVP_PKEY_CTX_set_rsa_padding(pCtx.get(), paddingType) <= 0) {
        return {};
    }
    if (EVP_PKEY_CTX_set_rsa_oaep_md(pCtx.get(), EVP_sha256()) <= 0 ||
        EVP_PKEY_CTX_set_rsa_mgf1_md(pCtx.get(), EVP_sha256()) <= 0) {
        return {};
    }

    // Determine buffer size
    size_t plaintext_len {0};
    if (EVP_PKEY_decrypt(
                pCtx.get(),
                nullptr,
                &plaintext_len,
                reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
                ciphertext.size()) <= 0) {
        return {};
    }

    // Perform decryption
    std::string result;
    result.resize(plaintext_len);
    if (EVP_PKEY_decrypt(
                pCtx.get(),
                reinterpret_cast<unsigned char*>(result.data()),
                &plaintext_len,
                reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
                ciphertext.size()) <= 0) {
        return {};
    }

    result.resize(plaintext_len); // Adjust to the actual decrypted size
    return result;
}

std::string rsaKeyToString(const RSAKeysPtr& pubKey, bool isPubkey, const std::string_view& passphrase)
{
    std::unique_ptr<BIO, decltype(&BIO_free)> bio(BIO_new(BIO_s_mem()), &BIO_free); // Create a memory BIO
    if (!bio) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[RSA-TOSTR] BIO_new",
                           "Failed to allocate memory BIO");
        return {};
    }
    char *pem_string = nullptr;

    int writeRes {};
    if (isPubkey) {
        writeRes = PEM_write_bio_PUBKEY(bio.get(), pubKey.get());
    } else if (passphrase.empty()) {
        writeRes = PEM_write_bio_PKCS8PrivateKey(
            bio.get(), pubKey.get(),
            nullptr, nullptr, 0,
            nullptr, nullptr
        );
    } else {
        writeRes = PEM_write_bio_PKCS8PrivateKey(
            bio.get(), pubKey.get(),
            EVP_aes_256_cbc(), passphrase.data(), passphrase.size(),
            nullptr, nullptr
        );
    }
    if (!writeRes) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[RSA-TOSTR]",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    long pem_length = BIO_get_mem_data(bio.get(), &pem_string);
    std::string res;
    res.reserve(pem_length);
    std::copy(pem_string, pem_string + pem_length, std::back_inserter(res));

    return res;
}

RSAKeysPtr rsaKeyFromString(const std::string& pubKey, bool isPubkey, const std::string_view& passphrase) {
    auto pBio = createBIO(pubKey);
    if (!pBio) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-FROMSTR]",
                           ERR_error_string(ERR_get_error(), nullptr));
        return nullptr;
    }

    EVP_PKEY* pkey {nullptr};
    if (isPubkey) {
        pkey = PEM_read_bio_PUBKEY(pBio.get(), nullptr, nullptr, nullptr);
    } else {
        KeyPassphrase passphraseData {passphrase.data(), passphrase.size()};
        pkey = PEM_read_bio_PrivateKey(
            pBio.get(), nullptr, f_keyReader,
            &passphraseData
            );
    }
    if (!pkey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[RSA-FROMSTR]",
                           ERR_error_string(ERR_get_error(), nullptr));
        return {};
    }

    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPtr(pkey, &EVP_PKEY_free);

    if (EVP_PKEY_base_id(keyPtr.get()) != EVP_PKEY_RSA) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-FROMSTR]",
                           ERR_error_string(ERR_get_error(), nullptr));
        return nullptr;
    }
    return RSAKeysPtr(keyPtr.release(), &EVP_PKEY_free);
}

bool rsaEncryptFile(const std::string &targetFile, const RSAKeysPtr &pubkey)
{    
    std::ifstream inputFile(targetFile, std::ios::binary);
    if (!inputFile) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] open",
                           std::string("Failed to open target file: ") + strerror(errno));
        return false;
    }
    Filework::TemporaryFile tmpFile(targetFile, false);

    const auto BUFF_SIZE {9128}; // Idk what value is best
    char buffer [BUFF_SIZE];
    while (inputFile) {
        auto readCnt = inputFile.readsome(buffer, BUFF_SIZE);
        if (readCnt <= 0) { break; }
        auto encData = rsaEncryptString(pubkey, std::string(buffer, readCnt));
        if (!encData.has_value()) { return false; }
        tmpFile.write(encData->c_str(), encData->size());
        tmpFile.flush();
    }

    if (inputFile.bad()) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] read",
                           "I/O error while reading file");
        return false;
    }

    tmpFile.accept();
    return true;
}

bool rsaEncryptFile(const std::string &targetFile, const std::string &pubkeyPath)
{
    if (!std::filesystem::exists(targetFile)) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] existence",
                           "Invalid file");
        return false;
    }
    auto rsaPublicKey = rsaReadKey(pubkeyPath);
    if (nullptr == rsaPublicKey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-FILE] key load",
                           "Failed to load public key");
        return false;
    }
    return rsaEncryptFile(targetFile, rsaPublicKey);
}

bool rsaDecryptFile(const std::string &targetFile, const RSAKeysPtr &privkey)
{
    std::ifstream inputFile(targetFile, std::ios::binary);
    if (!inputFile) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] open",
                           std::string("Failed to open target file: ") + strerror(errno));
        return false;
    }
    Filework::TemporaryFile tmpFile(targetFile, false);

    const auto BUFF_SIZE {10240}; // Idk what value is best
    char buffer [BUFF_SIZE];
    while (inputFile) {
        auto readCnt = inputFile.readsome(buffer, BUFF_SIZE);
        if (readCnt <= 0) { break; }
        auto encData = rsaDecryptString(privkey, std::string(buffer, readCnt));
        if (!encData.has_value()) { return false; }
        tmpFile.write(encData->c_str(), encData->size());
        tmpFile.flush();
    }

    if (inputFile.bad()) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] read",
                           "I/O error while reading file");
        return false;
    }

    tmpFile.accept();
    return true;
}

bool rsaDecryptFile(const std::string &targetFile, const std::string &privkeyPath, const std::string &pass)
{
    if (!std::filesystem::exists(targetFile)) {
        setEncryptionError(ExtraClasses::ErrorCode::SystemFileError,
                           "[RSA-FILE] existence",
                           "Invalid file");
        return false;
    }
    auto rsaPublicKey = rsaReadKey(privkeyPath, false, pass);
    if (nullptr == rsaPublicKey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-FILE] key load",
                           "Failed to load public key");
        return false;
    }
    return rsaDecryptFile(targetFile, rsaPublicKey);
}


// ========================================================= //
// ================= RSA hybrid encryption ================= //
// ========================================================= //

namespace {

class ScopedTempPath {
public:
    explicit ScopedTempPath(const std::string_view& targetFile)
        : m_path(std::filesystem::path(targetFile).parent_path() /
                 ("DataMaster_hybrid_" + generateSymbols(24) + ".tmp")) {}

    ~ScopedTempPath() {
        std::error_code ec;
        std::filesystem::remove(m_path, ec);
    }

    std::string string() const { return m_path.string(); }

private:
    std::filesystem::path m_path;
};

constexpr std::size_t HYBRID_KEY_SIZE  = 32;
constexpr std::size_t HYBRID_SALT_SIZE = 16;

}

ExtraClasses::ErrorObject rsaHybridEncryptFile(const std::string_view &targetFile, const std::string_view &aes256gcmKey, const RSAKeysPtr &pubkey)
{
    if (!pubkey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-HYBRID] public key",
                           "Public key is not set");
        return getEncryptionError();
    }

    if (aes256gcmKey.size() != HYBRID_KEY_SIZE) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-HYBRID] AES key",
                           "AES-256-GCM key size is invalid (expected 32 bytes)");
        return getEncryptionError();
    }

    const int rsaBlockSize = EVP_PKEY_get_size(pubkey.get());
    const auto rsaPayloadSize = getDataChunkLength(pubkey, RSA_PKCS1_OAEP_PADDING);
    if (rsaBlockSize <= 0 || !rsaPayloadSize.has_value() ||
        rsaPayloadSize.value() < HYBRID_KEY_SIZE + HYBRID_SALT_SIZE) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[RSA-HYBRID] header size",
                           "RSA key is too small for hybrid header");
        return getEncryptionError();
    }

    std::string plaintext;
    auto readResult = Filework::Common::readFileData(targetFile, plaintext);
    if (!readResult.isOk()) { return readResult; }

    const std::string salt = generateBytes_CSPRNG(HYBRID_SALT_SIZE);

    std::string header;
    header.reserve(rsaPayloadSize.value());
    header.append(aes256gcmKey.data(), HYBRID_KEY_SIZE);
    header.append(salt);

    const auto paddingSize = rsaPayloadSize.value() - header.size();
    if (paddingSize != 0) {
        header.append(generateBytes_CSPRNG(paddingSize));
    }

    auto rsaBlock = rsaEncryptString(pubkey, header);
    if (!rsaBlock.has_value() ||
        rsaBlock->size() != static_cast<std::size_t>(rsaBlockSize)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncryptionFailed,
                           "[RSA-HYBRID] header encrypt",
                           "Failed to encrypt hybrid header");
        return getEncryptionError();
    }

    ScopedTempPath bodyFile(targetFile);
    auto writeBodyResult = Filework::Common::saveFileData(bodyFile.string(), plaintext);
    if (!writeBodyResult.isOk()) { return writeBodyResult; }

    auto bodyEncryptResult = aes256_gcm_encryptFile(bodyFile.string(), aes256gcmKey, salt);
    if (!bodyEncryptResult.isOk()) { return bodyEncryptResult; }

    std::string encryptedBody;
    auto readBodyResult = Filework::Common::readFileData(bodyFile.string(), encryptedBody);
    if (!readBodyResult.isOk()) { return readBodyResult; }

    std::string output;
    output.reserve(rsaBlock->size() + encryptedBody.size());
    output.append(rsaBlock.value());
    output.append(encryptedBody);

    return Filework::Common::saveFileData(targetFile, output);
}

ExtraClasses::ErrorObject rsaHybridDecryptFile(const std::string_view &targetFile, const std::string_view &aes256gcmKey, const RSAKeysPtr &privkey)
{
    if (!privkey) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-HYBRID] private key",
                           "Private key is not set");
        return getEncryptionError();
    }

    if (aes256gcmKey.size() != HYBRID_KEY_SIZE) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-HYBRID] AES key",
                           "AES-256-GCM key size is invalid (expected 32 bytes)");
        return getEncryptionError();
    }

    const int rsaBlockSize = EVP_PKEY_get_size(privkey.get());
    if (rsaBlockSize <= 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[RSA-HYBRID] block size",
                           "Failed to determine RSA block size");
        return getEncryptionError();
    }

    std::string input;
    auto readResult = Filework::Common::readFileData(targetFile, input);
    if (!readResult.isOk()) { return readResult; }

    if (input.size() <= static_cast<std::size_t>(rsaBlockSize)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[RSA-HYBRID] input",
                           "File is too short for hybrid header");
        return getEncryptionError();
    }

    const std::string rsaBlock = input.substr(0, static_cast<std::size_t>(rsaBlockSize));
    auto header = rsaDecryptString(privkey, rsaBlock);
    if (!header.has_value() ||
        header->size() < HYBRID_KEY_SIZE + HYBRID_SALT_SIZE) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[RSA-HYBRID] header decrypt",
                           "Failed to decrypt hybrid header");
        return getEncryptionError();
    }

    const std::string recoveredKey = header->substr(0, HYBRID_KEY_SIZE);
    const std::string salt = header->substr(HYBRID_KEY_SIZE, HYBRID_SALT_SIZE);

    if (recoveredKey != std::string(aes256gcmKey)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalDecryptionFailed,
                           "[RSA-HYBRID] AES key",
                           "AES key does not match the encrypted header");
        return getEncryptionError();
    }

    const std::string encryptedBody = input.substr(static_cast<std::size_t>(rsaBlockSize));

    ScopedTempPath bodyFile(targetFile);
    auto writeBodyResult = Filework::Common::saveFileData(bodyFile.string(), encryptedBody);
    if (!writeBodyResult.isOk()) { return writeBodyResult; }

    auto bodyDecryptResult = aes256_gcm_decryptFile(bodyFile.string(), recoveredKey, salt);
    if (!bodyDecryptResult.isOk()) { return bodyDecryptResult; }

    std::string plaintext;
    auto readBodyResult = Filework::Common::readFileData(bodyFile.string(), plaintext);
    if (!readBodyResult.isOk()) { return readBodyResult; }

    return Filework::Common::saveFileData(targetFile, plaintext);
}

}
