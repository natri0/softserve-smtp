#ifndef SMTPCONTROLLER_H
#define SMTPCONTROLLER_H

#include <QObject>
#include "Structures.h"

class SmtpController : public QObject
{
    Q_OBJECT
public:
    SmtpController(QObject* parent = nullptr);

public slots:
    void sendEmail(const Email& email, const SmtpSettings& settings);

signals:
    void statusUpdated(const QString& status);
    void sendSuccess();
    void sendFailed(const QString& error);
};

#endif // SMTPCONTROLLER_H
