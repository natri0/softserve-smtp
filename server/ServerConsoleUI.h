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
    void showBanner(unsigned short port);
    void logEvent(const std::string& msg);
    void logClientConnected(const std::string& addr);
    void logClientDisconnected(const std::string& addr);
    void logError(const std::string& msg);
    void showMenu();
    bool handleCommand(int cmd);

private:
    std::mutex consoleMutex;
    std::vector<std::string> logBuffer;
};



#endif //SERVERCONCOLEUI_H
