#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include "ThreadSafeQueue.hpp"


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
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool shutdown_flag = false;
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
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (shutdown_flag) return false;
            safe_queue.push(item);
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
    bool pop(T& out) {
        std::unique_lock<std::mutex> lock(queue_mutex);

        cv.wait(lock, [this]() {
            return !safe_queue.empty() || shutdown_flag;
        });

        if (safe_queue.empty())
            return false; 

        out = std::move(safe_queue.front());
        safe_queue.pop();
        return true;
    }

    /**
    * @brief Check if the queue is empty.
    *
    * @return true if queue is empty, false otherwise.
    */
    bool isEmpty() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return safe_queue.empty();
    }

    /**
    * @brief Shutdown the queue.
    *
    * Sets the shutdown flag and notifies all waiting threads.
    * After shutdown, no more elements can be pushed.
    */
    void shutDown() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            shutdown_flag = true;
        }
        cv.notify_all();
    }
};
