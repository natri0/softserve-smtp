#include "KeyExchanger.h"
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/kdf.h>

namespace smtp::ssl {
  std::pair<std::vector<unsigned char>, std::vector<unsigned char>> KeyExchange::generateKeyPair() {

    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_DH, nullptr);
    if (!pctx) throw std::runtime_error("Failed to create DH context");

    if (EVP_PKEY_paramgen_init(pctx) != 1) {
      EVP_PKEY_CTX_free(pctx);
      throw std::runtime_error("Failed to init DH paramgen");
    }

    if (EVP_PKEY_CTX_set_dh_paramgen_prime_len(pctx, 2048) != 1) {
      EVP_PKEY_CTX_free(pctx);
      throw std::runtime_error("Failed to set DH prime length");
    }

    EVP_PKEY *params = nullptr;
    if (EVP_PKEY_paramgen(pctx, &params) != 1) {
      EVP_PKEY_CTX_free(pctx);
      throw std::runtime_error("Failed to generate DH params");
    }
    EVP_PKEY_CTX_free(pctx);

    EVP_PKEY_CTX *kctx = EVP_PKEY_CTX_new(params, nullptr);
    if (!kctx) {
      EVP_PKEY_free(params);
      throw std::runtime_error("Failed to create key context");
    }

    if (EVP_PKEY_keygen_init(kctx) != 1) {
      EVP_PKEY_CTX_free(kctx);
      EVP_PKEY_free(params);
      throw std::runtime_error("Failed to init keygen");
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(kctx, &pkey) != 1) {
      EVP_PKEY_CTX_free(kctx);
      EVP_PKEY_free(params);
      throw std::runtime_error("Failed to generate key pair");
    }

    EVP_PKEY_CTX_free(kctx);
    EVP_PKEY_free(params);

    std::vector<unsigned char> privateKey;
    if (!assignKey(privateKey, pkey, false)) {
      throw std::runtime_error("Failed to extract private key");
    }

    std::vector<unsigned char> publicKey;
    if (!assignKey(publicKey, pkey, true)) {
      throw std::runtime_error("Failed to extract public key");
    }

    EVP_PKEY_free(pkey);
    return {privateKey, publicKey};
  }

  bool KeyExchange::assignKey(std::vector<unsigned char>& keyContainer, EVP_PKEY* pkey, const bool isPublic) {
    unsigned char *buf = nullptr;
    int len;

    if (isPublic)
      len = i2d_PUBKEY(pkey, &buf);
    else
      len = i2d_PrivateKey(pkey, &buf);

    if (len < 0) {
      EVP_PKEY_free(pkey);
      return false;
    }
    keyContainer.assign(buf, buf + len);
    OPENSSL_free(buf);
    return true;
  }

  std::vector<unsigned char> KeyExchange::performDHExchange(
    const std::vector<unsigned char> &peerPublicKey,
    const std::vector<unsigned char> &privateKey)
  {
    const unsigned char *priv_ptr = privateKey.data();
    EVP_PKEY *privKey = d2i_PrivateKey(EVP_PKEY_DH, nullptr, &priv_ptr, static_cast<long>(privateKey.size()));
    if (!privKey) {
      throw std::runtime_error("Failed to load private key");
    }

    const unsigned char *pub_ptr = peerPublicKey.data();
    EVP_PKEY *peerKey = d2i_PUBKEY(nullptr, &pub_ptr, static_cast<long>(peerPublicKey.size()));
    if (!peerKey) {
      EVP_PKEY_free(privKey);
      throw std::runtime_error("Failed to load peer public key");
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(privKey, nullptr);
    if (!ctx) {
      EVP_PKEY_free(privKey);
      EVP_PKEY_free(peerKey);
      throw std::runtime_error("Failed to create derivation context");
    }

    if (EVP_PKEY_derive_init(ctx) != 1) {
      EVP_PKEY_CTX_free(ctx);
      EVP_PKEY_free(privKey);
      EVP_PKEY_free(peerKey);
      throw std::runtime_error("Failed to init key derivation");
    }

    if (EVP_PKEY_derive_set_peer(ctx, peerKey) != 1) {
      EVP_PKEY_CTX_free(ctx);
      EVP_PKEY_free(privKey);
      EVP_PKEY_free(peerKey);
      throw std::runtime_error("Failed to set peer key");
    }

    size_t secretLen = 256;
    std::vector<unsigned char> sharedSecret(secretLen);
    if (EVP_PKEY_derive(ctx, sharedSecret.data(), &secretLen) != 1) {
      EVP_PKEY_CTX_free(ctx);
      EVP_PKEY_free(privKey);
      EVP_PKEY_free(peerKey);
      throw std::runtime_error("Failed to derive shared secret");
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(privKey);
    EVP_PKEY_free(peerKey);

    sharedSecret.resize(secretLen);
    return sharedSecret;
  }

  std::vector<unsigned char> KeyExchange::deriveSessionKey(const std::vector<unsigned char> &sharedSecret, const size_t keyLength) {
    EVP_KDF *kdf = EVP_KDF_fetch(nullptr, "HKDF", nullptr);
    if (!kdf) throw std::runtime_error("Failed to fetch HKDF");

    EVP_KDF_CTX *kctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);
    if (!kctx) throw std::runtime_error("Failed to create KDF context");

    const char *digest = "SHA256";
    const char *salt = "smtp-ssl-salt";
    const char *info = "session-key";

    OSSL_PARAM params[] = {
      OSSL_PARAM_construct_utf8_string("digest", const_cast<char *>(digest), 0),
      OSSL_PARAM_construct_octet_string("key", const_cast<unsigned char *>(sharedSecret.data()), sharedSecret.size()),
      OSSL_PARAM_construct_octet_string("salt", const_cast<char *>(salt), std::strlen(salt)),
      OSSL_PARAM_construct_octet_string("info", const_cast<char *>(info), std::strlen(info)),
      OSSL_PARAM_construct_end()
    };

    std::vector<unsigned char> sessionKey(keyLength);
    if (EVP_KDF_derive(kctx, sessionKey.data(), keyLength, params) != 1) {
      EVP_KDF_CTX_free(kctx);
      throw std::runtime_error("Key derivation failed");
    }

    EVP_KDF_CTX_free(kctx);
    return sessionKey;
  }
}
