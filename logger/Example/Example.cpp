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
    LOG_INFO(LogLevel::DEBUG) << "Function is started";
}

int main() {

    
    Logger& logger = Logger::getInstance(LogLevel::TRACE, "MyLogs", 5, true);

    logger.setFlush(true);

    std::cout << "Current log level: " << logger.toString(logger.getLevel()) << "\n\n";

    exampleFunction();

    logger.setLevel(LogLevel::DEBUG);
    std::cout << "Log level changed to: " << logger.toString(logger.getLevel()) << "\n";

    std::thread t1([] { LOG_INFO(LogLevel::DEBUG) << "Thread 1 running"; });
    std::thread t2([] { LOG_ERROR(LogLevel::PROD) << "Thread 2 error"; });
    t1.join();
    t2.join();


    

    logger.setFlush(false);
    std::cout << "Auto-flush disabled.\n\n";


    LogReader reader(logger.getOutputPath());
    auto all = reader.readAll();
    auto errors = reader.readByParameter("[ERROR]");

    std::cout << "\n--- Log file content ---\n";
    for (auto& l : all) std::cout << l << '\n';

    std::cout << "\n--- Only [ERROR] entries ---\n";
    for (auto& e : errors) std::cout << e << '\n';

    logger.setOutputPath("MyLogs_Updated");
    std::cout << "Output path changed to: " << logger.getOutputPath() << "\n";

    return 0;
}
