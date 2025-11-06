#pragma once
#include <vector>
#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <iostream>
#include "ThreadSafeQueue.hpp"
#include "FunctionWrapper.hpp"


/**
 * @class ThreadPool
 * @brief A thread pool implementation for concurrent task execution.
 *
 * This class manages a pool of worker threads that execute submitted tasks.
 * Tasks are queued in a thread-safe queue and executed by available worker threads.
 * Supports graceful shutdown and task result retrieval via futures.
 */
class ThreadPool {

private:

    enum class State {
        NotStarted,  
        Running,     
        Stopped      
    };

public:
    /**
    * @brief Constructs a ThreadPool with specified number of threads.
    *
    * If n <= 0, uses hardware_concurrency() with a minimum of 2 threads.
    *
    * @param n Number of worker threads to create.
    */
    explicit ThreadPool(int n);

    /**
    * @brief Destructor. Calls stop() to ensure graceful shutdown.
    */
    ~ThreadPool() noexcept;

    /** @brief Deleted copy constructor */
    ThreadPool(const ThreadPool&) = delete;

    /** @brief Deleted copy assignment operator */
    ThreadPool& operator=(const ThreadPool&) = delete;

    /** @brief Deleted move constructor */
    ThreadPool(ThreadPool&&) noexcept = delete;

    /** @brief Deleted move assignment operator */
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    /**
    * @brief Get pointer to the ThreadPool instance for current worker thread.
    *
    * @return Pointer to ThreadPool if called from a worker thread, nullptr otherwise.
    */
    static ThreadPool* Current() noexcept;

    /**
    * @brief Start all worker threads.
    *
    * Creates and launches all worker threads. Each thread will process tasks
    * from the task queue until shutdown is called.
    *
    * @throws std::runtime_error if ThreadPool is already started.
    */
    void start();

    /**
    * @brief Stop the ThreadPool and wait for all threads to finish.
    *
    * Shuts down the task queue and joins all worker threads.
    * This is a blocking operation that waits for all threads to complete.
    * Safe to call multiple times (subsequent calls are no-op).
    */
    void stop();

    /**
    * @brief Submit a task for asynchronous execution.
    *
    * The task is added to the queue and will be executed by an available worker thread.
    * Returns a future that can be used to retrieve the result.
    *
    * @tparam FunctionType Callable type (function, lambda, functor).
    * @param f The function/callable to execute.
    * @return std::future containing the result of the function execution.
    * @throws std::runtime_error if ThreadPool is shut down.
    */
    template<typename FunctionType, typename... Args>
    auto submit(FunctionType&& f, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<FunctionType>, std::decay_t<Args>...>>
    {
        if (CurrentState.load() != State::Running) {
            throw std::runtime_error("ThreadPool not started or is shut down");
        }

        using result_type = std::invoke_result_t<std::decay_t<FunctionType>, std::decay_t<Args>...>;

        auto task = std::make_shared<std::packaged_task<result_type()>>( 
            std::bind(std::forward<FunctionType>(f), std::forward<Args>(args)...)
        );

        std::future<result_type> res = task->get_future();

        if (!taskQueue->push(FunctionWrapper([task]() { (*task)(); }))) {
            throw std::runtime_error("Cannot submit task: ThreadPool is shut down");
        }

        return res;
    }

    State getState() const noexcept;

    /**
    * @brief Check if the ThreadPool is shut down.
    *
    * @return true if ThreadPool is shut down, false otherwise.
    */
    bool isShutDown() const noexcept;

private:

    /**
    * @brief Worker thread function.
    *
    * Continuously pops tasks from the queue and executes them.
    * Catches and logs exceptions thrown by tasks.
    * Exits when queue is shut down.
    */
    void workerThread();

    /** @brief Number of worker threads */
    int cnt_thread = std::max(std::thread::hardware_concurrency(), 2u);

    /** @brief Vector of worker threads */
    std::vector<std::thread> threads;

    std::atomic<State> CurrentState{State::NotStarted};

    /** @brief Thread-safe queue for storing pending tasks */
    std::shared_ptr<ThreadSafeQueue<FunctionWrapper>> taskQueue;

    /** @brief Thread-local pointer to current ThreadPool instance */
    static thread_local ThreadPool* currentThread;
};
