#include "encoding.hpp"

#include <sstream>
#include <iconv.h>
#include <limits>
#include <memory>

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

namespace Encryption
{

namespace {

inline constexpr char kHexDigitsUpper[17] = "0123456789ABCDEF";
inline constexpr char kHexDigitsLower[17] = "0123456789abcdef";

struct HexNibbleTable {
    unsigned char v[256];

    constexpr HexNibbleTable() noexcept : v{} {
        for (auto& x : v) {
            x = 0xFFu;
        }
        for (unsigned i = 0; i < 10u; ++i) {
            v[static_cast<unsigned char>('0' + i)] = static_cast<unsigned char>(i);
        }
        for (unsigned i = 0; i < 6u; ++i) {
            v[static_cast<unsigned char>('A' + i)] = static_cast<unsigned char>(10u + i);
            v[static_cast<unsigned char>('a' + i)] = static_cast<unsigned char>(10u + i);
        }
    }
};
inline constexpr HexNibbleTable kHexNibble{};

struct Base64Validation {
    const char* error = nullptr;
    std::size_t padding = 0;
};

using BioPtr = std::unique_ptr<BIO, decltype(&BIO_free)>;
using BioAllPtr = std::unique_ptr<BIO, decltype(&BIO_free_all)>;

inline bool isBase64AlphabetChar(const unsigned char c) noexcept
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/';
}

Base64Validation validateBase64Input(const std::string& input) noexcept
{
    Base64Validation result;

    if (input.empty()) {
        return result;
    }

    if (input.size() % 4u != 0u) {
        result.error = "Invalid length (must be a multiple of 4)";
        return result;
    }

    if (input[input.size() - 1u] == '=') {
        ++result.padding;
    }
    if (input.size() >= 2u && input[input.size() - 2u] == '=') {
        ++result.padding;
    }

    for (std::size_t i = 0; i < input.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(input[i]);

        if (c == '=') {
            if (i < input.size() - result.padding) {
                result.error = "Unexpected padding character";
                return result;
            }
            continue;
        }

        if (!isBase64AlphabetChar(c)) {
            result.error = "Invalid Base64 character";
            return result;
        }
    }

    return result;
}
}


std::optional<std::string> encodeBase64(const std::string &input)
{
    if (input.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] input",
                           "Input size exceeds supported limit");
        return {};
    }

    if (input.empty()) {
        return std::string {};
    }

    BioAllPtr bio(BIO_new(BIO_s_mem()), &BIO_free_all);
    if (!bio) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_new",
                           "Failed to allocate memory BIO");
        return {};
    }

    BioPtr b64(BIO_new(BIO_f_base64()), &BIO_free);
    if (!b64) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_new",
                           "Failed to allocate Base64 BIO");
        return {};
    }

    bio.reset(BIO_push(b64.release(), bio.release()));
    BIO_set_flags(bio.get(), BIO_FLAGS_BASE64_NO_NL);

    const int inputLength = static_cast<int>(input.size());
    const int written = BIO_write(bio.get(), input.data(), inputLength);
    if (written != inputLength) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_write",
                           "Failed to write input (written " + std::to_string(written) +
                               " of " + std::to_string(inputLength) + " bytes)");
        return {};
    }

    if (BIO_flush(bio.get()) != 1) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_flush",
                           "Failed to flush Base64 encoder");
        return {};
    }

    BUF_MEM *bufferPtr = nullptr;
    if (BIO_get_mem_ptr(bio.get(), &bufferPtr) != 1 || bufferPtr == nullptr) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_get_mem_ptr",
                           "Failed to get encoded buffer");
        return {};
    }

    if (bufferPtr->length != 0u && bufferPtr->data == nullptr) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-ENC] BIO_get_mem_ptr",
                           "Encoded buffer is null");
        return {};
    }

    std::string output;
    if (bufferPtr->length != 0u) {
        output.assign(bufferPtr->data, bufferPtr->length);
    }

    return output;
}

std::optional<std::string> decodeBase64(const std::string &input)
{
    if (input.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] input",
                           "Input size exceeds supported limit");
        return {};
    }

    const Base64Validation validation = validateBase64Input(input);
    if (validation.error != nullptr) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] input",
                           validation.error);
        return {};
    }

    if (input.empty()) {
        return std::string {};
    }

    BioAllPtr bio(BIO_new_mem_buf(input.data(), static_cast<int>(input.size())), &BIO_free_all);
    if (!bio) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] BIO_new_mem_buf",
                           "Failed to allocate input BIO");
        return {};
    }

    BioPtr b64(BIO_new(BIO_f_base64()), &BIO_free);
    if (!b64) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] BIO_new",
                           "Failed to allocate Base64 BIO");
        return {};
    }

    bio.reset(BIO_push(b64.release(), bio.release()));
    BIO_set_flags(bio.get(), BIO_FLAGS_BASE64_NO_NL);

    const std::size_t expectedLength = (input.size() / 4u) * 3u - validation.padding;
    std::string buffer(expectedLength, '\0');

    const int decoded = BIO_read(bio.get(), buffer.data(), static_cast<int>(buffer.size()));
    if (decoded < 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] BIO_read",
                           "Failed to decode Base64 input");
        return {};
    }

    if (static_cast<std::size_t>(decoded) != expectedLength) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[BASE64-DEC] BIO_read",
                           "Decoded size mismatch (expected " + std::to_string(expectedLength) +
                               ", got " + std::to_string(decoded) + " bytes)");
        return {};
    }

    buffer.resize(static_cast<std::size_t>(decoded));
    return buffer;
}

std::optional<std::string> encodeHex(const std::string& input, bool useUpperCase)
{
    if (input.size() > (std::numeric_limits<std::size_t>::max)() / 2u) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[HEX-ENC]",
                           "Result size overflow");
        return {};
    }

    const char* digits = useUpperCase ? kHexDigitsUpper : kHexDigitsLower;
    std::string out;
    out.resize(input.size() * 2u);

    for (std::size_t i = 0; i < input.size(); ++i) {
        const auto b = static_cast<unsigned char>(input[i]);
        out[2 * i]     = digits[(b >> 4) & 0x0Fu];
        out[2 * i + 1] = digits[b & 0x0Fu];
    }
    return out;
}

std::optional<std::string> decodeHex(const std::string& input, bool useUpperCase)
{
    if (input.size() % 2u != 0u) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[HEX-DEC]",
                           "Invalid length (odd)");
        return {};
    }

    std::string out;
    out.resize(input.size() / 2u);

    const unsigned char wrongCaseLow  = useUpperCase ? 'a' : 'A';
    const unsigned char wrongCaseHigh = useUpperCase ? 'f' : 'F';

    for (std::size_t i = 0; i < out.size(); ++i) {
        const unsigned char hiChar = static_cast<unsigned char>(input[2u * i]);
        const unsigned char loChar = static_cast<unsigned char>(input[2u * i + 1u]);

        const bool wrongCase =
            (hiChar >= wrongCaseLow && hiChar <= wrongCaseHigh) ||
            (loChar >= wrongCaseLow && loChar <= wrongCaseHigh);

        const unsigned char hi = kHexNibble.v[hiChar];
        const unsigned char lo = kHexNibble.v[loChar];

        if (wrongCase || hi == 0xFFu || lo == 0xFFu) {
            setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                               "[HEX-DEC]",
                               "Invalid hex character");
            return {};
        }

        out[i] = static_cast<char>((hi << 4u) | lo);
    }
    return out;
}

std::optional<std::string> cp1251_to_utf8(const std::string_view &input)
{
    if (input.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[CP1251->UTF8] input",
                           "Empty input");
        return {};
    }

    iconv_t cd = iconv_open("UTF-8", "CP1251");
    if (cd == reinterpret_cast<iconv_t>(-1)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[CP1251->UTF8] iconv_open",
                           std::string("Failed to open iconv descriptor: ") + std::strerror(errno));
        return {};
    }

    std::string output(input.size() * 3 + 1, '\0');
    char *inBuf = const_cast<char *>(input.data());
    size_t inBytesLeft = input.size();
    char *outBuf = output.data();
    size_t outBytesLeft = output.size();

    const size_t result = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    struct IconvCloser {
        iconv_t descriptor;
        ~IconvCloser() {
            if (descriptor != reinterpret_cast<iconv_t>(-1)) {
                iconv_close(descriptor);
            }
        }
    } closer{cd};

    if (result == static_cast<size_t>(-1)) {
        std::stringstream ss;
        ss << "Conversion failed: " << std::strerror(errno);

        if (errno == EILSEQ) {
            ss << " (invalid CP1251 byte"
               << ", offset: " << (input.size() - inBytesLeft) << ")";
        } else if (errno == EINVAL) {
            ss << " (incomplete input sequence at end of input)";
        } else if (errno == E2BIG) {
            ss << " (output buffer too small)";
        }

        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[CP1251->UTF8] iconv",
                           ss.str());
        return {};
    }

    if (inBytesLeft != 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[CP1251->UTF8] iconv",
                           "Incomplete conversion: " + std::to_string(inBytesLeft) + " input byte(s) left");
        return {};
    }

    output.resize(outBuf - output.data());
    return output;
}

std::optional<std::string> utf8_to_cp1251(const std::string_view &input)
{
    if (input.empty()) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "[UTF8->CP1251] input",
                           "Empty input");
        return {};
    }

    iconv_t cd = iconv_open("CP1251", "UTF-8");
    if (cd == reinterpret_cast<iconv_t>(-1)) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[UTF8->CP1251] iconv_open",
                           std::string("Failed to open iconv descriptor: ") + std::strerror(errno));
        return {};
    }

    std::string output(input.size() + 1, '\0');
    char *inBuf = const_cast<char *>(input.data());
    size_t inBytesLeft = input.size();
    char *outBuf = output.data();
    size_t outBytesLeft = output.size();

    const size_t result = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    struct IconvCloser {
        iconv_t descriptor;
        ~IconvCloser() {
            if (descriptor != reinterpret_cast<iconv_t>(-1)) {
                iconv_close(descriptor);
            }
        }
    } closer{cd};

    if (result == static_cast<size_t>(-1)) {
        std::stringstream ss;
        ss << "Conversion failed: " << std::strerror(errno);

        if (errno == EILSEQ) {
            ss << " (invalid UTF-8 sequence or character not representable in CP1251"
               << ", offset: " << (input.size() - inBytesLeft) << ")";
        } else if (errno == EINVAL) {
            ss << " (incomplete UTF-8 sequence at end of input)";
        } else if (errno == E2BIG) {
            ss << " (output buffer too small)";
        }

        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[UTF8->CP1251] iconv",
                           ss.str());
        return {};
    }

    if (inBytesLeft != 0) {
        setEncryptionError(ExtraClasses::ErrorCode::LogicalEncodingFailed,
                           "[UTF8->CP1251] iconv",
                           "Incomplete conversion: " + std::to_string(inBytesLeft) + " input byte(s) left");
        return {};
    }

    output.resize(outBuf - output.data());
    return output;
}

}
