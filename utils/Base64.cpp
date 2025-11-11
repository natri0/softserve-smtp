#include "Base64.h"
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <stdexcept>

std::vector<unsigned char> Base64::Encode(const std::vector<unsigned char> &data) {
  BIO *bio = BIO_new(BIO_s_mem());
  BIO *b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  BIO_push(b64, bio); // input -> b64 -> bio

  BIO_write(b64, data.data(), static_cast<int>(data.size()));
  BIO_flush(b64);

  BUF_MEM *bufferPtr;
  BIO_get_mem_ptr(b64, &bufferPtr);

  std::vector<unsigned char> result(bufferPtr->data, bufferPtr->data + bufferPtr->length);
  BIO_free_all(b64);

  return result;
}

std::vector<unsigned char> Base64::Decode(const std::vector<unsigned char> &encoded) {
  BIO *bio = BIO_new_mem_buf(encoded.data(), static_cast<int>(encoded.size()));
  BIO *b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  BIO_push(b64, bio);

  std::vector<unsigned char> result(encoded.size());
  const int decoded_length = BIO_read(b64, result.data(), static_cast<int>(encoded.size()));

  BIO_free_all(b64);

  if (decoded_length < 0) {
    throw std::runtime_error("Base64 decoding failed");
  }

  result.resize(decoded_length);
  return result;
}