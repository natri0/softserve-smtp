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

    // void logClientConnected(const std::string& addr);
    // void logClientDisconnected(const std::string& addr);

    bool do_flush;

private:
    void showBanner();
    bool handleCommand(int cmd);

    void updateScreen();
    inline void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    };

    void showMenu(std::string_view menu);

    std::string_view modifyLoggerMenu();
    bool runLoggerMenu();

    void logEvent(const std::string& msg);

    unsigned short currentPort = 0;
    std::mutex consoleMutex;
    std::vector<std::string> logBuffer;
};


#endif //SERVERCONSOLEUI_H
