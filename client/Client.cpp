//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

#include "CryptoManager.h"
#include "../networking/SSL/KeyExchanger.h"

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<Session>(std::make_shared<net::ip::tcp::socket>(io))),
    timer(io),
    sslContext(smtp::ssl::SSLContextFactory::createClientContext())
{
};

Client::~Client()
{
    stop();
}

bool Client::start()
{
    init();
    connect();
    run();

    return true;
}

bool Client::init()
{
    // crypto key exchange section
    session->setOnConnected([this]()
    {
        std::cout << "Client connected" << std::endl;
        auto [clientPriv, clientPub] = smtp::ssl::KeyExchange::generateKeyPair();

        session->setOnMessage([this, clientPriv, clientPub](boost::asio::const_buffer msg)
        {
            std::cout << "My client private key: " << clientPriv.size() << std::endl;

            std::cout << "Get msg: " << std::string(reinterpret_cast<const char*>(msg.data()), msg.size()) <<
                std::endl;
            std::cout << "My client private key: " << clientPriv.size() << std::endl;

            session->send(net::buffer(clientPub));

            std::vector<unsigned char> serverPub(
                static_cast<const unsigned char*>(msg.data()),
                static_cast<const unsigned char*>(msg.data()) + msg.size()
            );

            const auto sharedSecret = smtp::ssl::KeyExchange::performDHExchange(serverPub, clientPriv);
            const auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);

            session->setKey(sessionKey);

            std::cout << "Session key established" << std::endl;

            session->setOnMessage([this](boost::asio::const_buffer msg)
            {
                // here will be smtp logic for choosing proper reaction to the command from server
            });
        });
    });
    //

    session->setOnDisconnect([this]() { reconnect(); });

    return true;
}

void Client::connect()
{
    // if (session->isConnected()) return;
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
    if (isRunning) return false;
    std::cout << "Client running" << std::endl;

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
    // session->send(email_info.body);
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    session->disconnect();
    io.stop();

    return true;
}
