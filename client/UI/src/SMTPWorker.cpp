#include "SMTPWorker.h"
#include "Client.h"        
#include "EmailMessage.h"  
#include <string>
#include <vector>

SmtpWorker::SmtpWorker(const Email& email, const SmtpSettings& settings, QObject* parent) :
  QObject(parent), m_Email(email), m_Settings(settings)
{
};

void SmtpWorker::process() {
  emit statusUpdated("Worker process started!");

  std::string host = m_Settings.server.toStdString();
  unsigned short int port = m_Settings.port;

  EmailMessage msgToSend;
  msgToSend.from = m_Email.from.toStdString();
  msgToSend.subject = m_Email.subject.toStdString();
  msgToSend.body = m_Email.body.toStdString();

  msgToSend.to.reserve(m_Email.to.size());
  for (const QString& recipient : m_Email.to) {
    msgToSend.to.push_back(recipient.toStdString());
  }

  msgToSend.attachmentPaths.reserve(m_Email.attachmentPaths.size());
  for (const QString& path : m_Email.attachmentPaths) {
    msgToSend.attachmentPaths.push_back(path.toStdString());
  }

  emit statusUpdated("Connecting to " + m_Settings.server + "...");
  Client client(host, port);

  // Set authentication if provided

  if (!client.start()) {
    emit error("Failed to initialize client (client.start())");
    emit finished();
    return;
  }

  if (!client.sendMail(msgToSend)) {
    emit error("Failed to queue email " + QString::fromStdString(client.getLastError()));
    emit finished();
    return;
  }

  emit statusUpdated("Client connected. Running network loop.");

  bool networkLoopSuccess = client.run();
  std::string smtpError = client.getLastError();

  if (!networkLoopSuccess) {
    emit error("Client network loop failed (client.run())" + QString::fromStdString(smtpError));
  }
  else if (!smtpError.empty())
  {
    emit error("SMTP Error: " + QString::fromStdString(smtpError));
  }
  else
  {
    emit statusUpdated("Network loop finished. Email sent successfully.");
  }

  client.stop(); 
  emit finished();
}