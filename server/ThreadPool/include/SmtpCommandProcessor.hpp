#pragma once
#include <iostream>
#include <regex>
#include <memory>
#include "SMTPSession.hpp"

class SmtpCommandProcessor {
public:
    void handle(std::shared_ptr<SmtpSession> session, const std::string& cmd) {
        using Status = SmtpSession::SessionStatus;
        auto socket = session->getSocket();
        
        if (!socket || !socket->is_open()) {
            session->close();
            return;
        }

        std::string response;

        switch (session->getStatus()) {
        case Status::INITIAL:
            if (starts_with_icase(cmd, "HELO")) {
                response = "250 Hello, pleased to meet you\r\n";
                session->setStatus(Status::HELO);
            } else {
                response = "503 Send HELO first\r\n";
            }
            break;

        case Status::HELO:
            if (starts_with_icase(cmd, "MAIL FROM:")) {
                session->setSender(extractEmail(cmd));
                response = "250 OK\r\n";
                session->setStatus(Status::MAIL_FROM);
            } else {
                response = "503 Expected MAIL FROM\r\n";
            }
            break;

        case Status::MAIL_FROM:
            if (starts_with_icase(cmd, "RCPT TO:")) {
                session->setRecipient(extractEmail(cmd));
                response = "250 OK\r\n";
                session->setStatus(Status::RCPT);
            } else {
                response = "503 Expected RCPT TO\r\n";
            }
            break;

        case Status::RCPT:
            if (starts_with_icase(cmd, "DATA")) {
                response = "354 End data with <CR><LF>.<CR><LF>\r\n";
                session->setStatus(Status::DATA);
                session->clearMessage();
            } else {
                response = "503 Expected DATA\r\n";
            }
            break;

        case Status::DATA:
            if (cmd == ".") {
                response = "250 Message accepted for delivery\r\n";
                session->setStatus(Status::HELO);
                
                std::cout << "\n📧 Message from: " << session->getSender()
                          << " to: " << session->getRecipient()
                          << "\n" << session->getMessage() << std::endl;
            } else {
                session->appendMessageLine(cmd);
                return; 
            }
            break;

        default:
            if (starts_with_icase(cmd, "QUIT")) {
                response = "221 Bye\r\n";
                session->setStatus(Status::QUIT);
            } else {
                response = "500 Unrecognized command\r\n";
            }
            break;
        }

        sendResponse(session, socket, std::move(response));
    }

private:
    static void sendResponse(std::shared_ptr<SmtpSession> session,
                            std::shared_ptr<asio::ip::tcp::socket> socket,
                            std::string response) {
    
        auto exec = socket->get_executor();
        
        asio::post(asio::bind_executor(exec, 
            [session, socket, resp = std::make_shared<std::string>(std::move(response))]() {
                asio::async_write(*socket, asio::buffer(*resp),
                    [session, socket, resp](const asio::error_code& ec, std::size_t) {
                        if (ec) {
                            std::cerr << "Write error: " << ec.message() << std::endl;
                            session->close();
                        }
                        
                    });
            }));
    }

    static bool starts_with_icase(const std::string& s, const std::string& prefix) {
        if (s.size() < prefix.size()) return false;
        for (size_t i = 0; i < prefix.size(); ++i)
            if (tolower(s[i]) != tolower(prefix[i])) return false;
        return true;
    }

    static std::string extractEmail(const std::string& cmd) {
        std::smatch m;
        std::regex re("<(.*?)>");
        if (std::regex_search(cmd, m, re))
            return m[1].str();
        return "";
    }
};