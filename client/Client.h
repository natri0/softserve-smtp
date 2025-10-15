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

    void run();

    void stop();

    void send(std::string str);

private:
    bool isRunning = false;

    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session;
};


#endif //CLIENT_H
