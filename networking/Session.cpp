//
// Created by alkir on 10/3/2025.
//

#include "Session.h"
#include "Logger.h"
#include <iostream>

constexpr std::size_t BUFFER_SIZE = 1024;
constexpr int RECONNECT_DELAY_MS = 2000;

Session::Session(std::shared_ptr<net::ip::tcp::socket> _socket) : socket(_socket)
{
}

Session::~Session() { disconnect(); }

void Session::connect(const net::ip::tcp::endpoint& endpoint)
{
    if (socket->is_open()) socket->close();
    socket->async_connect(endpoint, [this](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            //LOG_INFO(PROD_LOG_LEVEL) << "Connected";// << std::endl;
            std::cerr << "Connected" << std::endl;
            connected = true;
            if (onConnected) onConnected();
        }
        else
        {
            connected = false;
            if (onDisconnect) onDisconnect();
            std::cerr << "Connect failed: " << ec.message() << std::endl;
            //LOG_INFO(PROD_LOG_LEVEL) << "Connect failed: " << ec.message();
        }
    });
}

bool Session::disconnect()
{
    if (!connected) return false;
    connected = false;

    boost::system::error_code ec;
    socket->cancel(ec);
    socket->shutdown(net::socket_base::shutdown_both, ec);

    socket->close(ec);

    writeQueue.clear();
    isWriting = false;

    return true;
}

bool Session::run()
{
    if (isRunning) return false;
    isRunning = true;

    try { read(); }
    catch (const boost::system::system_error& e)
    {
        connected = false;
        if (onDisconnect)
        {
            disconnect();
            onDisconnect();
        }
    }
    return true;
}

bool Session::send(boost::asio::const_buffer data)
{
    if (!socket->is_open()) return false;

    writeQueue.push_back(data);
    if (!isWriting) write();
    return true;
}

void Session::write()
{
    isWriting = true;

    boost::asio::const_buffer data = writeQueue.front();

    std::shared_ptr<std::vector<unsigned char>> encryptedData;

    if (cryptoManager.get())
    {
        std::string plain = std::string(static_cast<const char*>(writeQueue.front().data()),
                                        writeQueue.front().size());
        encryptedData = std::make_shared<std::vector<unsigned char>>(cryptoManager->encrypt(plain));

        // std::cout << "write data size:" << encryptedData->size() << std::endl;
        // std::cout << encryptedData->data() << std::endl;
        data = net::buffer(*encryptedData);
    }

    net::async_write(*socket, data,
                     [self = shared_from_this(), encryptedData](const boost::system::error_code& ec,
                                                                std::size_t /*bytes_transferred*/)
                     {
                         if (!ec)
                         {
                             self->writeQueue.pop_front();
                             if (!self->writeQueue.empty()) self->write();
                             else self->isWriting = false;
                         }
                         else if (self->onDisconnect && ec != net::error::operation_aborted)
                         {
                             if (self->onDisconnect) self->onDisconnect();
                             //LOG_INFO(PROD_LOG_LEVEL) << "write failed: " << ec.message();
                             std::cout << "write failed: " << ec.message() << std::endl;
                             self->connected = false;
                             self->disconnect();
                         }
                     });
}

void Session::read()
{
    if (!socket || !socket->is_open()) return;

    socket->async_read_some(net::buffer(buffer),
                            [self = shared_from_this()](const boost::system::error_code& ec,
                                                        const std::size_t bytes_transferred)
                            {
                                if (!ec)
                                {
                                    if (self->onMessageReceived)
                                    {
                                        if (self->cryptoManager.get())
                                        {
                                            const std::vector<unsigned char> data{
                                                self->buffer.begin(), self->buffer.begin() + bytes_transferred
                                            };
                                            // std::cout << "read data size:" << data.size() << std::endl;
                                            // std::cout << data.data() << std::endl;
                                            // std::cout << self->buffer.data() << std::endl;

                                            self->decrypted_data = self->cryptoManager->decrypt(data);

                                            self->onMessageReceived(
                                                net::buffer(self->decrypted_data, self->decrypted_data.size()));
                                        }
                                        else
                                            self->onMessageReceived(
                                                net::buffer(self->buffer.data(), bytes_transferred));
                                    }
                                    self->read();
                                }
                                else if (self->onDisconnect && ec != net::error::operation_aborted)
                                {
                                    if (self->onDisconnect) self->onDisconnect();
                                    //LOG_INFO(PROD_LOG_LEVEL) << "read failed: " << ec.message();
                                    std::cout << "read failed: " << ec.message() << std::endl;
                                    self->disconnect();
                                }
                            });
}
