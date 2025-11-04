//
// Created by alkir on 11/4/2025.
//

#ifndef SMARTSESSION_H
#define SMARTSESSION_H

#include "Session.h"
#include "../SMTP/include/SMTPSession.h"

class SmartSession {
    public:
    explicit SmartSession(std::shared_ptr<net::ip::tcp::socket> socket);

    std::shared_ptr<Session> net_session;
    std::shared_ptr<ISXSMTP::SMTPSession> smtp_session;
};



#endif //SMARTSESSION_H
