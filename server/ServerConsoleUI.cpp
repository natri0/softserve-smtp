//
// Created by alkir on 10/20/2025.
//

#include "ServerConsoleUI.h"

#include <iomanip>
#include <iostream>
#include <chrono>

#include "Logger.h"

static constexpr const char* MainBanner = R"(
=====================================
             SMTP SERVER
=====================================
Number of CLIENTS: {CLIENTS_NUM}
)";

static constexpr const char* ServerMenu = R"(
===== SERVER MENU =====
0 - Stop server
1 - Logger menu
2 - Clear screen
=======================
)";

static constexpr const char* LoggerMenu = R"(
========== LOGGER MENU ==========
Current LOG LEVEL: {log_level}
Show logs in real time: {BOOL}
0 - Back
1 - Set log level
2 - Get all logs
3 - Get logs by the KEYWORD
4 - Set flush
=================================
)";

std::string new_log_menu;
std::queue<std::string> last_info;

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
    showBanner(clients_num);
}

void ServerConsoleUI::run()
{
    int cmd;
    do
    {
        showMenu(ServerMenu);
        std::cout << "Enter command: ";
        std::cin >> cmd;
        cmd = handleCommand(cmd);
    }
    while (cmd != 0);
}

void ServerConsoleUI::updateOnConnected(int _clients_num)
{
    updateScreen(_clients_num);

    if (menuType == Main) showMenu(ServerMenu);
    else showMenu(modifyLoggerMenu());
    std::cout << "Enter command: ";
}

void ServerConsoleUI::showBanner(int clients_num)
{
    std::string banner = MainBanner;
    banner.replace(std::string(MainBanner).find("{CLIENTS_NUM}"), std::string("{CLIENTS_NUM}").length(),
                   std::to_string(clients_num));

    std::lock_guard lock(consoleMutex);
    std::cout << Color::CYAN
        << banner
        << Color::RESET;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << Color::GRAY << "Started: " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "Listening on port: " << Color::GREEN << currentPort << Color::RESET << "\n";
}

void ServerConsoleUI::updateScreen(int _clients_num)
{
    if (_clients_num != clients_num) clients_num = _clients_num;
    clearScreen();
    showBanner(clients_num);
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

// void ServerConsoleUI::logClientConnected(const std::string& addr)
// {
//     logEvent(std::string(Color::GREEN) + "Client connected: " + addr + Color::RESET);
// }
//
// void ServerConsoleUI::logClientDisconnected(const std::string& addr)
// {
//     logEvent(std::string(Color::YELLOW) + "Client disconnected: " + addr + Color::RESET);
// }

void ServerConsoleUI::showMenu(std::string_view menu)
{
    std::lock_guard lock(consoleMutex);
    std::cout << "\n" << Color::CYAN
        << menu
        << Color::RESET;
}

std::string_view ServerConsoleUI::modifyLoggerMenu()
{
    const std::string log_level = Logger::getLevelName(GET_LEVEL());

    new_log_menu = LoggerMenu;
    new_log_menu.replace(std::string(LoggerMenu).find("{log_level}"), std::string("{log_level}").length(), log_level);
    new_log_menu.replace(std::string(new_log_menu).find("{BOOL}"), std::string("{BOOL}").length(),
                         do_flush == true ? "TRUE" : "FALSE");
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
            menuType = Logger;
            updateScreen(clients_num);
            logger_run = runLoggerMenu();
        }
        while (logger_run);
        break;
    case 2:
        updateScreen(clients_num);
        break;
    default:
        std::cout << Color::YELLOW << "Unknown command." << Color::RESET << "\n";
        break;
    }
    return true;
}

bool ServerConsoleUI::runLoggerMenu()
{
    showMenu(modifyLoggerMenu());
    if (!last_info.empty())
        while (!last_info.empty())
        {
            std::cout << last_info.front() << "\n";
            last_info.pop();
        }

    std::cout << "Enter command: ";

    int cmd_l;
    std::cin >> cmd_l;
    switch (cmd_l)
    {
    case 0:
        {
            updateScreen(clients_num);
            menuType = Main;
            return false;
        }
    case 1:
        {
            int lvl;
            std::cout << "Enter the level:\n"
                << "\tNONE -  0\n\tPROD -  1\n\tDEBUG - 2\n\tTRACE - 3 " << std::endl;
            std::cin >> lvl;
            SET_LEVEL(LogLevel(lvl));
            return true;
        }
    case 2:
        {
            for (const std::string& log : Logger::getInstance().readAllLogs())
            {
                last_info.push(log);
                std::cout << log << std::endl;
            }
            return true;
        }
    case 3:
        {
            std::string keyword;
            std::cout << "Enter the KEYWORD: ";
            std::cin >> keyword;
            const std::vector<std::string> grape_logs = Logger::getInstance().readLogsByKeyword(keyword);
            for (const std::string& log : grape_logs)
            {
                last_info.push(log);
                std::cout << log << std::endl;
            }
            return true;
        }
    case 4:
        {
            std::cout << "YES - 1\tNO - 0" << std::endl;
            std::cin >> do_flush;
            Logger::getInstance().setFlush(do_flush);
            return true;
        }
    default:
        {
            std::cout << Color::YELLOW << "Unknown logger command." << Color::RESET << std::endl;
            return true;
        }
    }
}
