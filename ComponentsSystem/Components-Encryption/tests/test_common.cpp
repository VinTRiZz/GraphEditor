#include <gtest/gtest.h>

#include <Components/Encryption/Common.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#if !defined(_WIN32)
#include <sys/resource.h>
#endif

namespace {

constexpr std::size_t SECURE_CLEAR_ELEMENT_COUNT {8};
constexpr std::size_t SECURE_CLEAR_ZEROED_COUNT  {5};
constexpr std::uint32_t SECURE_CLEAR_FILL_VALUE  {0xFFFFFFFFu};
constexpr std::uint8_t   BUFFER_FILL_VALUE       {0xA5};
constexpr std::size_t    BUFFER_SMALL_SIZE       {64};
constexpr std::size_t    BUFFER_LARGE_SIZE       {4096 + 17};

} // namespace

// ============================================== //
// secureClearMemory
// ============================================== //

TEST(Components_Encryption, SecureClearMemory_ZeroesRequestedElements)
{
    std::vector<std::uint32_t> values(
        SECURE_CLEAR_ELEMENT_COUNT, SECURE_CLEAR_FILL_VALUE);

    Encryption::secureClearMemory(values.data(), SECURE_CLEAR_ZEROED_COUNT);

    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index < SECURE_CLEAR_ZEROED_COUNT) {
            EXPECT_EQ(0u, values[index]) << "Element " << index << " must be cleared";
        } else {
            EXPECT_EQ(SECURE_CLEAR_FILL_VALUE, values[index])
                << "Element " << index << " is outside of the cleared range";
        }
    }

    std::array<std::uint8_t, 4> bytes {1, 2, 3, 4};
    Encryption::secureClearMemory(bytes.data(), 2);
    EXPECT_EQ(0, bytes[0]);
    EXPECT_EQ(0, bytes[1]);
    EXPECT_EQ(3, bytes[2]);
    EXPECT_EQ(4, bytes[3]);
}

TEST(Components_Encryption, SecureClearMemory_IgnoresEmptyRanges)
{
    std::array<std::uint8_t, 4> bytes {1, 2, 3, 4};

    EXPECT_NO_THROW(Encryption::secureClearMemory(
        static_cast<std::uint8_t*>(nullptr), bytes.size()));
    EXPECT_NO_THROW(Encryption::secureClearMemory(bytes.data(), 0));

    EXPECT_EQ(1, bytes[0]);
    EXPECT_EQ(2, bytes[1]);
    EXPECT_EQ(3, bytes[2]);
    EXPECT_EQ(4, bytes[3]);
}

// ============================================== //
// SafeString
// ============================================== //

TEST(Components_Encryption, SafeString_CopyAndMove)
{
    Encryption::SafeString original {"secret-value"};

    Encryption::SafeString copy {original};
    ASSERT_EQ("secret-value", copy.value());

    original = std::string {"changed"};
    EXPECT_EQ("changed", original.value());
    EXPECT_EQ("secret-value", copy.value()) << "Copy must be independent";

    copy = original;
    EXPECT_EQ("changed", copy.value());

    copy = copy;
    EXPECT_EQ("changed", copy.value()) << "Self copy assignment must be safe";

    Encryption::SafeString moved {std::move(copy)};
    EXPECT_EQ("changed", moved.value());

    Encryption::SafeString assigned;
    assigned = std::move(moved);
    EXPECT_EQ("changed", assigned.value());

    assigned = std::move(assigned);
    EXPECT_EQ("changed", assigned.value()) << "Self move assignment must be safe";

    EXPECT_EQ(std::string("changed").size(), assigned->size());
}

TEST(Components_Encryption, SafeString_AssignmentFromDifferentTypes)
{
    Encryption::SafeString value;
    value = std::string {"binary\0data", 11};
    EXPECT_EQ(std::string("binary\0data", 11), value.value());

    value = "literal";
    EXPECT_EQ("literal", value.value());

    value = std::string_view {"string-view"};
    EXPECT_EQ("string-view", value.value());
}

// ============================================== //
// SecureBuffer
// ============================================== //

TEST(Components_Encryption, SecureBuffer_ZeroInitializedAndWritable)
{
    Encryption::SecureBuffer buffer {BUFFER_LARGE_SIZE};

    ASSERT_EQ(BUFFER_LARGE_SIZE, buffer.size());
    ASSERT_NE(nullptr, buffer.data());
    ASSERT_EQ(BUFFER_LARGE_SIZE, buffer.view().size());

    for (std::size_t index = 0; index < buffer.size(); ++index) {
        EXPECT_EQ(0, buffer.data()[index]) << "Byte " << index << " must be zeroed";
    }

    for (std::size_t index = 0; index < buffer.size(); ++index) {
        buffer.data()[index] = BUFFER_FILL_VALUE;
    }

    for (std::size_t index = 0; index < buffer.view().size(); ++index) {
        EXPECT_EQ(BUFFER_FILL_VALUE, static_cast<std::uint8_t>(buffer.view()[index]))
            << "Byte " << index << " must be writable";
    }
}

TEST(Components_Encryption, SecureBuffer_Empty)
{
    Encryption::SecureBuffer buffer {0};

    EXPECT_EQ(nullptr, buffer.data());
    EXPECT_EQ(0u, buffer.size());
    EXPECT_TRUE(buffer.view().empty());
}

TEST(Components_Encryption, SecureBuffer_MoveSemantics)
{
    Encryption::SecureBuffer source {BUFFER_SMALL_SIZE};
    ASSERT_NE(nullptr, source.data());

    for (std::size_t index = 0; index < source.size(); ++index) {
        source.data()[index] = static_cast<std::uint8_t>(index);
    }
    const auto* originalData = source.data();

    Encryption::SecureBuffer moved {std::move(source)};
    EXPECT_EQ(originalData, moved.data());
    EXPECT_EQ(BUFFER_SMALL_SIZE, moved.size());
    EXPECT_EQ(nullptr, source.data());
    EXPECT_EQ(0u, source.size());
    EXPECT_TRUE(source.view().empty());

    for (std::size_t index = 0; index < moved.size(); ++index) {
        EXPECT_EQ(static_cast<std::uint8_t>(index), moved.data()[index]);
    }

    Encryption::SecureBuffer target {BUFFER_SMALL_SIZE * 2};
    ASSERT_NE(originalData, target.data());

    target = std::move(moved);
    EXPECT_EQ(originalData, target.data());
    EXPECT_EQ(BUFFER_SMALL_SIZE, target.size());
    EXPECT_EQ(nullptr, moved.data());
    EXPECT_EQ(0u, moved.size());

    target = std::move(target);
    EXPECT_EQ(originalData, target.data()) << "Self move assignment must be safe";
    EXPECT_EQ(BUFFER_SMALL_SIZE, target.size());
}

// ============================================== //
// Process memory safety
// ============================================== //

TEST(Components_Encryption, SecureMemory_LockAndUnlock)
{
    std::array<std::uint8_t, BUFFER_SMALL_SIZE> data {};

    EXPECT_FALSE(Encryption::lockSecureMemory(nullptr, data.size()));
    EXPECT_FALSE(Encryption::lockSecureMemory(data.data(), 0));

    const bool locked = Encryption::lockSecureMemory(data.data(), data.size());
    if (!locked) {
        GTEST_SKIP() << "mlock is not permitted in the current environment";
    }

    EXPECT_NO_THROW(Encryption::unlockSecureMemory(data.data(), data.size()));
    EXPECT_NO_THROW(Encryption::unlockSecureMemory(nullptr, data.size()));
    EXPECT_NO_THROW(Encryption::unlockSecureMemory(data.data(), 0));
}

TEST(Components_Encryption, DisableCoreDumps)
{
    Encryption::disableCoreDumps();

#if !defined(_WIN32)
    struct rlimit limit {};
    ASSERT_EQ(0, getrlimit(RLIMIT_CORE, &limit));
    EXPECT_EQ(static_cast<rlim_t>(0), limit.rlim_cur);
    EXPECT_EQ(static_cast<rlim_t>(0), limit.rlim_max);

    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    (void)setrlimit(RLIMIT_CORE, &limit);
#else
    SUCCEED();
#endif
}
