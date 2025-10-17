//
// Created by alkir on 10/3/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "../networking/Session.h"
#include <mutex>

class ThreadPool;

class Server : public std::enable_shared_from_this<Server>{
public:
    Server();
    ~Server();

    bool init();

    bool stopServer();
    bool run();

private:
    std::shared_ptr<boost::asio::io_context> io;
    std::mutex sessionMutex;
    net::executor_work_guard<boost::asio::io_context::executor_type> work;

    // think about mechanism how to stop server manually
    bool isStopping = false;
    std::condition_variable mainThreadCV;
    std::mutex mainThreadMutex;

    // SMTP
    // Parser
    std::unique_ptr<ThreadPool> threadPool;

    // temporary value: waiting for parser
    unsigned short port = 12345;
    net::ip::tcp::acceptor acceptor;

    std::vector<std::shared_ptr<Session>> sessions;

    // temp
    std::string print(const std::string& str);
    //
    void runAcceptor();
    bool setUpAcceptor();
};

#endif //SERVER_H
