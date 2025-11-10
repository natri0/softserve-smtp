//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

#include "../networking/SSL/KeyExchanger.h"
#include "../logger/Include/Logger.h"

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
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
    changeLogLevel("PROD");

    init();
    connect();

    return true;
}

void Client::init()
{
    session->net_session->setOnDisconnect([this]() { reconnect(); });

    session->net_session->setOnConnected([this]()
    {
        LOG_INFO(PROD_LOG_LEVEL) << "Client connected";
        auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
            smtp::ssl::KeyExchange::generateKeyPair()
        );
        session->setSMTPHandling([this](boost::asio::const_buffer msg) { SMTPHandling(msg); });
        session->setConnection();

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
<<<<<<< HEAD:client/Client/Client.cpp
            if (!session->isConnected()) reconnect();
=======
            if (!session->net_session->isConnected()) reconnect();
            else run();
>>>>>>> origin/server:client/Client.cpp
        }
    });
}

bool Client::run()
{
<<<<<<< HEAD:client/Client/Client.cpp
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
=======
    if (isRunning) return false;
    LOG_INFO(PROD_LOG_LEVEL) << "Client is running";

    io_thread = std::jthread([this]() { io.run(); });
    isRunning = true;

    return true;
}

void Client::SMTPHandling(boost::asio::const_buffer msg)
{
    std::string cmd(static_cast<const char*>(msg.data()), msg.size());

    LOG_INFO(DEBUG_LOG_LEVEL) << "Received message: " << cmd;

    // temp; will integrate smtp logic in future
    if (cmd.starts_with("220"))
    {
        session->net_session->send(net::buffer("EHLO example.com\r\n"));
    }
    else if (cmd.find("250 HELP") != std::string::npos)
    {
        canSend = true;
        LOG_INFO(DEBUG_LOG_LEVEL) << "Received 250 HELP; Ready to send";
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
>>>>>>> origin/server:client/Client.cpp
};

bool Client::sendMail(EmailMessage e_msg)
{
    // temp
    sendInfo.emplace("MAIL FROM:<reverse@smtp.test>\r\n");
    sendInfo.emplace("RCPT TO:<forward1@smtp.test>\r\n");
    sendInfo.emplace("DATA\r\n");
    sendInfo.emplace("RSET\r\n");

    if (!session->net_session->isConnected())
    {
<<<<<<< HEAD:client/Client/Client.cpp
        std::cout << "Client is not connected" << std::endl;
        m_lastError = "Client is not connected.";
        return false;
    }

    m_emailInfo = e_msg;
    std::cout << "Email queued for sending..." << std::endl;
=======
        LOG_WARNING(DEBUG_LOG_LEVEL) << "Client is not connected";
        return false;
    }

    email_info = e_msg;
    if (canSend)
    {
        session->net_session->send(net::buffer(sendInfo.front()));
        sendInfo.pop();
    }
    LOG_INFO(PROD_LOG_LEVEL) << "Sending...";
>>>>>>> origin/server:client/Client.cpp
    return true;
}

// will be modified after GUI integration
void Client::changeLogLevel(const std::string& level) const
{
    if (level == "NONE")
        Logger::getInstance().setLevel(LogLevel::NONE);
    if (level == "PROD")
        Logger::getInstance().setLevel(PROD_LOG_LEVEL);
    if (level == "DEBUG")
        Logger::getInstance().setLevel(DEBUG_LOG_LEVEL);
    if (level == "TRACE")
        Logger::getInstance().setLevel(TRACE_LOG_LEVEL);
}

bool Client::stop()
{
<<<<<<< HEAD:client/Client/Client.cpp
    session->disconnect();
=======
    if (!isRunning) return false;
    isRunning = false;

    session->net_session->disconnect();

>>>>>>> origin/server:client/Client.cpp
    io.stop();

    return true;
}
