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

    void run(std::string str = "something") const;

private:
    net::io_context io;
    net::ip::tcp::endpoint server_endpoint;
    std::shared_ptr<Session> session = nullptr;
};


#endif //CLIENT_H
