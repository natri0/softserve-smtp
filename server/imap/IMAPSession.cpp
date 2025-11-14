//
// Created by natri on 13.11.2025.
//

#include "IMAPSession.h"

#include <Logger.h>
#include <Macros.h>
#include <unordered_map>

#include "../Database/Manager.hpp"

#define CRLF "\r\n"
#define HANDLER(name) ([](IMAPSession &s, const std::string_view &args){ IMAPHandlers().name(s, args); })

class IMAPHandlers {
public:
    void handleCapability(IMAPSession &session, const std::string_view &);
    void handleList(IMAPSession &session, const std::string_view &);
    void handleSelect(IMAPSession &session, const std::string_view &);
    void handleLogin(IMAPSession &session, const std::string_view &);
    void handleFetch(IMAPSession &session, const std::string_view &);
};

static const std::unordered_map<std::string, std::function<void(IMAPSession &, const std::string_view &)>> HANDLERS = {
    { "CAPABILITY", HANDLER(handleCapability) },
    { "LIST", HANDLER(handleList) },
    { "SELECT", HANDLER(handleSelect) },
    { "LOGIN", HANDLER(handleLogin) },
    { "FETCH", HANDLER(handleFetch) },
};

IMAPSession::IMAPSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) :
    net_session(std::make_shared<Session>(socket))
{}

void IMAPSession::init() {
    reply_untagged("OK [CAPABILITY IMAP4rev1 LITERAL+ IDLE] IMAP4rev1 Server Ready" CRLF);

    std::shared_ptr<IMAPSession> self = shared_from_this();
    net_session->setOnMessage([self](net::const_buffer msg) {
        LOG_INFO(LogLevel::DEBUG) << "New message with size: " << msg.size();

        // Append to buffer
        self->input_buffer.append(
            static_cast<const char*>(msg.data()),
            msg.size()
        );

        // Process all complete lines in the buffer
        while (true) {
            // Check if we have a complete line (CRLF)
            size_t crlf_pos = self->input_buffer.find("\r\n");
            if (crlf_pos == std::string::npos) {
                // Not a complete line yet, wait for more data
                break;
            }

            // Extract the complete line (without CRLF)
            std::string input_str(std::string_view(self->input_buffer.data(), crlf_pos));
            std::string_view input(input_str);

            // Remove the processed line from buffer
            self->input_buffer.erase(0, crlf_pos + 2);

            auto tag_end = input.find(' ');
            if (tag_end == std::string_view::npos) {
                self->reply_untagged("BAD Missing command");
                return;
            }

            std::string tag(input.substr(0, tag_end));
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
        }
    });

    net_session->run();
}

void IMAPSession::clearUserData() {
    cur_localpart = "";
    cur_mailbox = "";
    cur_tag = "";
    mails.clear();
}

void IMAPSession::reply_tagged(const std::string &reply) {
    net_session->send(net::buffer(cur_tag + " " + std::string(reply) + CRLF));
}

void IMAPSession::reply_untagged(const std::string &reply) {
    net_session->send(net::buffer("* " + std::string(reply) + CRLF));
}

void IMAPHandlers::handleCapability(IMAPSession &session, const std::string_view &) {
    session.reply_untagged("CAPABILITY IMAP4rev1 LITERAL+ IDLE NAMESPACE");
    session.reply_tagged("OK CAPABILITY completed");
}

void IMAPHandlers::handleList(IMAPSession &session, const std::string_view &) {
    if (session.cur_localpart.empty()) {
        session.reply_tagged("NO LIST failed: not logged in");
        return;
    }

    // TODO: implement real LIST handling with args parsing
    session.reply_untagged(R"(LIST (\HasNoChildren) "/" INBOX)");
    session.reply_tagged("OK LIST completed");
}

void IMAPHandlers::handleSelect(IMAPSession &session, const std::string_view &args) {
    if (session.cur_localpart.empty()) {
        session.reply_tagged("NO SELECT failed: not logged in");
        return;
    }

    // TODO: fetch real mailbox from db; possibly also multiple mailboxes?
    if (args.empty() || args != "INBOX") {
        session.reply_tagged("NO SELECT failed: unknown mailbox");
        return;
    }

    session.cur_mailbox = "INBOX";
    session.reply_untagged("FLAGS ()");

    if (auto res = DatabaseManager::get().fetchMailsForUser(session.cur_localpart, DatabaseManager::ALL_MAILS); res.isErr()) {
        session.reply_tagged("NO SELECT failed: error fetching mails");
        LOG_ERROR(LogLevel::DEBUG) << "Error fetching mails for user '" << session.cur_localpart << "': " << res.unwrapErr();
        return;
    } else {
        auto mails = res.unwrapUnchecked();
        session.mails.reserve(mails.size());
        for (auto &mail : mails) {
            session.mails.push_back(std::make_unique<DbMail>(std::move(mail)));
        }
        // std::ranges::transform(mails, session.mails.begin(), [](auto mail){ return std::move(std::make_unique<DbMail>(std::move(mail))); });
    }

    // TODO: return real values for EXISTS & RECENT
    session.reply_untagged(std::format("{} EXISTS", session.mails.size()));
    session.reply_untagged(std::format("{} RECENT", session.mails.size()));

    // TODO: return last unseen UID

    session.reply_tagged("OK [READ-WRITE] SELECT completed");
}

void IMAPHandlers::handleLogin(IMAPSession &session, const std::string_view &args) {
    // TODO: proper auth for imap LOGIN

    if (args.empty()) {
        session.reply_tagged("BAD LOGIN failed: missing arguments");
        return;
    }

    if (auto notExists = DatabaseManager::get().checkMailboxAvailability(args); notExists.isErr() || notExists.unwrap()) {
        session.reply_tagged("NO LOGIN failed: mailbox does not exist");

        if (notExists.isErr()) {
            LOG_ERROR(LogLevel::DEBUG) << "Error fetching mailbox availability: " << notExists.unwrapErr();
        }

        return;
    }

    session.cur_localpart = args;
    session.reply_tagged("OK LOGIN completed");
}
