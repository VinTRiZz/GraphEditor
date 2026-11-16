#include <gtest/gtest.h>

#ifdef COMPONENTS_SW_encryption_rsa

#include <Components/Encryption/RSA.h>
#include <Components/Filework/Common.h>
#include <Components/Filework/TemporaryFile.h>

#include <filesystem>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/pem.h>

namespace {

// RSA-OAEP uses SHA-256 for both the digest and MGF1 in this component.
constexpr int OAEP_SHA256_DIGEST_SIZE {32};
constexpr int OAEP_SHA256_OVERHEAD {2 * OAEP_SHA256_DIGEST_SIZE + 2};

std::size_t getRsaKeySize(const Encryption::RSAKeysPtr& key) {
    const int keySize = EVP_PKEY_get_size(key.get());
    EXPECT_GT(keySize, 0);
    return static_cast<std::size_t>(keySize);
}

std::size_t getOaepSha256ChunkSize(const Encryption::RSAKeysPtr& key) {
    const auto keySize = getRsaKeySize(key);
    EXPECT_GT(keySize, static_cast<std::size_t>(OAEP_SHA256_OVERHEAD));
    return keySize - static_cast<std::size_t>(OAEP_SHA256_OVERHEAD);
}

}

TEST(Components_Encryption, RSA_Init) {
    auto key = Encryption::rsaGenerateKeys();
    ASSERT_NE(key, nullptr);
}

TEST(Components_Encryption, RSA_StringSizes) {
    auto key = Encryption::rsaGenerateKeys();

    // Small strings
    auto sample = Encryption::generateBytes(5);
    auto encStr = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encStr.has_value());
    auto decStr = Encryption::rsaDecryptString(key, *encStr);
    ASSERT_FALSE(encStr->empty());
    ASSERT_NE(sample, encStr);
    ASSERT_EQ(sample, decStr);

    // Zero strings
    encStr = Encryption::rsaEncryptString(key, {});
    ASSERT_FALSE(encStr.has_value());
    decStr = Encryption::rsaDecryptString(key, {});
    ASSERT_FALSE(encStr.has_value());

    // Regular strings
    sample = Encryption::generateBytes(79);
    encStr = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encStr.has_value());
    decStr = Encryption::rsaDecryptString(key, *encStr);
    ASSERT_FALSE(encStr->empty());
    ASSERT_NE(sample, encStr);
    ASSERT_EQ(sample, decStr);

    // Chunk-size strings
    sample = Encryption::generateBytes(256);
    encStr = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encStr.has_value());
    decStr = Encryption::rsaDecryptString(key, *encStr);
    ASSERT_FALSE(encStr->empty());
    ASSERT_NE(sample, encStr);
    ASSERT_EQ(sample, decStr);

    // Big unaligned strings
    sample = Encryption::generateBytes(1733);
    encStr = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encStr.has_value());
    decStr = Encryption::rsaDecryptString(key, *encStr);
    ASSERT_FALSE(encStr->empty());
    ASSERT_NE(sample, encStr);
    ASSERT_EQ(sample, decStr);

    // Big strings
    sample = Encryption::generateBytes(2048);
    encStr = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encStr.has_value());
    decStr = Encryption::rsaDecryptString(key, *encStr);
    ASSERT_FALSE(encStr->empty());
    ASSERT_NE(sample, encStr);
    ASSERT_EQ(sample, decStr);
}

TEST(Components_Encryption, RSA_OAEP_SHA256_ChunkBoundaries) {
    auto key = Encryption::rsaGenerateKeys();
    const auto keySize = getRsaKeySize(key);
    const auto chunkSize = getOaepSha256ChunkSize(key);

    const std::vector<std::size_t> sizes {
        1,
        chunkSize - 1,
        chunkSize,
        chunkSize + 1,
        2 * chunkSize,
        2 * chunkSize + 1,
        3 * chunkSize + 7
    };

    for (const auto size : sizes) {
        const auto sample = Encryption::generateBytes(size);
        const auto encrypted = Encryption::rsaEncryptString(key, sample);
        ASSERT_TRUE(encrypted.has_value())
            << "Size: " << size << ", error: " << Encryption::getEncryptionErrorText();

        const auto expectedBlocks = (size + chunkSize - 1) / chunkSize;
        EXPECT_EQ(keySize * expectedBlocks, encrypted->size())
            << "Size: " << size << " must be split into " << expectedBlocks << " RSA blocks";

        const auto decrypted = Encryption::rsaDecryptString(key, encrypted.value());
        ASSERT_TRUE(decrypted.has_value())
            << "Size: " << size << ", error: " << Encryption::getEncryptionErrorText();
        EXPECT_EQ(sample, decrypted.value()) << "Size: " << size;
    }
}

TEST(Components_Encryption, RSA_OAEP_SHA256_RejectsInvalidCiphertext) {
    auto key = Encryption::rsaGenerateKeys();
    auto otherKey = Encryption::rsaGenerateKeys();
    const auto sample = Encryption::generateBytes(64);

    const auto encrypted = Encryption::rsaEncryptString(key, sample);
    ASSERT_TRUE(encrypted.has_value()) << Encryption::getEncryptionErrorText();

    EXPECT_FALSE(Encryption::rsaDecryptString(otherKey, encrypted.value()).has_value())
        << "A different private key must not decrypt the ciphertext";

    auto tampered = encrypted.value();
    tampered[tampered.size() / 2] ^= static_cast<char>(0x01);
    EXPECT_FALSE(Encryption::rsaDecryptString(key, tampered).has_value())
        << "A tampered ciphertext must be rejected";
}

TEST(Components_Encryption, RSA_KeySaveRestore) {
    auto key = Encryption::rsaGenerateKeys();

    // Regular save test
    Filework::TemporaryFile privFreeTmp;
    ASSERT_TRUE(Encryption::rsaSaveKey(key, std::string(privFreeTmp.getFilename()), false));
    ASSERT_TRUE(std::filesystem::exists(privFreeTmp.getFilename()) &&
                std::filesystem::file_size(privFreeTmp.getFilename()) != 0);

    // Encrypted save test
    const std::string encryptionPassphrase = Encryption::generateSymbols(100);
    Filework::TemporaryFile privEncrTmp;
    ASSERT_TRUE(Encryption::rsaSaveKey(key, std::string(privEncrTmp.getFilename()), false, encryptionPassphrase));
    ASSERT_TRUE(std::filesystem::exists(privEncrTmp.getFilename()) &&
                std::filesystem::file_size(privEncrTmp.getFilename()) != 0);

    // Check encryption is invalid
    std::string keyData;
    ASSERT_TRUE(Filework::Common::readFileData(std::string(privFreeTmp.getFilename()), keyData));
    std::string encKeyData;
    ASSERT_TRUE(Filework::Common::readFileData(std::string(privEncrTmp.getFilename()), encKeyData));
    ASSERT_NE(keyData, encKeyData);

    // Public save test
    Filework::TemporaryFile pubTmp;
    ASSERT_TRUE(Encryption::rsaSaveKey(key, std::string(pubTmp.getFilename())));
    ASSERT_TRUE(std::filesystem::exists(privFreeTmp.getFilename()) &&
                std::filesystem::file_size(privFreeTmp.getFilename()) != 0);

    // Reading keys
    auto pubKey = Encryption::rsaReadKey(std::string(pubTmp.getFilename()));
    ASSERT_NE(pubKey, nullptr);

    auto privKey = Encryption::rsaReadKey(std::string(privFreeTmp.getFilename()), false);
    ASSERT_NE(privKey, nullptr);

    auto privEncKey = Encryption::rsaReadKey(std::string(privEncrTmp.getFilename()), false, encryptionPassphrase);
    ASSERT_NE(privEncKey, nullptr);

    // Check if read keys are correct
    ASSERT_TRUE(EVP_PKEY_eq(key.get(), privKey.get())) << "Loaded private key is invalid";
    ASSERT_TRUE(EVP_PKEY_eq(pubKey.get(), privKey.get())) << "Loaded public key is invalid";
    ASSERT_TRUE(EVP_PKEY_eq(key.get(), privEncKey.get())) << "Loaded encrypted key is invalid";
}

TEST(Components_Encryption, RSA_FileEncrypt) {
    auto key = Encryption::rsaGenerateKeys();

    std::string testData;
    const auto BUFF_SIZE = 7641; // Must be enough for test
    testData = Encryption::generateBytes(BUFF_SIZE);

    Filework::TemporaryFile srcFile;
    srcFile.write(testData.c_str(), testData.size());
    srcFile.flush();
    srcFile.accept();

    const std::string path(srcFile.getFilename());

    // Encrypt file data
    ASSERT_TRUE(Encryption::rsaEncryptFile(path, key)) << "ERROR TEXT: " << Encryption::getEncryptionErrorText();

    // Read what encrypted
    std::string encryptedData;
    ASSERT_TRUE(Filework::Common::readFileData(path, encryptedData));
    ASSERT_GT(encryptedData.size(), testData.size()); // Must be 256*N, more that source
    ASSERT_NE(testData, encryptedData);

    // Check encrypting of file
    ASSERT_TRUE(Encryption::rsaDecryptFile(path, key)) << "ERROR TEXT: " << Encryption::getEncryptionErrorText();

    // Read what decrypted
    std::string decryptedData;
    ASSERT_TRUE(Filework::Common::readFileData(path, decryptedData));
    ASSERT_EQ(testData, decryptedData);
}


TEST(Components_Encryption, RSA_FileHybridEncrypt) {
    auto key = Encryption::rsaGenerateKeys();
    auto aesKey = Encryption::generateBytes(32);

    std::string testData;
    const auto BUFF_SIZE = 7641; // Must be enough for test
    testData = Encryption::generateBytes(BUFF_SIZE);

    Filework::TemporaryFile srcFile;
    srcFile.write(testData.c_str(), testData.size());
    srcFile.flush();
    srcFile.accept();

    const std::string path(srcFile.getFilename());

    // Encrypt file data
    ASSERT_TRUE(Encryption::rsaHybridEncryptFile(path, aesKey, key)) << "ERROR TEXT: " << Encryption::getEncryptionErrorText();

    // Read what encrypted
    std::string encryptedData;
    ASSERT_TRUE(Filework::Common::readFileData(path, encryptedData));
    ASSERT_GT(encryptedData.size(), testData.size()); // Must be 256*N, more that source
    ASSERT_NE(testData, encryptedData);

    ASSERT_FALSE(Encryption::rsaHybridDecryptFile(path, Encryption::generateBytes(32), key).isOk());
    std::string stillEncrypted;
    ASSERT_TRUE(Filework::Common::readFileData(path, stillEncrypted));
    ASSERT_EQ(encryptedData, stillEncrypted);

    // Check encrypting of file
    ASSERT_TRUE(Encryption::rsaHybridDecryptFile(path, aesKey, key)) << "ERROR TEXT: " << Encryption::getEncryptionErrorText();

    // Read what decrypted
    std::string decryptedData;
    ASSERT_TRUE(Filework::Common::readFileData(path, decryptedData));
    ASSERT_EQ(testData, decryptedData);
}

#endif // COMPONENTS_SW_encryption_rsa
