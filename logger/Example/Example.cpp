#include "Logger.h"
#include <iostream>
#include <thread>

void exampleFunction() {
    /*LOG_INFO(LogLevel::DEBUG) << "Function is started";
    LOG_INFO(LogLevel::TRACE) << "Example function started";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    LOG_WARNING(LogLevel::DEBUG) << "This is a warning from exampleFunction";
    LOG_ERROR(LogLevel::PROD) << "Error occurred in exampleFunction";
    LOG_INFO(LogLevel::TRACE) << "Example function finished";
    LOG_INFO(LogLevel::DEBUG) << "Function is finished";*/
    LOG_INFO(LogLevel::Trace) << "Program started!";
    LOG_WARNING(LogLevel::Debug) << "Potential issue detected";
    LOG_ERROR(LogLevel::Prod) << "Critical failure!";
}


void multiThreadLogging() {
    
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::Prod);
    logger.setFlush(false);

    std::thread t1([] { LOG_INFO(LogLevel::Debug) << "Thread 1 running"; });
    std::thread t2([] { LOG_ERROR(LogLevel::Prod) << "Thread 2 error"; });
    t1.join();
    t2.join();
}

void readingLogs() {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);

    auto all = logger.readAllLogs();
    auto errors = logger.readLogsByKeyword("[INFO]");

    std::cout << "--- All logs ---\n";
    for (auto& l : all) std::cout << l << '\n';

    std::cout << "--- Only [INFO] ---\n";
    for (auto& e : errors) std::cout << e << '\n';
}


int main() {
    exampleFunction();
    //multiThreadLogging();

    //readingLogs();

    return 0;
}
