#pragma once
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "asio.hpp"

/**
* @class SmtpSession
* @brief Represents a single SMTP session with a client.
*
* This class manages the state of an SMTP session, including the
* command queue, sender/recipient information, message content,
* and busy status. It also provides thread-safe access to the
* underlying socket and session data.
*/
class SmtpSession : public std::enable_shared_from_this<SmtpSession> {

public:

    /**
    * @enum SessionStatus
    * @brief Represents the current state of the SMTP session.
    */
    enum SessionStatus {
        INITIAL, HELO, MAIL_FROM, RCPT, DATA, QUIT
    };

    /**
    * @brief Construct a new SmtpSession object.
    * @param sock Shared pointer to the client's socket.
    */
    explicit SmtpSession(std::shared_ptr<asio::ip::tcp::socket> sock);

    // Delete copy and move operations to enforce unique ownership
    SmtpSession(const SmtpSession&) = delete;
    SmtpSession& operator=(const SmtpSession&) = delete;
    SmtpSession(SmtpSession&&) = delete;
    SmtpSession& operator=(SmtpSession&&) = delete;

    /**
    * @brief Add a command to the session's command queue.
    * @param cmd SMTP command string.
    * @return true if session is not close and elem push to queue, false otherwise.
    */
    bool enqueueCommand(const std::string& cmd);

    /**
    * @brief Check if there are more commands in the queue.
    * @return true if queue is not empty, false otherwise.
    */
    bool hasNextCommand();

    /**
    * @brief Pop the next command from the queue.
    * @return Next command string.
    */
    std::string popCommand();

    /**
    * @brief Set the busy flag.
    * @param val true to mark session as busy, false otherwise.
    */
    void setBusy(bool val);

    /**
    * @brief Check if session is busy.
    * @return true if busy, false otherwise.
    */
    bool isBusy(); 

    /**
    * @brief Release busy flag if no commands remain.
    * @return true if session was released, false otherwise.
    */
    bool releaseIfEmpty();

    /**
    * @brief Closes the SMTP session and its socket safely.
    *
    * Marks the session as closed, resets the busy flag,
    * clears pending commands, and shuts down the socket
    * if it is still open. Safe to call multiple times.
    */
    void close();

    /**
    * @brief Closes the SMTP session and its socket safely.
    * @return false if comandquee is empty(), true otherwise.
    */
    bool clearQueue();

    /**
    * @brief Atomically compare and set busy flag.
    * @return true if flag was successfully set, false otherwise.
    */
    bool compareBusy();

    /**
    * @brief Get shared pointer to the client socket.
    * @return Shared pointer to socket.
    */
    std::shared_ptr<asio::ip::tcp::socket> getSocket();

    /**
    * @brief Get client IP address.
    * @return IP address string.
    */
    const std::string getClientIp() const noexcept;

    /**
    * @brief Get current session status.
    * @return SessionStatus enum value.
    */
    SessionStatus getStatus() const;

    /**
    * @brief Set the session status.
    * @param st New session status.
    */
    void setStatus(SessionStatus st);
    bool isClosed() const;
    // --- SMTP data ---
    void setSender(const std::string& addr);
    void setRecipient(const std::string& addr);
    void appendMessageLine(const std::string& line);
    void clearMessage();

    const std::string getSender() const;
    const std::string getRecipient() const;
    const std::string getMessage() const;

private:
    SessionStatus current_state = INITIAL;
    std::string client_ip;
    std::string sender_address;
    std::string recipient_address;
    std::string message_buffer;

    std::atomic<bool> busy{false};
    std::atomic<bool> closed{false};

    mutable std::mutex session_mutex;
    std::queue<std::string> commandQueue;

    mutable std::mutex socket_mutex;
    std::shared_ptr<asio::ip::tcp::socket> socket;

    /**
    * @brief initialize session constructure.
    */
    void initialize();
};
