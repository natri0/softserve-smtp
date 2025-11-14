//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

#include "../networking/SSL/KeyExchanger.h"
#include "../logger/Include/Logger.h"

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const ClientSettings& settings) :
    //server_endpoint(net::ip::make_address(settings.server), settings.port),
    session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
    timer(io),
    m_recipientIndex(0),
    m_settings(settings)
{
    if (m_settings.securityType == 1)
    {
        try
        {
            LOG_INFO(DEBUG_LOG_LEVEL) << "SSL/TLS enabled. Creating SSL context.";
            sslContext = smtp::ssl::SSLContextFactory::createClientContext();
        }
        catch (std::exception& e)
        {
            throw std::runtime_error(std::string("Failed to create SSL context: ") + e.what());
        }
    }
    else
    {
        LOG_INFO(DEBUG_LOG_LEVEL) << "SSL/TLS disabled. Skipping SSL context creation.";
    }
};

Client::~Client()
{
    stop();
}

bool Client::start()
{
    changeLogLevel(m_settings.logLevel);

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

        session->setSMTPHandling([this](boost::asio::const_buffer msg) {
            SMTPHandling(msg);
        });
        session->setConnection();
        session->net_session->run();
    });}

void Client::connect()
{
    if (session->net_session->isConnected()) return;

    try
    {
        // Resolver to make both IP and hostnames work
        net::ip::tcp::resolver resolver(io);

        auto endpoints = resolver.resolve(m_settings.server, std::to_string(m_settings.port));

        LOG_INFO(DEBUG_LOG_LEVEL) << "Hostname resolved.";

        session->net_session->connect(*endpoints.begin());
    }
    catch (std::exception &e)
    {
        LOG_WARNING(PROD_LOG_LEVEL) << "Failed to resolve or connect to "
                                    << m_settings.server << ": " << e.what();

        m_lastError = "Failed to resolve hostname: " + std::string(e.what());

        reconnect();
    }
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
        LOG_WARNING(DEBUG_LOG_LEVEL) << "Client is not connected";
        m_lastError = "Client is not connected.";
        return false;
    }

    m_emailInfo = e_msg;
    LOG_INFO(PROD_LOG_LEVEL) << "Sending...";

    if (canSend)
    {
        session->net_session->send(net::buffer(sendInfo.front()));
        sendInfo.pop();
    }

    return true;
}

// Modified by passing settings that contain LogLevel
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
    LOG_WARNING(DEBUG_LOG_LEVEL) << "Client stopped";

    session->net_session->disconnect();

    io.stop();

    return true;
}
