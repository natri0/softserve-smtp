#ifndef SSL_CONTEXT_FACTORY_H
#define SSL_CONTEXT_FACTORY_H

#include <boost/asio/ssl.hpp>
#include <memory>
#include <string>

namespace smtp::ssl {
  /**
   * @brief A factory for creating and configuring Boost.Asio SSL contexts.
   */
  class SSLContextFactory {
  public:

    using SSLContext = boost::asio::ssl::context;

    /**
     * @brief Creates and configures a TLS 1.3 server-side SSL context.
     * @return A shared pointer to the configured server SSL context.
     */
    static std::shared_ptr<SSLContext> createServerContext();

    /**
     * @brief Creates and configures a TLS 1.3 client-side SSL context.
     * @return A shared pointer to the configured client SSL context.
     */
    static std::shared_ptr<SSLContext> createClientContext();

  private:
    static inline const std::string certFilePath{SSL_CERT_FILE};
    static inline const std::string keyFilePath{SSL_KEY_FILE};
    static inline const std::string dhParamsFilePath{SSL_DH_PARAMS_FILE};

    /**
     * @brief Applies common configuration options to an SSL context.
     * @param ctx The SSL context to configure.
     */
    static void configureContext(SSLContext &ctx);
  };
}

#endif