#ifndef KEY_EXCHANGER_H
#define KEY_EXCHANGER_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <vector>

namespace smtp::ssl {
  class KeyExchange {
  public:
    static std::pair<std::vector<unsigned char>, std::vector<unsigned char>> generateKeyPair();

    static std::vector<unsigned char> performDHExchange(
      const std::vector<unsigned char> &peerPublicKey,
      const std::vector<unsigned char> &privateKey
    );

    static std::vector<unsigned char> deriveSessionKey(
      const std::vector<unsigned char> &sharedSecret,
      size_t keyLength = 32
    );

  private:
    static bool assignKey(std::vector<unsigned char>& keyContainer, const EVP_PKEY* pkey, bool isPublic);
  };
}

#endif