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
        std::cout << "FROM: ";
        std::cin >> e_msg.from;
        std::cout << "\nTO: ";
        std::cin >> e_msg.to;
        std::cout << "\nSubject: ";
        std::cin >> e_msg.subj;
        std::cout << "\nContent: ";
        std::cin >> e_msg.body;
        client.sendMail(e_msg);
        std::cout << "Want to proceed? Yes: 1\tNo: 0" << std::endl;
        std::cin >> escape;
    }
    while (escape == 1);
}
