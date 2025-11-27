//
// Created by alkir on 10/20/2025.
//

#include <iostream>

#include "Client.h"

int main()
{
    Client client("127.0.0.1", 1025);

    EmailMessage e_msg;

    client.start();

    // temp loop till there's no proper email manager snd UI loop to keep server alive
    do
    {
        std::cout << "\nContent: ";
        std::cin >> e_msg.body;
        client.sendMail(e_msg);
    }
    while (true);
}