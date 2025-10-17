//
// Created by alkir on 10/3/2025.
//

#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <functional>
#include <deque>
#include <array>

namespace net = boost::asio;

class Session : public std::enable_shared_from_this<Session>
{
public:
    using OnMessage = std::function<void(const std::string&)>;
    using OnDisconnect = std::function<void()>;

    //for server
    explicit Session(std::shared_ptr<net::ip::tcp::socket> _socket);

    ~Session();

    bool connect(const net::ip::tcp::endpoint& endpoint);
    bool disconnect();

    // setters
    void setOnMessage(OnMessage cb) noexcept { onMessageReceived = std::move(cb); }
    void setOnDisconnect(OnDisconnect cb) noexcept { onDisconnect = std::move(cb); };

    // functional
    bool run();
    bool send(const std::string& data);

    // getters
    [[nodiscard]] bool isConnected() const noexcept { return socket->is_open(); }

    std::shared_ptr<net::ip::tcp::socket> getSocket() const noexcept { return socket; };

private:
    void read();
    void write();

    std::array<char, 1024> buffer;
    std::deque<std::string> writeQueue;
    bool isWriting = false;
    bool isRunning = false;

    std::shared_ptr<net::ip::tcp::socket> socket;

    // callbacks for smtp
    OnMessage onMessageReceived;
    OnDisconnect onDisconnect;
};


#endif //SESSION_H
