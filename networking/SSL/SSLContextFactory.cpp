#include "SSLContextFactory.h"
#include <boost/asio/ssl/context.hpp>

namespace smtp::ssl {
  void SSLContextFactory::configureContext(SSLContext &ctx) {
    ctx.set_options(
      boost::asio::ssl::context::default_workarounds |
      boost::asio::ssl::context::no_sslv2 |
      boost::asio::ssl::context::no_sslv3
    );
  }

  std::shared_ptr<SSLContextFactory::SSLContext>
  SSLContextFactory::createServerContext() {
    auto ctx = std::make_shared<SSLContext>(boost::asio::ssl::context::tlsv12_server);

    configureContext(*ctx);
    ctx->set_options(boost::asio::ssl::context::single_dh_use);

    ctx->use_certificate_chain_file(certFilePath);
    ctx->use_private_key_file(keyFilePath, boost::asio::ssl::context::pem);
    ctx->use_tmp_dh_file(dhParamsFilePath);

    return ctx;
  }

  std::shared_ptr<SSLContextFactory::SSLContext>
  SSLContextFactory::createClientContext() {
    auto ctx = std::make_shared<SSLContext>(boost::asio::ssl::context::tlsv12_client);

    configureContext(*ctx);

    ctx->load_verify_file(certFilePath);
    ctx->set_verify_mode(boost::asio::ssl::verify_peer);

    return ctx;
  }
}
