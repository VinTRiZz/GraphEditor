#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#endif

#include <Components/ExtraClasses/Error.h>

namespace Encryption
{

// ===================================================== //

// Inlines for compiler optimisation potential (assuming lib can build in shared mode)

// Encryption processes error
thread_local inline ExtraClasses::ErrorObject g_error {};

inline void setEncryptionError(
    ExtraClasses::ErrorCode errCode,
    const std::string& errLocation,
    const std::string& openSSLdescr) {
    g_error = ExtraClasses::ErrorObject(errCode,
                                      errLocation + " : " + openSSLdescr);
}

inline std::string getEncryptionErrorText() {
    return g_error.getDetailText();
}

inline ExtraClasses::ErrorObject getEncryptionError() {
    return g_error;
}


// ===================================================== //

/**
 * @brief secureClearMemory Clean memory with avoiding Dead Store Elimination
 * @param ptr               Pointer to array
 * @param size              Size of memory pointer holds
 */
template <typename T>
void secureClearMemory(T* ptr, const std::size_t count) {
    if (ptr == nullptr || count == 0) { return; }
    const auto byteCount = count * sizeof(T);
#if defined(_WIN32)
    SecureZeroMemory(ptr, byteCount);
#elif defined(__linux__)
    explicit_bzero(ptr, byteCount);
#elif defined(__APPLE__) || defined(__FreeBSD__)
    explicit_bzero(ptr, byteCount);
#elif defined(__GNUC__) || defined(__clang__)
    std::memset(ptr, 0, byteCount);
    asm volatile("" : : "r"(ptr) : "memory"); // Assembler barrier for compiler
#else
    std::memset(ptr, 0, byteCount);
#endif
}

/**
 * @brief The SafeString class RAII to use secure clear memory
 */
class SafeString
{
public:
    SafeString() = default;
    ~SafeString() { secureClear(); }

    SafeString(const SafeString& other) : m_value {other.m_value} {}
    SafeString(SafeString&& other) noexcept : m_value {std::move(other.m_value)} {}

    template <typename T>
    SafeString(const T& iStr) : m_value {iStr} {}

    SafeString(std::string&& iStr) : m_value {std::move(iStr)} {}

    SafeString& operator=(const SafeString& other) {
        if (this != &other) {
            secureClear();
            m_value = other.m_value;
        }
        return *this;
    }

    SafeString& operator=(SafeString&& other) noexcept {
        if (this != &other) {
            secureClear();
            m_value = std::move(other.m_value);
        }
        return *this;
    }

    template <typename T,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<T>, SafeString>>>
    inline SafeString& operator=(T&& iStr) {
        secureClear();
        m_value = std::forward<T>(iStr);
        return *this;
    }

    const std::string* operator->() const { return &m_value; }
    std::string* operator->() { return &m_value; }

    std::string& value() { return m_value; }
    const std::string& value() const { return m_value; }
    operator std::string&() { return value(); }
    operator const std::string&() const { return value(); }

    void secureClear() { secureClearMemory(m_value.data(), m_value.size()); }

private:
    std::string m_value;
};

/**
 * @brief disableCoreDumps Prohibit writing process memory into a core dump
 * @note  Does not protect against privileged ptrace / process_vm_readv
 */
void disableCoreDumps();

/**
 * @brief lockSecureMemory Locks a memory range and excludes it from process dumps (where supported)
 * @param ptr               Start of the range
 * @param size              Size in bytes
 * @return                  true if the range was locked
 */
bool lockSecureMemory(void* ptr, std::size_t size);

/**
 * @brief unlockSecureMemory Unlocks a previously locked memory range
 */
void unlockSecureMemory(void* ptr, std::size_t size);

/**
 * @brief The SecureBuffer class Page-aligned, locked, dump-excluded, zero-on-free buffer
 */
class SecureBuffer
{
public:
    explicit SecureBuffer(std::size_t size);
    ~SecureBuffer();

    SecureBuffer(const SecureBuffer&) = delete;
    SecureBuffer& operator=(const SecureBuffer&) = delete;

    SecureBuffer(SecureBuffer&& other) noexcept;
    SecureBuffer& operator=(SecureBuffer&& other) noexcept;

    std::uint8_t* data() noexcept { return m_data; }
    const std::uint8_t* data() const noexcept { return m_data; }
    std::size_t size() const noexcept { return m_size; }

    std::string_view view() const noexcept {
        if (m_data == nullptr) { return {}; }
        return {reinterpret_cast<const char*>(m_data), m_size};
    }

private:
    void release() noexcept;

    std::uint8_t* m_data {nullptr};
    std::size_t   m_size {0};
    std::size_t   m_allocSize {0};
    bool          m_isMapped {false};
};


/**
 * @brief generateBytes_CSPRNG  Generates bytes using CSPRNG (for salt, keys, etc.)
 * @param lengthByte            Size of a string to generate
 * @return
 * @throws                      std::runtime_error or std::invalid_argument on errors
 */
std::string generateBytes_CSPRNG(const std::size_t lengthByte);

/**
 * @brief generateBytes Generates bytes using std::uniform_distribution
 * @param lengthByte    Size of a string to generate
 * @return
 */
std::string generateBytes(const std::size_t lengthByte);

/**
 * @brief generateSybmols   Generate symbols with no escape sequences using std::uniform_distribution
 * @param lengthByte        Size of a string to generate
 * @return
 */
std::string generateSymbols(const std::size_t lengthByte);

/**
 * @brief aes256_fixKeySize Resize key to correct (32 byte) size of AES-256
 * @param key               Key to fix, will be filled with 0x00 or cut
 * @note                    Function is NOT cryptosafe, key creating is user's responsibility
 */
void aes256_fixKeySize(std::string &key);

/**
 * @brief hideSymbols   Hides symbols of input string with '*'
 * @param val           Any sized string
 * @return              For <2 symbols "***", for >2 FRST"***"LST, else FRST2"***"LST2
 * @example ""          --> "***"
 * @example "az"        --> "a***z"
 * @example "abcdefg"   --> "ab***fg"
 */
std::string hideSymbols(const std::string &val);

}

