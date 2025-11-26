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
    /** @brief Mutex for protecting queue access */
    mutable std::mutex queue_mutex;

    /** @brief Condition variable for blocking pop operations */
    std::condition_variable cv;

    /** @brief Atomic flag indicating if queue is shut down */
    std::atomic<bool> shutdown_flag{false};

    /** @brief Underlying queue container */
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
    * @brief Push an element into the queue (move).
    * @return true if added, false if queue is closed.
    */
    bool push(T&& item) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (shutdown_flag) return false;
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
    * @return std::optional<T> containing the element, or std::nullopt if shutdown.
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
    bool isEmpty() const noexcept{
        std::lock_guard<std::mutex> lock(queue_mutex);
        return safe_queue.empty();
    }

    /**
    * @brief Check if the queue is shutdown.
    *
    * @return true if queue is shutdown, false otherwise.
    */
    bool isShutdown() const noexcept {
        return shutdown_flag.load(std::memory_order_acquire);
    }

    /**
    * @brief Clear all elements from the queue.
    *
    * Removes all pending elements from the queue in a thread-safe manner.
    * Does not affect the shutdown state.
    *
    * @return true if queue was cleared (had elements), false if queue was already empty.
    */
    bool clear() noexcept { 
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (safe_queue.empty()) return false;
        std::queue<T> tmp;
        std::swap(safe_queue, tmp);
        return true;
    }

    /**
    * @brief Get the number of elements in the queue.
    *
    * Thread-safe operation that returns the current queue size.
    *
    * @return Number of elements currently in the queue.
    */
    size_t size() const noexcept {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return safe_queue.size();
    }
    /**
    * @brief Shutdown the queue.
    *
    * Sets the shutdown flag and notifies all waiting threads.
    * After shutdown, no more elements can be pushed.
    *
    * @param clear If true, clears all pending items from the queue.
    */
    bool shutDown(bool clear = false) noexcept {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (shutdown_flag.exchange(true, std::memory_order_acq_rel)){
                return false; 
            }

            if (clear) {
                std::queue<T> tmp;
                std::swap(safe_queue, tmp);
            }
        }

        cv.notify_all();
        return true;
    }
};