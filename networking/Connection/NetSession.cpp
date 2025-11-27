//
// Created by alkir on 10/3/2025.
//

#include "NetSession.h"
#include "../../logger/Include/Logger.h"
#include <iostream>

constexpr std::size_t BUFFER_SIZE = 1024 * 1024;
constexpr int RECONNECT_DELAY_MS = 2000;

NetSession::NetSession(std::shared_ptr<net::ip::tcp::socket> _socket) : socket(_socket)
{
}

NetSession::~NetSession() { disconnect(); }

void NetSession::connect(const net::ip::tcp::endpoint& endpoint)
{
    if (socket->is_open()) socket->close();
    socket->async_connect(endpoint, [this](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                LOG_INFO(DEBUG_LOG_LEVEL) << "Connected";
                connected = true;
                if (onConnected) onConnected();
            }
            else
            {
                connected = false;
                if (onDisconnect) onDisconnect();
                LOG_INFO(DEBUG_LOG_LEVEL) << "Connect failed: " << ec.message();
            }
        });
}

bool NetSession::disconnect()
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

bool NetSession::run()
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

bool NetSession::send(boost::asio::const_buffer data)
{
    if (!socket->is_open()) return false;
    auto buf = std::make_shared<std::vector<uint8_t>>(
        (const uint8_t*)data.data(),
        (const uint8_t*)data.data() + data.size()
    );
    writeQueue.push_back(buf);
    //writeQueue.push_back(data);
    if (!isWriting) write();
    return true;
}

void NetSession::write()
{
    isWriting = true;
   
    auto& buf = writeQueue.front();
    net::const_buffer data(buf->data(), buf->size());
   
    std::shared_ptr<std::vector<unsigned char>> encryptedData;

    if (cryptoManager.get())
    {
        std::string plain(
            reinterpret_cast<const char*>(buf->data()),
            buf->size()
        );
        encryptedData = std::make_shared<std::vector<unsigned char>>(cryptoManager->encrypt(plain));
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
                             LOG_INFO(DEBUG_LOG_LEVEL) << "write failed: " << ec.message();
                             self->connected = false;
                             self->disconnect();
                         }
                     });
}

void NetSession::read()
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
                                    LOG_INFO(PROD_LOG_LEVEL) << "read failed: " << ec.message();
                                    self->disconnect();
                                }
                            });
}

void NetSession::clearCrypto()
{
    cryptoManager.reset();
}
