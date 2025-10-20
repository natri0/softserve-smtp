//
// Created by alkir on 10/20/2025.
//

#include "Client.h"

int main()
{
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

    client_thread_00.join();
    client_thread_01.join();
}