//
// Created by alkir on 10/20/2025.
//

#ifndef SERVERCONCOLEUI_H
#define SERVERCONCOLEUI_H
#include <mutex>
#include <string>
#include <vector>


class ServerConsoleUI {
public:
    void start(unsigned short port);
    void run();

    void logClientConnected(const std::string& addr);
    void logClientDisconnected(const std::string& addr);
    void logError(const std::string& msg);

private:
    unsigned short currentPort = 0;

    void showBanner();
    void updateScreen();

    void showMenu(std::string_view menu);
    std::string_view modifyLoggerMenu();

    bool handleCommand(int cmd);
    bool runLoggerMenu();

    void logEvent(const std::string& msg);

    std::mutex consoleMutex;
    std::vector<std::string> logBuffer;
};



#endif //SERVERCONCOLEUI_H
