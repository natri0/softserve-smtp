//
// Created by alkir on 10/8/2025.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool();
    explicit ThreadPool(int numThreads);
    ~ThreadPool();

    void add_task_to_queue(std::function<void()> task);
    void join_threads();

private:
    void init(int numThreads);

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    std::atomic<bool> stop{false};
};


#endif //THREADPOOL_H
