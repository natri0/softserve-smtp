#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <string>
#include <vector>

namespace smtp::ssl {
  /**
   * @brief Manages AES-256-CBC encryption and decryption with session keys.
   */
  class CryptoManager {
  public:
    /**
     * @brief Constructs a CryptoManager with the given session key.
     * @param sessionKey A 32-byte encryption key.
     * @throws std::invalid_argument if sessionKey is not 32 bytes.
     */
    explicit CryptoManager(const std::vector<unsigned char> &sessionKey);

    /**
     * @brief Encrypts plaintext using AES-256-CBC and returns Base64-encoded ciphertext.
     * @param plaintext The data to encrypt.
     * @return Base64-encoded ciphertext with prepended IV.
     */
    std::vector<unsigned char> encrypt(const std::string &plaintext) const;

    /**
     * @brief Decrypts Base64-encoded ciphertext using AES-256-CBC.
     * @param ciphertext Base64-encoded encrypted data with IV.
     * @return The decrypted plaintext string.
     */
    std::string decrypt(const std::vector<unsigned char> &ciphertext) const;

  private:
    std::vector<unsigned char> sessionKey;

    /**
     * @brief Generates a random 16-byte initialization vector.
     * @return A cryptographically secure random IV.
     */
    static std::vector<unsigned char> generateIV();
  };
}

#endif