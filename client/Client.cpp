//
// Created by alkir on 10/3/2025.
//

#include "Client.h"
#include <iostream>

Client::Client(const std::string& host, const unsigned short port) :
    server_endpoint(net::ip::make_address(host), port),
    session(std::make_shared<Session>(std::make_shared<net::ip::tcp::socket>(io)))
{
    io.run();
};

Client::~Client()
{
    stop();
}

void Client::run()
{
    if (isRunning) return;
    isRunning = true;

    if (session->connect(server_endpoint)) session->run();
};

void Client::send(std::string str)
{
    session->send(str);
}

void Client::stop()
{
    if (!isRunning) return;
    isRunning = false;

    session->disconnect();
    io.stop();
}
