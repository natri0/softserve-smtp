//
// Created by alkir on 10/3/2025.
//

#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <functional>
#include <memory>
#include <deque>
#include <array>

// #include "SSL/CryptoManager.h"
// #include "SSL/KeyExchanger.h"
// #include "SSL/SSLContextFactory.h"

namespace net = boost::asio;

class Session : public std::enable_shared_from_this<Session>
{
public:
    using OnMessage = std::function<void(const std::string&)>;
    using OnConnected = std::function<void()>;
    using OnDisconnect = std::function<void()>;

    //for server
    explicit Session(std::shared_ptr<net::ip::tcp::socket> _socket);

    ~Session();

    bool connect(const net::ip::tcp::endpoint& endpoint);
    bool disconnect();

    // setters
    void setOnMessage(OnMessage cb) noexcept { onMessageReceived = std::move(cb); }
    void setOnConnected(OnConnected cb) noexcept { onConnected = std::move(cb); };
    void setOnDisconnect(OnDisconnect cb) noexcept { onDisconnect = std::move(cb); };

    // functional
    bool run();
    bool send(const std::string& data);

    // getters
    [[nodiscard]] bool isConnected() const noexcept { return connected; }

    std::shared_ptr<net::ip::tcp::socket> getSocket() const noexcept { return socket; };

    static std::string serializeKey(const std::vector<unsigned char>& key)
    {
        std::string encoded;
        encoded.resize(boost::beast::detail::base64::encoded_size(key.size()));

        auto len = boost::beast::detail::base64::encode(
            &encoded[0],
            key.data(),
            key.size()
        );

        encoded.resize(len);
        return encoded;
    }

    static std::vector<unsigned char> deserializeKey(const std::string& encoded)
    {
        std::vector<unsigned char> decoded(boost::beast::detail::base64::decoded_size(encoded.size()));
        auto [fst, snd] = boost::beast::detail::base64::decode(decoded.data(), encoded.data(), encoded.size());
        decoded.resize(fst);
        return decoded;
    }

    void setKey(std::vector<unsigned char> key) {Key = key;}

private:
    void read();
    void write();

    // std::unique_ptr<smtp::ssl::CryptoManager> cryptoManager;

    std::array<char, 1024> buffer;
    std::deque<std::string> writeQueue;
    bool isWriting = false;
    bool isRunning = false;
    std::atomic<bool> connected = false;

    std::shared_ptr<net::ip::tcp::socket> socket;

    // callbacks for smtp
    OnMessage onMessageReceived;
    OnConnected onConnected;
    OnDisconnect onDisconnect;

    std::vector<unsigned char> Key{};
};

#endif //SESSION_H
