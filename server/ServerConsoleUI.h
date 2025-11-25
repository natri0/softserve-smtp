//
// Created by alkir on 10/20/2025.
//

#ifndef SERVERCONSOLEUI_H
#define SERVERCONSOLEUI_H
#include <cstring>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <format>

class ServerConsoleUI
{
public:
    void start(unsigned short port);
    void run();
    void updateOnConnected(int _clients_num);

    bool do_flush = false;
    unsigned short clients_num = 0;

private:
    enum MenuType
    {
        Main,
        Logger
    } menuType = Main;

    void showBanner(int clients_num);
    void updateScreen(int _clients_num);
    void showMenu(std::string_view menu);

    bool handleCommand(int cmd);

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    };

    std::string_view modifyLoggerMenu();
    bool runLoggerMenu();

    void logEvent(const std::string& msg);

    template <typename T>
    T readCommand(const char* str, T& out) const
    {
        while (true)
        {
            std::cin >> out;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                const std::string s = std::format("\033[A\033[{}C\033[K", std::strlen(str));
                std::cout << s;
            }
            else return out;
        }
    };

    unsigned short currentPort = 0;
    std::mutex consoleMutex;
    std::vector<std::string> logBuffer;
};


#endif //SERVERCONSOLEUI_H
