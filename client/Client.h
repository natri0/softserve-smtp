//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../networking/Session.h"
#include <thread>

#include "EmailMessage.h"
#include "../networking/SSL/SSLContextFactory.h"

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
    void reconnect();

    void init();
    void connect();
    bool run();

    // SMTP
    std::string lastResponse;
    std::promise<void> done;
    //

    // Logger
    //

    void onMessage(std::string& msg);

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
    net::steady_timer timer;

    bool isStopping = false;
    std::condition_variable mainThreadCV;
    std::mutex mainThreadMutex;

    std::jthread io_thread;
    std::jthread session_thread;

    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;
};


#endif //CLIENT_H
