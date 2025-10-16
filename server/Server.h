//
// Created by alkir on 10/3/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "../networking/Session.h"
#include <mutex>

class Server {
public:
    Server();
    ~Server();

    bool init();

    bool stopServer();
    bool run();

private:
    boost::asio::io_context io;
    std::mutex sessionMutex;

    // SMTP
    // Parser
    // Thread Pool

    // temporary value: waiting for parser
    unsigned short port = 12345;
    net::ip::tcp::acceptor acceptor;

    std::vector<std::shared_ptr<Session>> sessions;

    // temp
    std::string print(const std::string& str);
    //
    void runAcceptor();
    bool setUpAcceptor();

    void onDisconnect(std::shared_ptr<Session> session);
};

#endif //SERVER_H
