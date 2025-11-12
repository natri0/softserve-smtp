//
// Created by alkir on 11/4/2025.
//
// SmartSession.h
// ----------------------
// Represents an advanced session wrapper that integrates
// both networking (TCP) and SMTP logic into a unified interface.
// Used by both client and server components.
//

#ifndef SMARTSESSION_H
#define SMARTSESSION_H

#include "Connection/Session.h"
#include "../SMTP/include/SMTPSession.h"

/**
 * @class SmartSession
 * @brief Combines TCP session management with SMTP logic.
 *
 * The SmartSession class serves as a bridge between the network layer (`Session`)
 * and the SMTP protocol layer (`SMTPSession`).
 * It is capable of managing both **client** and **server** connections,
 * handling encryption setup, and providing callback hooks for incoming SMTP messages.
 */
class SmartSession
{
public:
    /**
     * @enum Type
     * @brief Defines the role of the session (client or server).
     */
    enum Type
    {
        CLIENT, ///< Represents an SMTP client connection.
        SERVER ///< Represents an SMTP server connection.
    };

    /**
     * @brief Constructs a SmartSession object.
     * @param socket Shared pointer to a TCP socket.
     * @param sessionType Defines whether this session acts as CLIENT or SERVER.
     */
    explicit SmartSession(std::shared_ptr<net::ip::tcp::socket> socket, Type sessionType);

    /**
     * @brief Establishes the connection and initializes protocol handling.
     *
     * This method binds the low-level session to the appropriate SMTP handler
     * and sets up necessary event callbacks.
     */
    void setConnection();

    /**
     * @brief Sets a callback to handle incoming SMTP messages.
     * @param cb Function that processes received SMTP data.
     */
    void setSMTPHandling(std::function<void(net::const_buffer)> cb)
    {
        SMTPHandling = std::move(cb);
    }

    std::shared_ptr<Session> net() const { return net_session; };
    std::shared_ptr<ISXSMTP::SMTPSession> smtp() const { return smtp_session; };

private:
    std::shared_ptr<Session> net_session; ///< Underlying TCP session responsible for socket I/O.
    std::shared_ptr<ISXSMTP::SMTPSession> smtp_session; ///< Associated SMTP protocol handler.

    Type type; ///< Indicates whether this SmartSession represents a client or server.

    /**
     * @brief Optional callback for custom SMTP message handling.
     */
    std::function<void(net::const_buffer)> SMTPHandling;

    /**
     * @brief Configures encryption keys for the current session.
     * @param msg Incoming message containing encryption data.
     * @param keys Shared pointer to a pair of symmetric encryption keys.
     */
    void setKeys(net::const_buffer msg,
                 std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys);
};

#endif // SMARTSESSION_H
