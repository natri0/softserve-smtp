//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

#include <cmath>

#include "ThreadPool/include/ThreadPool.hpp"
#include "../networking/SSL/KeyExchanger.h"
#include "SMTPSession.h"

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
        if (!ec)
        {
            std::cout << "New connection from " << socket->remote_endpoint() << std::endl;
            const auto session = std::make_shared<Session>(socket);
            auto smtp_session = std::make_shared<ISXSMTP::SMTPSession>();

            auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
                smtp::ssl::KeyExchange::generateKeyPair()
            );

            session->setOnDisconnect([this, session]()
            {
                {
                    std::lock_guard lock(sessionMutex);
                    sessions.remove(session);
                }
                std::cout << "Client disconnected" << std::endl;
                std::cout << "Number of active clients: " << sessions.size() << std::endl;
            });

            session->setOnMessage([this, session, keys, smtp_session](boost::asio::const_buffer msg)
            {
                SSLHandling(msg, session, keys);

                boost::asio::post(session->getSocket()->get_executor(), [this, session, smtp_session]()
                {
                    session->setOnMessage([this, session, smtp_session](boost::asio::const_buffer msg)
                    {
                        SMTPHandling(msg, session, smtp_session);
                    });
                    session->send(net::buffer(ISXSMTP::SMTPSession().OnConnect()));
                });
            });

            session->send(net::buffer(keys->second));

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

void Server::SSLHandling(boost::asio::const_buffer msg, std::shared_ptr<Session> session,
                         std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys)
{
    auto& serverPriv = keys->first;
    auto& serverPub = keys->second;

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
}

void Server::SMTPHandling(boost::asio::const_buffer msg, std::shared_ptr<Session> session,
                          std::shared_ptr<ISXSMTP::SMTPSession> smtp_session)
{
    const std::string cmd(
        std::string(static_cast<const char*>(msg.data()), msg.size()));

    auto rpl = smtp_session->OnMessage(cmd.c_str());
    session->send(net::buffer(rpl));

    std::cout << "Received message from: " << session->getSocket()->
                                                       remote_endpoint() << std::endl;
    std::cout << "Received message: " << cmd << std::endl;
    std::cout << "Reply: " << rpl << std::endl;
}
