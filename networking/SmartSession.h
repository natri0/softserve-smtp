//
// Created by alkir on 11/4/2025.
//

#ifndef SMARTSESSION_H
#define SMARTSESSION_H

#include "Session.h"
#include "../SMTP/include/SMTPSession.h"

class SmartSession
{
public:
    enum Type
    {
        CLIENT,
        SERVER
    };

    explicit SmartSession(std::shared_ptr<net::ip::tcp::socket> socket, Type sessionType);

    void setConnection();
    void setSMTPHandling(std::function<void(net::const_buffer)> cb) { SMTPHandling = std::move(cb); }

    Type type;
    std::shared_ptr<Session> net_session;
    std::shared_ptr<ISXSMTP::SMTPSession> smtp_session;

private:
    std::function<void(net::const_buffer)> SMTPHandling;

    void setKeys(net::const_buffer msg,
                               std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys);
};


#endif //SMARTSESSION_H
