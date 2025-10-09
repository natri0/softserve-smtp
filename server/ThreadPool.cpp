//
// Created by alkir on 10/8/2025.
//

#include "ThreadPool.h"

constexpr std::uint8_t DEFAULT_NUM_OF_THREADS = 4;

ThreadPool::ThreadPool()
{
    unsigned numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = DEFAULT_NUM_OF_THREADS;

    init(numThreads);
}

ThreadPool::ThreadPool(int numThreads)
{
    init(numThreads);
}

void ThreadPool::init(int numThreads)
{
    for (std::size_t i = 0; i < numThreads; ++i)
        workers.emplace_back([&]()
        {
            while (!stop)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]() { return stop || !tasks.empty(); });

                    if (stop) return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
}

ThreadPool::~ThreadPool()
{
    join_threads();
}

void ThreadPool::add_task_to_queue(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(queueMutex);

    tasks.push(std::move(task));

    condition.notify_one();
}

void ThreadPool::join_threads()
{
    stop = true;

    condition.notify_all();

    for (std::thread& worker : workers)
        if (worker.joinable()) worker.join();
}
