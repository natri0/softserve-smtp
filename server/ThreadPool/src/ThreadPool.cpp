#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int n) {
    cnt_thread = n <= 0 ? std::max(std::thread::hardware_concurrency(), 2u) : n;
    threads.reserve(cnt_thread);
    taskQueue = std::make_shared<ThreadSafeQueue<FunctionWrapper>>();
}

void ThreadPool::start() {

    State expected = State::NotStarted;
    
    if (!CurrentState.compare_exchange_strong(expected, State::Running)) {
        if (expected == State::Running) {
            throw std::runtime_error("ThreadPool already running");
        } else {
            throw std::runtime_error("Cannot restart stopped ThreadPool");
        }
    }
    
    for (int i = 0; i < cnt_thread; ++i) {
        threads.emplace_back([this] { 
            currentThread = this;
            workerThread(); 
        });
    }
}

bool ThreadPool::isShutDown() const noexcept{
    return CurrentState.load(std::memory_order_acquire) == State::Stopped;
}

void ThreadPool::stop() {
    State expected = State::Running;
    if (CurrentState.compare_exchange_strong(expected, State::Stopped)) {
        taskQueue->shutDown();
        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
        threads.clear();
    }
    else if (expected == State::NotStarted) {
        expected = State::NotStarted;
        if (CurrentState.compare_exchange_strong(expected, State::Stopped)) {
            taskQueue->shutDown();
        }
    }
}

ThreadPool::State ThreadPool::getState() const noexcept {
    return CurrentState.load(std::memory_order_acquire);
}

ThreadPool* ThreadPool::Current() noexcept
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

ThreadPool::~ThreadPool() noexcept{
    stop();
}

thread_local ThreadPool* ThreadPool::currentThread{nullptr};