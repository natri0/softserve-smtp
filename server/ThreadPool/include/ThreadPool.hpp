#pragma once
#include <vector>
#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <iostream>
#include "ThreadSafeQueue.hpp"
#include "FunctionWrapper.hpp"

class ThreadPool {
public:
    explicit ThreadPool(int n);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    static ThreadPool* Current();

    void start();
    void stop();

    template<typename FunctionType>

    auto submit(FunctionType f)->std::future<std::invoke_result_t<FunctionType>>{
        if (isShutDown()) {
            throw std::runtime_error("Cannot submit task: ThreadPool is shut down");
        }
        using result_type = typename std::invoke_result_t<FunctionType>;

        auto task = std::make_shared<std::packaged_task<result_type()>>(std::move(f));

        std::future<result_type> res = task->get_future();

        taskQueue->push(FunctionWrapper([task]() { (*task)(); }));

        return res;
    }

    bool isShutDown() const noexcept;

private:

    void workerThread();
    std::atomic<bool> shutDown{false};
    int cnt_thread;
    std::vector<std::thread> threads;
    std::shared_ptr<ThreadSafeQueue<FunctionWrapper>> taskQueue;
    static thread_local ThreadPool* currentThread;
};
