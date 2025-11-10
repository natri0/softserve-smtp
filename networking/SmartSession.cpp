//
// Created by alkir on 11/4/2025.
//

#include "SmartSession.h"

#include <iostream>

#include "SSL/KeyExchanger.h"
#include "Logger.h"

SmartSession::SmartSession(std::shared_ptr<net::ip::tcp::socket> socket, Type sessionType) :
    type(sessionType),
    net_session(std::make_shared<Session>(socket)),
    smtp_session(std::make_shared<ISXSMTP::SMTPSession>())
{
}

void SmartSession::setConnection()
{
    auto keys = std::make_shared<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>>(
        smtp::ssl::KeyExchange::generateKeyPair()
    );

    net_session->setOnMessage([this, keys](boost::asio::const_buffer msg) { setKeys(msg, keys); });

    if (type == SERVER) net_session->send(net::buffer(keys->second));
    LOG_INFO(DEBUG_LOG_LEVEL) << "Sent public key";
    net_session->run();
}

void SmartSession::setKeys(boost::asio::const_buffer msg,
                           std::shared_ptr<std::pair<std::vector<unsigned char>, std::vector<unsigned char>>> keys)
{
    auto& private_key = keys->first;
    auto& public_key = keys->second;

    LOG_INFO(DEBUG_LOG_LEVEL) << "Received peer public key";

    if (type == CLIENT) net_session->send(net::buffer(public_key));

    std::vector peer_public_key(
        static_cast<const unsigned char*>(msg.data()),
        static_cast<const unsigned char*>(msg.data()) + msg.size()
    );

    const auto sharedSecret = smtp::ssl::KeyExchange::performDHExchange(
        peer_public_key, private_key);
    const auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);

    net_session->setKey(sessionKey);
    LOG_INFO(DEBUG_LOG_LEVEL) << "Session key established successfully";

    boost::asio::post(net_session->getSocket()->get_executor(), [this]()
    {
        if (SMTPHandling) net_session->setOnMessage(SMTPHandling);
        if (type == SERVER) net_session->send(net::buffer(ISXSMTP::SMTPSession().OnConnect()));
    });
}
