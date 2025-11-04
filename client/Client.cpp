//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

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
    sendInfo.emplace("MAIL FROM:<reverse@smtp.test>\r\n");
    sendInfo.emplace("RCPT TO:<forward1@smtp.test>\r\n");
    sendInfo.emplace("DATA\r\n");
    sendInfo.emplace("QUIT\r\n");

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
        auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
            smtp::ssl::KeyExchange::generateKeyPair()
        );
        session->setOnMessage([this, keys](boost::asio::const_buffer msg)
        {
            SSLHandling(msg, keys);
            boost::asio::post(session->getSocket()->get_executor(), [this]()
            {
                session->setOnMessage([this](boost::asio::const_buffer msg) { SMTPHandling(msg); });
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
}

void Client::SSLHandling(boost::asio::const_buffer msg,
                         std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys)
{
    auto& clientPriv = keys->first;
    auto& clientPub = keys->second;

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
}

void Client::SMTPHandling(boost::asio::const_buffer msg)
{
    std::string cmd(static_cast<const char*>(msg.data()), msg.size());

    std::cout << "Received message: " << cmd << std::endl;

    if (cmd.starts_with("220"))
    {
        session->send(net::buffer("EHLO example.com\r\n"));
    }
    else if (cmd.find("250 HELP") != std::string::npos)
    {
        session->send(net::buffer(sendInfo.front()));
        sendInfo.pop();
    }
    else if (cmd.starts_with("250 Action completed"))
    {
        if (!sendInfo.empty())
        {
            session->send(net::buffer(sendInfo.front()));
            sendInfo.pop();
        }
    }
    else if (cmd.find("503") != std::string::npos)
    {
        std::cout << "problem" << std::endl;
    }
    else if (cmd.starts_with("354"))
    {
        session->send(net::buffer("test body\r\n.\r\n"));
    }
    else
    {
        std::cout << "Want to proceed? Yes: 1\tNo: 0" << std::endl;
    }
};

bool Client::sendMail(EmailMessage e_msg)
{
    if (!session->isConnected())
    {
        std::cout << "Client is not connected" << std::endl;
        return false;
    }

    email_info = e_msg;
    // session->send(net::buffer("EHLO example.com\r\n"));
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
