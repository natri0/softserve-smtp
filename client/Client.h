//
// Created by alkir on 10/3/2025.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../networking/Session.h"

class Client
{
public:
    Client(const std::string& host, const unsigned short port);
    ~Client();

    void init();
    void connect();
    void reconnect();
    void run();

    void stop();

    void sendMail();

private:
    bool isRunning = false;

    // SMTP
    // Mail
    void onMessage(std::string& msg);

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
    net::steady_timer timer;

    std::thread io_thread;
};


#endif //CLIENT_H
