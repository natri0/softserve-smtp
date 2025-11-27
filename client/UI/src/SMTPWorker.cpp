#include "SMTPWorker.h"
#include "ClientSettings.h"
#include "EmailMessage.h"
#include <string>
#include <vector>



SmtpWorker::SmtpWorker(const SmtpSettings& settings, QObject* parent) :
  QObject(parent), m_Settings(settings)
{
    ClientSettings clientSettings;
    clientSettings.logLevel = m_Settings.logLevel.toStdString();
    clientSettings.password = m_Settings.password.toStdString();
    clientSettings.server = m_Settings.server.toStdString();
    clientSettings.username = m_Settings.username.toStdString();
    clientSettings.port = m_Settings.port;
    clientSettings.securityType = m_Settings.securityType;

    auto onStatus = [this](const std::string& msg) {
        emit statusUpdated(QString::fromStdString(msg));
    };

    m_Client = std::make_unique<Client>(clientSettings, onStatus);

    m_Client->start();

    emit statusUpdated("Worker started and ready.");
};

void SmtpWorker::processEmail(const Email& email) {
    try{
        emit statusUpdated("Worker process started!");

        EmailMessage msgToSend;
        msgToSend.setFrom(email.from.toStdString());
        msgToSend.setSubject(email.subject.toStdString());
        msgToSend.setBody(email.body.toStdString());

       
        for (const QString& recipient : email.to) {
            msgToSend.setTo(recipient.toStdString());
        }

        //msgToSend.attachmentPaths.reserve(email.attachmentPaths.size());
        for (const QString& path : email.attachmentPaths) {
            QFileInfo info(path);

            QString fileName = info.fileName();             // test.png
            QString extension = info.suffix();              // png
            QString baseName = info.completeBaseName();     // test

            emit statusUpdated("Worker process started!");

            QFile file(path);
            if (!file.open(QIODevice::ReadOnly)) {
                emit statusUpdated("Cannot open attachment: " + path);
                continue;
            }

            QByteArray data = file.readAll();
            QString mime = QMimeDatabase().mimeTypeForFile(info).name();
            msgToSend.addAttachment({
                fileName.toStdString(),
                mime.toStdString(),
                "attachment",
                std::vector<uint8_t>(data.begin(), data.end())
                });
        }

        emit statusUpdated("Connecting to " + m_Settings.server + "...");

        if (!m_Client->sendMail(msgToSend)) {
            emit error("Failed to queue email " + QString::fromStdString(m_Client->getLastError()));
            //emit finished();
            return;
        }
        emit statusUpdated("Running network loop.");

        std::string smtpError = m_Client->getLastError();

    }catch (std::exception &e)
    {
        emit error(QString("An unhandled exception occurred: %1").arg(e.what()));
    }
    catch (...)
    {
        emit error("An unknown, non-standard exception occurred.");
    }
    emit finished();
}

void SmtpWorker::updateSettings(const SmtpSettings& settings)
{
    m_Settings = settings;

    ClientSettings clientSettings;
    clientSettings.logLevel = m_Settings.logLevel.toStdString();
    clientSettings.password = m_Settings.password.toStdString();
    clientSettings.server = m_Settings.server.toStdString();
    clientSettings.username = m_Settings.username.toStdString();
    clientSettings.port = m_Settings.port;
    clientSettings.securityType = m_Settings.securityType;

    emit statusUpdated("Reconnecting to new server...");

   
    if (m_Client) {
        emit statusUpdated("Stopping old client...");
        m_Client->stop();        
        m_Client.reset();        
    }

    
    auto onStatus = [this](const std::string& msg) {
        emit statusUpdated(QString::fromStdString(msg));
        };

    m_Client = std::make_unique<Client>(clientSettings, onStatus);

    
    m_Client->start();

    emit statusUpdated("Client reconnected with updated settings!");
}
