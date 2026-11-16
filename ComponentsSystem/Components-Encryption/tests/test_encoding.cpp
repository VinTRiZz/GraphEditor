#include <gtest/gtest.h>

#include <Components/Encryption/Encoding.h>

#include <string>
#include <vector>

namespace {

std::string makeAllBytes() {
    std::string bytes;
    bytes.reserve(256);
    for (int i = 0; i < 256; ++i) {
        bytes.push_back(static_cast<char>(i));
    }
    return bytes;
}

const std::string kCp1251Hello {"\xCF\xF0\xE8\xE2\xE5\xF2"};
const std::string kUtf8Hello   {"\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82"};

void expectEncodingFailed(const std::string& description) {
    EXPECT_EQ(Encryption::getEncryptionError().getCode(),
              ExtraClasses::ErrorCode::LogicalEncodingFailed)
        << description << " : " << Encryption::getEncryptionErrorText();
}

}

// ============================================== //
// Base64
// ============================================== //

TEST(Components_Encryption, Base64_EncodeKnownVectors) {
    const std::vector<std::pair<std::string, std::string>> cases {
        {"",       ""},
        {"f",      "Zg=="},
        {"fo",     "Zm8="},
        {"foo",    "Zm9v"},
        {"foob",   "Zm9vYg=="},
        {"fooba",  "Zm9vYmE="},
        {"foobar", "Zm9vYmFy"},
    };

    for (const auto& [input, expected] : cases) {
        const auto result = Encryption::encodeBase64(input);
        ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(expected, result.value()) << "Input: " << input;
    }
}

TEST(Components_Encryption, Base64_DecodeKnownVectors) {
    const std::vector<std::pair<std::string, std::string>> cases {
        {"",        ""},
        {"Zg==",    "f"},
        {"Zm8=",    "fo"},
        {"Zm9v",    "foo"},
        {"Zm9vYg==", "foob"},
        {"Zm9vYmE=", "fooba"},
        {"Zm9vYmFy", "foobar"},
    };

    for (const auto& [input, expected] : cases) {
        const auto result = Encryption::decodeBase64(input);
        ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(expected, result.value()) << "Input: " << input;
    }
}

TEST(Components_Encryption, Base64_RoundTrip) {
    const std::vector<std::string> samples {
        "",
        "a",
        "ab",
        "abc",
        Encryption::generateBytes(1),
        Encryption::generateBytes(2),
        Encryption::generateBytes(3),
        Encryption::generateBytes(31),
        Encryption::generateBytes(32),
        Encryption::generateBytes(33),
        Encryption::generateSymbols(257),
        makeAllBytes(),
    };

    for (const auto& sample : samples) {
        const auto encoded = Encryption::encodeBase64(sample);
        ASSERT_TRUE(encoded.has_value()) << Encryption::getEncryptionErrorText();

        const auto decoded = Encryption::decodeBase64(encoded.value());
        ASSERT_TRUE(decoded.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(sample, decoded.value()) << "Sample size: " << sample.size();
    }
}

TEST(Components_Encryption, Base64_DecodeInvalid) {
    const std::vector<std::string> cases {
        "Zg=",        // length is not a multiple of 4
        "Zm9vYmE",    // missing padding
        "Z===",       // invalid padding layout
        "A===",       // padding starts too early
        "=AAA",       // padding at the beginning
        "A=AA",       // padding in the middle
        "Zm9v!",      // invalid character
        "Zm9v\n",     // whitespace/newline is not valid Base64
        "Zm 9v",      // whitespace inside the payload
    };

    for (const auto& input : cases) {
        const auto result = Encryption::decodeBase64(input);
        EXPECT_FALSE(result.has_value()) << "Input must be rejected: " << input;
        expectEncodingFailed("Invalid Base64 input: " + input);
    }
}

// ============================================== //
// Hex
// ============================================== //

TEST(Components_Encryption, Hex_EncodeKnownVectors) {
    const std::string raw {"\x00\x01\x0F\x10\xFF", 5};

    auto result = Encryption::encodeHex("");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("", result.value());

    result = Encryption::encodeHex("f");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("66", result.value());

    result = Encryption::encodeHex("foo");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("666F6F", result.value());

    result = Encryption::encodeHex(raw);
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("00010F10FF", result.value());
}

TEST(Components_Encryption, Hex_DecodeKnownVectors) {
    const std::string raw {"\x00\x01\x0F\x10\xFF", 5};

    auto result = Encryption::decodeHex("");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("", result.value());

    result = Encryption::decodeHex("66");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("f", result.value());

    result = Encryption::decodeHex("666F6F");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("foo", result.value());

    result = Encryption::decodeHex("666f6f", false);
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("foo", result.value());

    result = Encryption::decodeHex("00010F10FF");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(raw, result.value());
}

TEST(Components_Encryption, Hex_DecodeInvalid) {
    auto result = Encryption::decodeHex("ABC");
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Odd length");

    result = Encryption::decodeHex("GG");
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Invalid character 'G'");

    result = Encryption::decodeHex("0Z");
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Invalid character 'Z'");

    result = Encryption::decodeHex(" 0");
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Whitespace is not a hex digit");
}

TEST(Components_Encryption, Hex_CaseFlag) {
    const std::string raw {"\x00\x7F\x80\xFF", 4};

    const auto upper = Encryption::encodeHex(raw, true);
    ASSERT_TRUE(upper.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("007F80FF", upper.value());

    const auto lower = Encryption::encodeHex(raw, false);
    ASSERT_TRUE(lower.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("007f80ff", lower.value());

    auto decoded = Encryption::decodeHex(upper.value(), true);
    ASSERT_TRUE(decoded.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(raw, decoded.value());

    decoded = Encryption::decodeHex(lower.value(), false);
    ASSERT_TRUE(decoded.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(raw, decoded.value());

    EXPECT_FALSE(Encryption::decodeHex(lower.value(), true).has_value())
        << "Lowercase input must be rejected when useUpperCase is true";
    expectEncodingFailed("Lowercase input with useUpperCase=true");

    EXPECT_FALSE(Encryption::decodeHex(upper.value(), false).has_value())
        << "Uppercase input must be rejected when useUpperCase is false";
    expectEncodingFailed("Uppercase input with useUpperCase=false");
}

TEST(Components_Encryption, Hex_RoundTrip) {
    const std::vector<std::string> samples {
        "",
        "a",
        "ab",
        "abc",
        std::string("Hello, world!"),
        Encryption::generateBytes(255),
        Encryption::generateSymbols(1024),
        makeAllBytes(),
    };

    for (const auto& sample : samples) {
        const auto encoded = Encryption::encodeHex(sample);
        ASSERT_TRUE(encoded.has_value()) << Encryption::getEncryptionErrorText();

        const auto decoded = Encryption::decodeHex(encoded.value());
        ASSERT_TRUE(decoded.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(sample, decoded.value()) << "Sample size: " << sample.size();
    }
}

// ============================================== //
// CP1251 -> UTF-8
// ============================================== //

TEST(Components_Encryption, Cp1251ToUtf8_KnownVectors) {
    auto result = Encryption::cp1251_to_utf8(kCp1251Hello);
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(kUtf8Hello, result.value());

    result = Encryption::cp1251_to_utf8("Hello, world!");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("Hello, world!", result.value());

    const std::string cp1251Ascii {"\x80\x81\x82", 3};
    result = Encryption::cp1251_to_utf8(cp1251Ascii);
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("\xD0\x82\xD0\x83\xE2\x80\x9A", result.value());
}

TEST(Components_Encryption, Cp1251ToUtf8_Empty) {
    const auto result = Encryption::cp1251_to_utf8("");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(Encryption::getEncryptionError().getCode(),
              ExtraClasses::ErrorCode::LogicalInvalidValue)
        << Encryption::getEncryptionErrorText();
}

TEST(Components_Encryption, Cp1251ToUtf8_InvalidByte) {
    const std::string invalid {"\x98", 1};
    const auto result = Encryption::cp1251_to_utf8(invalid);
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Undefined CP1251 byte 0x98");
}

TEST(Components_Encryption, Cp1251ToUtf8_RoundTrip) {
    const std::vector<std::string> samples {
        kCp1251Hello,
        "Hello, world!",
        std::string("1234567890 !@#$%^&*()"),
        std::string("\xC0\xC1\xC2\xDF\xE0\xE1\xE2\xFF", 8),
    };

    for (const auto& sample : samples) {
        const auto utf8 = Encryption::cp1251_to_utf8(sample);
        ASSERT_TRUE(utf8.has_value()) << Encryption::getEncryptionErrorText();

        const auto back = Encryption::utf8_to_cp1251(utf8.value());
        ASSERT_TRUE(back.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(sample, back.value());
    }
}

// ============================================== //
// UTF-8 -> CP1251
// ============================================== //

TEST(Components_Encryption, Utf8ToCp1251_KnownVectors) {
    auto result = Encryption::utf8_to_cp1251(kUtf8Hello);
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(kCp1251Hello, result.value());

    result = Encryption::utf8_to_cp1251("Hello, world!");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ("Hello, world!", result.value());

    result = Encryption::utf8_to_cp1251("\xD0\x82\xD0\x83\xE2\x80\x9A");
    ASSERT_TRUE(result.has_value()) << Encryption::getEncryptionErrorText();
    EXPECT_EQ(std::string("\x80\x81\x82", 3), result.value());
}

TEST(Components_Encryption, Utf8ToCp1251_Empty) {
    const auto result = Encryption::utf8_to_cp1251("");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(Encryption::getEncryptionError().getCode(),
              ExtraClasses::ErrorCode::LogicalInvalidValue)
        << Encryption::getEncryptionErrorText();
}

TEST(Components_Encryption, Utf8ToCp1251_InvalidSequence) {
    const std::string invalid {"\xFF", 1};
    const auto result = Encryption::utf8_to_cp1251(invalid);
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Invalid UTF-8 byte 0xFF");
}

TEST(Components_Encryption, Utf8ToCp1251_UnrepresentableCharacter) {
    const std::string emoji {"\xF0\x9F\x98\x80", 4};
    const auto result = Encryption::utf8_to_cp1251(emoji);
    EXPECT_FALSE(result.has_value());
    expectEncodingFailed("Emoji is not representable in CP1251");
}

TEST(Components_Encryption, Utf8ToCp1251_RoundTrip) {
    const std::vector<std::string> samples {
        kUtf8Hello,
        "Hello, world!",
        std::string("1234567890 !@#$%^&*()"),
        std::string("\xD0\x90\xD0\xAF\xD0\xB0\xD1\x8F", 8),
    };

    for (const auto& sample : samples) {
        const auto cp1251 = Encryption::utf8_to_cp1251(sample);
        ASSERT_TRUE(cp1251.has_value()) << Encryption::getEncryptionErrorText();

        const auto back = Encryption::cp1251_to_utf8(cp1251.value());
        ASSERT_TRUE(back.has_value()) << Encryption::getEncryptionErrorText();
        EXPECT_EQ(sample, back.value());
    }
}
