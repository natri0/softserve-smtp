#include <benchmark/benchmark.h>
#include "../../Include/Logger.h"
#include "../../Include/LogQueue.h"
#include "../../Include/LogLevel.h"

constexpr int NUM_THREADS = 4;
constexpr int NUM_MESSAGES = 1000;

static void BM_SingleThreaded(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    logger.setLevel(LogLevel::TRACE);
    logger.setOutput(".");

    for (auto _ : state) {
        logger.logInfo("Single-threaded log message");
    }



}

static void BM_MultiThreaded(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    logger.setLevel(LogLevel::TRACE);
    logger.setOutput(".");
    int num_threads = NUM_THREADS;
    int messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (int i = 0; i < messages_per_thread; ++i) {
                    logger.logInfo("Thread " + std::to_string(t) + " message " + std::to_string(i));
                }
                });
        }

        for (auto& th : threads) th.join();
    }



}

static void BM_LockFree_Queue_PushPop(benchmark::State& state) {
    boost::lockfree::queue<int> queue(4096);
    std::atomic<int> counter{ 0 };

    auto producer = [&] {
        for (int i = 0; i < NUM_MESSAGES; ++i) queue.push(i);
        };

    auto consumer = [&] {
        int value;
        while (counter < NUM_THREADS * NUM_MESSAGES) {
            while (queue.pop(value)) ++counter;
            std::this_thread::yield();
        }
        };

    for (auto _ : state) {
        std::vector<std::thread> producers(NUM_THREADS);
        std::vector<std::thread> consumers(1);

        for (auto& t : producers) t = std::thread(producer);
        for (auto& t : consumers) t = std::thread(consumer);

        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();
    }


}

static void BM_ThreadSafe_Queue_PushPop(benchmark::State& state) {
    LogQueue<int> queue;
    std::atomic<int> counter{ 0 };

    auto producer = [&] {
        for (int i = 0; i < NUM_MESSAGES; ++i) queue.Push(i);
        };

    auto consumer = [&] {
        while (counter < NUM_THREADS * NUM_MESSAGES) {
            auto value = queue.Pop();
            if (value) ++counter;
            else std::this_thread::yield();
        }
        };

    for (auto _ : state) {
        std::vector<std::thread> producers(NUM_THREADS);
        std::vector<std::thread> consumers(1);

        for (auto& t : producers) t = std::thread(producer);
        for (auto& t : consumers) t = std::thread(consumer);

        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();
    }


}


BENCHMARK(BM_LockFree_Queue_PushPop)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_ThreadSafe_Queue_PushPop)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_SingleThreaded)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_MultiThreaded)->Iterations(1000)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
