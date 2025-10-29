//
// Created by alkir on 10/23/2025.
//

#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <../networking/Session.h>
using namespace boost;

using namespace boost;
using tcp = asio::ip::tcp;

class TestServer {
public:
    explicit TestServer(asio::io_context& io, unsigned short port)
        : acceptor(io, tcp::endpoint(tcp::v4(), port)), socket(io)
    {
        accept();
    }

    void send(const std::string& msg)
    {
        asio::write(socket, asio::buffer(msg));
    }

    std::string read()
    {
        std::vector<char> buf(1024);
        size_t len = socket.read_some(asio::buffer(buf));
        return std::string(buf.begin(), buf.begin() + len);
    }

private:
    tcp::acceptor acceptor;
    tcp::socket socket;

    void accept()
    {
        acceptor.async_accept(socket, [this](auto ec)
        {
            if (!ec)
                std::cerr << "[TestServer] Client connected\n";
        });
    }
};

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
    EXPECT_FALSE(session.send(asio::buffer(data)));
}

TEST(SessionTest, OnConnectedCallbackFires)
{
    asio::io_context io;
    asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 5500));
    asio::ip::tcp::socket server_socket(io);
    bool connected = false;

    auto socket = std::make_shared<asio::ip::tcp::socket>(io);
    Session s(socket);
    s.setOnConnected([&]() { connected = true; });

    acceptor.async_accept(server_socket, [](const boost::system::error_code& ec) {});
    s.connect(asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), 5500));
    io.run_for(std::chrono::milliseconds(200));

    EXPECT_TRUE(connected);
}

TEST(SessionTest, SendOnClosedSocketReturnsFalse)
{
    asio::io_context io;
    auto socket = std::make_shared<asio::ip::tcp::socket>(io);
    Session s(socket);

    socket->close();
    EXPECT_FALSE(s.send(asio::buffer("data", 4)));
}