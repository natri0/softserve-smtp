//
// Created by alkir on 10/20/2025.
//

#ifndef SERVERCONSOLEUI_H
#define SERVERCONSOLEUI_H
#include <mutex>
#include <string>
#include <vector>

class ServerConsoleUI
{
public:
    void start(unsigned short port);
    void run();
    void updateOnConnected(int _clients_num);

    // void logClientConnected(const std::string& addr);
    // void logClientDisconnected(const std::string& addr);

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

    inline void clearScreen()
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

    unsigned short currentPort = 0;
    std::mutex consoleMutex;
    std::vector<std::string> logBuffer;
};


#endif //SERVERCONSOLEUI_H
