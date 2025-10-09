#include <iostream>
#include <memory>
#include <string>
#include "asio.hpp"

#include "ThreadPool.hpp"
#include "SMTPSession.hpp"

auto pool = std::make_unique<ThreadPool>(12);

void processCommand(std::shared_ptr<SmtpSession> session, const std::string& cmd) {
    auto socket = session->getSocket();
    if (!socket || !socket->is_open()) return;

    std::string response;
    std::string upper_cmd = cmd;
    for (auto &c : upper_cmd) c = std::toupper(c);

    if (upper_cmd.starts_with("EHLO") || upper_cmd.starts_with("HELO")) {
        response = "250 Hello " + cmd.substr(5) + "\r\n";
    } else if (upper_cmd.starts_with("MAIL FROM")) {
        response = "250 OK\r\n";
    } else if (upper_cmd.starts_with("RCPT TO")) {
        response = "250 OK\r\n";
    } else if (upper_cmd == "DATA") {
        response = "354 End data with <CR><LF>.<CR><LF>\r\n";
    } else if (upper_cmd == ".") {
        response = "250 Message accepted\r\n";
    } else if (upper_cmd.starts_with("QUIT")) {
        response = "221 Bye\r\n";
        asio::async_write(*socket, asio::buffer(response),
            [session](std::error_code ec, std::size_t){
                if (!ec) session->close();
            });
        return;
    } else {
        response = "500 Unknown command\r\n";
    }

    asio::async_write(*socket, asio::buffer(response),
        [socket](std::error_code ec, std::size_t) {
            if (ec) std::cout << "❌ Write failed: " << ec.message() << "\n";
        });
}

void onClientCommand(std::shared_ptr<SmtpSession> session, const std::string& cmd) {
    session->enqueueCommand(cmd);
    pool->submitNextCommand(session);
}

int main() {
    try {
        asio::io_context io;
        asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 2525));

        pool->start();
        std::cout << "✅ SMTP test server started on port 2525\n";

        std::function<void()> do_accept;
        do_accept = [&]() {
            auto socket = std::make_shared<asio::ip::tcp::socket>(io);
            acceptor.async_accept(*socket, [&, socket](std::error_code ec) mutable {
                if (!ec) {
                    std::string ip;
                    std::error_code ep;
                    ip = socket->remote_endpoint(ep).address().to_string();
                    if (ep) ip = "unknown";
                    std::cout << "📡 New connection from " << ip << "\n";

                    auto session = std::make_shared<SmtpSession>(socket);

                    std::string hello = "220 Simple SMTP Server Ready\r\n";
                    asio::async_write(*socket, asio::buffer(hello),
                        [socket](std::error_code ec, std::size_t) {
                            if (ec) std::cout << "❌ Failed to send greeting: " << ec.message() << "\n";
                        });

                    auto buffer = std::make_shared<asio::streambuf>();
                    auto readLoop = std::make_shared<std::function<void()>>();

                    *readLoop = [=]() mutable {
                        if (session->isClosed()) {
                            std::cout << "⚠️ Session already closed, skipping read\n";
                            return;
                        }

                        asio::async_read_until(*socket, *buffer, "\r\n",
                            [=](std::error_code ec, std::size_t) mutable {
                                std::string ip;
                                std::error_code ep;
                                ip = session->getClientIp();
                                if (ec) {
                                    std::cout << "❌ Connection closed from [" << ip << "]: " << ec.message() << "\n";
                                    session->close();
                                    return;
                                }

                                std::istream is(buffer.get());
                                std::string line;
                                std::getline(is, line);
                                if (!line.empty() && line.back() == '\r') line.pop_back();

                                if (!line.empty()) {
                                    std::cout << "[" << ip << "] CMD: " << line << "\n";
                                    onClientCommand(session, line);
                                }

                                if (!session->isClosed()) {
                                    (*readLoop)();
                                } else {
                                    std::cout << "🛑 Session closed, stopping read loop\n";
                                }
                            });
                    };

                    (*readLoop)(); 
                }
                do_accept(); 
            });
        };

        do_accept();
        io.run();
    }
    catch (const std::exception &ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
    }

    pool->stop();
    return 0;
}
