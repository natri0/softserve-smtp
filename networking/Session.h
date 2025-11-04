//
// Created by alkir on 10/3/2025.
//

#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <functional>
#include <memory>
#include <deque>
#include <array>
#include <iostream>

#include "SSL/CryptoManager.h"

namespace net = boost::asio;

class Session : public std::enable_shared_from_this<Session>
{
public:
    using OnMessage = std::function<void(boost::asio::const_buffer)>;
    using OnConnected = std::function<void()>;
    using OnDisconnect = std::function<void()>;

    //for server
    explicit Session(std::shared_ptr<net::ip::tcp::socket> _socket);

    ~Session();

    void connect(const net::ip::tcp::endpoint& endpoint);
    bool disconnect();

    // setters
    void setOnMessage(OnMessage cb) noexcept { onMessageReceived = std::move(cb); }
    void setOnConnected(OnConnected cb) noexcept { onConnected = std::move(cb); }
    void setOnDisconnect(OnDisconnect cb) noexcept { onDisconnect = std::move(cb); };

    // functional
    bool run();
    bool send(boost::asio::const_buffer data);

    // getters
    [[nodiscard]] bool isConnected() const noexcept { return connected; }

    std::shared_ptr<net::ip::tcp::socket> getSocket() const noexcept { return socket; };

    void setKey(std::vector<unsigned char> key)
    {
        cryptoManager = std::make_unique<smtp::ssl::CryptoManager>(key);
    }

private:
    void read();
    void write();

    std::array<char, 1024> buffer;
    std::deque<boost::asio::const_buffer> writeQueue;
    bool isWriting = false;
    bool isRunning = false;
    std::atomic<bool> connected = false;

    std::shared_ptr<net::ip::tcp::socket> socket;

    // callbacks for smtp
    OnMessage onMessageReceived;
    OnConnected onConnected;
    OnDisconnect onDisconnect;

    std::unique_ptr<smtp::ssl::CryptoManager> cryptoManager;
    std::string decrypted_data;
};

#endif //SESSION_H
