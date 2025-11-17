#include <benchmark/benchmark.h>
#include "Logger.h"
#include "LogQueue.h"

constexpr std::uint8_t NUM_THREADS = 6;
constexpr std::uint16_t NUM_MESSAGES = 1000;

static void BM_2Threads(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    ///logger.setLevel(LogLevel::Trace);
    //logger.setOutputPath(".");
    std::uint8_t num_threads = NUM_THREADS / 3;
    std::uint16_t messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (size_t i = 0; i < messages_per_thread; ++i) {
                    LOG_INFO(LogLevel::Debug) << "Thread " << std::to_string(t) << " message " << std::to_string(i);
                }
                });
        }

        for (auto& th : threads) th.join();
    }


}

static void BM_4Threads(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    //logger.setLevel(LogLevel::Trace);
    //logger.setOutputPath(".");
    std::uint8_t num_threads = NUM_THREADS * 2 / 3;
    std::uint16_t messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (size_t i = 0; i < messages_per_thread; ++i) {
                    LOG_INFO(LogLevel::Debug) << "Thread " << std::to_string(t) << " message " << std::to_string(i);
                }
                });
        }

        for (auto& th : threads) th.join();
    }

}

static void BM_6Threads(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    //logger.setLevel(LogLevel::Trace);
    //logger.setOutputPath(".");
    std::uint8_t num_threads = NUM_THREADS;
    int messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (size_t i = 0; i < messages_per_thread; ++i) {
                    LOG_INFO(LogLevel::Debug) << "Thread " << std::to_string(t) << " message " << std::to_string(i);
                }
                });
        }

        for (auto& th : threads) th.join();
    }

}

BENCHMARK(BM_2Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_4Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_6Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);





BENCHMARK_MAIN();
