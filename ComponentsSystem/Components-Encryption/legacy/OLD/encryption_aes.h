#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

namespace Encryption
{

std::string sha256(const std::string& input);

std::string generateKey(size_t lengthByte);
std::string aes256encrypt(const std::string& input, const std::string& key);
std::string aes256decrypt(const std::string& input, const std::string& key);

std::string encodeBase64(const std::string& input);
std::string decodeBase64(const std::string& input);

std::string encodeHex(const std::string& input);
std::string decodeHex(const std::string& input);


std::string getEncryptionErrorText();

}

#endif // ENCRYPTION_H
