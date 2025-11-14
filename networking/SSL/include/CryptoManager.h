#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <string>
#include <vector>

namespace smtp::ssl {
  class CryptoManager {
  public:
    explicit CryptoManager(const std::vector<unsigned char> &sessionKey);

    std::vector<unsigned char> encrypt(const std::string &plaintext) const;
    std::string decrypt(const std::vector<unsigned char> &ciphertext) const;

  private:
    std::vector<unsigned char> sessionKey;
    std::vector<unsigned char> generateIV();
  };
}

#endif