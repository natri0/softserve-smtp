#include "SMTPWorker.h"
//#include "ClientClassName.h"

SmtpWorker::SmtpWorker(const Email& email, const SmtpSettings& settings, QObject* parent) :
    QObject(parent), m_Email(email), m_Settings(settings)
{};

void SmtpWorker::process(){
    emit statusUpdated("Worker process started!");

    //ClientClassName ClientName;

    //if(!ClientName.sendEmail(m_Email, m_Settings)){
    //    emit error("The SMTP library reported an error: " + QString::fromStdString(ClientName.getLastError()));
    //}

    emit finished();
}
