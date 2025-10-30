#include "SMTPWorker.h"
//#include "ClientClassName.h"

SmtpWorker::SmtpWorker(const Email& email, const SmtpSettings& settings, QObject* parent) :
    QObject(parent), m_Email(email), m_Settings(settings)
{};

void SmtpWorker::process(){
    emit statusUpdated("Worker process started!");

    //ClientClassName ClientName;

    emit statusUpdated("Connecting to the server...");
    /*if(!ClientName.start(m_Settings)){
        emit error("Failed to connect to the server!");
        emit finished();
        return;
    }*/
    emit statusUpdated(m_Settings.username);

    emit statusUpdated("Connection successful! Sending email...");

    /*if(!ClientName.sendEmail(m_Email, m_Settings)){
        emit error("The SMTP library reported an error: " + QString::fromStdString(ClientName.getLastError()));
        ClientName.end();
        emit finished();
        return;
    }*/

    emit finished();
}
