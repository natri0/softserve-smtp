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

bool Client::start()
{
    if (!init()) return false;
    if (!connect()) return false;
    if (!run()) return false;
    return true;
}

bool Client::init()
{
    session->setOnMessage([this](const std::string& msg)
    {
        // here will be smtp logic for choosing proper reaction to the command from server
        std::cout << msg << std::endl;
        session->send("smth");
    });

    session->setOnDisconnect([this]() { reconnect(); });

    return true;
}

bool Client::connect()
{
    if (session->isConnected()) return false;

    if (!session->connect(server_endpoint))
    {
        reconnect();
        return false;
    }
    return true;
}

void Client::reconnect()
{
    timer.expires_after(std::chrono::seconds(RECONNECT_DELAY_TIME));
    timer.async_wait([this](boost::system::error_code ec)
    {
        if (!ec) this->connect();
    });
}


bool Client::run()
{
    if (!session->isConnected()) return false;

    if (isRunning) return false;
    isRunning = true;

    io_thread = std::jthread([this]() { io.run(); });

    session->run();

    return true;
};

void Client::sendMail()
{
    if (!isRunning || !session->isConnected()) return;
    // here will be init msg for e-mail transferring
    session->send("EHLO");
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    session->disconnect();
    io.stop();

    return true;
}
