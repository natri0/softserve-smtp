//
// Created by alkir on 10/20/2025.
//

#include <iostream>

#include "Client.h"

int main()
{
    Client client("127.0.0.1", 12345);

    EmailMessage e_msg;

    client.start();

    // temp loop till there's no proper email manager snd UI loop to keep server alive
    int escape = 1;
    do
    {
        std::cout << "\nContent: ";
        std::cin >> e_msg.body;
        client.sendMail(e_msg);
    }
    while (escape == 1);
}