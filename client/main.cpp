//
// Created by alkir on 10/20/2025.
//

#include <iostream>

#include "Client.h"

int main()
{
    Client client00("127.0.0.1", 12345);

    EmailMessage e_msg;

    std::thread client_thread_00([&]()
    {
        client00.start();

        // temp loop till there's no proper email manager snd UI loop to keep server alive
        uint8_t escape;
        do
        {
            std::cout << "FROM: ";
            std::cin >> e_msg.from;
            std::cout << "\nTO: ";
            std::cin >> e_msg.to;
            std::cout << "\nSubject: ";
            std::cin >> e_msg.subj;
            std::cout << "\nContent: ";
            std::cin >> e_msg.body;
            client00.sendMail(e_msg);
            std::cout << "Want to proceed? Yes: 1\tNo: 0" << std::endl;
            std::cin >> escape;
        } while (escape == 0);
    });

    client_thread_00.join();
}
