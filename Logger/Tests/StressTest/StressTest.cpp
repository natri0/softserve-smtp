#include "../../Include/Logger.h"
#include "../../Include/LogLevel.h"
#include <thread>
#include <vector>
#include <cassert>

int main1() {
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::TRACE);
    logger.setFlush(false);

    constexpr int THREADS = 8;
    constexpr int MESSAGES_PER_THREAD = 100000;

    std::vector<std::thread> threads;
    std::atomic<int> counter{ 0 };
    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < MESSAGES_PER_THREAD; ++j) {
                logger.logInfo("Thread " + std::to_string(i) + " msg " + std::to_string(j));
                ++counter;
            }
            });
    }

    for (auto& t : threads) t.join();

    assert(counter == THREADS * MESSAGES_PER_THREAD);

    std::cout << "Stress test completed successfully.\n";
}
