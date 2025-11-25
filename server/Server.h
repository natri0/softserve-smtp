//
// Created by alkir on 10/3/2025.
//
// Server.h
// ----------------------
// Defines the main server class responsible for accepting,
// managing, and processing client connections over TCP and SMTP.
//

#ifndef SERVER_H
#define SERVER_H

#include "../networking/Connection/NetSession.h"
#include "../networking/SmartSession.h"
#include "../networking/SSL/SSLContextFactory.h"
#include "ServerConsoleUI.h"
#include "config/config.h"

#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Database/Manager.hpp"

class ThreadPool;

/**
 * @class Server
 * @brief Core server component responsible for managing network connections and handling SMTP communication.
 *
 * The Server class handles all major runtime tasks:
 * - Initializes the networking layer (Boost.Asio)
 * - Accepts and manages client connections
 * - Handles SMTP protocol messages through SmartSession
 * - Manages a worker thread pool for concurrent request processing
 * - Interfaces with configuration files and console UI
 */
class Server : public std::enable_shared_from_this<Server>
{
public:
    /**
     * @brief Constructs a Server instance.
     *
     * Initializes internal components and prepares the server for startup.
     */
    Server(bool isWinService = false);

    /**
     * @brief Destroys the Server instance.
     *
     * Ensures all active sessions are safely terminated.
     */
    ~Server();

    /**
     * @brief Initializes the server subsystems.
     *
     * Loads configuration, sets up database connections,
     * prepares the thread pool, and initializes the network acceptor.
     *
     * @return True if initialization succeeded, false otherwise.
     */
    bool init();

    /**
     * @brief Stops the server and all active sessions.
     * @return True if shutdown completed successfully.
     */
    bool stop();

    /**
     * @brief Restarts the server by reinitializing all components.
     * @return True if restart succeeded.
     */
    bool reset();

    /**
     * @brief Runs the main server loop.
     *
     * Starts accepting new client connections and dispatches
     * incoming requests to worker threads.
     */
    void run();

private:
    // --- Core Networking Components ---

    /// Main I/O context used by Boost.Asio for asynchronous operations.
    std::shared_ptr<boost::asio::io_context> io;

    /// Ensures the I/O context does not exit prematurely.
    net::executor_work_guard<boost::asio::io_context::executor_type> work;

    /// TCP acceptor responsible for listening for new client connections.
    net::ip::tcp::acceptor acceptor;

    /// TCP port on which the server listens for incoming connections.
    unsigned short port = 12345;

    /// List of active SmartSession connections.
    std::list<std::shared_ptr<SmartSession>> sessions;

    /// Mutex to synchronize access to the session list.
    std::mutex sessionMutex;

    // --- Threading & Concurrency ---

    /// Pool of worker threads handling asynchronous operations.
    std::unique_ptr<ThreadPool> threadPool;

    /// Number of threads in the thread pool.
    unsigned short thread_pool_size = 4;

    /// Flag indicating if the server is in the process of stopping.
    bool isStopping = false;

    /// Flag indicating if the server is being run as windows background service.
    bool isWinService = false;

    // --- User Interface ---

    /// Console-based UI for runtime interaction and status output.
    std::shared_ptr<ServerConsoleUI> ui = std::make_shared<ServerConsoleUI>();

    // --- Security & Encryption ---

    /// SSL context factory used for managing TLS certificates and secure sockets.
    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;

    // --- Private Methods ---

    /**
     * @brief Begins listening for new client connections asynchronously.
     */
    void runAcceptor();

    /**
     * @brief Sets up the network acceptor (bind, listen, etc.).
     * @return True if the acceptor was successfully created.
     */
    bool setUpAcceptor();

    /**
     * @brief Configures a new session once a client connection is accepted.
     * @param socket Shared pointer to the accepted TCP socket.
     */
    void setConnection(std::shared_ptr<net::ip::tcp::socket> socket);

    /**
     * @brief Handles incoming SMTP data from a connected session.
     * @param msg The received data buffer.
     * @param session Pointer to the session that sent the message.
     */
    void SMTPHandling(boost::asio::const_buffer msg, std::shared_ptr<SmartSession> session);
};

#endif // SERVER_H