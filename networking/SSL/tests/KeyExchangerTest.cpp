#include <gtest/gtest.h>
#include "KeyExchanger.h"

using namespace smtp::ssl;

TEST(KeyExchangeTest, GenerateKeyPair) {
  auto [privateKey, publicKey] = KeyExchange::generateKeyPair();

  EXPECT_FALSE(privateKey.empty());
  EXPECT_FALSE(publicKey.empty());
  EXPECT_NE(privateKey, publicKey);
}

TEST(KeyExchangeTest, DHExchangeProducesSharedSecret) {
  auto [privKey1, pubKey1] = KeyExchange::generateKeyPair();
  auto [privKey2, pubKey2] = KeyExchange::generateKeyPair();

  const auto secret1 = KeyExchange::performDHExchange(pubKey2, privKey1);
  const auto secret2 = KeyExchange::performDHExchange(pubKey1, privKey2);

  EXPECT_EQ(secret1, secret2);
}

TEST(KeyExchangeTest, DeriveSessionKey) {
  const std::vector<unsigned char> sharedSecret(256, 0x42);

  const auto sessionKey = KeyExchange::deriveSessionKey(sharedSecret, 32);

  EXPECT_EQ(32u, sessionKey.size());
}

TEST(KeyExchangeTest, DeriveSessionKeyDeterministic) {
  const std::vector<unsigned char> sharedSecret(256, 0x42);

  const auto key1 = KeyExchange::deriveSessionKey(sharedSecret, 32);
  const auto key2 = KeyExchange::deriveSessionKey(sharedSecret, 32);

  EXPECT_EQ(key1, key2);
}

TEST(KeyExchangeTest, InvalidPeerPublicKey) {
  auto [privKey, pubKey] = KeyExchange::generateKeyPair();
  const std::vector<unsigned char> invalidKey(10, 0xFF);

  EXPECT_THROW(KeyExchange::performDHExchange(invalidKey, privKey), std::runtime_error);
}

TEST(KeyExchangeTest, InvalidPrivateKey) {
  auto [privKey, pubKey] = KeyExchange::generateKeyPair();
  const std::vector<unsigned char> invalidPrivKey(10, 0xFF);

  EXPECT_THROW(KeyExchange::performDHExchange(pubKey, invalidPrivKey), std::runtime_error);
}

TEST(KeyExchangeTest, EmptyPeerPublicKey) {
  auto [privKey, pubKey] = KeyExchange::generateKeyPair();
  const std::vector<unsigned char> emptyKey;

  EXPECT_THROW(KeyExchange::performDHExchange(emptyKey, privKey), std::runtime_error);
}

TEST(KeyExchangeTest, EmptyPrivateKey) {
  auto [privKey, pubKey] = KeyExchange::generateKeyPair();
  const std::vector<unsigned char> emptyKey;

  EXPECT_THROW(KeyExchange::performDHExchange(pubKey, emptyKey), std::runtime_error);
}

TEST(KeyExchangeTest, PrivateKeyCannotBeUsedAsPublicKey) {
  auto [privKey1, pubKey1] = KeyExchange::generateKeyPair();
  auto [privKey2, pubKey2] = KeyExchange::generateKeyPair();

  EXPECT_THROW(KeyExchange::performDHExchange(privKey2, privKey1), std::runtime_error);
}

TEST(KeyExchangeTest, PublicKeyCannotBeUsedAsPrivateKey) {
  auto [privKey1, pubKey1] = KeyExchange::generateKeyPair();
  auto [privKey2, pubKey2] = KeyExchange::generateKeyPair();

  EXPECT_THROW(KeyExchange::performDHExchange(pubKey2, pubKey1), std::runtime_error);
}

TEST(KeyExchangeTest, DifferentSharedSecretsProduceDifferentSessionKeys) {
  const std::vector<unsigned char> secret1(256, 0xAA);
  const std::vector<unsigned char> secret2(256, 0xBB);

  const auto key1 = KeyExchange::deriveSessionKey(secret1, 32);
  const auto key2 = KeyExchange::deriveSessionKey(secret2, 32);

  EXPECT_NE(key1, key2);
}

