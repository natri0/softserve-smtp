#include <gtest/gtest.h>
#include "SSLContextFactory.h"
#include "boost/asio/ip/tcp.hpp"

using namespace smtp::ssl;

TEST(SSLContextFactoryTest, CreateServerContext) {
  EXPECT_NO_THROW({
      const auto ctx = SSLContextFactory::createServerContext();
      EXPECT_NE(nullptr, ctx);
  });
}

TEST(SSLContextFactoryTest, CreateClientContext) {
  EXPECT_NO_THROW({
      const auto ctx = SSLContextFactory::createClientContext();
      EXPECT_NE(nullptr, ctx);
  });
}

TEST(SSLContextFactoryTest, ServerContextHasCorrectProtocol) {
  const auto ctx = SSLContextFactory::createServerContext();
  ASSERT_NE(nullptr, ctx);

  EXPECT_NO_THROW({
    boost::asio::io_context io;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(io, *ctx);
  });
}

TEST(SSLContextFactoryTest, ClientContextHasCorrectProtocol) {
  const auto ctx = SSLContextFactory::createClientContext();
  ASSERT_NE(nullptr, ctx);

  EXPECT_NO_THROW({
    boost::asio::io_context io;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(io, *ctx);
  });
}

TEST(SSLContextFactoryTest, MultipleServerContexts) {
  const auto ctx1 = SSLContextFactory::createServerContext();
  const auto ctx2 = SSLContextFactory::createServerContext();

  EXPECT_NE(nullptr, ctx1);
  EXPECT_NE(nullptr, ctx2);
  EXPECT_NE(ctx1, ctx2);
}

TEST(SSLContextFactoryTest, MultipleClientContexts) {
  const auto ctx1 = SSLContextFactory::createClientContext();
  const auto ctx2 = SSLContextFactory::createClientContext();

  EXPECT_NE(nullptr, ctx1);
  EXPECT_NE(nullptr, ctx2);
  EXPECT_NE(ctx1, ctx2);
}

TEST(SSLContextFactoryTest, ServerAndClientContextsAreDifferent) {
  const auto serverCtx = SSLContextFactory::createServerContext();
  const auto clientCtx = SSLContextFactory::createClientContext();

  EXPECT_NE(nullptr, serverCtx);
  EXPECT_NE(nullptr, clientCtx);
  EXPECT_NE(serverCtx, clientCtx);
}