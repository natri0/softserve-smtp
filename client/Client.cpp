//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

#include "../networking/SSL/include/KeyExchanger.h"
#include "../networking/SSL/include/SSLContextFactory.h"
#include "../utils/Base64.h"

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
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

void Client::init()
{
    session->net_session->setOnDisconnect([this]() { reconnect(); });

    session->net_session->setOnConnected([this]()
    {
        std::cout << "Client connected" << std::endl;
        auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
            smtp::ssl::KeyExchange::generateKeyPair()
        );
        session->setSMTPHandling([this](boost::asio::const_buffer msg) { SMTPHandling(msg); });
        session->setConnection();

        session->net_session->run();
    });
}

void Client::connect()
{
    if (session->net_session->isConnected()) return;
    session->net_session->connect(server_endpoint);
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
            if (!session->net_session->isConnected()) reconnect();
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

void Client::SMTPHandling(boost::asio::const_buffer msg)
{
    std::string cmd(static_cast<const char*>(msg.data()), msg.size());

    std::cout << "Received message: " << cmd << std::endl;

    // temp; will integrate smtp logic in future
    if (cmd.starts_with("220"))
    {
        session->net_session->send(net::buffer("EHLO example.com\r\n"));
    }
    else if (cmd.find("250 HELP") != std::string::npos)
    {
        canSend = true;
        std::cout << "Ready to send" << std::endl;
    }
    else if (cmd.starts_with("250 Action completed"))
    {
        if (!sendInfo.empty())
        {
            session->net_session->send(net::buffer(sendInfo.front()));
            sendInfo.pop();
        }
    }
    else if (cmd.starts_with("354"))
    {
        session->net_session->send(net::buffer("test body\r\n.\r\n"));
    }
};

bool Client::sendMail(EmailMessage e_msg)
{
    // temp
    const char raw_creds[] = "\0testuser\0testpass";
    std::string creds(raw_creds, sizeof(raw_creds) - 1);
    auto encoded = Base64::Encode({creds.begin(), creds.end()});
    std::string encodedStr = {encoded.begin(), encoded.end()};
    sendInfo.emplace("AUTH " + encodedStr + "\r\n");
    sendInfo.emplace("MAIL FROM:<reverse@smtp.test>\r\n");
    sendInfo.emplace("RCPT TO:<forward1@smtp.test>\r\n");
    sendInfo.emplace("DATA\r\n");
    sendInfo.emplace("RSET\r\n");

    if (!session->net_session->isConnected())
    {
        std::cout << "Client is not connected" << std::endl;
        return false;
    }

    email_info = e_msg;
    if (canSend)
    {
        session->net_session->send(net::buffer(sendInfo.front()));
        sendInfo.pop();
    }
    std::cout << "Sending..." << std::endl;
    return true;
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    session->net_session->disconnect();

    io.stop();

    return true;
}
