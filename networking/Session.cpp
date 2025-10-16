//
// Created by alkir on 10/3/2025.
//

#include "Session.h"
#include <iostream>
#include <mutex>

constexpr std::size_t BUFFER_SIZE = 1024;
constexpr int RECONNECT_DELAY_MS = 2000;

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

bool Session::run()
{
    if (isRunning) return false;
    isRunning = true;

    try
    {
        read();
    }
    catch (const boost::system::system_error& e) { if (onDisconnect) onDisconnect(); }
    return true;
}

bool Session::send(const std::string& data)
{
    if (!socket->is_open()) return false;

    writeQueue.push_back(data);
    if (!isWriting) write();
    return true;
};

void Session::write()
{
    isWriting = true;
    net::async_write(*socket, net::buffer(writeQueue.front()),
                     [this](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/)
                     {
                         if (!ec)
                         {
                             writeQueue.pop_front();
                             if (!writeQueue.empty()) write();
                             else isWriting = false;
                         }
                         else
                             if (onDisconnect && ec != net::error::operation_aborted) onDisconnect();

                     });
}

void Session::read()
{
    if (!socket || !socket->is_open()) return;

    socket->async_read_some(net::buffer(buffer),
                            [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
                            {
                                if (!ec)
                                {
                                    if (onMessageReceived)
                                        onMessageReceived(std::string(buffer.data(), bytes_transferred));
                                    read();
                                }
                                else if (onDisconnect && ec != net::error::operation_aborted) onDisconnect();
                            });
}