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

void IMAPHandlers::handleFetch(IMAPSession &session, const std::string_view &args) {
    using FetchAttrHandler = std::function<std::string(IMAPSession &, DbMail &)>;

    static const std::unordered_map<std::string_view, FetchAttrHandler> FETCH_ATTR_HANDLERS = {
        { "FLAGS", [](IMAPSession &, DbMail &mail) {
            return "()";
        } },
        { "BODY[]", [](IMAPSession &, DbMail &mail) {
            return std::format("{{{}}}\r\n{}", mail.content.size(), mail.content);
        } },
        { "BODY[HEADER]", [](IMAPSession &session, DbMail &mail) {
            return std::format("From: {}\r\nTo: {}\r\n", mail.from, session.cur_localpart);
        } },
        { "RFC822.SIZE", [](IMAPSession &, DbMail &mail) {
            return std::to_string(mail.content.size());
        } },
        { "UID", [](IMAPSession &, DbMail &mail) {
            return std::to_string(mail.uid);
        } },
    };

    if (args.empty() || args.find(' ') == std::string_view::npos) {
        session.reply_tagged("BAD FETCH failed: missing arguments");
        return;
    }

    std::string_view msg_set(args.begin(), args.begin() + args.find(' '));
    std::string_view fetch_attrs(args.begin() + args.find(' ') + 1, args.end());

    std::vector<DbMail *> mails_to_fetch;
    std::vector<std::pair<std::string, FetchAttrHandler>> parsed_fetch_attrs;

    for (int i = 0; i < fetch_attrs.length(); i = fetch_attrs.find_first_not_of(' ', i)) {
        if (i == std::string_view::npos) break;

        std::cout << "Parsing fetch_attrs at position " << i << ": " << fetch_attrs.substr(i) << std::endl;

        auto attr_end = fetch_attrs.find_first_of(" \n", i);
        std::string_view attr(fetch_attrs.begin() + i, attr_end == std::string_view::npos ? fetch_attrs.end() : fetch_attrs.begin() + attr_end);
        if (!FETCH_ATTR_HANDLERS.contains(attr)) continue;

        parsed_fetch_attrs.emplace_back(attr, FETCH_ATTR_HANDLERS.at(attr));
        i += attr.length();
    }

    for (int i = 0; i < msg_set.length(); i = msg_set.find_first_not_of(',', i)) {
        if (i == std::string_view::npos) break;

        std::cout << "Parsing msg_set at position " << i << ": " << msg_set.substr(i) << std::endl;

        if (!isdigit(msg_set[i])) {
            session.reply_tagged("BAD FETCH failed: invalid message set");
            return;
        }

        int start = msg_set[i++] - '0';
        while (isdigit(msg_set[i])) start = start * 10 + (msg_set[i++] - '0');

        int end = start + 1;
        if (msg_set[i++] == '*') {
            end = msg_set[i++] - '0';
            while (isdigit(msg_set[i+1])) end = end * 10 + (msg_set[i++] - '0');
        }

        for (int j = start; j < end; j++) {
            if (j == 0 || j > static_cast<int>(session.mails.size())) {
                session.reply_tagged("BAD FETCH failed: message number out of range");
                return;
            }
            mails_to_fetch.push_back(&*session.mails[j - 1]);
        }
    }

    int i = 1;
    for (DbMail *mail : mails_to_fetch) {
        std::string response = std::to_string(i++) + " FETCH (";
        for (size_t j = 0; j < parsed_fetch_attrs.size(); j++) {
            const auto &[attr_name, handler] = parsed_fetch_attrs[j];
            response += attr_name;
            response += " ";
            response += handler(session, *mail);
            if (j < parsed_fetch_attrs.size() - 1) {
                response += " ";
            }
        }

        response += ")";
        session.reply_untagged(response);
    }

    session.reply_tagged("OK FETCH completed");
}

