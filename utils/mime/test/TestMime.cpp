#include "EmailBuilder.h"
#include "MimeParser.h"
#include "CryptoManager.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <cstring>

using ::testing::Contains;
using ::testing::HasSubstr;
using ::testing::SizeIs;

// ============================================================================
// Helper Functions
// ============================================================================

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file: " + path);

    return buffer;
}

std::vector<uint8_t> stringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

// ============================================================================
// EmailBuilder Tests
// ============================================================================

class EmailBuilderTest : public ::testing::Test {
protected:
    EmailBuilder builder;
};

TEST_F(EmailBuilderTest, BasicEmailCreation) {
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Test Subject")
           .body("Test Body");

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
    
    std::string mimeStr(mime.begin(), mime.end());
    EXPECT_THAT(mimeStr, HasSubstr("sender@example.com"));
    EXPECT_THAT(mimeStr, HasSubstr("recipient@example.com"));
    EXPECT_THAT(mimeStr, HasSubstr("Test Subject"));
    EXPECT_THAT(mimeStr, HasSubstr("Test Body"));
}

TEST_F(EmailBuilderTest, MultipleRecipients) {
    builder.from("sender@example.com")
           .to("recipient1@example.com")
           .to("recipient2@example.com")
           .to("recipient3@example.com")
           .subject("Multiple Recipients")
           .body("Body");

    auto mime = builder.buildBinary();
    std::string mimeStr(mime.begin(), mime.end());
    
    EXPECT_THAT(mimeStr, HasSubstr("recipient1@example.com"));
    EXPECT_THAT(mimeStr, HasSubstr("recipient2@example.com"));
    EXPECT_THAT(mimeStr, HasSubstr("recipient3@example.com"));
}

TEST_F(EmailBuilderTest, EmptyBody) {
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Empty Body Test")
           .body("");

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
}

TEST_F(EmailBuilderTest, UnicodeContent) {
    builder.from("тарас@example.com")
           .to("володимир@example.com")
           .subject("Привіт")
           .body("Це тестовий email українською мовою! 🚀");

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
    
    std::string mimeStr(mime.begin(), mime.end());
    // Unicode content should be present (possibly encoded)
    EXPECT_FALSE(mimeStr.empty());
}

TEST_F(EmailBuilderTest, SpecialCharactersInSubject) {
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Test <>&\"'")
           .body("Body");

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
}

TEST_F(EmailBuilderTest, LongBody) {
    std::string longBody(10000, 'A');
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Long Body")
           .body(longBody);

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
    EXPECT_GT(mime.size(), 10000);
}

TEST_F(EmailBuilderTest, SingleAttachment) {
    std::vector<uint8_t> fileData = {0x89, 0x50, 0x4E, 0x47}; // PNG header
    
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("With Attachment")
           .body("See attached file")
           .attachment("test.png", "image/png", "attachment", fileData);

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
    
    std::string mimeStr(mime.begin(), mime.end());
    EXPECT_THAT(mimeStr, HasSubstr("test.png"));
    EXPECT_THAT(mimeStr, HasSubstr("image/png"));
}

TEST_F(EmailBuilderTest, MultipleAttachments) {
    std::vector<uint8_t> pngData = {0x89, 0x50, 0x4E, 0x47};
    std::vector<uint8_t> pdfData = {0x25, 0x50, 0x44, 0x46}; // PDF header
    
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Multiple Files")
           .body("See attached files")
           .attachment("image.png", "image/png", "attachment", pngData)
           .attachment("document.pdf", "application/pdf", "attachment", pdfData);

    auto mime = builder.buildBinary();
    std::string mimeStr(mime.begin(), mime.end());
    
    EXPECT_THAT(mimeStr, HasSubstr("image.png"));
    EXPECT_THAT(mimeStr, HasSubstr("document.pdf"));
    EXPECT_THAT(mimeStr, HasSubstr("image/png"));
    EXPECT_THAT(mimeStr, HasSubstr("application/pdf"));
}

TEST_F(EmailBuilderTest, EmptyAttachment) {
    std::vector<uint8_t> emptyData;
    
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Empty File")
           .body("Empty attachment")
           .attachment("empty.txt", "text/plain", "attachment", emptyData);

    auto mime = builder.buildBinary();
    ASSERT_FALSE(mime.empty());
}

TEST_F(EmailBuilderTest, LargeAttachment) {
    std::vector<uint8_t> largeData(1024 * 1024, 0xAB); // 1MB
    
    builder.from("sender@example.com")
           .to("recipient@example.com")
           .subject("Large File")
           .body("Large attachment")
           .attachment("large.bin", "application/octet-stream", "attachment", largeData);

    auto mime = builder.buildBinary();
    EXPECT_GT(mime.size(), 1024 * 1024);
}

// ============================================================================
// MimeParser Tests
// ============================================================================

class MimeParserTest : public ::testing::Test {
protected:
    MimeParser parser;
    EmailBuilder builder;
};

TEST_F(MimeParserTest, ParseBasicEmail) {
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Parse Test")
           .body("Test Body Content");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_EQ(message.getFrom(), "sender@test.com");
    EXPECT_THAT(message.getTo(), Contains("recipient@test.com"));
    EXPECT_EQ(message.getSubject(), "Parse Test");
    EXPECT_EQ(message.getBody(), "Test Body Content");
}

TEST_F(MimeParserTest, ParseMultipleRecipients) {
    builder.from("sender@test.com")
           .to("recipient1@test.com")
           .to("recipient2@test.com")
           .subject("Multi Recipient")
           .body("Body");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_THAT(message.getTo(), SizeIs(2));
    EXPECT_THAT(message.getTo(), Contains("recipient1@test.com"));
    EXPECT_THAT(message.getTo(), Contains("recipient2@test.com"));
}

TEST_F(MimeParserTest, RoundTripConsistency) {
    builder.from("taras@gmail.com")
           .to("volodymyr@gmail.com")
           .to("Valik@gmail.com")
           .subject("Hello")
           .body("This is a test email.");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_EQ(message.getFrom(), "taras@gmail.com");
    EXPECT_THAT(message.getTo(), SizeIs(2));
    EXPECT_EQ(message.getSubject(), "Hello");
    EXPECT_EQ(message.getBody(), "This is a test email.");
}

TEST_F(MimeParserTest, ParseEmptyBody) {
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Empty")
           .body("");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_EQ(message.getBody(), "");
}

TEST_F(MimeParserTest, ContentTypeExtraction) {
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Content Type Test")
           .body("Body");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_FALSE(message.getContentType().empty());
}

TEST_F(MimeParserTest, ParseAttachment) {
    std::vector<uint8_t> fileData = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A}; // PNG header
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("With File")
           .body("See attachment")
           .attachment("test.png", "image/png", "attachment", fileData);

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_FALSE(message.getAttachments().empty());
    EXPECT_EQ(message.getAttachments().size(), 1);
    
    const auto& attachment = message.getAttachments()[0];
    EXPECT_EQ(attachment.getFileName(), "test.png");
    EXPECT_EQ(attachment.getMimeType(), "image/png");
    EXPECT_EQ(attachment.getContentDisposition(), "attachment");
    EXPECT_FALSE(attachment.getData().empty());
}

TEST_F(MimeParserTest, ParseMultipleAttachments) {
    std::vector<uint8_t> png = {0x89, 0x50, 0x4E, 0x47};
    std::vector<uint8_t> pdf = {0x25, 0x50, 0x44, 0x46};
    std::vector<uint8_t> txt = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Multiple Files")
           .body("Three attachments")
           .attachment("image.png", "image/png", "attachment", png)
           .attachment("doc.pdf", "application/pdf", "attachment", pdf)
           .attachment("readme.txt", "text/plain", "attachment", txt);

    auto mime = builder.buildBinary();
    std::string mimeString(reinterpret_cast<const char*>(mime.data()), mime.size());
    EmailMessage message = parser.parse(mime);

    EXPECT_EQ(message.getAttachments().size(), 3);
    
    bool foundPng = false, foundPdf = false, foundTxt = false;
    for (const auto& att : message.getAttachments()) {
        
        if (att.getFileName() == "image.png") {
            foundPng = true;
            EXPECT_EQ(att.getMimeType(), "image/png");
        }
        if (att.getFileName() == "doc.pdf") {
            foundPdf = true;
            EXPECT_EQ(att.getMimeType(), "application/pdf");
        }
        if (att.getFileName() == "readme.txt") {
            foundTxt = true;
            EXPECT_EQ(att.getMimeType(), "text/plain");
        }
    }
    
    EXPECT_TRUE(foundPng);
    EXPECT_TRUE(foundPdf);
    EXPECT_TRUE(foundTxt);
}

TEST_F(MimeParserTest, AttachmentDataIntegrity) {
    std::vector<uint8_t> originalData;
    for (int i = 0; i < 256; i++) {
        originalData.push_back(static_cast<uint8_t>(i));
    }
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Data Integrity")
           .body("Binary data")
           .attachment("binary.bin", "application/octet-stream", "attachment", originalData);

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    ASSERT_EQ(message.getAttachments().size(), 1);
    
    const auto& attachment = message.getAttachments()[0];
    EXPECT_EQ(attachment.getFileName(), "binary.bin");
    EXPECT_EQ(attachment.getMimeType(), "application/octet-stream");
    EXPECT_EQ(attachment.getData().size(), originalData.size());
    EXPECT_EQ(attachment.getData(), originalData);
}

TEST_F(MimeParserTest, EmailWithoutAttachments) {
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("No Files")
           .body("Just text");

    auto mime = builder.buildBinary();
    EmailMessage message = parser.parse(mime);

    EXPECT_TRUE(message.getAttachments().empty());
}

// ============================================================================
// CryptoManager Tests
// ============================================================================

class CryptoManagerTest : public ::testing::Test {
protected:
    std::vector<unsigned char> sessionKey;
    
    void SetUp() override {
        sessionKey = std::vector<unsigned char>(32, 0x01);
    }
};

TEST_F(CryptoManagerTest, EncryptDecryptBasic) {
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string original = "Hello, World!";
    
    auto encrypted = crypto.encrypt(original);
    ASSERT_FALSE(encrypted.empty());
    EXPECT_NE(encrypted.size(), original.size()); // Should have padding/overhead
    
    auto decrypted = crypto.decrypt(encrypted);
    EXPECT_EQ(decrypted, original);
}

TEST_F(CryptoManagerTest, EncryptDecryptEmptyString) {
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string original = "";
    
    auto encrypted = crypto.encrypt(original);
    auto decrypted = crypto.decrypt(encrypted);
    
    EXPECT_EQ(decrypted, original);
}

TEST_F(CryptoManagerTest, EncryptDecryptLargeData) {
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string original(100000, 'X');
    
    auto encrypted = crypto.encrypt(original);
    auto decrypted = crypto.decrypt(encrypted);
    
    EXPECT_EQ(decrypted, original);
}

TEST_F(CryptoManagerTest, EncryptDecryptBinaryData) {
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string original;
    for (int i = 0; i < 256; i++) {
        original += static_cast<char>(i);
    }
    
    auto encrypted = crypto.encrypt(original);
    auto decrypted = crypto.decrypt(encrypted);
    
    EXPECT_EQ(decrypted, original);
}

TEST_F(CryptoManagerTest, DifferentKeysProduceDifferentCiphertext) {
    std::vector<unsigned char> key1(32, 0x01);
    std::vector<unsigned char> key2(32, 0x02);
    
    smtp::ssl::CryptoManager crypto1(key1);
    smtp::ssl::CryptoManager crypto2(key2);
    
    std::string original = "Test message";
    
    auto encrypted1 = crypto1.encrypt(original);
    auto encrypted2 = crypto2.encrypt(original);
    
    EXPECT_NE(encrypted1, encrypted2);
}

// ============================================================================
// Integration Tests
// ============================================================================

class IntegrationTest : public ::testing::Test {
protected:
    EmailBuilder builder;
    MimeParser parser;
    std::vector<unsigned char> sessionKey;
    
    void SetUp() override {
        sessionKey = std::vector<unsigned char>(32, 0x01);
    }
};

TEST_F(IntegrationTest, FullEmailFlowWithEncryption) {
    // Build email
    builder.from("taras@gmail.com")
           .to("volodymyr@gmail.com")
           .to("Valik@gmail.com")
           .subject("Hello")
           .body("This is a test email.");

    auto mimeBinary = builder.buildBinary();
    ASSERT_FALSE(mimeBinary.empty());

    // Encrypt
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string mimeString(mimeBinary.begin(), mimeBinary.end());
    auto encrypted = crypto.encrypt(mimeString);
    ASSERT_FALSE(encrypted.empty());

    // Decrypt
    auto decrypted = crypto.decrypt(encrypted);
    std::vector<uint8_t> decryptedMime(decrypted.begin(), decrypted.end());

    // Verify integrity
    ASSERT_EQ(decryptedMime.size(), mimeBinary.size());
    EXPECT_EQ(std::memcmp(decryptedMime.data(), mimeBinary.data(), mimeBinary.size()), 0);

    // Parse decrypted
    EmailMessage message = parser.parse(decryptedMime);
    
    EXPECT_EQ(message.getFrom(), "taras@gmail.com");
    EXPECT_THAT(message.getTo(), SizeIs(2));
    EXPECT_THAT(message.getTo(), Contains("volodymyr@gmail.com"));
    EXPECT_THAT(message.getTo(), Contains("Valik@gmail.com"));
    EXPECT_EQ(message.getSubject(), "Hello");
    EXPECT_EQ(message.getBody(), "This is a test email.");
}

TEST_F(IntegrationTest, UnicodeEmailWithEncryption) {
    builder.from("іван@укр.net")
           .to("петро@укр.net")
           .subject("Тестовий лист")
           .body("Привіт! Це тестове повідомлення.");

    auto mimeBinary = builder.buildBinary();
    
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string mimeString(mimeBinary.begin(), mimeBinary.end());
    auto encrypted = crypto.encrypt(mimeString);
    auto decrypted = crypto.decrypt(encrypted);
    std::vector<uint8_t> decryptedMime(decrypted.begin(), decrypted.end());

    EmailMessage message = parser.parse(decryptedMime);
    
    EXPECT_FALSE(message.getFrom().empty());
    EXPECT_FALSE(message.getTo().empty());
    EXPECT_FALSE(message.getSubject().empty());
    EXPECT_FALSE(message.getBody().empty());
}

TEST_F(IntegrationTest, MultipleEncryptionCycles) {
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Cycle Test")
           .body("Testing multiple encryption cycles");

    auto mimeBinary = builder.buildBinary();
    smtp::ssl::CryptoManager crypto(sessionKey);
    
    std::string current(mimeBinary.begin(), mimeBinary.end());
    
    // Encrypt and decrypt 5 times
    for (int i = 0; i < 5; i++) {
        auto encrypted = crypto.encrypt(current);
        current = crypto.decrypt(encrypted);
    }
    
    std::vector<uint8_t> finalMime(current.begin(), current.end());
    
    // Verify no data corruption
    ASSERT_EQ(finalMime.size(), mimeBinary.size());
    EXPECT_EQ(std::memcmp(finalMime.data(), mimeBinary.data(), mimeBinary.size()), 0);
    
    EmailMessage message = parser.parse(finalMime);
    EXPECT_EQ(message.getSubject(), "Cycle Test");
}

TEST_F(IntegrationTest, AttachmentWithEncryption) {
    std::vector<uint8_t> fileData = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}; // PNG
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Encrypted File")
           .body("See encrypted attachment")
           .attachment("secret.png", "image/png", "attachment", fileData);

    auto mimeBinary = builder.buildBinary();
    
    // Encrypt
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string mimeString(reinterpret_cast<const char*>(mimeBinary.data()), mimeBinary.size());
    auto encrypted = crypto.encrypt(mimeString);
    
    // Decrypt
    auto decrypted = crypto.decrypt(encrypted);
    std::vector<uint8_t> decryptedMime(decrypted.begin(), decrypted.end());
    
    // Parse
    EmailMessage message = parser.parse(decryptedMime);
    
    EXPECT_EQ(message.getSubject(), "Encrypted File");
    ASSERT_EQ(message.getAttachments().size(), 1);
    
    const auto& attachment = message.getAttachments()[0];
    EXPECT_EQ(attachment.getFileName(), "secret.png");
    EXPECT_EQ(attachment.getMimeType(), "image/png");
    EXPECT_EQ(attachment.getData(), fileData);
}

TEST_F(IntegrationTest, MultipleAttachmentsWithEncryption) {
    std::vector<uint8_t> png = {0x89, 0x50, 0x4E, 0x47};
    std::vector<uint8_t> pdf = {0x25, 0x50, 0x44, 0x46};
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Multiple Encrypted Files")
           .body("Two encrypted files")
           .attachment("image.png", "image/png", "attachment", png)
           .attachment("doc.pdf", "application/pdf", "attachment", pdf);

    auto mimeBinary = builder.buildBinary();
    
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string mimeString(mimeBinary.begin(), mimeBinary.end());
    auto encrypted = crypto.encrypt(mimeString);
    auto decrypted = crypto.decrypt(encrypted);
    std::vector<uint8_t> decryptedMime(decrypted.begin(), decrypted.end());
    
    EmailMessage message = parser.parse(decryptedMime);
    
    ASSERT_EQ(message.getAttachments().size(), 2);
    
    for (const auto& att : message.getAttachments()) {
        if (att.getFileName() == "image.png") {
            EXPECT_EQ(att.getData(), png);
            EXPECT_EQ(att.getMimeType(), "image/png");
        } else if (att.getFileName() == "doc.pdf") {
            EXPECT_EQ(att.getData(), pdf);
            EXPECT_EQ(att.getMimeType(), "application/pdf");
        }
    }
}

TEST_F(IntegrationTest, LargeAttachmentIntegrity) {

    std::vector<uint8_t> largeFile(100000);
    for (size_t i = 0; i < largeFile.size(); i++) {
        largeFile[i] = static_cast<uint8_t>(i % 256);
    }
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Large File Test")
           .body("Large attachment")
           .attachment("large.bin", "application/octet-stream", "attachment", largeFile);

    auto mimeBinary = builder.buildBinary();
    
    // Encrypt/Decrypt
    smtp::ssl::CryptoManager crypto(sessionKey);
    std::string mimeString(mimeBinary.begin(), mimeBinary.end());
    auto encrypted = crypto.encrypt(mimeString);
    auto decrypted = crypto.decrypt(encrypted);
    std::vector<uint8_t> decryptedMime(decrypted.begin(), decrypted.end());
    
    // Parse and verify
    EmailMessage message = parser.parse(decryptedMime);
    
    ASSERT_EQ(message.getAttachments().size(), 1);
    
    const auto& attachment = message.getAttachments()[0];
    EXPECT_EQ(attachment.getFileName(), "large.bin");
    EXPECT_EQ(attachment.getMimeType(), "application/octet-stream");
    EXPECT_EQ(attachment.getData().size(), largeFile.size());
    EXPECT_EQ(attachment.getData(), largeFile);
}

// ============================================================================
// Attachment Edge Cases Tests
// ============================================================================

TEST_F(IntegrationTest, AttachmentWithSpecialCharactersInFilename) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03};
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Special Filename")
           .body("Test")
           .attachment("файл тест.txt", "text/plain", "attachment", data);

    auto mimeBinary = builder.buildBinary();
    EmailMessage message = parser.parse(mimeBinary);
    
    ASSERT_EQ(message.getAttachments().size(), 1);
    EXPECT_FALSE(message.getAttachments()[0].getFileName().empty());
}

TEST_F(IntegrationTest, AttachmentContentDisposition) {
    std::vector<uint8_t> data = {0xAA, 0xBB};
    
    builder.from("sender@test.com")
           .to("recipient@test.com")
           .subject("Disposition Test")
           .body("Test")
           .attachment("inline.jpg", "image/jpeg", "attachment", data);

    auto mimeBinary = builder.buildBinary();
    EmailMessage message = parser.parse(mimeBinary);
    
    ASSERT_EQ(message.getAttachments().size(), 1);
    const auto& att = message.getAttachments()[0];
    EXPECT_EQ(att.getContentDisposition(), "attachment");
    EXPECT_EQ(att.getMimeType(), "image/jpeg");
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}