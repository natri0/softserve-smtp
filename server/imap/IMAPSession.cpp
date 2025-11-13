//
// Created by natri on 13.11.2025.
//

#include "IMAPSession.h"

#include <Logger.h>
#include <Macros.h>
#include <unordered_map>

#define CRLF "\r\n"

static void handleCapability(IMAPSession &session, const std::string_view &);

static constexpr std::unordered_map<std::string, std::function<void(IMAPSession &, const std::string_view &)>> HANDLERS = {
    { "CAPABILITY", handleCapability }
};

IMAPSession::IMAPSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) :
    net_session(std::make_shared<Session>(socket))
{}

void IMAPSession::init() {
    reply_untagged("OK [CAPABILITY IMAP4rev1 LITERAL+ IDLE] IMAP4rev1 Server Ready" CRLF);

    std::shared_ptr<IMAPSession> self = shared_from_this();
    net_session->setOnMessage([self](net::const_buffer msg) {
        std::string_view input(static_cast<const char*>(msg.data()), msg.size());

        while (!input.empty() && std::isspace(static_cast<unsigned char>(input.back())))
            input.remove_suffix(1);

        auto tag_end = input.find(' ');
        if (tag_end == std::string_view::npos) {
            self->reply_untagged("BAD Missing command");
            return;
        }

        std::string_view tag = input.substr(0, tag_end);
        if (tag.empty()) {
            self->reply_untagged("BAD Missing tag");
            return;
        }

        input.remove_prefix(tag_end + 1);
        self->cur_tag.assign(tag);

        while (!input.empty() && std::isspace(static_cast<unsigned char>(input.front())))
            input.remove_prefix(1);

        if (input.empty()) {
            self->reply_untagged("BAD Missing command");
            return;
        }

        auto cmd_end = input.find(' ');
        std::string_view command = (cmd_end == std::string_view::npos)
                                     ? input
                                     : input.substr(0, cmd_end);
        std::string_view args = (cmd_end == std::string_view::npos)
                                  ? std::string_view{}
                                  : input.substr(cmd_end + 1);

        std::string cmd_upper(command);
        for (auto& c : cmd_upper) c = std::toupper(static_cast<unsigned char>(c));

        if (auto it = HANDLERS.find(cmd_upper); it != HANDLERS.end()) {
            it->second(*self, args);
        } else {
            LOG_ERROR(LogLevel::DEBUG) << "Unknown command: " << command;
            self->reply_tagged("BAD Command unrecognized");
        }
    });
}

void IMAPSession::reply_tagged(const std::string &reply) {
    net_session->send(net::buffer(cur_tag + " " + std::string(reply) + CRLF));
}

void IMAPSession::reply_untagged(const std::string &reply) {
    net_session->send(net::buffer("* " + std::string(reply) + CRLF));
}

void handleCapability(IMAPSession &session, const std::string_view &) {
    session.reply_untagged("CAPABILITY IMAP4rev1 LITERAL+ IDLE NAMESPACE");
    session.reply_tagged("OK CAPABILITY completed");
}
