#include "encryptor.hpp"

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>

// RSA
#include <openssl/rsa.h>

// ChaCha20
// #define ENCRYPTION_CHA_CHA_20 // TODO: Add Libsodium define detection

#ifdef ENCRYPTION_CHA_CHA_20
#include <sodium.h>
#endif // ENCRYPTION_CHA_CHA_20

#include <cstring>
#include <fstream>
#include <string.h>
#include <vector>

#include <algorithm>

struct Libraries::Encryptor::Impl {
    std::string m_errorText;
};

Libraries::Encryptor::Encryptor() : d{new Impl()} {}

Libraries::Encryptor::~Encryptor()
{
    OPENSSL_cleanup();
}

void Libraries::Encryptor::init(ENCRYPTION_METHOD method, bool enableBase64,
                                 bool generateKeys, size_t keySize)
{
    // Инициализируем OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    m_config.encryptMethod  = method;
    m_config.base64_enabled = enableBase64;
    m_config.keySize        = keySize;

    if (generateKeys)
    {
        if (m_config.encryptMethod == ENCRYPTION_METHOD::AES_256)
            m_config.encryptionKey = generateKey(m_config.keySize);
    }
}

bool Libraries::Encryptor::setKeyAES(const std::string& keyString)
{
    m_config.encryptionKey = keyString;
    return true;
}

bool Libraries::Encryptor::encryptAES(const std::string& input,
                                       std::string& output)
{
    if (m_config.encryptMethod != ENCRYPTION_METHOD::AES_256)
    {
        return false;
    }
    std::string encryptedPacket;
    std::string iv = generateKey(m_config.encryptionKey.size());

    bool enc_res =
        aesEncrypt(input, m_config.encryptionKey, iv, encryptedPacket);

    output = iv;
    output.append(encryptedPacket);

    if (m_config.base64_enabled) output = base64_encode(output);

    return enc_res;
}

bool Libraries::Encryptor::decryptAES(const std::string& input,
                                       std::string& output)
{
    if (m_config.encryptMethod != ENCRYPTION_METHOD::AES_256)
    {
        return false;
    }

    std::string encryptedPacket;
    if (m_config.base64_enabled)
        encryptedPacket = base64_decode(input);
    else
        encryptedPacket = input;

    std::string iv = encryptedPacket.substr(0, m_config.encryptionKey.size());
    encryptedPacket.erase(0, m_config.encryptionKey.size());

    return aesDecrypt(encryptedPacket, m_config.encryptionKey, iv, output);
}

bool Libraries::Encryptor::encryptChaCha20(const std::string& plaintext,
                                            const std::string& iv,
                                            std::string& encryptedText)
{
#ifdef ENCRYPTION_CHA_CHA_20
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        d->m_errorText = "Invalid encryption key size";
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        d->m_errorText = "Invalid init vector size";
        return false;
    }

    encryptedText.resize(plaintext.size());
    if (crypto_stream_chacha20_xor(
            (unsigned char*)encryptedText.data(),
            (const unsigned char*)plaintext.data(), plaintext.size(),
            (const unsigned char*)iv.data(),
            (const unsigned char*)m_config.encryptionKey.data()) != 0)
    {
        d->m_errorText =
            "Error encrypting"; // TODO: Write correctly getting error text
        return false;
    }

    return true;
#else
    encryptedText  = iv + plaintext;     // To avoid warnings
    d->m_errorText = "Libsodium not installed, encryption not provided";
    return false;
#endif // ENCRYPTION_CHA_CHA_20
}

bool Libraries::Encryptor::decryptChaCha20(const std::string& encryptedText,
                                            const std::string& iv,
                                            std::string& decryptedText)
{
#ifdef ENCRYPTION_CHA_CHA_20
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        d->m_errorText = "Invalid encryption key size";
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        d->m_errorText = "Invalid init vector size";
        return false;
    }

    decryptedText.resize(encryptedText.size());
    if (crypto_stream_chacha20_xor(
            (unsigned char*)decryptedText.data(),
            (const unsigned char*)encryptedText.data(), encryptedText.size(),
            (const unsigned char*)iv.data(),
            (const unsigned char*)m_config.encryptionKey.data()) != 0)
    {
        d->m_errorText =
            "Error encrypting"; // TODO: Write correctly getting error text
        return false;
    }
    return true;
#else
    decryptedText  = iv + encryptedText; // To avoid warnings
    d->m_errorText = "Libsodium not installed, encryption not provided";
    return false;
#endif // ENCRYPTION_CHA_CHA_20
}

std::string Libraries::Encryptor::errorText() const
{
    return d->m_errorText;
}

Libraries::Encryptor::ENCRYPTION_METHOD
Libraries::Encryptor::encryptMethod() const
{
    return m_config.encryptMethod;
}

std::string Libraries::Encryptor::generateKey(size_t keySize)
{
    std::string result;
    char test_symbol;

    for (int8_t i = 0; result.size() < keySize; i++)
    {
        // TODO: std::mt19937
        test_symbol = rand() % 255;
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }
    return result;
}

bool Libraries::Encryptor::aesEncrypt(const std::string& plaintext,
                                       const std::string& key,
                                       const std::string& iv,
                                       std::string& ciphertext)
{
    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Шифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(),
                           (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertextBytes + ciphertextLength,
                            &ciphertextFinalLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    ciphertextLength += ciphertextFinalLength;
    ciphertext.assign((const char*)ciphertextBytes, ciphertextLength);
    delete[] ciphertextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

bool Libraries::Encryptor::aesDecrypt(std::string& ciphertext,
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintextBytes + plaintextLength,
                            &plaintextFinalLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    plaintextLength += plaintextFinalLength;
    plaintext.assign((const char*)plaintextBytes, plaintextLength);
    delete[] plaintextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

std::string Libraries::Encryptor::base64_encode(const std::string& input)
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

std::string Libraries::Encryptor::base64_decode(const std::string& input)
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
