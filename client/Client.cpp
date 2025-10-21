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
    // stop();
}

bool Client::start()
{
    if (!init()) return false;
    connect();
    if (!run()) return false;
    return true;
}

bool Client::init()
{
    session->setOnMessage([this](const std::string& msg)
    {
        // here will be smtp logic for choosing proper reaction to the command from server
        // std::cout << msg << std::endl;
        session->send(email_info.body);
    });

    session->setOnConnected([this]()
    {
        std::cout << "Client started" << std::endl;
    });

    session->setOnDisconnect([this]() { reconnect(); });

    return true;
}

void Client::connect()
{
    if (session->isConnected()) return;
    session->connect(server_endpoint);
}

void Client::reconnect()
{
    timer.cancel();
    timer.expires_after(std::chrono::seconds(RECONNECT_DELAY_TIME));
    timer.async_wait([this](boost::system::error_code ec)
    {
        if (!ec)
        {
            connect();
            if (!session->isConnected()) reconnect();
            else run();
        }
    });
}


bool Client::run()
{
    // if (!session->isConnected()) return false;

    if (isRunning) return false;
    isRunning = true;

    io_thread = std::jthread([this]() { io.run(); });
    session_thread = std::jthread([this]() { session->run(); });

    return true;
};

void Client::sendMail(EmailMessage e_msg)
{
    if (!isRunning) return;
    // here will be init msg for e-mail transferring

    email_info = e_msg;
    session->send(email_info.body);
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    session->disconnect();
    io.stop();

    return true;
}
