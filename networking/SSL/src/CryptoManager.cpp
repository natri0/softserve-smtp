#include "../include/CryptoManager.h"
#include "../../utils/Base64.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <cstring>

namespace smtp::ssl {
  CryptoManager::CryptoManager(const std::vector<unsigned char> &sessionKey) : sessionKey(sessionKey) {
    if (sessionKey.size() != 32) {
      throw std::invalid_argument("Session key must be 32 bytes");
    }
  }

  std::vector<unsigned char> CryptoManager::generateIV() {
    std::vector<unsigned char> iv(16);
    if (RAND_bytes(iv.data(), static_cast<int>(iv.size())) != 1) {
      throw std::runtime_error("Failed to generate IV");
    }
    return iv;
  }

  std::vector<unsigned char> CryptoManager::encrypt(const std::string &plaintext) const {
    const std::vector<unsigned char> data(plaintext.begin(), plaintext.end());

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
      throw std::runtime_error("Failed to create cipher context");
    }

    const auto iv = generateIV();

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, sessionKey.data(), iv.data()) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Failed to initialize encryption");
    }

    // IV + ciphertext + padding
    std::vector<unsigned char> ciphertext(iv.size() + data.size() + 16);
    std::memcpy(ciphertext.data(), iv.data(), iv.size());

    int len = 0;
    int ciphertext_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data() + iv.size(), &len, data.data(), static_cast<int>(data.size())) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Encryption failed");
    }
    ciphertext_len = len;

    // Padding
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + iv.size() + len, &len) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Encryption finalization failed");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    // Remove unused
    ciphertext.resize(iv.size() + ciphertext_len);
    return Base64::Encode(ciphertext);
  }

  std::string CryptoManager::decrypt(const std::vector<unsigned char> &ciphertext) const {
    const auto binaryCiphertext = Base64::Decode(ciphertext);
    if (binaryCiphertext.size() < 16) {
      throw std::invalid_argument("Ciphertext too short");
    }

    const std::vector iv(binaryCiphertext.begin(), binaryCiphertext.begin() + 16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
      throw std::runtime_error("Failed to create cipher context");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, sessionKey.data(), iv.data()) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Failed to initialize decryption");
    }

    std::vector<unsigned char> plaintext(binaryCiphertext.size());
    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, binaryCiphertext.data() + 16, static_cast<int>(binaryCiphertext.size()) - 16) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Decryption failed");
    }
    plaintext_len = len;

    // Remove padding
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
      EVP_CIPHER_CTX_free(ctx);
      throw std::runtime_error("Decryption finalization failed");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintext_len);
    return std::string(plaintext.begin(), plaintext.end());
  }
}
