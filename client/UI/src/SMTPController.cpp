#include "SMTPController.h"
#include "SMTPWorker.h"
#include <QThread>
#include <memory>

SmtpController::SmtpController(QObject *parent)
    : QObject{parent}
{
}

void SmtpController::sendEmail(const Email& email, const SmtpSettings& settings){
    QThread* thread = new QThread();
    SmtpWorker* worker = new SmtpWorker(email, settings);

    worker->moveToThread(thread);

    auto success = std::make_shared<bool>(true);

    connect(worker, &SmtpWorker::statusUpdated, this, &SmtpController::statusUpdated);

    connect(worker, &SmtpWorker::error, this, [this, success](const QString& err){
        *success = false;
        emit sendFailed(err);
    });

    connect(worker, &SmtpWorker::finished, this, [this, success](){
        if(*success){
            emit sendSuccess();
        }
    });

    connect(worker, &SmtpWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &SmtpWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(thread, &QThread::started, worker, &SmtpWorker::process);

    thread->start();
}
