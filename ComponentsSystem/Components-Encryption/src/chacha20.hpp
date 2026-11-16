#pragma once

/// **************************************************************************** ///
/// *************************** ChaCha20 algorithm ***************************** ///
/// **************************************************************************** ///

#include <string>

namespace Encryption
{

/** @brief Creates CHACHA20_KEY_SIZE byte string with random char symbols, but not with 0..32 codes
 *
 * @param const size_t keySize Size of array must be generated
 * @return const std::string
 *
 */
std::string generateKeyChaCha20(const size_t keySize);

/** @brief Encrypts string using ChaCha20 algorithm
 *
 * @param plaintext const std::string&
 * @param key const std::string&
 * @param iv const std::string&
 * @param encryptedText std::string&
 * @return bool
 *
 */
bool chacha20_encrypt(const std::string & plaintext, const std::string & key, const std::string & iv, std::string & encryptedText);

/** @brief Decripts string encrypted using ChaCha20 algorithm
 *
 * @param encryptedText const std::string&
 * @param key const std::string&
 * @param iv const std::string&
 * @param decryptedText std::string&
 * @return bool
 *
 */
bool chacha20_decrypt(const std::string & encryptedText, const std::string & key, const std::string & iv, std::string & decryptedText);

}
