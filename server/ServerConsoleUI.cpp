//
// Created by alkir on 10/20/2025.
//

#include "ServerConsoleUI.h"

#include <iomanip>
#include <iostream>

namespace Color
{
    constexpr const char* RESET = "\033[0m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* GRAY = "\033[90m";
}

void ServerConsoleUI::showBanner(unsigned short port)
{
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::cout << Color::CYAN
        << "=====================================\n"
        << "         SMTP SERVER STARTED         \n"
        << "=====================================\n"
        << Color::RESET;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << Color::GRAY << "Started: " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "Listening on port: " << Color::GREEN << port << Color::RESET << "\n\n";
}

void ServerConsoleUI::logEvent(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(consoleMutex);

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%H:%M:%S");
    std::string timestamp = oss.str();

    std::cout << Color::GRAY << "[" << timestamp << "] "
              << Color::RESET << msg << "\n";

    logBuffer.push_back("[" + timestamp + "] " + msg);
}

void ServerConsoleUI::logClientConnected(const std::string& addr)
{
    logEvent(std::string(Color::GREEN) + "Client connected: " + addr + Color::RESET);
}

void ServerConsoleUI::logClientDisconnected(const std::string& addr)
{
    logEvent(std::string(Color::YELLOW) + "Client disconnected: " + addr + Color::RESET);
}

void ServerConsoleUI::logError(const std::string& msg)
{
    logEvent(std::string(Color::RED) + "Error: " + msg + Color::RESET);
}

void ServerConsoleUI::showMenu()
{
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::cout << "\n" << Color::CYAN
        << "===== SERVER MENU =====\n"
        << "0 - Stop server\n"
        << "1 - Show logs\n"
        << "2 - Clear screen\n"
        << "=======================\n"
        << Color::RESET
        << "Enter command: ";
}

bool ServerConsoleUI::handleCommand(int cmd)
{
    switch (cmd)
    {
    case 0:
        logEvent("Server stopping...");
        return false;
    case 1:
        std::cout << Color::CYAN << "\n--- SERVER LOG ---\n" << Color::RESET;
        for (auto& line : logBuffer)
            std::cout << line << "\n";
        std::cout << Color::CYAN << "------------------\n" << Color::RESET;
        break;
    case 2:
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        showBanner(0);
        break;
    default:
        std::cout << Color::YELLOW << "Unknown command." << Color::RESET << "\n";
        break;
    }
    return true;
}
