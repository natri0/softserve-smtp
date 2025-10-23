#ifndef SMTPWORKER_H
#define SMTPWORKER_H

#include <QObject>
#include "Structures.h"

class SmtpWorker : public QObject{
    Q_OBJECT
public:
    SmtpWorker(const Email& email, const SmtpSettings& settings, QObject* parent = nullptr);

public slots:
    void process();

signals:
    void statusUpdated(const QString& message);
    void finished();
    void error(const QString& message);

private:
    Email m_Email;
    SmtpSettings m_Settings;
};

#endif // SMTPWORKER_H
