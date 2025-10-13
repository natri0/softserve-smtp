//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

Server::Server() : threadPool(std::make_unique<ThreadPool>()), acceptor(net::ip::tcp::acceptor(io))
{
}

Server::~Server()
{
    stopServer();
}

void Server::stopServer()
{
    boost::system::error_code ec;

    acceptor.cancel(ec);

    if (acceptor.is_open())
        acceptor.close(ec);

    if (ec) std::cerr << "Error closing acceptor: " << ec.message() << std::endl;

    io.stop();
}

void Server::run()
{
    setUpAcceptor();

    runAcceptor();

    io.run();
}

void Server::runAcceptor()
{
    if (!acceptor.is_open()) return;
    auto socket = std::make_shared<net::ip::tcp::socket>(io);

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            const auto session = std::make_shared<Session>(socket);

            session->setOnMessage([this](const std::string& msg)
            {
                this->print(msg);
            });

            session->setOnDisconnect([]()
            {
                std::cout << "Client disconnected" << std::endl;
            });

            sessions.push_back(session);
            session->run();
        }
        else
        {
            std::cerr << "Accept failed: " << ec.message() << std::endl;
        }

        runAcceptor();
    });
}

void Server::setUpAcceptor()
{
    acceptor.open(net::ip::tcp::v6());
    acceptor.set_option(net::ip::v6_only(false));
    acceptor.set_option(net::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind({net::ip::tcp::v6(), port});
    acceptor.listen();
}

void Server::print(const std::string& str)
{
    std::cout << str << std::endl;
}
