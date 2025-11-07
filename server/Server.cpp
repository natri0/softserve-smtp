//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

#include <cmath>

#include "ThreadPool/include/ThreadPool.hpp"
#include "../networking/SSL/KeyExchanger.h"
#include "SMTPSession.h"
#include "config/config.h"
#include "Database/Manager.hpp"

// temp till we don't have parser

Server::Server() : io(std::make_shared<boost::asio::io_context>()),
                   work(io->get_executor()), acceptor(net::ip::tcp::acceptor(*io)),
                   sslContext(smtp::ssl::SSLContextFactory::createServerContext())
{
}

Server::~Server()
{
    stop();
}

bool Server::init()
{
    // initialization from config
    if (Config config; !config.load_from_file("server/config/config.json"))
        std::cout << "Couldn't load config.json" << std::endl;
    else
    {
        if (config.has_key("port")) port = config.get<unsigned short>("port");
        if (config.has_key("thread_pool_size")) thread_pool_size = config.get<unsigned short>("thread_pool_size");
    }

    // mailbox lifespan = server lifespan
    static std::shared_ptr<ISXSMTP::SMTPIMailbox> mailbox = std::make_shared<SQLiteMailbox>();
    SMTPConfigBuilder builder;
    builder.SetMailbox(mailbox);
    //builder.SetDomain(domain_from_config_file_should_be_here);
    builder.SetCurrentAsDefault();

    threadPool = std::make_unique<ThreadPool>(thread_pool_size);

    ui->showBanner(port);

    if (!setUpAcceptor()) return false;
    threadPool->start();

    return true;
}

bool Server::stop()
{
    boost::system::error_code ec;
    acceptor.cancel(ec);
    if (acceptor.is_open()) acceptor.close(ec);

    if (ec) std::cerr << "Error closing acceptor: " << ec.message() << std::endl;

    sessions.clear();
    io->stop();
    threadPool->stop();

    return true;
}

bool Server::restart()
{
    stop();

    isStopping = false;
    io->restart();
    threadPool->start();

    if (!init())return false;

    runAcceptor();

    return true;
}

void Server::run()
{
    if (!acceptor.is_open()) return;

    for (unsigned short i = 0; i < thread_pool_size; ++i)
        threadPool->submit([self = shared_from_this()]()
        {
            self->io->run();
        });

    runAcceptor();

    ui->logEvent("Server is Running");

    bool running = true;
    while (running)
    {
        ui->showMenu();
        int cmd;
        std::cin >> cmd;
        running = ui->handleCommand(cmd);
    }

    ui->logEvent("Server shut down.");

    stop();
}

void Server::runAcceptor()
{
    auto socket = std::make_shared<net::ip::tcp::socket>(*io);

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec)
    {
        if (!ec) setConnection(socket);
        else { std::cerr << "Accept failed: " << ec.message() << std::endl; }

        runAcceptor();
    });
}

bool Server::setUpAcceptor()
{
    acceptor.open(net::ip::tcp::v6());

    acceptor.set_option(net::ip::v6_only(false));
    acceptor.set_option(net::ip::tcp::acceptor::reuse_address(true));

    boost::system::error_code ec;
    acceptor.bind({net::ip::tcp::v6(), port}, ec);

    if (ec)
    {
        std::cerr << "Bind failed: " << ec.message() << std::endl;
        return false;
    }

    acceptor.listen();
    return true;
}

void Server::setConnection(std::shared_ptr<net::ip::tcp::socket> socket)
{
    std::cout << "New connection from " << socket->remote_endpoint() << std::endl;
    auto session = std::make_shared<SmartSession>(socket, SmartSession::Type::SERVER);

    {
        std::lock_guard lock(sessionMutex);
        sessions.push_back(session);
    }

    session->net_session->setOnDisconnect([this, session]()
    {
        {
            std::lock_guard lock(sessionMutex);
            sessions.remove(session);
        }
        std::cout << "Client disconnected" << std::endl;
        std::cout << "Number of active clients: " << sessions.size() << std::endl;
    });

    session->setSMTPHandling([this, session](boost::asio::const_buffer msg) { SMTPHandling(msg, session); });

    session->setConnection();
}

void Server::SMTPHandling(boost::asio::const_buffer msg, std::shared_ptr<SmartSession> session)
{
    const std::string cmd(
        std::string(static_cast<const char*>(msg.data()), msg.size()));

    auto rpl = session->smtp_session->OnMessage(cmd.c_str());
    session->net_session->send(net::buffer(rpl));

    std::cout << "Received message from: " << session->net_session->getSocket()->
                                                       remote_endpoint() << std::endl;
    std::cout << "Received message: " << cmd << std::endl;
    std::cout << "Reply: " << rpl << std::endl;
}
