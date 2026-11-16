#include "common.hpp"

#include <random>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <new>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

#if defined(__linux__)
#include <sys/random.h>
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <stdlib.h>
#endif

namespace Encryption
{

static void fillArrayCSPRNG(uint8_t* buffer, const std::size_t lengthByte) {
    if (lengthByte == 0) return;

#if defined(_WIN32)
    // Использование Windows CNG API
    NTSTATUS status = BCryptGenRandom(
        nullptr,
        buffer,
        static_cast<ULONG>(lengthByte),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
        );
    if (!BCRYPT_SUCCESS(status)) {
        throw std::runtime_error("Windows BCryptGenRandom failed.");
    }

#elif defined(__linux__)
    if (lengthByte > 33'000'000) { // Linux core's generating issues
        throw std::invalid_argument("Invalid array size to generate random one (> 33 000 000)");
    }

    // (Linux 3.17+)
    std::size_t bytes_generated = 0;
    while (bytes_generated < lengthByte) {
        ssize_t result = getrandom(
            buffer + bytes_generated,
            lengthByte - bytes_generated,
            0
            );
        if (result < 0) {
            if (errno == EINTR) continue;
            throw std::runtime_error("Byte generation failed");
        }
        bytes_generated += result;
    }
#elif defined(__APPLE__) || defined(__FreeBSD__)
    // Использование безопасного arc4random_buf на macOS/BSD
    arc4random_buf(buffer, lengthByte);

#else
    throw std::runtime_error("Unknown OS, cound not determine cryptosafe byte source");
#endif
}

std::string generateBytes_CSPRNG(const std::size_t lengthByte)
{
    std::string result;
    result.resize(lengthByte);
    fillArrayCSPRNG(reinterpret_cast<uint8_t*>(result.data()), lengthByte);
    return result;
}

std::string generateBytes(const std::size_t lengthByte)
{
    std::string result;
    result.reserve(lengthByte);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, 255);

    std::generate_n(std::back_inserter(result), lengthByte, [&](){ return uni(rng); });

    return result;
}

std::string generateSymbols(const std::size_t lengthByte)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, 255);

    std::string result;
    for (int8_t i = 0; result.size() < lengthByte; i++) {
        char test_symbol = uni(rng);
        if (((test_symbol >= '0') && (test_symbol <= '9')) ||
            ((test_symbol >= 'a') && (test_symbol <= 'z')) ||
            ((test_symbol >= 'A') && (test_symbol <= 'Z'))) {
            result += test_symbol;
        }
    }

    return result;
}

void aes256_fixKeySize(std::string &key)
{
    if (key.size() > 32) {
        for (int i = 32; i < key.size() - 32; ++i) {
            key[i % 32] ^= key[i];
        }
        key.erase(key.begin() + 32, key.end());
    } else if (key.size() < 32) {
        std::fill_n(std::back_inserter(key), 32 - key.size(), 0x00);
    }
}

// ===================================================== //
// ================= Process memory safety ============= //
// ===================================================== //

void disableCoreDumps()
{
#if defined(_WIN32)
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    SetErrorMode(GetErrorMode() | SEM_NOOPENFILEERRORBOX);
#else
    struct rlimit limit {};
    limit.rlim_cur = 0;
    limit.rlim_max = 0;
    (void)setrlimit(RLIMIT_CORE, &limit);
#endif
}

bool lockSecureMemory(void* ptr, std::size_t size)
{
    if (ptr == nullptr || size == 0) { return false; }

#if defined(_WIN32)
    const bool locked = (VirtualLock(ptr, size) != 0);
    return locked;
#else
    const long pageSizeRaw = sysconf(_SC_PAGESIZE);
    const std::size_t pageSize = (pageSizeRaw > 0)
                                     ? static_cast<std::size_t>(pageSizeRaw)
                                     : 4096;
    const auto address = reinterpret_cast<std::uintptr_t>(ptr);
    const auto alignedStart = address & ~(static_cast<std::uintptr_t>(pageSize) - 1);
    const auto alignedEnd = (address + size + pageSize - 1) &
                            ~(static_cast<std::uintptr_t>(pageSize) - 1);
    const auto alignedSize = static_cast<std::size_t>(alignedEnd - alignedStart);

    const bool locked = (mlock(reinterpret_cast<void*>(alignedStart), alignedSize) == 0);
#if defined(MADV_DONTDUMP)
    (void)madvise(reinterpret_cast<void*>(alignedStart), alignedSize, MADV_DONTDUMP);
#endif
    return locked;
#endif
}

void unlockSecureMemory(void* ptr, std::size_t size)
{
    if (ptr == nullptr || size == 0) { return; }

#if defined(_WIN32)
    (void)VirtualUnlock(ptr, size);
#else
    const long pageSizeRaw = sysconf(_SC_PAGESIZE);
    const std::size_t pageSize = (pageSizeRaw > 0)
                                     ? static_cast<std::size_t>(pageSizeRaw)
                                     : 4096;
    const auto address = reinterpret_cast<std::uintptr_t>(ptr);
    const auto alignedStart = address & ~(static_cast<std::uintptr_t>(pageSize) - 1);
    const auto alignedEnd = (address + size + pageSize - 1) &
                            ~(static_cast<std::uintptr_t>(pageSize) - 1);
    const auto alignedSize = static_cast<std::size_t>(alignedEnd - alignedStart);
    (void)munlock(reinterpret_cast<void*>(alignedStart), alignedSize);
#endif
}

SecureBuffer::SecureBuffer(const std::size_t size) :
    m_size {size}
{
    if (size == 0) { return; }

#if defined(_WIN32)
    m_data = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (m_data == nullptr) {
        m_data = new (std::nothrow) std::uint8_t[size];
        m_allocSize = size;
        m_isMapped = false;
    } else {
        m_allocSize = size;
        m_isMapped = true;
        (void)VirtualLock(m_data, m_allocSize);
    }
#else
    const long pageSizeRaw = sysconf(_SC_PAGESIZE);
    const std::size_t pageSize = (pageSizeRaw > 0)
                                     ? static_cast<std::size_t>(pageSizeRaw)
                                     : 4096;
    const auto mappedSize = ((size + pageSize - 1) / pageSize) * pageSize;
    void* mapped = mmap(nullptr, mappedSize, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mapped == MAP_FAILED) {
        m_data = new (std::nothrow) std::uint8_t[size];
        m_allocSize = size;
        m_isMapped = false;
    } else {
        m_data = static_cast<std::uint8_t*>(mapped);
        m_allocSize = mappedSize;
        m_isMapped = true;
        (void)mlock(mapped, mappedSize);
#if defined(MADV_DONTDUMP)
        (void)madvise(mapped, mappedSize, MADV_DONTDUMP);
#endif
    }
#endif
    if (m_data != nullptr) {
        std::memset(m_data, 0, m_allocSize);
    }
}

SecureBuffer::~SecureBuffer()
{
    release();
}

SecureBuffer::SecureBuffer(SecureBuffer&& other) noexcept :
    m_data {other.m_data},
    m_size {other.m_size},
    m_allocSize {other.m_allocSize},
    m_isMapped {other.m_isMapped}
{
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_allocSize = 0;
    other.m_isMapped = false;
}

SecureBuffer& SecureBuffer::operator=(SecureBuffer&& other) noexcept
{
    if (this == &other) { return *this; }
    release();
    m_data = other.m_data;
    m_size = other.m_size;
    m_allocSize = other.m_allocSize;
    m_isMapped = other.m_isMapped;
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_allocSize = 0;
    other.m_isMapped = false;
    return *this;
}

void SecureBuffer::release() noexcept
{
    if (m_data == nullptr) { return; }

    secureClearMemory(m_data, m_allocSize);
#if defined(_WIN32)
    if (m_isMapped) {
        (void)VirtualUnlock(m_data, m_allocSize);
        (void)VirtualFree(m_data, 0, MEM_RELEASE);
    } else {
        delete[] m_data;
    }
#else
    if (m_isMapped) {
        (void)munlock(m_data, m_allocSize);
        (void)munmap(m_data, m_allocSize);
    } else {
        delete[] m_data;
    }
#endif
    m_data = nullptr;
    m_size = 0;
    m_allocSize = 0;
    m_isMapped = false;
}

std::string hideSymbols(const std::string &val)
{
    if (val.empty()) { return {}; }
    std::string res;
    const int requiredSize = val.size() < 5 ? val.size() : val.size() - 3;
    res.reserve(requiredSize);
    if (val.size() > 4) { res += std::string(val.data(), 2); }
    std::fill_n(std::back_inserter(res), requiredSize, '*');
    if (val.size() > 4) { res += std::string(val.data() + val.size() - 2, 2); }
    return res;
}

}
