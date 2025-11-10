//
// Created by alkir on 10/3/2025.
//
// Client.h
// ----------------------
// Defines the SMTP client responsible for establishing a connection
// to the SMTP server, managing message exchange, and sending emails.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <thread>
#include <string>
#include <memory>

#include "EmailMessage.h"
#include "SmartSession.h"
#include "../networking/SSL/SSLContextFactory.h"

/**
 * @class Client
 * @brief Represents an SMTP client capable of connecting to an SMTP server,
 *        managing secure sessions, and sending email messages.
 *
 * The Client class encapsulates the entire client-side networking logic:
 * - Establishes and maintains a TCP connection to the SMTP server
 * - Handles SSL/TLS encryption via the SSLContextFactory
 * - Manages SMTP message exchange through SmartSession
 * - Provides asynchronous I/O using Boost.Asio and std::jthread
 */
class Client
{
public:
    /**
     * @brief Constructs a new Client instance.
     *
     * @param host Hostname or IP address of the SMTP server.
     * @param port Port number to connect to.
     */
    Client(const std::string& host, unsigned short port);

    /**
     * @brief Destroys the Client instance.
     *
     * Ensures all running threads and network resources are safely released.
     */
    ~Client();

    /**
     * @brief Stops the client and terminates the connection.
     *
     * @return True if the client stopped successfully.
     */
    bool stop();

    /**
     * @brief Starts the client and attempts to establish a connection.
     *
     * @return True if the connection was successfully established.
     */
    bool start();
    bool run();

    /**
     * @brief Sends an email message to the SMTP server.
     *
     * @param e_msg Email message containing sender, recipient, subject, and body.
     * @return True if the message was successfully sent.
     */
    bool sendMail(EmailMessage e_msg);

    std::string getLastError() const { return m_lastError; }
private:
    EmailMessage m_emailInfo;

    std::string m_lastError;
    size_t m_recipientIndex;

    void changeLogLevel(const std::string& level) const;

private:
    /// Stores the currently prepared email message.
    EmailMessage email_info;

    /// Indicates whether the client is running.
    bool isRunning = false;

    /// Indicates whether the client is ready to send data.
    bool canSend = false;

    /**
     * @brief Attempts to reconnect to the server if the connection is lost.
     */
    void reconnect();

    /**
     * @brief Initializes the client components and prepares the connection.
     */
    void init();

    /**
     * @brief Establishes a TCP connection to the configured server endpoint.
     */
    void connect();

    /**
     * @brief Runs the main I/O loop for asynchronous operations.
     * @return True if the client runs successfully.
     */
    bool run();

    // --- Networking Core ---

    /// Boost.Asio I/O context for managing asynchronous networking.
    net::io_context io;

    /// Server endpoint (IP + port).
    net::ip::tcp::endpoint server_endpoint;

    /// Manages the connection and SMTP protocol logic.
    std::shared_ptr<SmartSession> session;

    /// Timer used for delayed reconnection or retry logic.
    net::steady_timer timer;

    // --- Threading ---

    /// Thread handling I/O context execution.
    std::jthread io_thread;

    /// Thread managing the client session logic.
    std::jthread session_thread;

    // --- Security ---

    /// SSL context used for secure communication setup.
    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;

    // --- Message Queue ---

    /// Queue of outgoing messages waiting to be sent.
    std::queue<std::string> sendInfo;

    // --- Callbacks ---

    /**
     * @brief Handles incoming SMTP messages from the server.
     * @param msg Received message buffer.
     */
    void SMTPHandling(boost::asio::const_buffer msg);
};

#endif // CLIENT_H
