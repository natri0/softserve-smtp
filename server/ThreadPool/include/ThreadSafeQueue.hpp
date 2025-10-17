#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>

/**
* @class ThreadSafeQueue
* @brief A thread-safe queue implementation for concurrent task processing.
*
* This class provides a queue that can be safely accessed by multiple threads.
* It supports blocking pop operations, safe push, and graceful shutdown.
* 
* @tparam T Type of elements stored in the queue.
*/
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex queue_mutex;
    std::condition_variable cv;
    std::atomic<bool> shutdown_flag = false;
    std::queue<T> safe_queue;

public:

    /** @brief Default constructor */
    ThreadSafeQueue() = default;

    /** @brief Deleted copy constructor */
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;

    /** @brief Deleted copy assignment operator */
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
    * @brief Push an element into the queue.
    *
    * Thread-safe operation. If the queue has been shut down, push is rejected.
    *
    * @param item Element to push into the queue.
    * @return true if the element was added, false if queue is shutdown.
    */
    bool push(const T& item) {
        if (shutdown_flag) return false;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            safe_queue.push(item);
        }
        cv.notify_one();
        return true;
    }

    /**
    * @brief Push an element into the queue (move).
    * @return true if added, false if queue is closed.
    */
    bool push(T&& item) {
        if (shutdown_flag) return false;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            safe_queue.push(std::move(item));
        }
        cv.notify_one();
        return true;
    }

    /**
    * @brief Pop an element from the queue.
    *
    * Blocks until an element is available or the queue is shutdown.
    *
    * @param out Reference where the popped element will be stored.
    * @return true if an element was popped, false if queue is empty and shutdown.
    */
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(queue_mutex);

        cv.wait(lock, [this]() {
            return !safe_queue.empty() || shutdown_flag;
        });

        if (safe_queue.empty())
            return std::nullopt; 

        auto result = std::move(safe_queue.front());
        safe_queue.pop();
        return result;
    }

    /**
    * @brief Check if the queue is empty.
    *
    * @return true if queue is empty, false otherwise.
    */
    bool isEmpty() const{
        std::lock_guard<std::mutex> lock(queue_mutex);
        return safe_queue.empty();
    }

    bool isShutdown() const noexcept {
        return shutdown_flag.load(std::memory_order_acquire);
    }

    /**
    * @brief Shutdown the queue.
    *
    * Sets the shutdown flag and notifies all waiting threads.
    * After shutdown, no more elements can be pushed.
    */
    void shutDown() noexcept {
        shutdown_flag.store(true, std::memory_order_release);
        cv.notify_all();
    }

};
