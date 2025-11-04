//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <queue>

#include "../networking/Session.h"
#include <thread>

#include "EmailMessage.h"
#include "../networking/SSL/SSLContextFactory.h"

namespace ISXSMTP
{
    class SMTPSession;
}

class Client
{
public:
    Client(const std::string& host, unsigned short port);
    ~Client();

    bool stop();
    bool start();

    bool sendMail(EmailMessage e_msg);

private:
    EmailMessage email_info;

    bool isRunning = false;
    bool canSend = false;
    void reconnect();

    void init();
    void connect();
    bool run();

    void setConnection(std::shared_ptr<net::ip::tcp::socket> socket);

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
    net::steady_timer timer;

    std::jthread io_thread;
    std::jthread session_thread;

    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;

    std::queue<std::string> sendInfo;

    // networking callbacks
    void SSLHandling(boost::asio::const_buffer msg, std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys);
    void SMTPHandling(boost::asio::const_buffer msg);
};


#endif //CLIENT_H
