#pragma once
#include <boost/asio/ssl.hpp>
#include <memory>
#include <string>

namespace smtp::ssl {
  class SSLContextFactory {
  public:
    using SSLContext = boost::asio::ssl::context;

    static std::shared_ptr<SSLContext> createServerContext();

    static std::shared_ptr<SSLContext> createClientContext();

  private:
    static inline const std::string certFilePath{SSL_CERT_FILE};
    static inline const std::string keyFilePath{SSL_KEY_FILE};
    static inline const std::string dhParamsFilePath{SSL_DH_PARAMS_FILE};

    static void configureContext(SSLContext &ctx);
  };
}
