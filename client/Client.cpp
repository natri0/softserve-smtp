//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<Session>(std::make_shared<net::ip::tcp::socket>(io))),
    timer(io)
{
};

Client::~Client()
{
    stop();
}

void Client::init()
{
    session->setOnMessage([this](const std::string& msg)
    {
        // here will be smtp logic for choosing proper reaction to the command from server
        std::cout << msg << std::endl;
        session->send("smth");
    });

    session->setOnDisconnect([this]() { reconnect(); });
}

void Client::connect()
{
    if (!session->connect(server_endpoint))
    {
        reconnect();
    }
}

void Client::reconnect()
{
    timer.expires_after(std::chrono::seconds(RECONNECT_DELAY_TIME));
    timer.async_wait([this](boost::system::error_code ec)
    {
        if (!ec) this->connect();
    });
}


void Client::run()
{
    if (!session->isConnected()) return;

    if (isRunning) return;
    isRunning = true;

    {
        session->run();
        io_thread = std::thread([this]() { io.run(); });
    }
};

void Client::sendMail()
{
    if (!isRunning || !session->isConnected()) return;
    // here will be init msg for e-mail transferring
    session->send("EHLO");
}

void Client::stop()
{
    if (!isRunning) return;
    isRunning = false;

    session->disconnect();
    io_thread.join();
    io.stop();
}
