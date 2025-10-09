#define NOMINMAX
#include <algorithm>
#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int n) {
    cnt_thread = n <= 0 ? std::max(std::thread::hardware_concurrency(), static_cast<unsigned int>(2)) : n;
    SessionQueue = std::make_shared<ThreadSafeQueue<SessionTask>>();
}

void ThreadPool::start(){
    for (int i = 0; i < cnt_thread; ++i) {
        threads.emplace_back([this] { workerThread(); });
    }
}

void ThreadPool::stop(){
    SessionQueue->shutDown();
    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }
}

bool ThreadPool::submitNextCommand(std::shared_ptr<SmtpSession> session) {
    if (!session) return false;
    if (session->isClosed()) return false;
    if (!session->compareBusy()) return false; 

    if (!session->hasNextCommand()) {
        session->setBusy(false);
        return false;
    }

    SessionQueue->push({ session });
    return true;
}


void ThreadPool::workerThread() {
    thread_local SmtpCommandProcessor processor;
    SessionTask task;

    while (SessionQueue->pop(task)) {
        try {
            auto session = task.session_smpt;
            if (!session || session->isClosed()) {
                if (session) session->setBusy(false);
                continue;
            }

            if (!session->hasNextCommand()) {
                session->setBusy(false);
                continue;
            }

            auto cmd = session->popCommand();

            auto socket = session->getSocket();
            if (!socket || !socket->is_open()) {
                session->setBusy(false);
                session->close();
                continue;
            }
            try {
                processor.handle(session, cmd);
            } catch (const std::exception& e) {
                std::cerr << "Processor exception: " << e.what() << std::endl;
                session->close();
                continue;
            }

            if (!session->releaseIfEmpty()) {
                SessionQueue->push({ session });
            }

        } catch (const std::exception& e) {
            std::cerr << "Worker exception: " << e.what() << std::endl;
        }
    }
}

ThreadPool::~ThreadPool(){
    stop();
}