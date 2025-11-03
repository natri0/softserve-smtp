//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

#include <cmath>

#include "ThreadPool/include/ThreadPool.hpp"
#include "../networking/SSL/KeyExchanger.h"

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

    threadPool = std::make_unique<ThreadPool>(thread_pool_size);

    //
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

    for (auto& session : sessions)
        session->disconnect();
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

    for (uint8_t i = 0; i < thread_pool_size; ++i)
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
        if (!ec)
        {
            std::cout << "New connection from " << socket->remote_endpoint() << std::endl;
            const auto session = std::make_shared<Session>(socket);
            auto [serverPriv, serverPub] = smtp::ssl::KeyExchange::generateKeyPair();

            session->setOnDisconnect([this, session]()
            {
                {
                    std::lock_guard lock(sessionMutex);
                    sessions.remove(session);
                }
                std::cout << "Client disconnected" << std::endl;
                std::cout << "Number of active clients: " << sessions.size() << std::endl;
            });

            session->setOnMessage([this, session, serverPriv, serverPub](boost::asio::const_buffer msg)
            {
                std::cout << "Get msg: [Received " << msg.size() << " bytes of client public key]"
                    << std::endl;
                std::cout << "Server private key size: " << serverPriv.size() << std::endl;
                std::cout << "Server public key size: " << serverPub.size() << std::endl;

                std::vector clientPub(
                    static_cast<const unsigned char*>(msg.data()),
                    static_cast<const unsigned char*>(msg.data()) + msg.size()
                );

                const auto sharedSecret = smtp::ssl::KeyExchange::performDHExchange(
                    clientPub, serverPriv);
                const auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);

                session->setKey(sessionKey);
                std::cout << "Session key established" << std::endl;

                boost::asio::post(session->getSocket()->get_executor(), [this, session]()
                {
                    session->setOnMessage([this, session](boost::asio::const_buffer msg)
                    {
                        const std::string cmd(
                            std::string(static_cast<const char*>(msg.data()), msg.size()));

                        std::cout << "Received message from: " << session->getSocket()->
                                                                           remote_endpoint() << std::endl;
                        std::cout << "Received message: " << cmd << std::endl;

                        if (cmd.starts_with("HELO"))
                            session->send(net::buffer("250 Hello, pleased to meet you\r\n"));
                        else if (cmd.starts_with("MAIL FROM"))
                            session->send(net::buffer("250 OK\r\n"));
                        else if (cmd.starts_with("RCPT TO"))
                            session->send(net::buffer("250 Accepted\r\n"));
                        else if (cmd.starts_with("DATA"))
                            session->send(net::buffer("354 End data with <CR><LF>.<CR><LF>\r\n"));
                        else if (cmd.find("\r\n.\r\n") != std::string::npos)
                            session->send(net::buffer("250 Message accepted for delivery\r\n"));
                        else if (cmd.starts_with("QUIT"))
                            session->send(net::buffer("221 Bye\r\n"));
                        else
                            session->send(net::buffer("500 Unknown command\r\n"));
                    });
                });
            });

            session->setOnDisconnect([this, session]()
            {
                {
                    std::lock_guard lock(sessionMutex);
                    sessions.remove(session);
                }
                std::cout << "Client disconnected" << std::endl;
                std::cout << "Number of active clients: " << sessions.size() << std::endl;
            });

            session->send(net::buffer(serverPub));

            session->run();
            std::cout << "Sent server public key" << std::endl;

            {
                std::lock_guard lock(sessionMutex);
                sessions.push_back(session);
            }
        }
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
