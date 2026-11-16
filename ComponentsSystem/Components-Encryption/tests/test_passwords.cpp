#include <gtest/gtest.h>

#include <Components/Encryption/Hash.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint32_t ARGON2_MIN_MEMORY_KIB      {8};
constexpr std::uint32_t ARGON2_MAX_MEMORY_KIB      {1024 * 1024};
constexpr std::uint32_t ARGON2_MAX_ITERATIONS      {64};
constexpr std::uint32_t ARGON2_MAX_PARALLELISM     {64};
constexpr std::uint32_t ARGON2_MIN_HASH_LENGTH     {4};
constexpr std::uint32_t ARGON2_MAX_HASH_LENGTH     {1024};
constexpr std::size_t   ARGON2_MIN_SALT_SIZE       {8};
constexpr std::size_t   HMAC_SHA256_TAG_SIZE       {32};

constexpr auto ARGON2_RFC9106_EXPECTED {
    "0d640df58d78766c08c037a34a8b53c9d01ef0452d75b65eb52520e96b01e659"
};

const auto ARGON2_TEST_PASSWORD {"correct horse battery staple"};
const auto ARGON2_TEST_SALT     {"0123456789abcdef0123456789abcdef"};

struct HmacCase {
    std::string key;
    std::string data;
    const char* expected;
};

std::string repeatedByte(const unsigned char value, const std::size_t count)
{
    return std::string(count, static_cast<char>(value));
}

std::string toHex(const std::string& input)
{
    static constexpr char HEX[] = "0123456789abcdef";
    std::string result;
    result.reserve(input.size() * 2);
    for (const unsigned char symbol : input) {
        result.push_back(HEX[(symbol >> 4) & 0x0F]);
        result.push_back(HEX[symbol & 0x0F]);
    }
    return result;
}

Encryption::Argon2idParams makeFastParams()
{
    Encryption::Argon2idParams params;
    params.memoryKiB = 16 * 1024;
    params.iterations = 2;
    params.parallelism = 1;
    params.hashLength = 32;
    return params;
}

void expectHashFailed(const std::string& description)
{
    EXPECT_EQ(Encryption::getEncryptionError().getCode(),
              ExtraClasses::ErrorCode::LogicalHashFailed)
        << description << " : " << Encryption::getEncryptionErrorText();
}

void expectArgon2Failure(const std::string& description,
                         const std::string_view& password,
                         const std::string_view& salt,
                         const Encryption::Argon2idParams& params)
{
    EXPECT_FALSE(Encryption::argon2id(password, salt, params).has_value())
        << description;
    expectHashFailed(description);
}

// RFC 4231, section 4 (HMAC-SHA256 test cases)
const std::vector<HmacCase> HMAC_RFC4231_CASES {
    {
        repeatedByte(0x0b, 20),
        "Hi There",
        "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7"
    },
    {
        "Jefe",
        "what do ya want for nothing?",
        "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843"
    },
    {
        repeatedByte(0xaa, 20),
        repeatedByte(0xdd, 50),
        "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe"
    },
    {
        repeatedByte(0xaa, 131),
        "Test Using Larger Than Block-Size Key - Hash Key First",
        "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54"
    },
    {
        repeatedByte(0xaa, 131),
        "This is a test using a larger than block-size key and a larger than "
        "block-size data. The key needs to be hashed before being used by the "
        "HMAC algorithm.",
        "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2"
    },
    {
        "key",
        {},
        "5d5d139563c95b5967b9bd9a8c9b233a9dedb45072794cd232dc1b74832607d0"
    },
    {
        "key",
        "The quick brown fox jumps over the lazy dog",
        "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8"
    },
};

} // namespace

// ============================================== //
// Argon2id
// ============================================== //

TEST(Components_Encryption, Argon2id_Rfc9106Vector)
{
    // RFC 9106, section 5.3 (Argon2id test vector).
    const auto password = repeatedByte(0x01, 32);
    const auto salt     = repeatedByte(0x02, 16);

    Encryption::Argon2idParams params;
    params.memoryKiB = 32;
    params.iterations = 3;
    params.parallelism = 4;
    params.hashLength = 32;
    params.secret = repeatedByte(0x03, 8);
    params.associatedData = repeatedByte(0x04, 12);

    const auto derived = Encryption::argon2id(password, salt, params);
    ASSERT_TRUE(derived.has_value()) << Encryption::getEncryptionErrorText();
    ASSERT_EQ(ARGON2_RFC9106_EXPECTED, toHex(derived.value()));
}

TEST(Components_Encryption, Argon2id_DeterministicAndSensitive)
{
    auto params = makeFastParams();

    const auto first = Encryption::argon2id(ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
    const auto second = Encryption::argon2id(ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
    ASSERT_TRUE(first.has_value()) << Encryption::getEncryptionErrorText();
    ASSERT_TRUE(second.has_value()) << Encryption::getEncryptionErrorText();
    ASSERT_EQ(first.value(), second.value()) << "Argon2id must be deterministic";

    const auto otherPassword = Encryption::argon2id(
        std::string(ARGON2_TEST_PASSWORD) + "x", ARGON2_TEST_SALT, params);
    ASSERT_TRUE(otherPassword.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_NE(first.value(), otherPassword.value()) << "Password must affect the key";

    const auto otherSalt = Encryption::argon2id(
        ARGON2_TEST_PASSWORD, std::string(ARGON2_TEST_SALT) + "x", params);
    ASSERT_TRUE(otherSalt.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_NE(first.value(), otherSalt.value()) << "Salt must affect the key";

    params.secret = "pepper";
    const auto withSecret = Encryption::argon2id(ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
    ASSERT_TRUE(withSecret.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_NE(first.value(), withSecret.value()) << "Secret must affect the key";

    params.secret.clear();
    params.associatedData = "domain-separation";
    const auto withAssociatedData = Encryption::argon2id(
        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
    ASSERT_TRUE(withAssociatedData.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_NE(first.value(), withAssociatedData.value())
        << "Associated data must affect the key";
}

TEST(Components_Encryption, Argon2id_OutputLength)
{
    auto params = makeFastParams();
    params.memoryKiB = ARGON2_MIN_MEMORY_KIB;
    params.iterations = 1;
    params.parallelism = 1;

    for (const std::uint32_t hashLength : {ARGON2_MIN_HASH_LENGTH, 16u, 31u, 32u, 33u, 64u, ARGON2_MAX_HASH_LENGTH}) {
        params.hashLength = hashLength;
        const auto derived = Encryption::argon2id(
            ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
        ASSERT_TRUE(derived.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(hashLength, derived->size()) << "Requested hash length: " << hashLength;
    }
}

TEST(Components_Encryption, Argon2id_AcceptsBoundaryParameters)
{
    const auto password = repeatedByte(0x11, 16);
    const auto minSalt  = repeatedByte(0x22, ARGON2_MIN_SALT_SIZE);

    Encryption::Argon2idParams params;
    params.memoryKiB = ARGON2_MIN_MEMORY_KIB;
    params.iterations = 1;
    params.parallelism = 1;
    params.hashLength = ARGON2_MIN_HASH_LENGTH;

    auto derived = Encryption::argon2id(password, minSalt, params);
    ASSERT_TRUE(derived.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(ARGON2_MIN_HASH_LENGTH, derived->size());

    params.iterations = ARGON2_MAX_ITERATIONS;
    derived = Encryption::argon2id(password, minSalt, params);
    ASSERT_TRUE(derived.has_value()) << Encryption::getEncryptionErrorText();

    params.iterations = 1;
    params.parallelism = ARGON2_MAX_PARALLELISM;
    params.memoryKiB = ARGON2_MIN_MEMORY_KIB * ARGON2_MAX_PARALLELISM;
    derived = Encryption::argon2id(password, minSalt, params);
    ASSERT_TRUE(derived.has_value()) << Encryption::getEncryptionErrorText();
}

TEST(Components_Encryption, Argon2id_RejectsInvalidInput)
{
    auto params = makeFastParams();

    expectArgon2Failure("Empty password",
                        {}, ARGON2_TEST_SALT, params);
    expectArgon2Failure("Empty salt",
                        ARGON2_TEST_PASSWORD, {}, params);
    expectArgon2Failure("Salt shorter than 8 bytes",
                        ARGON2_TEST_PASSWORD,
                        std::string_view(ARGON2_TEST_SALT).substr(0, ARGON2_MIN_SALT_SIZE - 1),
                        params);

    params.memoryKiB = ARGON2_MIN_MEMORY_KIB - 1;
    expectArgon2Failure("Memory cost below minimum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.memoryKiB = ARGON2_MAX_MEMORY_KIB + 1;
    expectArgon2Failure("Memory cost above maximum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.memoryKiB = ARGON2_MIN_MEMORY_KIB;
    params.iterations = 0;
    expectArgon2Failure("Zero iterations",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.iterations = ARGON2_MAX_ITERATIONS + 1;
    expectArgon2Failure("Iterations above maximum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.iterations = 1;
    params.parallelism = 0;
    expectArgon2Failure("Zero parallelism",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.parallelism = ARGON2_MAX_PARALLELISM + 1;
    expectArgon2Failure("Parallelism above maximum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.parallelism = ARGON2_MAX_PARALLELISM;
    params.memoryKiB = ARGON2_MIN_MEMORY_KIB;
    expectArgon2Failure("Memory cost below 8 * parallelism",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.parallelism = 1;
    params.memoryKiB = ARGON2_MIN_MEMORY_KIB;
    params.hashLength = ARGON2_MIN_HASH_LENGTH - 1;
    expectArgon2Failure("Hash length below minimum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.hashLength = 0;
    expectArgon2Failure("Zero hash length",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);

    params.hashLength = ARGON2_MAX_HASH_LENGTH + 1;
    expectArgon2Failure("Hash length above maximum",
                        ARGON2_TEST_PASSWORD, ARGON2_TEST_SALT, params);
}

// ============================================== //
// HMAC-SHA256
// ============================================== //

TEST(Components_Encryption, HMAC_SHA256_Rfc4231Vectors)
{
    for (const auto& testCase : HMAC_RFC4231_CASES) {
        const auto tag = Encryption::hmacSha256(testCase.key, testCase.data);
        ASSERT_TRUE(tag.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(HMAC_SHA256_TAG_SIZE, tag->size());
        EXPECT_EQ(testCase.expected, toHex(tag.value()))
            << "Key size: " << testCase.key.size()
            << ", data size: " << testCase.data.size();
    }
}

TEST(Components_Encryption, HMAC_SHA256_BinaryInputs)
{
    const auto key = std::string("\x00\x01\x02\x00\x03\x04", 6);
    const auto data = std::string("\x00\xFF\x00\x80\x7F\x00", 6);

    const auto tag = Encryption::hmacSha256(key, data);
    ASSERT_TRUE(tag.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(HMAC_SHA256_TAG_SIZE, tag->size());

    const auto sameTag = Encryption::hmacSha256(key, data);
    ASSERT_TRUE(sameTag.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(tag.value(), sameTag.value()) << "HMAC-SHA256 must be deterministic";

    const auto otherKey = std::string("\x00\x01\x02\x00\x03\x05", 6);
    const auto otherTag = Encryption::hmacSha256(otherKey, data);
    ASSERT_TRUE(otherTag.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_NE(tag.value(), otherTag.value()) << "Key must affect the tag";
}

TEST(Components_Encryption, HMAC_SHA256_RejectsEmptyKey)
{
    EXPECT_FALSE(Encryption::hmacSha256({}, "data").has_value());
    expectHashFailed("Empty HMAC key");
}
