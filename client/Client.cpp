//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<Session>(std::make_shared<net::ip::tcp::socket>(io))),
    timer(io),
    sslContext(smtp::ssl::SSLContextFactory::createServerContext())
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
    return true;
}

bool Client::init()
{
    // auto [clientPub, clientPriv] = smtp::ssl::KeyExchange::generateKeyPair();
    // crypto key exchange section
    // session->setOnMessage([this, clientPriv, clientPub](const std::string& msg)
    session->setOnMessage([this](const std::string& msg)
    {
        // auto serverPub = Session::deserializeKey(msg);
        //
        // session->send(Session::serializeKey(clientPub));
        //
        // auto sharedSecret = smtp::ssl::KeyExchange::performDHExchange(serverPub, clientPriv);
        // auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);
        //
        // session->setKey(sessionKey);
        //
        // std::cout << "Session key established" << std::endl;

        run();
    });
    //

    session->setOnConnected([this]()
    {
        std::cout << "Client connected" << std::endl;
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

    session->setOnMessage([](const std::string& msg)
    {
        // here will be smtp logic for choosing proper reaction to the command from server
    });

    io_thread = std::jthread([this]() { io.run(); });
    isRunning = true;
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
