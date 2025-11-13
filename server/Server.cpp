//
// Created by alkir on 10/3/2025.
//
#include <iostream>
#include "Server.h"

#include <cmath>

#include "ThreadPool/include/ThreadPool.hpp"
#include "../networking/SSL/KeyExchanger.h"
#include "SMTPSession.h"
#include "../logger/Include/Logger.h"
#include "config/config.h"

// temp till we don't have parser

Server::Server() : io(std::make_shared<boost::asio::io_context>()),
                   work(io->get_executor()), acceptor(net::ip::tcp::acceptor(*io)),
                   imapAcceptor(net::ip::tcp::acceptor(*io)),
                   sslContext(smtp::ssl::SSLContextFactory::createServerContext())
{
}

Server::~Server()
{
    stop();
}

bool Server::init()
{
    // setting logger
    Logger::getInstance().setLevel(DEBUG_LOG_LEVEL);
    Logger::getInstance().setFlush(false);
    ui->do_flush = false;

    Config config;
    if (!config.load_from_file("server/config/config.json")) {
        LOG_ERROR(LogLevel::PROD) << "Couldn't load config.json";
        return false;
    }

    port = config.get_with_default<unsigned short>("port", 12345);
    imapPort = config.get_with_default<unsigned short>("imap_port", 12346);
    thread_pool_size = config.get_with_default<unsigned short>("thread_pool_size", 4);

    // mailbox lifespan = server lifespan
    static std::shared_ptr<ISXSMTP::SMTPIMailbox> mailbox = std::make_shared<SQLiteMailbox>();
    ISXSMTP::SMTPConfigBuilder builder;
    builder.SetMailbox(mailbox);
    builder.SetDomain(config.get_with_default<std::string>("smtp.domain", "smtp.test"));
    builder.SetCurrentAsDefault();

    threadPool = std::make_unique<ThreadPool>(thread_pool_size);

    // net
    if (!setUpAcceptor()) return false;

    threadPool->start();

    // starting the console UI
    ui->start(port);

    return true;
}

bool Server::stop()
{
    boost::system::error_code ec;
    acceptor.cancel(ec);
    if (acceptor.is_open()) acceptor.close(ec);

    if (ec)
        LOG_ERROR(DEBUG_LOG_LEVEL) << "Error closing acceptor: " << ec.message();

    sessions.clear();
    io->stop();
    threadPool->stop();

    return true;
}

bool Server::reset()
{
    stop();
    isStopping = false;
    io->restart();
    threadPool->start();

    if (!init())return false;
    runAcceptor(acceptor, std::bind(&Server::setConnection, this, std::placeholders::_1));
    runAcceptor(imapAcceptor, std::bind(&Server::onIMAPAccept, this, std::placeholders::_1));

    return true;
}

void Server::run()
{
    if (!acceptor.is_open()) return;

    for (unsigned short i = 0; i < thread_pool_size; ++i)
        threadPool->submit([self = shared_from_this()]()
        {
            self->io->run();
        });

    runAcceptor(acceptor, std::bind(&Server::setConnection, this, std::placeholders::_1));
    runAcceptor(imapAcceptor, std::bind(&Server::onIMAPAccept, this, std::placeholders::_1));

    LOG_INFO(PROD_LOG_LEVEL) << "Server is running";
    ui->run();

    LOG_INFO(PROD_LOG_LEVEL) << "Server shut down";
    stop();
}

void Server::runAcceptor(net::ip::tcp::acceptor &acceptor, std::function<void(std::shared_ptr<net::ip::tcp::socket>)> onAccept)
{
    auto socket = std::make_shared<net::ip::tcp::socket>(*io);

    acceptor.async_accept(*socket, [this, &acceptor, onAccept, socket](const boost::system::error_code& ec)
    {
        if (!ec) onAccept(socket);
        else
            LOG_ERROR(PROD_LOG_LEVEL) << "Accept failed: " << ec.message();

        runAcceptor(acceptor, onAccept);
    });
}

bool Server::setUpAcceptor()
{
    acceptor.open(net::ip::tcp::v6());

    acceptor.set_option(net::ip::v6_only(false));
    acceptor.set_option(net::ip::tcp::acceptor::reuse_address(true));

    boost::system::error_code ec;
    acceptor.bind({net::ip::tcp::v6(), port}, ec);

    if (ec)
    {
        LOG_ERROR(PROD_LOG_LEVEL) << "Bind failed: " << ec.message();
        return false;
    }

    acceptor.listen();

    imapAcceptor.open(net::ip::tcp::v6());
    imapAcceptor.set_option(net::ip::v6_only(false));
    imapAcceptor.set_option(net::ip::tcp::socket::reuse_address(true));
    imapAcceptor.bind({net::ip::tcp::v6(), imapPort}, ec);
    if (ec) {
        LOG_ERROR(PROD_LOG_LEVEL) << "Bind failed: " << ec.message();
        return false;
    }

    imapAcceptor.listen();

    return true;
}

void Server::setConnection(std::shared_ptr<net::ip::tcp::socket> socket)
{
    LOG_INFO(PROD_LOG_LEVEL) << "New connection from " << socket->remote_endpoint();
    auto session = std::make_shared<SmartSession>(socket, SmartSession::Type::SERVER);

    {
        std::lock_guard lock(sessionMutex);
        sessions.push_back(session);
        ui->updateOnConnected(sessions.size());
    }

    session->net()->setOnDisconnect([this, session]()
    {
        {
            std::lock_guard lock(sessionMutex);
            sessions.remove(session);
        }
        LOG_INFO(PROD_LOG_LEVEL) << "Client disconnected";

        ui->updateOnConnected(sessions.size());
    });

    session->setSMTPHandling([this, session](boost::asio::const_buffer msg) { SMTPHandling(msg, session); });
    session->setConnection();
}

void Server::onIMAPAccept(std::shared_ptr<net::ip::tcp::socket> socket) {
    LOG_INFO(PROD_LOG_LEVEL) << "New connection from " << socket->remote_endpoint();
    auto session = std::make_shared<IMAPSession>(socket);

    {
        std::lock_guard lock(imapSessionMutex);
        imapSessions.push_back(session);
    }

    session->net()->setOnDisconnect([this, session] {
        std::lock_guard lock(imapSessionMutex);
        imapSessions.remove(session);

        LOG_INFO(LogLevel::DEBUG) << "IMAP Client disconnected";
    });

    session->init();
}

void Server::SMTPHandling(boost::asio::const_buffer msg, std::shared_ptr<SmartSession> session)
{
    const std::string cmd(
        std::string(static_cast<const char*>(msg.data()), msg.size()));

    auto rpl = session->smtp()->OnMessage(cmd.c_str());
    session->net()->send(net::buffer(rpl));

    LOG_INFO(DEBUG_LOG_LEVEL) << "Received message from: " << session->net()->getSocket()->
                                                                       remote_endpoint();
    LOG_INFO(DEBUG_LOG_LEVEL) << "Message: " << cmd;
    LOG_INFO(DEBUG_LOG_LEVEL) << "Reply: " << rpl;
}
