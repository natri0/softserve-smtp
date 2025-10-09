//
// Created by alkir on 10/3/2025.
//

#include "Session.h"

constexpr std::size_t BUFFER_SIZE = 1024;

Session::Session(std::shared_ptr<net::ip::tcp::socket> _socket) : socket(_socket)
{
}

Session::~Session() { disconnect(); }

bool Session::connect(const net::ip::tcp::endpoint& endpoint)
{
    try
    {
        if (socket->is_open()) socket->close();
        socket->connect(endpoint);
        return true;
    }
    catch (const boost::system::system_error& e)
    {
        if (onDisconnect) onDisconnect();
        return false;
    }
}

bool Session::disconnect()
{
    if (!socket->is_open()) return false;
    boost::system::error_code ec;
    socket->shutdown(net::socket_base::shutdown_both, ec);

    socket->close(ec);
    return true;
}

void Session::run()
{
    try
    {
        while (socket->is_open()) read();
    }
    catch (const boost::system::system_error& e) { if (onDisconnect) onDisconnect(); }
}

void Session::send(const std::string& data)
{
    if (!socket->is_open()) return;
    try
    {
        net::write(*socket, net::buffer(data));
    }
    catch (const boost::system::system_error& e) { if (onDisconnect) onDisconnect(); }
};

void Session::read()
{
    if (!socket->is_open()) return;

    try
    {
        char buffer[BUFFER_SIZE];
        const std::size_t n = socket->read_some(net::buffer(buffer));
        if (onMessageReceived) onMessageReceived(std::string(buffer, n));
    }
    catch (const boost::system::system_error& e)
    {
        if (onDisconnect) onDisconnect();
        throw;
    }
};
