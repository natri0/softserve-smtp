#ifndef KEY_EXCHANGER_H
#define KEY_EXCHANGER_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <vector>

namespace smtp::ssl {
  /**
   * @brief Handles Diffie-Hellman key exchange and session key derivation.
   */
  class KeyExchange {
  public:
    /**
     * @brief Generates a DH key pair using RFC 5114 group 2.
     * @return A pair of {privateKey, publicKey} as byte vectors.
     */
    static std::pair<std::vector<unsigned char>, std::vector<unsigned char>> generateKeyPair();

    /**
     * @brief Performs DH key exchange to compute a shared secret.
     * @param peerPublicKey The peer's public key in DER format.
     * @param privateKey Your private key in DER format.
     * @return The computed shared secret.
     */
    static std::vector<unsigned char> performDHExchange(
      const std::vector<unsigned char> &peerPublicKey,
      const std::vector<unsigned char> &privateKey
    );

    /**
     * @brief Derives a session key from the shared secret using HKDF-SHA256.
     * @param sharedSecret The DH-derived shared secret.
     * @param keyLength Desired key length in bytes (default: 32).
     * @return A derived session key suitable for AES encryption.
     */
    static std::vector<unsigned char> deriveSessionKey(
      const std::vector<unsigned char> &sharedSecret,
      size_t keyLength = 32
    );

  private:
    /**
     * @brief Extracts a key (public or private) from an EVP_PKEY into a byte vector.
     * @param keyContainer Output vector to store the key.
     * @param pkey OpenSSL EVP_PKEY structure.
     * @param isPublic True for public key, false for private key.
     * @return True on success, false on failure.
     */
    static bool assignKey(std::vector<unsigned char>& keyContainer, const EVP_PKEY* pkey, bool isPublic);
  };
}

#endif