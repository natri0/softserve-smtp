#ifndef SMTPCONTROLLER_H
#define SMTPCONTROLLER_H

#include <QObject>
#include <QThread>
#include "Structures.h"
#include "SMTPWorker.h"

class SmtpController : public QObject
{
    Q_OBJECT
public:
    SmtpController(QObject* parent = nullptr);

    void init(const SmtpSettings& initialSettings);
public slots:
    void sendEmail(const Email& email, const SmtpSettings& settings);

signals:
    void statusUpdated(const QString& status);
    void sendSuccess();
    void sendFailed(const QString& error);

signals:
    void workerProcessEmail(const Email& email);
    void workerUpdateSettings(const SmtpSettings& settings);

private:
    QThread* m_workerThread;
    SmtpWorker* m_worker;
    SmtpSettings m_lastSettings;
};

#endif // SMTPCONTROLLER_H
