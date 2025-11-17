#include "SMTPController.h"
#include "SMTPWorker.h"
#include <QThread>

SmtpController::SmtpController(QObject *parent)
    : QObject{parent},
    m_workerThread(nullptr),
    m_worker(nullptr)
{
}

void SmtpController::init(const SmtpSettings& initialSettings)
{
    m_lastSettings = initialSettings;

    m_workerThread = new QThread(this);
    m_worker = new SmtpWorker(initialSettings);
    m_worker->moveToThread(m_workerThread);

    connect(m_worker, &SmtpWorker::statusUpdated, this, &SmtpController::statusUpdated);
    connect(m_worker, &SmtpWorker::error, this, &SmtpController::sendFailed);
    connect(m_worker, &SmtpWorker::finished, this, &SmtpController::sendSuccess);

    connect(this, &SmtpController::workerProcessEmail, m_worker, &SmtpWorker::processEmail);
    connect(this, &SmtpController::workerUpdateSettings, m_worker, &SmtpWorker::updateSettings);

    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_workerThread->start();
}

void SmtpController::sendEmail(const Email& email, const SmtpSettings& settings){
    if (m_lastSettings != settings)
    {
        m_lastSettings = settings;
        emit workerUpdateSettings(settings);
    }

    emit workerProcessEmail(email);
}
