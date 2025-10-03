#include <AppInfrastructure/Encryption.h>
#include <Components/Logger/Logger.h>
#include <gtest/gtest.h>

#include <QString>

TEST(Common, EncryptionShortKey) {
    QString testStringShort = "My example string";
    QString testStringAverage =
        "My example string of text to encrypt. Size is big";
    QString testStringLong =
        "My example string of text to encrypt. Size is big, but it's just "
        "bytes, so ez to encrypt, yeah";

    QString keyString = "The key";

    auto encryptedText = Encryption::encryptAes256Cbc(testStringShort.toUtf8(),
                                                      keyString.toUtf8());
    auto decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringShort.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringAverage.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringAverage.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringLong.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringLong.toUtf8(), decryptedText);
}

TEST(Common, EncryptionRegularKey) {
    QString testStringShort = "My example string";
    QString testStringAverage =
        "My example string of text to encrypt. Size is big";
    QString testStringLong =
        "My example string of text to encrypt. Size is big, but it's just "
        "bytes, so ez to encrypt, yeah";

    QString keyString = "01234567890123456789012345678901";  // 32 byte

    auto encryptedText = Encryption::encryptAes256Cbc(testStringShort.toUtf8(),
                                                      keyString.toUtf8());
    auto decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringShort.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringAverage.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringAverage.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringLong.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringLong.toUtf8(), decryptedText);
}

TEST(Common, EncryptionLongKey) {
    QString testStringShort = "My example string";
    QString testStringAverage =
        "My example string of text to encrypt. Size is big";
    QString testStringLong =
        "My example string of text to encrypt. Size is big, but it's just "
        "bytes, so ez to encrypt, yeah";

    QString keyString =
        "The key is too long for AES to work with, need to process by other "
        "methogs";

    auto encryptedText = Encryption::encryptAes256Cbc(testStringShort.toUtf8(),
                                                      keyString.toUtf8());
    auto decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringShort.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringAverage.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringAverage.toUtf8(), decryptedText);

    encryptedText = decryptedText = {};
    encryptedText = Encryption::encryptAes256Cbc(testStringLong.toUtf8(),
                                                 keyString.toUtf8());
    decryptedText =
        Encryption::decryptAes256Cbc(encryptedText, keyString.toUtf8());
    EXPECT_EQ(testStringLong.toUtf8(), decryptedText);
}
