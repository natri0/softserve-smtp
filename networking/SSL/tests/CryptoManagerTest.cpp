#include <gtest/gtest.h>
#include "CryptoManager.h"

using namespace smtp::ssl;

class CryptoManagerTest : public ::testing::Test {
protected:
    std::vector<unsigned char> sessionKey{
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
};

TEST_F(CryptoManagerTest, ConstructorValidKey) {
    EXPECT_NO_THROW(CryptoManager manager(sessionKey));
}

TEST_F(CryptoManagerTest, ConstructorInvalidKeyLength) {
    const std::vector<unsigned char> invalidKey(16);
    EXPECT_THROW(CryptoManager manager(invalidKey), std::invalid_argument);
}

TEST_F(CryptoManagerTest, EncryptDecrypt) {
    const CryptoManager manager(sessionKey);
    const std::string plaintext = "Hello, SMTP!";
    
    const auto encrypted = manager.encrypt(plaintext);
    const auto decrypted = manager.decrypt(encrypted);
    
    EXPECT_EQ(plaintext, decrypted);
}

TEST_F(CryptoManagerTest, EncryptProducesDifferentCiphertext) {
    const CryptoManager manager(sessionKey);
    const std::string plaintext = "Test message";
    
    const auto encrypted1 = manager.encrypt(plaintext);
    const auto encrypted2 = manager.encrypt(plaintext);

    const auto decrypted1 = manager.decrypt(encrypted1);
    const auto decrypted2 = manager.decrypt(encrypted2);

    EXPECT_NE(encrypted1, encrypted2);
    EXPECT_EQ(decrypted1, decrypted2);
}

TEST_F(CryptoManagerTest, EncryptEmptyString) {
    const CryptoManager manager(sessionKey);
    const auto encrypted = manager.encrypt("");
    const auto decrypted = manager.decrypt(encrypted);
    
    EXPECT_EQ("", decrypted);
}

TEST_F(CryptoManagerTest, EncryptLargeMessage) {
    const CryptoManager manager(sessionKey);
    const std::string largeText(10000, 'A');
    
    const auto encrypted = manager.encrypt(largeText);
    const auto decrypted = manager.decrypt(encrypted);
    
    EXPECT_EQ(largeText, decrypted);
}

TEST_F(CryptoManagerTest, DecryptInvalidCiphertext) {
    const CryptoManager manager(sessionKey);
    const std::vector<unsigned char> invalidCiphertext(10, 0xFF);

    EXPECT_THROW(manager.decrypt(invalidCiphertext), std::invalid_argument);
}

TEST_F(CryptoManagerTest, EncryptSpecialCharacters) {
    const CryptoManager manager(sessionKey);
    const std::string plaintext = "Test\n\r\t@#$%^&*()";

    const auto encrypted = manager.encrypt(plaintext);
    const auto decrypted = manager.decrypt(encrypted);

    EXPECT_EQ(plaintext, decrypted);
}

TEST_F(CryptoManagerTest, EncryptUnicodeCharacters) {
    const CryptoManager manager(sessionKey);
    const std::string plaintext = "Hello мир 世界 🌍";

    const auto encrypted = manager.encrypt(plaintext);
    const auto decrypted = manager.decrypt(encrypted);

    EXPECT_EQ(plaintext, decrypted);
}

TEST_F(CryptoManagerTest, EncryptedDataIsBase64) {
    const CryptoManager manager(sessionKey);
    const std::string plaintext = "Test";

    for (const auto encrypted = manager.encrypt(plaintext); const auto byte : encrypted) {
        EXPECT_TRUE(
            (byte >= 'A' && byte <= 'Z') ||
            (byte >= 'a' && byte <= 'z') ||
            (byte >= '0' && byte <= '9') ||
            byte == '+' || byte == '/' || byte == '='
        ) << "Invalid Base64 character: " << static_cast<int>(byte);
    }
}