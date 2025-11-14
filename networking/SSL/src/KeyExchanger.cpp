#include "../include/KeyExchanger.h"
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/kdf.h>

namespace smtp::ssl {
  struct EVPCipherCtxDeleter {void operator()(EVP_CIPHER_CTX* ctx) const {EVP_CIPHER_CTX_free(ctx);}};
  struct EVPPkeyDeleter      {void operator()(EVP_PKEY* pkey)      const {EVP_PKEY_free(pkey);}};
  struct EVPPkeyCtxDeleter   {void operator()(EVP_PKEY_CTX* ctx)   const {EVP_PKEY_CTX_free(ctx);}};
  struct EVPKdfDeleter       {void operator()(EVP_KDF* kdf)        const {EVP_KDF_free(kdf);}};
  struct EVPKdfCtxDeleter    {void operator()(EVP_KDF_CTX* ctx)    const {EVP_KDF_CTX_free(ctx);}};

  using CipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, EVPCipherCtxDeleter>;
  using PkeyPtr = std::unique_ptr<EVP_PKEY, EVPPkeyDeleter>;
  using PkeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, EVPPkeyCtxDeleter>;
  using KdfPtr = std::unique_ptr<EVP_KDF, EVPKdfDeleter>;
  using KdfCtxPtr = std::unique_ptr<EVP_KDF_CTX, EVPKdfCtxDeleter>;

  std::pair<std::vector<unsigned char>, std::vector<unsigned char>> KeyExchange::generateKeyPair() {
    const PkeyCtxPtr pctx(EVP_PKEY_CTX_new_id(EVP_PKEY_DH, nullptr));
    if (!pctx) throw std::runtime_error("Failed to create DH context");

    if (EVP_PKEY_paramgen_init(pctx.get()) != 1) {
      throw std::runtime_error("Failed to init DH paramgen");
    }

    if (EVP_PKEY_CTX_set_dh_rfc5114(pctx.get(), 2) != 1) {
      throw std::runtime_error("Failed to set DH RFC 5114 group");
    }

    EVP_PKEY* params_raw = nullptr;
    if (EVP_PKEY_paramgen(pctx.get(), &params_raw) != 1) {
      throw std::runtime_error("Failed to generate DH params");
    }
    const PkeyPtr params(params_raw);

    const PkeyCtxPtr kctx(EVP_PKEY_CTX_new(params.get(), nullptr));
    if (!kctx) {
      throw std::runtime_error("Failed to create key context");
    }

    if (EVP_PKEY_keygen_init(kctx.get()) != 1) {
      throw std::runtime_error("Failed to init keygen");
    }

    EVP_PKEY* pkey_raw = nullptr;
    if (EVP_PKEY_keygen(kctx.get(), &pkey_raw) != 1) {
      throw std::runtime_error("Failed to generate key pair");
    }
    const PkeyPtr pkey(pkey_raw);

    std::vector<unsigned char> privateKey;
    if (!assignKey(privateKey, pkey.get(), false)) {
      throw std::runtime_error("Failed to extract private key");
    }

    std::vector<unsigned char> publicKey;
    if (!assignKey(publicKey, pkey.get(), true)) {
      throw std::runtime_error("Failed to extract public key");
    }

    return {privateKey, publicKey};
  }

  bool KeyExchange::assignKey(std::vector<unsigned char>& keyContainer, const EVP_PKEY* pkey, const bool isPublic) {
    unsigned char* buf = nullptr;
    const int len = isPublic ? i2d_PUBKEY(pkey, &buf) : i2d_PrivateKey(pkey, &buf);

    if (len < 0) return false;

    keyContainer.assign(buf, buf + len);
    OPENSSL_free(buf);
    return true;
  }

  std::vector<unsigned char> KeyExchange::performDHExchange(
    const std::vector<unsigned char>& peerPublicKey,
    const std::vector<unsigned char>& privateKey)
  {
    const unsigned char* priv_ptr = privateKey.data();
    const PkeyPtr privKey(d2i_PrivateKey(EVP_PKEY_DH, nullptr, &priv_ptr, static_cast<long>(privateKey.size())));
    if (!privKey) {
      throw std::runtime_error("Failed to load private key");
    }

    const unsigned char* pub_ptr = peerPublicKey.data();
    const PkeyPtr peerKey(d2i_PUBKEY(nullptr, &pub_ptr, static_cast<long>(peerPublicKey.size())));
    if (!peerKey) {
      throw std::runtime_error("Failed to load peer public key");
    }

    const PkeyCtxPtr ctx(EVP_PKEY_CTX_new(privKey.get(), nullptr));
    if (!ctx) {
      throw std::runtime_error("Failed to create derivation context");
    }

    if (EVP_PKEY_derive_init(ctx.get()) != 1) {
      throw std::runtime_error("Failed to init key derivation");
    }

    if (EVP_PKEY_derive_set_peer(ctx.get(), peerKey.get()) != 1) {
      throw std::runtime_error("Failed to set peer key");
    }

    size_t secretLen = 256;
    std::vector<unsigned char> sharedSecret(secretLen);
    if (EVP_PKEY_derive(ctx.get(), sharedSecret.data(), &secretLen) != 1) {
      throw std::runtime_error("Failed to derive shared secret");
    }

    sharedSecret.resize(secretLen);
    return sharedSecret;
  }

  std::vector<unsigned char> KeyExchange::deriveSessionKey(const std::vector<unsigned char>& sharedSecret, const size_t keyLength) {
    const KdfPtr kdf(EVP_KDF_fetch(nullptr, "HKDF", nullptr));
    if (!kdf) throw std::runtime_error("Failed to fetch HKDF");

    const KdfCtxPtr kctx(EVP_KDF_CTX_new(kdf.get()));
    if (!kctx) throw std::runtime_error("Failed to create KDF context");

    static auto digest = "SHA256";
    static auto salt = "smtp-ssl-salt";
    static auto info = "session-key";

    const OSSL_PARAM params[] = {
      OSSL_PARAM_construct_utf8_string("digest", const_cast<char*>(digest), 0),
      OSSL_PARAM_construct_octet_string("key", const_cast<unsigned char*>(sharedSecret.data()), sharedSecret.size()),
      OSSL_PARAM_construct_octet_string("salt", const_cast<char*>(salt), std::strlen(salt)),
      OSSL_PARAM_construct_octet_string("info", const_cast<char*>(info), std::strlen(info)),
      OSSL_PARAM_construct_end()
    };

    std::vector<unsigned char> sessionKey(keyLength);
    if (EVP_KDF_derive(kctx.get(), sessionKey.data(), keyLength, params) != 1) {
      throw std::runtime_error("Key derivation failed");
    }

    return sessionKey;
  }
}