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
    sslContext(smtp::ssl::SSLContextFactory::createClientContext()),
    m_recipientIndex(0)
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
                    session->send(net::buffer("MAIL FROM:<" + m_emailInfo.from + ">\r\n"));
                }
                else if (cmd.starts_with("250 OK"))
                {
                  m_recipientIndex = 0;
                  if (!m_emailInfo.to.empty())
                  {
                    session->send(net::buffer("RCPT TO:<" + m_emailInfo.to[m_recipientIndex] + ">\r\n"));
                    m_recipientIndex++;
                  }
                  else
                  {
                    m_lastError = "Error: No recipients specified.";
                    session->disconnect();
                    io.stop();
                  }
                }
                else if (cmd.starts_with("250 Accepted"))
                {
                  if (m_recipientIndex < m_emailInfo.to.size())
                  {
                    session->send(net::buffer("RCPT TO:<" + m_emailInfo.to[m_recipientIndex] + ">\r\n"));
                    m_recipientIndex++;
                  }
                  else
                  {
                    session->send(net::buffer("DATA\r\n"));
                  }
                }
                else if (cmd.starts_with("354"))
                {
                  std::string fullBody = "Subject: " + m_emailInfo.subject + "\r\n";
                  fullBody += "From: " + m_emailInfo.from + "\r\n";

                  fullBody += "To: " + m_emailInfo.to[0] + "\r\n\r\n";
                  fullBody += m_emailInfo.body + "\r\n.\r\n";

                  session->send(net::buffer(m_emailInfo.body));
                }
                else if (cmd.starts_with("250 Message"))
                {
                    session->send(net::buffer("QUIT\r\n"));
                }
                else if (cmd.starts_with("5") || cmd.starts_with("4"))
                {
                  std::cerr << "SMTP Error: " << cmd << std::endl;
                  m_lastError = cmd;
                  session->disconnect();
                  io.stop();
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
        }
    });
}

bool Client::run()
{
  std::cout << "Client is running." << std::endl;
  try {
    io.run();
    std::cout << "Client io.run() finished." << std::endl;
    return true; // Loop finished cleanly
  }
  catch (std::exception& e) {
    std::cerr << "Client io.run() exception: " << e.what() << std::endl;
    m_lastError = e.what(); 
    return false; // Loop failed
  }
};

bool Client::sendMail(EmailMessage e_msg)
{
    if (!session->isConnected())
    {
        std::cout << "Client is not connected" << std::endl;
        m_lastError = "Client is not connected.";
        return false;
    }

    m_emailInfo = e_msg;
    std::cout << "Email queued for sending..." << std::endl;
    return true;
}

bool Client::stop()
{
    session->disconnect();
    io.stop();

    return true;
}
