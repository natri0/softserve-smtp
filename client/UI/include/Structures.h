#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QStringList>

struct SmtpSettings {
    QString server;
    quint16 port;
    QString username;
    QString password;
    QString logLevel;
    quint16 securityType = 1; // 0: None, 1: SSL/TLS

    bool operator!=(const SmtpSettings& other) const {
        return  server != other.server ||
                port != other.port ||
                username != other.username ||
                password != other.password ||
                logLevel != other.logLevel ||
               securityType != other.securityType;
    };
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
