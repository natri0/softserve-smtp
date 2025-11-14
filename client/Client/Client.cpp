//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

#include "../networking/SSL/KeyExchanger.h"
#include "../logger/Include/Logger.h"

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

Client::Client(const ClientSettings& settings, StatusCallback statusCallback) :
    //server_endpoint(net::ip::make_address(settings.server), settings.port),
    session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
    timer(io),
    m_recipientIndex(0),
    m_settings(settings),
    m_statusCallback(statusCallback)
{
    if (m_settings.securityType == 0) // 0 = None
    {
        m_useEncryption = false;
        if (m_statusCallback) m_statusCallback("SSL/TLS disabled. Skipping SSL context creation.");
        LOG_INFO(DEBUG_LOG_LEVEL) << "SSL/TLS disabled. Skipping SSL context creation.";
    }
    else // 1 = SSL/TLS
    {
        m_useEncryption = true;
        if (m_statusCallback) m_statusCallback("Encryption enabled.");
        LOG_INFO(DEBUG_LOG_LEVEL) << "Encryption enabled.";

        if (m_settings.securityType == 1)
        {
            try
            {
                if (m_statusCallback) m_statusCallback("SSL/TLS enabled. Creating SSL context.");
                LOG_INFO(DEBUG_LOG_LEVEL) << "SSL/TLS enabled. Creating SSL context.";
                sslContext = smtp::ssl::SSLContextFactory::createClientContext();
            }
            catch (std::exception& e)
            {
                throw std::runtime_error(std::string("Failed to create SSL context: ") + e.what());
            }
        }
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
        if (m_statusCallback) m_statusCallback("Client connected.");

        session->setSMTPHandling([this](boost::asio::const_buffer msg) {
            SMTPHandling(msg);
        });
        session->setConnection(m_useEncryption);

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


        if (m_statusCallback) m_statusCallback("Failed to resolve or connect: " + std::string(e.what()));
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
        if (!ec) connect();
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
    std::string reply(static_cast<const char*>(msg.data()), msg.size());

    LOG_INFO(DEBUG_LOG_LEVEL) << "Received: " << reply;

    if(reply.starts_with("220")) {
        if(!m_SMTPLogic || m_commandQueue.empty()){
            m_lastError = "Received server 220, but no email is queued";
            LOG_INFO(PROD_LOG_LEVEL) << m_lastError;
            if (m_statusCallback) m_statusCallback(m_lastError);
            return;
        }
        std::string cmd_to_send = m_commandQueue.front();
        LOG_INFO(DEBUG_LOG_LEVEL) << "Sending: " << cmd_to_send;
        if (m_statusCallback) m_statusCallback("Sending: " + cmd_to_send);

        session->net_session->send(net::buffer(cmd_to_send));
        m_commandQueue.pop();
        return; 
    }
    if (!m_SMTPLogic) {
        LOG_INFO(DEBUG_LOG_LEVEL) << "Received reply, but no SMTP logic is active. Ignoring.";
        return;
    }

    auto status = m_SMTPLogic->OnReply(reply);

    switch (status)
    {
    case ISXSMTP::SMTPTransactionStatus::SEND_NEXT_COMMAND:
        if (m_commandQueue.empty()) {
            LOG_INFO(PROD_LOG_LEVEL) << "SMTP transaction complete.";
            if (m_statusCallback) m_statusCallback("Transaction complete.");
            m_SMTPLogic.reset(); // Transaction is done.
        } else {
            std::string cmd_to_send = m_commandQueue.front();
            LOG_INFO(DEBUG_LOG_LEVEL) << "Sending: " << cmd_to_send;
            if (m_statusCallback) m_statusCallback("Sending: " + cmd_to_send);

            session->net_session->send(net::buffer(cmd_to_send));
            m_commandQueue.pop();
        }
        break;

    case ISXSMTP::SMTPTransactionStatus::SEND_DATA:
        // The DATA command was accepted (354)
        {
            LOG_INFO(DEBUG_LOG_LEVEL) << "Sending email data...";
            if (m_statusCallback) m_statusCallback("Sending email data...");

            std::string fullBody = "Subject: " + m_emailInfo.subject + "\r\n";
            fullBody += "From: " + m_emailInfo.from + "\r\n";
            fullBody += "To: ";
            for(size_t i = 0; i < m_emailInfo.to.size(); ++i) {
                fullBody += m_emailInfo.to[i] + (i == m_emailInfo.to.size() - 1 ? "" : ", ");
            }
            fullBody += "\r\n\r\n"; // Headers/body separator
            fullBody += m_emailInfo.body;
            fullBody += "\r\n.\r\n"; // End of data

            session->net_session->send(net::buffer(fullBody));
        }
        break;

    case ISXSMTP::SMTPTransactionStatus::ABORTED:
        m_lastError = "SMTP Transaction Aborted: " + reply;
        LOG_WARNING(PROD_LOG_LEVEL) << m_lastError;
        if (m_statusCallback) m_statusCallback("Error: " + m_lastError);

        session->net_session->send(net::buffer(m_SMTPLogic->OnAbort()));
        m_SMTPLogic.reset(); // Transaction is failed.
        break;

    case ISXSMTP::SMTPTransactionStatus::RETRY_LATER:
        m_lastError = "SMTP Server busy (4xx): " + reply;
        LOG_WARNING(PROD_LOG_LEVEL) << m_lastError;
        if (m_statusCallback) m_statusCallback("Server busy: " + m_lastError);

        session->net_session->send(net::buffer(m_SMTPLogic->OnAbort()));
        m_SMTPLogic.reset();
        break;

    case ISXSMTP::SMTPTransactionStatus::REPLY_PARSE_ERROR:
        m_lastError = "Could not parse server reply: " + reply;
        LOG_WARNING(PROD_LOG_LEVEL) << m_lastError;
        if (m_statusCallback) m_statusCallback("Server reply invalid: " + m_lastError);

        session->net_session->send(net::buffer(m_SMTPLogic->OnAbort()));
        m_SMTPLogic.reset();
        break;

    case ISXSMTP::SMTPTransactionStatus::WAIT_FOR_REPLY:
        LOG_INFO(DEBUG_LOG_LEVEL) << "Waiting for more (multi-line reply): " << reply;
        if (m_statusCallback) m_statusCallback("Receiving multi-line reply...");
        break;
    }
};

bool Client::sendMail(EmailMessage e_msg)
{
    LOG_INFO(DEBUG_LOG_LEVEL) << "sendMail called. Queuing email.";
    if (m_statusCallback) m_statusCallback("Preparing to send email...");

    m_emailInfo = e_msg;

    std::string domain = m_settings.server;

    m_SMTPLogic = std::make_unique<ISXSMTP::SMTPClient>(domain, m_emailInfo.from, m_emailInfo.to, true);

    std::queue<std::string> empty;
    std::swap(m_commandQueue, empty);

    auto commands = m_SMTPLogic->GenCommands();
    for (const auto& cmd : commands) {
        m_commandQueue.push(cmd);
    }

    LOG_INFO(PROD_LOG_LEVEL) << "Email queued. " << m_commandQueue.size() << " commands generated.";
    if (m_statusCallback) m_statusCallback("Email queued (" + std::to_string(m_commandQueue.size()) + " commands).");

    if (!session->net_session->isConnected())
    {
        LOG_INFO(DEBUG_LOG_LEVEL) << "Client is not yet connected. Commands are queued.";
        if (m_statusCallback) m_statusCallback("Waiting for connection...");
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
