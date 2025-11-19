//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

#include "../networking/SSL/KeyExchanger.h"
#include "../logger/Include/Logger.h"
#include <sstream>

constexpr uint8_t RECONNECT_DELAY_TIME = 2;

static std::string createEmailBody(const EmailMessage& msg) {
  std::stringstream ss;
  ss << "From: " << msg.from << "\r\n";
  ss << "To: ";
  for (size_t i = 0; i < msg.to.size(); ++i) {
    ss << msg.to[i] << (i == msg.to.size() - 1 ? "" : ", ");
  }
  ss << "\r\n";
  ss << "Subject: " << msg.subject << "\r\n";
  ss << "\r\n"; // Empty line separates headers from body
  ss << msg.body << "\r\n";
  ss << ".\r\n"; // End of data indicator
  return ss.str();
}

Client::Client(const ClientSettings& settings, StatusCallback statusCallback) :
  server_endpoint(net::ip::make_address(settings.server), settings.port),
  session(std::make_shared<SmartSession>(std::make_shared<net::ip::tcp::socket>(io), SmartSession::Type::CLIENT)),
  timer(io),
  sslContext(smtp::ssl::SSLContextFactory::createClientContext()),
  m_statusCallback(statusCallback),
  m_SMTPLogic(std::make_unique<ISXSMTP::SMTPClient>())
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
  if (m_statusCallback) m_statusCallback("Waiting " + std::to_string(static_cast<int>(RECONNECT_DELAY_TIME)) + "s before reconnecting...");
  LOG_INFO(DEBUG_LOG_LEVEL) << "Waiting " << (int)RECONNECT_DELAY_TIME << "s before reconnecting...";

  timer.cancel();
  timer.expires_after(std::chrono::seconds(RECONNECT_DELAY_TIME));
  timer.async_wait([this](boost::system::error_code ec)
    {
      if (!ec) {
        if (m_statusCallback) m_statusCallback("Attempting to connect to " + m_settings.server);
        LOG_INFO(PROD_LOG_LEVEL) << "Attempting to connect to " << m_settings.server;
        connect();

        if (!session->net()->isConnected()) {
          if (m_statusCallback) m_statusCallback("Connection failed. Retrying...");
          LOG_WARNING(DEBUG_LOG_LEVEL) << "Connection failed. Retrying...";
          reconnect();
        }
        else {
          if (!isRunning) run();
        }
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
  std::string reply(static_cast<const char*>(msg.data()), msg.size());
  LOG_INFO(DEBUG_LOG_LEVEL) << "Received message: " << reply;

  auto status = m_SMTPLogic->OnReply(reply);

  switch (status)
  {
  case ISXSMTP::SMTPTransactionStatus::SEND_NEXT_COMMAND:
    if (!m_commandQueue.empty()) {
      std::string cmd = m_commandQueue.front();
      m_commandQueue.pop();
      LOG_INFO(DEBUG_LOG_LEVEL) << "Sending command: " << cmd;
      session->net()->send(net::buffer(cmd));
    }
    else {
      LOG_INFO(PROD_LOG_LEVEL) << "Transaction finished.";
      if (m_statusCallback) m_statusCallback("Ready / Email Sent");
    }
    break;

  case ISXSMTP::SMTPTransactionStatus::SEND_DATA:
  {
    LOG_INFO(DEBUG_LOG_LEVEL) << "Sending email body...";
    if (m_statusCallback) m_statusCallback("Sending email content...");

    std::string dataPayload = createEmailBody(m_emailInfo);
    session->net()->send(net::buffer(dataPayload));
  }
  break;

  case ISXSMTP::SMTPTransactionStatus::WAIT_FOR_REPLY:
    break;

  case ISXSMTP::SMTPTransactionStatus::ABORTED:
  case ISXSMTP::SMTPTransactionStatus::RETRY_LATER:
  case ISXSMTP::SMTPTransactionStatus::REPLY_PARSE_ERROR:
    LOG_ERROR(PROD_LOG_LEVEL) << "SMTP Error: " << reply;
    if (m_statusCallback) m_statusCallback("SMTP Error: " + reply);

    std::queue<std::string> empty;
    std::swap(m_commandQueue, empty);
    break;
  }
};

bool Client::sendMail(EmailMessage e_msg)
{
  m_emailInfo = e_msg;

  m_SMTPLogic->SetDomain("localhost");
  m_SMTPLogic->SetFrom(e_msg.from);
  m_SMTPLogic->SetTo(e_msg.to);
  m_SMTPLogic->SetQuitOnFinish(false);

  std::vector<std::string> cmds = m_SMTPLogic->GenCommands();

  // Reset queue
  std::queue<std::string> empty;
  std::swap(m_commandQueue, empty);

  for (const auto& cmd : cmds) {
    m_commandQueue.push(cmd);
  }

  if (!session->net()->isConnected())
  {
    LOG_INFO(PROD_LOG_LEVEL) << "Client is not connected. Connecting...";
    connect();
    return true;
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

  if (mustReconnect) {
    reconnect();
  }
  return mustReconnect;
}

bool Client::stop()
{
  if (!isRunning) return false;
  isRunning = false;

  session->net()->disconnect();

  io.stop();

  return true;
}
