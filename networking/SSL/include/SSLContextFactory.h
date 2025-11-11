#ifndef SSL_CONTEXT_FACTORY_H
#define SSL_CONTEXT_FACTORY_H

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
    static inline const std::string certFilePath{"networking/SSL/certs/server.crt"};
    static inline const std::string keyFilePath{"networking/SSL/certs/server.key"};
    static inline const std::string dhParamsFilePath{"networking/SSL/certs/dh.pem"};

    static void configureContext(SSLContext &ctx);
  };
}

#endif