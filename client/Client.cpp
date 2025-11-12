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
    sslContext(smtp::ssl::SSLContextFactory::createClientContext())
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
    run();

    return true;
}

void Client::init()
{
    session->net()->setOnDisconnect([this]() { reconnect(); });

    session->net()->setOnConnected([this]()
    {
        LOG_INFO(PROD_LOG_LEVEL) << "Client connected";
        auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
            smtp::ssl::KeyExchange::generateKeyPair()
        );
        session->setSMTPHandling([this](boost::asio::const_buffer msg) { SMTPHandling(msg); });
        session->setConnection();

        session->net()->run();
    });
}

void Client::connect()
{
    if (session->net()->isConnected()) return;
    session->net()->connect(server_endpoint);
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
            if (!session->net()->isConnected()) reconnect();
            else run();
        }
    });
}

bool Client::run()
{
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
        session->net()->send(net::buffer("EHLO example.com\r\n"));
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
            session->net()->send(net::buffer(sendInfo.front()));
            sendInfo.pop();
        }
    }
    else if (cmd.starts_with("354"))
    {
        session->net()->send(net::buffer("test body\r\n.\r\n"));
    }
};

bool Client::sendMail(EmailMessage e_msg)
{
    // temp
    sendInfo.emplace("MAIL FROM:<reverse@smtp.test>\r\n");
    sendInfo.emplace("RCPT TO:<forward1@smtp.test>\r\n");
    sendInfo.emplace("DATA\r\n");
    sendInfo.emplace("RSET\r\n");

    if (!session->net()->isConnected())
    {
        LOG_WARNING(DEBUG_LOG_LEVEL) << "Client is not connected";
        return false;
    }

    email_info = e_msg;
    if (canSend)
    {
        session->net()->send(net::buffer(sendInfo.front()));
        sendInfo.pop();
    }
    LOG_INFO(PROD_LOG_LEVEL) << "Sending...";
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
    if (!isRunning) return false;
    isRunning = false;

    session->net()->disconnect();

    io.stop();

    return true;
}
