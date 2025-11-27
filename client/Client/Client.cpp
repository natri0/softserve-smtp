//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

#include "../networking/SSL/KeyExchanger.h"
#include "../logger/Include/Logger.h"
#include <sstream>
#include <EmailBuilder.h>

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

static std::string createEmailBody(const EmailMessage& msg) {
    std::stringstream ss;
    ss << "From: " << msg.getFrom() << "\r\n";
    ss << "To: ";
    for (size_t i = 0; i < msg.getTo().size(); ++i) {
        ss << msg.getTo()[i] << (i == msg.getTo().size() - 1 ? "" : ", ");
    }
    ss << "\r\n";
    ss << "Subject: " << msg.getSubject() << "\r\n";
    ss << "\r\n"; // Empty line separates headers from body
    ss << msg.getBody() << "\r\n";
    ss << ".\r\n"; // End of data indicator
    return ss.str();
}

Client::Client(const ClientSettings& settings, StatusCallback statusCallback) :
    server_endpoint(net::ip::make_address(settings.server), settings.port),
    session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
    timer(io),
    sslContext(smtp::ssl::SSLContextFactory::createClientContext()),
    m_statusCallback(statusCallback),
    m_SMTPLogic(std::make_unique<ISXSMTP::SMTPClient>()),
    m_settings(settings)
{
};

Client::~Client()
{
    stop();
}

bool Client::start()
{
    changeLogLevel("DEBUG");

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

                                       m_responseBuffer.clear();
                                       
                                       session->setSMTPHandling([this](boost::asio::const_buffer msg) { SMTPHandling(msg); });
                                       std::cout <<"Secure Type: " << m_settings.securityType << std::endl;
                                       if (session && session->net()) {
                                           session->net()->clearCrypto();
                                       }
                                       if (m_settings.securityType == 1) {
                                           auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
                                               smtp::ssl::KeyExchange::generateKeyPair()
                                           );
                                           session->setConnection(true);
                                       }else{
                                           session->setConnection(false);
                                       }

                                       session->net()->run();

                                       if (m_onConnectedCallback) {
                                           m_onConnectedCallback();
                                           m_onConnectedCallback = nullptr;
                                       }
                                   });
}

void Client::connect()
{
    if (session->net()->isConnected()) return;
  /*  std::cout << m_settings.server << ": test" << std::endl;
    server_endpoint = boost::asio::ip::tcp::endpoint(
        net::ip::make_address(m_settings.server),
        m_settings.port
    );
    if (session->net()->isConnected()) {
        session->net()->disconnect();
    }*/
    
   
    session->net()->connect(server_endpoint);
}

void Client::reconnect()
{
    if (m_statusCallback) m_statusCallback("Waiting " + std::to_string(static_cast<int>(RECONNECT_DELAY_TIME)) + "s before reconnecting...");
    LOG_INFO(DEBUG_LOG_LEVEL) << "Waiting " << (int)RECONNECT_DELAY_TIME << "s before reconnecting...";

    if (session && session->net()) {
        session->net()->clearCrypto();
    }

    if (io.stopped()) {
        LOG_INFO(DEBUG_LOG_LEVEL) << "Restarting io_context";
        io.restart();
    }

    timer.cancel();
    timer.expires_after(std::chrono::seconds(RECONNECT_DELAY_TIME));
    timer.async_wait([this](boost::system::error_code ec)
                     {
                         if (!ec){
                             if (m_statusCallback) m_statusCallback("Attempting to connect to " + m_settings.server);
                             LOG_INFO(DEBUG_LOG_LEVEL) << "Attempting to connect to " << m_settings.server;

                             
                             /*if (m_settings.securityType == 1) {
                                 session->setConnection(true);
                             }
                             else {
                                 session->setConnection(false);
                             }*/
                             
                             connect();

                             if (!session->net()->isConnected()) {
                                 if (m_statusCallback) m_statusCallback("Connection failed. Retrying...");
                                 LOG_WARNING(DEBUG_LOG_LEVEL) << "Connection failed. Retrying...";
                                 reconnect();
                             }
                             else {
                                 if (!isRunning) run();
                             }
                             if (m_statusCallback) m_statusCallback("Attempting sucsses");
                             LOG_INFO(PROD_LOG_LEVEL) << "Attempting sucsses";
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
    std::string chunk(static_cast<const char*>(msg.data()), msg.size());
    m_responseBuffer += chunk;

    while (true)
    {
        size_t pos = m_responseBuffer.find('\n');
        if (pos == std::string::npos) {
            break;
        }

        std::string line = m_responseBuffer.substr(0, pos + 1);
        m_responseBuffer.erase(0, pos + 1);

        std::string cleanLine = line;
        while (!cleanLine.empty() && (cleanLine.back() == '\r' || cleanLine.back() == '\n')) {
            cleanLine.pop_back();
        }
        LOG_INFO(DEBUG_LOG_LEVEL) << "S: " << cleanLine;

        auto status = m_SMTPLogic->OnReply(line);

        switch (status)
        {
        case ISXSMTP::SMTPTransactionStatus::SEND_NEXT_COMMAND:
            if (!m_commandQueue.empty()) {
                std::string cmd = m_commandQueue.front();
                m_commandQueue.pop();
                LOG_INFO(DEBUG_LOG_LEVEL) << "C: " << cmd;
                session->net()->send(net::buffer(cmd));
            } else {
                LOG_INFO(PROD_LOG_LEVEL) << "Transaction finished.";
                if (m_statusCallback) m_statusCallback("Ready / Email Sent");
            }
            break;

        case ISXSMTP::SMTPTransactionStatus::SEND_DATA:
        {
            
            if (m_statusCallback) m_statusCallback("Sending email content...");
            EmailBuilder builder;
            builder.from(m_emailInfo.getFrom());
            for (const auto& s : m_emailInfo.getTo()) {
                builder.to(s);
            }
            builder.subject(m_emailInfo.getSubject())
                   .body(m_emailInfo.getBody());
            for (const auto& info_attach : m_emailInfo.getAttachments()) {
                builder.attachment(info_attach.getFileName(), info_attach.getMimeType(), info_attach.getContentDisposition(), info_attach.getData());
            }
            auto mimeBinary = builder.buildBinary();
            std::string dataPayload = createEmailBody(m_emailInfo);
            static const char endMarker[] = "\r\n.\r\n";

            std::vector<uint8_t> fullData;
            fullData.reserve(mimeBinary.size() + sizeof(endMarker) - 1);

            fullData.insert(fullData.end(), mimeBinary.begin(), mimeBinary.end());
            fullData.insert(fullData.end(), endMarker, endMarker + sizeof(endMarker) - 1);

            
            LOG_INFO(DEBUG_LOG_LEVEL) << "Sending email body...";
            session->net()->send(boost::asio::buffer(fullData));
            LOG_INFO(DEBUG_LOG_LEVEL) << "Sending succes";
        }
        break;

        case ISXSMTP::SMTPTransactionStatus::WAIT_FOR_REPLY:
            break;

        case ISXSMTP::SMTPTransactionStatus::ABORTED:
        case ISXSMTP::SMTPTransactionStatus::RETRY_LATER:
        case ISXSMTP::SMTPTransactionStatus::REPLY_PARSE_ERROR:
            LOG_ERROR(PROD_LOG_LEVEL) << "SMTP Error: " << cleanLine;
            if (m_statusCallback) m_statusCallback("SMTP Error: " + cleanLine);

            std::queue<std::string> empty;
            std::swap(m_commandQueue, empty);
            break;
        }
    }
};

bool Client::sendMail(EmailMessage e_msg)
{   

    auto prepareMail = [this, e_msg]() {
        m_emailInfo = e_msg;
        m_SMTPLogic = std::make_unique<ISXSMTP::SMTPClient>();
        m_SMTPLogic->SetDomain("localhost");
        m_SMTPLogic->SetFrom(e_msg.getFrom());
        m_SMTPLogic->SetTo(e_msg.getTo());
        m_SMTPLogic->SetQuitOnFinish(false);

        std::vector<std::string> cmds = m_SMTPLogic->GenCommands();
        std::queue<std::string> empty;
        std::swap(m_commandQueue, empty);
        for (const auto& cmd : cmds) {
            m_commandQueue.push(cmd);
        }
    };

    //if (!session->net()->isConnected())
    //{
    //    LOG_INFO(PROD_LOG_LEVEL) << "Client is not connected. Connecting...";
    //    connect();
    //    m_onConnectedCallback = prepareMail;
    //    //return true;
    //}
    if (!session->net()->isConnected())
    {
        LOG_INFO(PROD_LOG_LEVEL) << "Not connected. Will send after connection.";
        m_onConnectedCallback = prepareMail;  
        connect();
    }
    else
    {
        prepareMail();  
    }

    if (!m_commandQueue.empty())
    {
        std::string cmd = m_commandQueue.front();
        m_commandQueue.pop();
        LOG_INFO(DEBUG_LOG_LEVEL) << "Sending command: " << cmd;
        session->net()->send(net::buffer(cmd));
    }

    LOG_INFO(PROD_LOG_LEVEL) << "Starting email transmission...";
    return true;
}

void Client::changeLogLevel(const std::string& level) const
{
    if (m_statusCallback) m_statusCallback("New LOG level: " + level);
    if (level == "NONE")
        Logger::getInstance().setLevel(LogLevel::NONE);
    if (level == "PROD")
        Logger::getInstance().setLevel(PROD_LOG_LEVEL);
    if (level == "DEBUG")
        Logger::getInstance().setLevel(DEBUG_LOG_LEVEL);
    if (level == "TRACE")
        Logger::getInstance().setLevel(TRACE_LOG_LEVEL);
}

bool Client::setSettings(const ClientSettings& newSettings) {
    bool mustReconnect =
        (newSettings.server != m_settings.server) ||
        (newSettings.port != m_settings.port) ||
        (newSettings.username != m_settings.username) ||
        (newSettings.password != m_settings.password) ||
        (newSettings.securityType != m_settings.securityType);

    m_settings = newSettings;

    changeLogLevel(newSettings.logLevel);

    if (mustReconnect) {
        reconnect();
    }
    return mustReconnect;
}

bool Client::stop()
{
    if (!isRunning) return false;
    isRunning = false;

    if (session && session->net()) {
        session->net()->disconnect();
    }
    //
    timer.cancel();
    m_responseBuffer.clear();
    std::queue<std::string> empty;
    std::swap(m_commandQueue, empty);
    m_onConnectedCallback = nullptr;
    //
    io.stop();

    if (io_thread.joinable()) {
        io_thread.join();
    }

    return true;
}

