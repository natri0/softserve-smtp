#include <benchmark/benchmark.h>
#include "Logger.h"
#include "LogQueue.h"

constexpr std::uint8_t NUM_THREADS = 6;
constexpr int NUM_MESSAGES = 1000;

static void BM_2Threads(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    logger.setLevel(LogLevel::Trace);
    logger.setOutputPath(".");
    int num_threads = NUM_THREADS / 3;
    int messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (int i = 0; i < messages_per_thread; ++i) {
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
    logger.setLevel(LogLevel::Trace);
    logger.setOutputPath(".");
    int num_threads = NUM_THREADS * 2 / 3;
    int messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (int i = 0; i < messages_per_thread; ++i) {
                    LOG_INFO(LogLevel::Debug) << "Thread " << std::to_string(t) << " message " << std::to_string(i);
                    //logger.logInfo("Thread " + std::to_string(t) + " message " + std::to_string(i));
                }
                });
        }

        for (auto& th : threads) th.join();
    }

}

static void BM_6Threads(benchmark::State& state) {
    Logger& logger = Logger::getInstance();
    logger.setFlush(false);
    logger.setLevel(LogLevel::Trace);
    logger.setOutputPath(".");
    int num_threads = 6;
    int messages_per_thread = NUM_MESSAGES;

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&logger, t, messages_per_thread]() {
                for (int i = 0; i < messages_per_thread; ++i) {
                    LOG_INFO(LogLevel::Debug) << "Thread " << std::to_string(t) << " message " << std::to_string(i);
                    //logger.logInfo("Thread " + std::to_string(t) + " message " + std::to_string(i));
                }
                });
        }

        for (auto& th : threads) th.join();
    }

}

static void BM_LockFree_Queue_PushPop(benchmark::State& state) {
    for (auto _ : state) {
        boost::lockfree::queue<int> queue(4096);
        std::atomic<bool> done{ false };

        auto producer = [&]() {
            for (int i = 0; i < NUM_MESSAGES; ++i)
                queue.push(i);
            };

        auto consumer = [&]() {
            int value;
            {
                while (true)
                {
                    while (!queue.empty())
                    {
                        queue.pop(value);
                    }

                    if (done.load() && queue.empty())
                        break;
                }

            }

            //queue.pop(value);
            };

        std::vector<std::thread> producers(NUM_THREADS);
        for (auto& t : producers) t = std::thread(producer);

        std::thread consumer_thread(consumer);

        for (auto& t : producers) t.join();
        done.store(true);
        consumer_thread.join();

    }
}


static void BM_ThreadSafe_Queue_PushPop(benchmark::State& state) {
    for (auto _ : state) {
        LogQueue<int> queue;
        std::atomic<bool> done{ false };

        auto producer = [&]() {
            for (int i = 0; i < NUM_MESSAGES; ++i)
                queue.Push(i);
            };

        auto consumer = [&]() {
            while (true)
            {
                //std::queue<int> localQueue = queue.Extract();
                while (!queue.Empty())
                {
                    queue.Pop();
                }

                if (done.load() && queue.Empty())
                    break;
            }


            };

        std::vector<std::thread> producers(NUM_THREADS);
        for (auto& t : producers) t = std::thread(producer);

        std::thread consumer_thread(consumer);

        for (auto& t : producers) t.join();
        done.store(true);
        consumer_thread.join();

    }
}

BENCHMARK(BM_2Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_4Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_6Threads)->Iterations(1000)->Unit(benchmark::kMicrosecond);
//BENCHMARK(BM_LockFree_Queue_PushPop)->Iterations(1000)->Unit(benchmark::kMicrosecond);
//BENCHMARK(BM_ThreadSafe_Queue_PushPop)->Iterations(1000)->Unit(benchmark::kMicrosecond);




BENCHMARK_MAIN();
