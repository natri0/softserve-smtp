#include "../include/ThreadPool.hpp"

ThreadPool::ThreadPool(int n) {
    cnt_thread = n <= 0 ? std::max(std::thread::hardware_concurrency(), 2u) : n;
    threads.reserve(cnt_thread);
    taskQueue = std::make_shared<ThreadSafeQueue<FunctionWrapper>>();
}

void ThreadPool::start() {

    if (!threads.empty()) {
        throw std::runtime_error("ThreadPool already started");
    }

    for (int i = 0; i < cnt_thread; ++i) {
        threads.emplace_back([this] { 
            currentThread = this;
            workerThread(); 
        });
    }
}

bool ThreadPool::isShutDown() const noexcept{
    return shutDown.load();
}

void ThreadPool::stop() {

    bool expected = false;
    if (!shutDown.compare_exchange_strong(expected, true)) {
        return;
    }

    taskQueue->shutDown();
    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }
}

ThreadPool* ThreadPool::Current()
{
	return  currentThread;
}

void ThreadPool::workerThread() {

    while (auto task = taskQueue->pop()) {
        try {
            task.value()();
        } catch (const std::exception& e) {
            //loger in future
            std::cerr << "Worker thread exception: " << e.what() << std::endl;
        } catch (...) {
            //loger in future
            std::cerr << "Worker thread unknown exception" << std::endl;
        }
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

thread_local ThreadPool* ThreadPool::currentThread{nullptr};