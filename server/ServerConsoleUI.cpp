//
// Created by alkir on 10/20/2025.
//

#include "ServerConsoleUI.h"

#include <iomanip>
#include <iostream>
#include <chrono>

#include "Logger.h"

std::string MainBanner =
    "=====================================\n"
    "         SMTP SERVER STARTED         \n"
    "=====================================\n";

std::string ServerMenu =
    "===== SERVER MENU =====\n"
    "0 - Stop server\n"
    "1 - Logger menu\n"
    "2 - Clear screen\n"
    "=======================\n";

std::string LoggerMenu =
    "===== LOGGER MENU =====\n"
    "Current LOG LEVEL: {log_level}\n"
    "0 - Back\n"
    "1 - Set log level\n"
    "2 - Get all logs\n"
    "3 - Get logs by the KEYWORD\n"
    "=======================\n";

std::string new_log_menu;

namespace Color
{
    constexpr const char* RESET = "\033[0m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* GRAY = "\033[90m";
}

void ServerConsoleUI::start(unsigned short port)
{
    currentPort = port;

    showBanner();
}

void ServerConsoleUI::run()
{
    int cmd;

    do
    {
        showMenu(ServerMenu);
        std::cin >> cmd;
        cmd = handleCommand(cmd);
    }
    while (cmd != 0);
}

void ServerConsoleUI::showBanner()
{
    std::lock_guard lock(consoleMutex);
    std::cout << Color::CYAN
        << MainBanner
        << Color::RESET;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << Color::GRAY << "Started: " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "Listening on port: " << Color::GREEN << currentPort << Color::RESET << "\n\n";
}

void ServerConsoleUI::updateScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    showBanner();
}

void ServerConsoleUI::logEvent(const std::string& msg)
{
    std::lock_guard lock(consoleMutex);

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

void ServerConsoleUI::showMenu(std::string_view menu)
{
    std::lock_guard lock(consoleMutex);
    std::cout << "\n" << Color::CYAN
        << menu
        << Color::RESET
        << "Enter command: ";
}

std::string_view ServerConsoleUI::modifyLoggerMenu()
{
    const std::string log_level = Logger::getInstance().toString(Logger::getInstance().getLevel());

    new_log_menu = LoggerMenu;
    new_log_menu.replace(LoggerMenu.find("{log_level}"), std::string("{log_level}").length(), log_level);
    return new_log_menu;
}

bool ServerConsoleUI::handleCommand(int cmd)
{
    bool logger_run = true;

    switch (cmd)
    {
    case 0:
        logEvent("Server stopping...");
        return false;
    case 1:
        do
        {
            logger_run = runLoggerMenu();
        }
        while (logger_run);
        break;
    case 2:
        updateScreen();
        break;
    default:
        std::cout << Color::YELLOW << "Unknown command." << Color::RESET << "\n";
        break;
    }
    return true;
}

bool ServerConsoleUI::runLoggerMenu()
{
    int cmd_l;

    showMenu(modifyLoggerMenu());

    std::cin >> cmd_l;
    switch (cmd_l)
    {
    default:
        updateScreen();
        return false;
    case 0:
        updateScreen();
        return false;
    case 1:
        int lvl;
        std::cout << "Enter the level:\n"
            << "\tNONE -  0\n\tPROD -  1\n\tDEBUG - 2\n\tTRACE - 3 " << std::endl;
        std::cin >> lvl;
        Logger::getInstance().setLevel(LogLevel(lvl));
        return true;
    case 2:
        for (const std::string& log : Logger::getInstance().readAllLogs()) std::cout << log;
        return true;
    case 3:
        std::string keyword;
        std::cout << "Enter the KEYWORD: ";
        std::cin >> keyword;
        const std::vector<std::string> grape_logs = Logger::getInstance().readLogsByKeyword(keyword);
        for (const std::string& log : grape_logs) std::cout << log << std::endl;
        return true;
    }
}
