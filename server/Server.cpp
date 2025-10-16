//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

Server::Server() : acceptor(net::ip::tcp::acceptor(io))
{
}

Server::~Server()
{
    stopServer();
}

bool Server::init()
{
    // initialization from config

    if (!setUpAcceptor()) return false;
    return true;
}

bool Server::stopServer()
{
    boost::system::error_code ec;

    acceptor.cancel(ec);

    if (acceptor.is_open())
        acceptor.close(ec);

    if (ec) std::cerr << "Error closing acceptor: " << ec.message() << std::endl;

    for (auto& session : sessions)
        session->disconnect();
    sessions.clear();

    io.stop();
    return true;
}

bool Server::run()
{
    if (!acceptor.is_open()) return false;
    runAcceptor();

    io.run();

    return true;
}

void Server::runAcceptor()
{
    auto socket = std::make_shared<net::ip::tcp::socket>(io);

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            const auto session = std::make_shared<Session>(socket);

            session->setOnMessage([this, session](const std::string& msg)
            {
                session->send(this->print(msg));
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

void Server::onDisconnect(std::shared_ptr<Session> session)
{
    session->disconnect();

    sessionMutex.lock();
    sessions.erase(std::remove(sessions.begin(), sessions.end(), session), sessions.end());
    sessionMutex.unlock();
}

std::string Server::print(const std::string& str)
{
    std::cout << str << std::endl;
    return "hey";
}
