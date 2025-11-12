//
// Created by alkir on 10/3/2025.
//
// Session.h
// ----------------------
// Represents an asynchronous TCP session
// Handles reading and writing using Boost.Asio, provides callbacks
// for message handling, connection, and disconnection events.
// Supports encryption through CryptoManager.
//

#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <functional>
#include <memory>
#include <deque>
#include <array>

#include "../SSL/CryptoManager.h"

namespace net = boost::asio;

/**
 * @class Session
 * @brief Represents an asynchronous TCP session (connection).
 *
 * Manages communication over a TCP socket using Boost.Asio.
 * It supports non-blocking read and write operations,
 * as well as user-defined callbacks for key events:
 * - Message received
 * - Connection established
 * - Disconnection
 *
 */
class Session : public std::enable_shared_from_this<Session>
{
public:
    /**
     * @brief Callback type for handling received messages.
     * @param net::const_buffer Buffer containing received data.
     */
    using OnMessage = std::function<void(net::const_buffer)>;

    /**
     * @brief Callback type invoked when a connection is established.
     */
    using OnConnected = std::function<void()>;

    /**
     * @brief Callback type invoked when a disconnection occurs.
     */
    using OnDisconnect = std::function<void()>;

    /**
     * @brief Constructs a server-side session.
     * @param _socket Shared pointer to a TCP socket.
     */
    explicit Session(std::shared_ptr<net::ip::tcp::socket> _socket);

    /**
     * @brief Destroys the session and releases resources.
     */
    ~Session();

    /**
     * @brief Connects to a remote endpoint (client-side).
     * @param endpoint Target IP address and port.
     */
    void connect(const net::ip::tcp::endpoint& endpoint);

    /**
     * @brief Disconnects the session.
     * @return True if the session was successfully disconnected.
     */
    bool disconnect();

    // -------- Callback setters --------

    /**
     * @brief Sets the callback for handling incoming messages.
     */
    void setOnMessage(OnMessage cb) noexcept { onMessageReceived = std::move(cb); }

    /**
     * @brief Sets the callback for successful connection events.
     */
    void setOnConnected(OnConnected cb) noexcept { onConnected = std::move(cb); }

    /**
     * @brief Sets the callback for disconnection events.
     */
    void setOnDisconnect(OnDisconnect cb) noexcept { onDisconnect = std::move(cb); }

    // -------- Core functionality --------

    /**
     * @brief Starts the session’s asynchronous read loop.
     * @return True if the loop started successfully.
     */
    bool run();

    /**
     * @brief Asynchronously sends data through the socket.
     * @param data Buffer containing the data to send.
     * @return True if the send operation was successfully initiated.
     */
    bool send(boost::asio::const_buffer data);

    // -------- Getters --------

    /**
     * @brief Checks if the session is currently connected.
     * @return True if the session is active.
     */
    [[nodiscard]] bool isConnected() const noexcept { return connected; }

    /**
     * @brief Returns the underlying TCP socket.
     */
    std::shared_ptr<net::ip::tcp::socket> getSocket() const noexcept { return socket; }

    /**
     * @brief Sets an encryption key and initializes the CryptoManager.
     * @param key Encryption key as a byte vector.
     */
    void setKey(std::vector<unsigned char> key)
    {
        cryptoManager = std::make_unique<smtp::ssl::CryptoManager>(key);
    }

private:
    /**
     * @brief Performs asynchronous read operations.
     */
    void read();

    /**
     * @brief Performs asynchronous write operations.
     */
    void write();

    std::array<char, 1024> buffer;                       ///< Read buffer
    std::deque<net::const_buffer> writeQueue;            ///< Pending write operations
    bool isWriting = false;                              ///< Indicates if writing is in progress
    bool isRunning = false;                              ///< Indicates if the session loop is active
    std::atomic<bool> connected = false;                 ///< Connection state

    std::shared_ptr<net::ip::tcp::socket> socket;        ///< Underlying TCP socket

    // --- Event callbacks ---
    OnMessage onMessageReceived;                         ///< Called when new data arrives
    OnConnected onConnected;                             ///< Called when connected
    OnDisconnect onDisconnect;                           ///< Called when disconnected

    // --- Security ---
    std::unique_ptr<smtp::ssl::CryptoManager> cryptoManager; ///< Handles message encryption/decryption
    std::string decrypted_data;                               ///< Stores last decrypted message
};

#endif // SESSION_H