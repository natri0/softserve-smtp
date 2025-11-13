#pragma once

#include "Connection/Session.h"

class IMAPSession : public std::enable_shared_from_this<IMAPSession> {
public:
    explicit IMAPSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void init();

    std::shared_ptr<Session> net() { return net_session; }

    std::string &mailbox() { return cur_mailbox; }

    void reply_tagged(const std::string &reply);
    void reply_untagged(const std::string &reply);
private:
    std::shared_ptr<Session> net_session;

    std::string cur_mailbox;
    std::string cur_tag;
    std::string input_buffer;  // accumulate input across reads
};
