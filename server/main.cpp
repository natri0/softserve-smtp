#include "Server.h"
#include "../client/Client.h"

int main()
{
    Server server;
    std::thread server_thread([&]() { server.init(); server.run(); });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Client client00("127.0.0.1", 12345);
    Client client01("127.0.0.1", 12345);

    std::thread client_thread_00([&]()
    {
        client00.start();
        client00.sendMail();
    });

    std::thread client_thread_01([&]()
    {
        client01.start();
        client01.sendMail();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client_thread_00.join();
    client_thread_01.join();
    server_thread.join();

    return 0;
}
