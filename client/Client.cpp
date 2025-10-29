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
    timer(io)
    //sslContext(smtp::ssl::SSLContextFactory::createClientContext())
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

void Client::init()
{
    // crypto key exchange section
    session->setOnConnected([this]()
    {
        std::cout << "Client connected" << std::endl;
        const auto [clientPriv, clientPub] = smtp::ssl::KeyExchange::generateKeyPair();

        session->setOnMessage([this, clientPriv, clientPub](boost::asio::const_buffer msg)
        {
            std::cout << "client private key size: " << clientPriv.size() << std::endl;
            std::cout << "client public key size: " << clientPub.size() << std::endl;
            std::cout << "Get msg: [Received " << msg.size() << " bytes of server public key]" << std::endl;

            session->send(net::buffer(clientPub));

            std::vector serverPub(
                static_cast<const unsigned char*>(msg.data()),
                static_cast<const unsigned char*>(msg.data()) + msg.size()
            );

            const auto sharedSecret = smtp::ssl::KeyExchange::performDHExchange(serverPub, clientPriv);
            const auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);

            session->setKey(sessionKey);

            std::cout << "Session key established" << std::endl;
            std::cout << sessionKey.size() << std::endl;

            session->setOnMessage([this](boost::asio::const_buffer msg)
            {
                std::string cmd(static_cast<const char*>(msg.data()), msg.size());

                std::cout << "Received message: " << cmd << std::endl;

                if (cmd.starts_with("220"))
                {
                    session->send(net::buffer("HELO example.com\r\n"));
                }
                else if (cmd.starts_with("250") && cmd.find("Hello") != std::string::npos)
                {
                    session->send(net::buffer("MAIL FROM:<test@example.com>\r\n"));
                }
                else if (cmd.starts_with("250 OK"))
                {
                    session->send(net::buffer("RCPT TO:<admin@example.com>\r\n"));
                }
                else if (cmd.starts_with("250 Accepted"))
                {
                    session->send(net::buffer("DATA\r\n"));
                }
                else if (cmd.starts_with("354"))
                {
                    session->send(net::buffer(email_info.body + "\r\n.\r\n"));
                }
                else if (cmd.starts_with("250 Message"))
                {
                    session->send(net::buffer("QUIT\r\n"));
                }
                else
                {
                    std::cout << "Want to proceed? Yes: 1\tNo: 0" << std::endl;
                }
            });
        });
        session->run();
    });

    session->setOnDisconnect([this]() { reconnect(); });
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
    if (isRunning) return false;
    std::cout << "Client is running" << std::endl;

    io_thread = std::jthread([this]() { io.run(); });
    isRunning = true;

    return true;
};

bool Client::sendMail(EmailMessage e_msg)
{
    if (!session->isConnected())
    {
        std::cout << "Client is not connected" << std::endl;
        return false;
    }

    email_info = e_msg;
    session->send(net::buffer("HELO example.com\r\n"));
    std::cout << "Sending..." << std::endl;
    return true;
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    session->disconnect();
    io.stop();

    return true;
}
