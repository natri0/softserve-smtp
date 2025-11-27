#ifndef SMTPWORKER_H
#define SMTPWORKER_H

#include <QObject>
#include <QFileInfo>
#include <QMimeDatabase>
#include "Structures.h"
#include "Client.h"

class SmtpWorker : public QObject{
    Q_OBJECT
public:
    SmtpWorker(const SmtpSettings& settings, QObject* parent = nullptr);

public slots:
    void processEmail(const Email& email);

    void updateSettings(const SmtpSettings& settings);

signals:
    void statusUpdated(const QString& message);
    void finished();
    void error(const QString& message);

private:
    SmtpSettings m_Settings;
    std::unique_ptr<Client> m_Client;
};

#endif // SMTPWORKER_H
