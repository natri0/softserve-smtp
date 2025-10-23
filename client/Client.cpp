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
    std::cout << "Client[" << port << "]" << std::endl;
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
    session->setOnConnected([this]()
    {
        // std::cout << "Connected to server " << server_endpoint << std::endl;

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
                // session->send(net::buffer("Hello from test!\r\n.\r\n"));
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
