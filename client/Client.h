//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../networking/Session.h"
#include <thread>

class Client
{
public:
    Client(const std::string& host, unsigned short port);
    ~Client();

    bool stop();
    bool start();

    void sendMail();

private:
    bool isRunning = false;
    void reconnect();

    bool init();
    bool connect();
    bool run();

    // SMTP
    // Logger
    void onMessage(std::string& msg);

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
    net::steady_timer timer;

    std::jthread io_thread;
};


#endif //CLIENT_H
