//
// Created by alkir on 10/23/2025.
//

#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <../networking/Session.h>
using namespace boost;

TEST(SessionTest, CanConnectAndDisconnect)
{
    asio::io_context io;
    auto socket = std::make_shared<asio::ip::tcp::socket>(io);
    Session session(socket);
    asio::ip::tcp::endpoint ep(asio::ip::make_address("127.0.0.1"), 65000);
    EXPECT_NO_THROW(session.connect(ep));

    EXPECT_NO_THROW(session.disconnect());
}

TEST(SessionTest, WriteQueueWorks)
{
    asio::io_context io;
    auto socket = std::make_shared<asio::ip::tcp::socket>(io);
    Session session(socket);
    std::vector<char> data{'h', 'e', 'l', 'l', 'o'};
    EXPECT_TRUE(session.send(asio::buffer(data)));
}