//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../networking/Session.h"
#include <thread>

#include "EmailMessage.h"

class Client
{
public:
    Client(const std::string& host, unsigned short port);
    ~Client();

    bool stop();
    bool start();

    void sendMail(EmailMessage e_msg);

private:
    EmailMessage email_info;

    bool isRunning = false;
    void reconnect();

    bool init();
    void connect();
    bool run();

    // SMTP
    // Logger
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
};


#endif //CLIENT_H
