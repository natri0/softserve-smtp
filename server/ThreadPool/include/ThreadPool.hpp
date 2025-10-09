#pragma once
#include <thread>
#include <vector>
#include <memory>
#include "SmtpCommandProcessor.hpp"
#include "ThreadSafeQueue.hpp"
#include "SMTPSession.hpp"


/**
* @struct SessionTask
* @brief Represents a single task to be executed by the thread pool.
*
* Each task contains a shared pointer to an SMTP session. The worker threads
* in ThreadPool will fetch SessionTask objects from the queue and process
* the associated SMTP session commands.
*/
struct SessionTask{
    std::shared_ptr<SmtpSession> session_smpt;
};

/**
* @class ThreadPool
* @brief A simple thread pool for processing SMTP session tasks.
*
* This class manages a fixed number of worker threads that continuously
* fetch and execute tasks from a thread-safe queue of SessionTask objects.
* It ensures that tasks are processed concurrently while providing
* methods to start, stop, and submit commands for execution.
*/
class ThreadPool {
private:

    std::vector<std::thread> threads;
    std::shared_ptr<ThreadSafeQueue<SessionTask>> SessionQueue;

    int cnt_thread;

    /**
    * @brief Main function executed by each worker thread.
    *
    * This function continuously fetches tasks from the command/session queue
    * and processes them. It runs until the thread pool is shutting down.
    */
    void workerThread();


public:

    /**
    * @brief Construct a new ThreadPool object with a given number of threads.
    * @param n Number of worker threads to create.
    */
    explicit ThreadPool(int n);

    /**
    * @brief Disable copy and move operations for ThreadPool.
    *
    * Copy constructor, copy assignment operator, move constructor, and
    * move assignment operator are deleted to prevent copying or moving
    * the ThreadPool instance, ensuring unique ownership of threads and resources.
    */
	ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    /** 
    * @brief start end add threads task to vector
    */
    void start();

    /** 
    * @brief stop all threads in vector
    */
    void stop();

    /**
    * @brief Submits the next SMTP command for processing in the given session.
    * 
    * This function checks if the session has pending commands and, if so,
    * schedules the next command for execution.
    *
    * @param session Shared pointer to the SmtpSession to process.
    * @return true if a command was successfully submitted, false if there
    *         are no more commands in the session.
    */
    bool submitNextCommand(std::shared_ptr<SmtpSession> session);

    /**
    * @brief if threads not join, join all threads.
    */
    ~ThreadPool();
};
