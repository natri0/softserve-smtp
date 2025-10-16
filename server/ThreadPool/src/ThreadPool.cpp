#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int n) {
    cnt_thread = n <= 0 ? std::max(std::thread::hardware_concurrency(), 2u) : n;
    taskQueue = std::make_shared<ThreadSafeQueue<std::function<void()>>>();
}

void ThreadPool::start() {
    for (int i = 0; i < cnt_thread; ++i) {
        threads.emplace_back([this] { workerThread(); });
    }
}

void ThreadPool::stop() {
    taskQueue->shutDown();
    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }
}

bool ThreadPool::submit(std::function<void()> task) {
    if (!task) return false;
    return taskQueue->push(std::move(task));
}

void ThreadPool::workerThread() {
    std::function<void()> task;

    while (taskQueue->pop(task)) {
        try {
            if (task) {
                task();
            }
        } catch (const std::exception& e) {
            std::cerr << "Worker thread exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Worker thread unknown exception" << std::endl;
        }
    }
}

ThreadPool::~ThreadPool() {
    stop();
}
