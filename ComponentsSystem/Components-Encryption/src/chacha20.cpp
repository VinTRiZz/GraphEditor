#include "chacha20.hpp"

#define CHACHA20_KEY_SIZE 32
#define CHACHA20_IV_SIZE 8

#include <sodium.h>

namespace Encryption
{

std::string generateKeyChaCha20(const size_t keySize)
{
    std::string result;
    char test_symbol;

    for (int8_t i = 0; result.size() < keySize; i++)
    {
        test_symbol = rand() % 255;
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }

    return result;
}

bool chacha20_encrypt(const std::string & plaintext, const std::string & key, const std::string & iv, std::string & encryptedText)
{
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        return false;
    }

    encryptedText.resize(plaintext.size());
    if (crypto_stream_chacha20_xor((unsigned char*)encryptedText.data(),
                                   (const unsigned char*)plaintext.data(),
                                   plaintext.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)key.data()) != 0)
    {
        return false;
    }

    return true;
}


bool chacha20_decrypt(const std::string & encryptedText, const std::string & key, const std::string & iv, std::string & decryptedText)
{
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        return false;
    }

    decryptedText.resize(encryptedText.size());
    if (crypto_stream_chacha20_xor((unsigned char*)decryptedText.data(),
                                   (const unsigned char*)encryptedText.data(),
                                   encryptedText.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)key.data()) != 0)
    {
        return false;
    }
    return true;
}

}
