#include "encryption_aes.h"

#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <boost/algorithm/hex.hpp>

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

namespace Encryption
{
std::string global_encryptionErrorText;

std::string sha256(const std::string &input)
{
    if (input.empty())
    {
        return "";
    }

    // Create a buffer to hold the hash
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Compute the SHA-256 hash
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(),
           hash);

    // Convert the hash to a hexadecimal string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::setw(2) << std::setfill('0') << std::hex
           << static_cast<int>(hash[i]);
    }
    return ss.str();

    //    EVP_MD_CTX *mdctx = EVP_MD_CTX_new(); // Create a new context
    //    const EVP_MD *md = EVP_sha256(); // Get the SHA-256 algorithm
    //    std::vector<unsigned char> hash(EVP_MD_size(md)); // Prepare a vector
    //    to hold the hash

    //    if (mdctx == nullptr || md == nullptr ||
    //        EVP_DigestInit_ex(mdctx, md, nullptr) != 1 || // Initialize the
    //        digest EVP_DigestUpdate(mdctx, input.c_str(), input.size()) != 1
    //        ||
    //        // Update the digest with the input EVP_DigestFinal_ex(mdctx,
    //        hash.data(), nullptr) != 1) { // Finalize the digest

    //        // Handle errors here (e.g. throw an exception or return an error
    //        code) EVP_MD_CTX_free(mdctx); COMPLOG_ERROR("Failed to compute hash");
    //        return "";
    //    }

    //    EVP_MD_CTX_free(mdctx); // Clean up the context

    //    // Convert the hash to a hex string
    //    std::stringstream ss;
    //    for (unsigned char byte : hash) {
    //        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    //    }

    //    return ss.str();
}

bool aes256encryptHelper(const std::string& plaintext,
                                       const std::string& key,
                                       const std::string& iv,
                                       std::string& ciphertext)
{
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(),
                           (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    const int plaintextLength = plaintext.length();
    const int maxCiphertextLength =
        plaintextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* ciphertextBytes = new unsigned char[maxCiphertextLength];
    int ciphertextLength           = 0;
    if (EVP_EncryptUpdate(ctx, ciphertextBytes, &ciphertextLength,
                          (const unsigned char*)plaintext.c_str(),
                          plaintextLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertextBytes + ciphertextLength,
                            &ciphertextFinalLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    ciphertextLength += ciphertextFinalLength;
    ciphertext.assign((const char*)ciphertextBytes, ciphertextLength);
    delete[] ciphertextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

bool aes256decryptHelper(std::string& ciphertext,
                                       const std::string& key,
                                       const std::string& iv,
                                       std::string& plaintext)
{
    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Дешифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        return false;
    }
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(),
                           (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    const int ciphertextLength = ciphertext.length();
    const int maxPlaintextLength =
        ciphertextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* plaintextBytes = new unsigned char[maxPlaintextLength];
    int plaintextLength           = 0;
    if (EVP_DecryptUpdate(ctx, plaintextBytes, &plaintextLength,
                          (const unsigned char*)ciphertext.c_str(),
                          ciphertextLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintextBytes + plaintextLength,
                            &plaintextFinalLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        global_encryptionErrorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    plaintextLength += plaintextFinalLength;
    plaintext.assign((const char*)plaintextBytes, plaintextLength);
    delete[] plaintextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

std::string aes256encrypt(const std::string &input, const std::string &key)
{
    std::string encryptedPacket;
    std::string iv = generateKey(key.size());

    if (aes256encryptHelper(input, key, iv, encryptedPacket)) {
        std::string output = iv;
        output.append(encryptedPacket);
        return output;
    }
    return "";
}

std::string aes256decrypt(const std::string &input, const std::string &key)
{
    std::string encryptedPacket = input;

    std::string iv = encryptedPacket.substr(0, key.size());
    encryptedPacket.erase(0, key.size());

    std::string output;
    if (aes256decryptHelper(encryptedPacket, key, iv, output)) {
        return output;
    }
    return "";
}

std::string generateKey(size_t lengthByte)
{
    std::string result;
    char test_symbol;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, 255);

    for (int8_t i = 0; result.size() < lengthByte; i++)
    {
        test_symbol = uni(rng);
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }

    return result;
}

std::string encodeBase64(const std::string &input)
{
    BIO *bio, *b64;
    BUF_MEM* bufferPtr;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string output(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return output;
}

std::string decodeBase64(const std::string &input)
{
    BIO *bio, *b64;
    char* buffer = new char[input.size()];
    memset(buffer, 0, input.size());

    bio = BIO_new_mem_buf(input.c_str(), static_cast<int>(input.length()));
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int len = BIO_read(bio, buffer, static_cast<int>(input.size()));

    BIO_free_all(bio);

    std::string output(buffer, len);
    delete[] buffer;

    return output;
}

std::string getEncryptionErrorText()
{
    return global_encryptionErrorText;
}

std::string encodeHex(const std::string& input)
{
    std::string convertedStr;
    convertedStr.reserve(input.size());
    boost::algorithm::hex(input.begin(), input.end(), std::back_inserter(convertedStr));
    return convertedStr;
}

std::string decodeHex(const std::string& input)
{
    std::string convertedStr;
    convertedStr.reserve(input.size());
    boost::algorithm::unhex(input, std::back_inserter(convertedStr));
    return convertedStr;
}

}
