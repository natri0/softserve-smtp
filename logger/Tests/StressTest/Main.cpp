#include "Logger.h"
#include "Macros.h"

#include <thread>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

void work(Logger& log) {
    std::ostringstream s;
    s << "hi my name is: " << std::this_thread::get_id();
    std::string id = s.str();
    for (size_t i = 0; i < 10; ++i) {
        LOG_ERROR(LogLevel::PROD)<<id;
        LOG_INFO(LogLevel::PROD) << id;
        LOG_WARNING(LogLevel::PROD) << id;
    }
}


int main(void) {


    Logger& log = Logger::getInstance(TRACE_LOG_LEVEL, "", DEFAULT_AMOUNT);
    log.setFlush(true);

    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    for (auto& t : threads) {
        t = std::thread(work, std::ref(log));

    }

    for (auto& thread : threads) {
        thread.join();
    }

    log.shutDown();

    return 0;
}

