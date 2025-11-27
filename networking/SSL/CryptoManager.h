#pragma once
#include <string>
#include <vector>

namespace smtp::ssl {
  class CryptoManager {
  public:
    explicit CryptoManager(std::vector<unsigned char> sessionKey);

    std::vector<unsigned char> encrypt(const std::string &plaintext) const;
    std::string decrypt(const std::vector<unsigned char> &ciphertext) const;

  private:
    std::vector<unsigned char> sessionKey;

    static std::vector<unsigned char> generateIV();

    static std::vector<unsigned char> base64Encode(const std::vector<unsigned char> &data);
    static std::vector<unsigned char> base64Decode(const std::vector<unsigned char> &encoded);
  };
}
