//
// Created by alkir on 10/3/2025.
//

#include "Client.h"

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<Session>(std::make_shared<net::ip::tcp::socket>(io)))
{
};

Client::~Client()
{
    session->disconnect();
}

void Client::run(std::string str) const
{
    session->connect(server_endpoint);
    if (!session || !session->isConnected()) return;

    while (true) session->send(str);
};
