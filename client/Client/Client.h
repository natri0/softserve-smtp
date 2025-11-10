//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../networking/Session.h"
#include <thread>
#include <string>

#include "EmailMessage.h"
#include "../networking/SSL/SSLContextFactory.h"

class Client
{
public:
    Client(const std::string& host, unsigned short port);
    ~Client();

    bool stop();
    bool start();
    bool run();

    bool sendMail(EmailMessage e_msg);

    std::string getLastError() const { return m_lastError; }
private:
    EmailMessage m_emailInfo;

    std::string m_lastError;
    size_t m_recipientIndex;

    void reconnect();

    void init();
    void connect();

    // SMTP
    // Logger

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
    net::steady_timer timer;

    std::jthread io_thread;
    std::jthread session_thread;

    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;
};


#endif //CLIENT_H
