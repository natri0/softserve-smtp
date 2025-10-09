//
// Created by alkir on 10/3/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "../networking/Session.h"
#include "ThreadPool.h"


class Server {
public:
    Server();
    ~Server();

    // void initServer();
    void stopServer();
    void run();

private:
    boost::asio::io_context io;

    // SMTP
    // Parser

    // temporary value: waiting for parser
    unsigned short port = 12345;
    net::ip::tcp::acceptor acceptor;

    // temp
    void print(const std::string& str);
    std::unique_ptr<ThreadPool> threadPool;
    //
    void runAcceptor();
    void setUpAcceptor();
};

#endif //SERVER_H
