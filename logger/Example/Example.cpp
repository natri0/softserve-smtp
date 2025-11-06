#include "Logger.h"
#include "LogReader.h"
#include <iostream>
#include <thread>

void exampleFunction() {
    LOG_INFO(LogLevel::DEBUG) << "Function is started";
    LOG_INFO(LogLevel::TRACE) << "Example function started";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    LOG_WARNING(LogLevel::DEBUG) << "This is a warning from exampleFunction";
    LOG_ERROR(LogLevel::PROD) << "Error occurred in exampleFunction";
    LOG_INFO(LogLevel::TRACE) << "Example function finished";
    LOG_INFO(LogLevel::DEBUG) << "Function is finished";
    
}


void multiThreadLogging() {
    
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::PROD);
    logger.setFlush(false);

    std::thread t1([] { LOG_INFO(LogLevel::DEBUG) << "Thread 1 running"; });
    std::thread t2([] { LOG_ERROR(LogLevel::PROD) << "Thread 2 error"; });
    t1.join();
    t2.join();
}

void readingLogs() {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);

    //LogReader reader("C:/Users/user/source/repos/softserve-smtp/out/build/x64-Release/logger/Logs/log_06-11-25-08_14_58.5619886.txt");

    auto all = logger.readAllLogs();
    auto errors = logger.readLogsByKeyword("[INFO]");

    std::cout << "--- All logs ---\n";
    for (auto& l : all) std::cout << l << '\n';

    std::cout << "--- Only [INFO] ---\n";
    for (auto& e : errors) std::cout << e << '\n';
}


int main() {
    exampleFunction();
    multiThreadLogging();

    readingLogs();

    return 0;
}
