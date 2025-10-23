#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QStringList>

struct SmtpSettings {
    QString server;
    quint16 port;
    QString username;
    QString password;
    quint16 securityType = 1; // 0: None, 1: SSL/TLS
};

struct Email {
    QString from;
    QStringList to;
    //QStringList cc;
    QString subject;
    QString body;
    QStringList attachmentPaths;
};

#endif // STRUCTURES_H
