#pragma once
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <iostream>
#include "ThreadSafeQueue.hpp"

class ThreadPool {
public:
    explicit ThreadPool(int n);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    void start();
    void stop();
    bool submit(std::function<void()> task);

private:
    void workerThread();

    int cnt_thread;
    std::vector<std::thread> threads;
    std::shared_ptr<ThreadSafeQueue<std::function<void()>>> taskQueue;
};
