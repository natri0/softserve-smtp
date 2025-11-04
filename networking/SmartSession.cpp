//
// Created by alkir on 11/4/2025.
//

#include "SmartSession.h"

SmartSession::SmartSession(std::shared_ptr<net::ip::tcp::socket> socket) :
    net_session(std::make_shared<Session>(socket)),
    smtp_session(std::make_shared<ISXSMTP::SMTPSession>())
{
}
