//
// Created by alkir on 10/3/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include "../networking/Session.h"
#include <mutex>
#include <condition_variable>
#include <memory>

#include "ServerConsoleUI.h"
#include "SMTPSession.h"
#include "../networking/SSL/SSLContextFactory.h"
#include "Commands/SMTPCommandArguments.h"
#include "config/config.h"

class ThreadPool;

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server();
    ~Server();

    bool init();
    bool stop();
    bool restart();
    void run();

private:
    // SMTP
    // Logger

    // networking
    std::shared_ptr<boost::asio::io_context> io;
    net::executor_work_guard<boost::asio::io_context::executor_type> work;
    net::ip::tcp::acceptor acceptor;
    unsigned short port = 12345;

    std::list<std::shared_ptr<Session>> sessions;
    std::mutex sessionMutex;

    void runAcceptor();
    bool setUpAcceptor();

    std::unique_ptr<ThreadPool> threadPool;
    unsigned short thread_pool_size = 4;
    bool isStopping = false;

    // UI
    std::shared_ptr<ServerConsoleUI> ui = std::make_shared<ServerConsoleUI>();

    // crypto
    std::shared_ptr<smtp::ssl::SSLContextFactory::SSLContext> sslContext;

    void SSLHandling(boost::asio::const_buffer msg, std::shared_ptr<Session> session, std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys);
    void SMTPHandling(boost::asio::const_buffer msg, std::shared_ptr<Session> session, std::shared_ptr<ISXSMTP::SMTPSession> smtp_session);
};

#endif //SERVER_H
