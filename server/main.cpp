#include "Server.h"
#include "../client/Client.h"

int main() {
    Server server;
    std::thread server_thread([&]() {server.run();});

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Client client00("127.0.0.1", 12345);
    Client client01("127.0.0.1", 12345);

    std::thread client_thread_00([&](){client00.run("I am client 00!");});
    std::thread client_thread_01([&](){client00.run("I am client 01!");});

    server_thread.join();
    client_thread_00.join();
    client_thread_01.join();

    return 0;
}