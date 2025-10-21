//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

#include <cmath>

#include "ThreadPool/include/ThreadPool.hpp"

// temp till we don't have parser
constexpr uint8_t THREADS_NUM = 8;

Server::Server() : io(std::make_shared<boost::asio::io_context>()),
                   work(io->get_executor()), acceptor(net::ip::tcp::acceptor(*io)),
                   threadPool(std::make_unique<ThreadPool>(THREADS_NUM))
                   ,sslContext(smtp::ssl::SSLContextFactory::createServerContext())
{
}

Server::~Server()
{
    stop();
}

bool Server::init()
{
    // initialization from config
    ui->showBanner(port);

    if (!setUpAcceptor()) return false;
    threadPool->start();

    return true;
}

bool Server::stop()
{
    boost::system::error_code ec;
    acceptor.cancel(ec);

    if (acceptor.is_open())
        acceptor.close(ec);

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

    for (uint8_t i = 0; i < THREADS_NUM; ++i)
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

    // std::unique_lock lock(mainThreadMutex);
    // mainThreadCV.wait(lock, [this]() { return isStopping; });
    stop();
}

void Server::runAcceptor()
{
    auto socket = std::make_shared<net::ip::tcp::socket>(*io);

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            const auto session = std::make_shared<Session>(socket);

            // auto [serverPub, serverPriv] = smtp::ssl::KeyExchange::generateKeyPair();

            session->setOnMessage([this, session](const std::string& msg)
            {
                // temp instead of waiting for smtp
                session->send(this->print(msg));
            });

            session->setOnConnected([this]()
            {
                std::cout << "Client connected" << std::endl;
            });

            session->setOnDisconnect([this]()
            {
                std::cout << "Client disconnected" << std::endl;
            });

            {
                std::lock_guard lock(sessionMutex);
                sessions.push_back(session);
            }

            session->run();
        }
        else
        {
            // std::cerr << "Accept failed: " << ec.message() << std::endl;
        }

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

std::string Server::print(const std::string& str)
{
    std::cout << str << std::endl;
    return str;
}
