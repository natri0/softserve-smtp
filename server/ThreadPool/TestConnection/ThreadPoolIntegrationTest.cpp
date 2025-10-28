#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ThreadPool.hpp"
#include "ThreadSafeQueue.hpp"
#include <chrono>
#include <numeric>
#include <algorithm>

class ThreadSafeQueueTest : public ::testing::Test {
protected:
    ThreadSafeQueue<int> safeQueue;
    ThreadSafeQueue<std::string> strSafeQueue;
};

TEST_F(ThreadSafeQueueTest, PushAndPopSingleElement) {

    EXPECT_TRUE(safeQueue.push(42));
    auto result = safeQueue.pop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST_F(ThreadSafeQueueTest, PushMoveSemantics) {

    std::string str = "test";
    EXPECT_TRUE(strSafeQueue.push(std::move(str)));
}

TEST_F(ThreadSafeQueueTest, isEmptyCheck) {
    
    EXPECT_TRUE(strSafeQueue.isEmpty());
    std::string str = "test";
    EXPECT_TRUE(strSafeQueue.push(std::move(str)));
    EXPECT_FALSE(strSafeQueue.isEmpty());
    EXPECT_EQ(*strSafeQueue.pop(), "test");
}

TEST_F(ThreadSafeQueueTest, isShutDownCheck) {
    
    EXPECT_FALSE(strSafeQueue.isShutdown());
    EXPECT_TRUE(strSafeQueue.shutDown());
    EXPECT_TRUE(strSafeQueue.isShutdown());
}

TEST_F(ThreadSafeQueueTest, isShutDownDoubleCheck) {
    
    EXPECT_FALSE(strSafeQueue.isShutdown());
    EXPECT_TRUE(strSafeQueue.shutDown());
    EXPECT_TRUE(strSafeQueue.isShutdown());
    EXPECT_FALSE(strSafeQueue.shutDown());
    EXPECT_TRUE(strSafeQueue.isShutdown());
}
TEST_F(ThreadSafeQueueTest, MultipleElementsPushPop) {
    EXPECT_TRUE(safeQueue.push(1));
    EXPECT_TRUE(safeQueue.push(2));
    EXPECT_TRUE(safeQueue.push(3));
    
    EXPECT_EQ(*safeQueue.pop(), 1);
    EXPECT_EQ(*safeQueue.pop(), 2);
    EXPECT_EQ(*safeQueue.pop(), 3);
    
    EXPECT_TRUE(safeQueue.isEmpty());
}

TEST_F(ThreadSafeQueueTest, PushAfterShutdownFails) {
    EXPECT_TRUE(safeQueue.shutDown());
    
    EXPECT_FALSE(safeQueue.push(42));
    EXPECT_TRUE(safeQueue.isEmpty());
}

TEST_F(ThreadSafeQueueTest, PopAfterShutdownReturnsNullopt) {
    EXPECT_TRUE(safeQueue.shutDown());
    
    auto result = safeQueue.pop();
    EXPECT_FALSE(result.has_value());
}

TEST_F(ThreadSafeQueueTest, ShutdownWithClearRemovesElements) {
    safeQueue.push(1);
    safeQueue.push(2);
    safeQueue.push(3);
    
    EXPECT_FALSE(safeQueue.isEmpty());
    EXPECT_TRUE(safeQueue.shutDown(true)); // clear = true
    EXPECT_TRUE(safeQueue.isEmpty());
}

TEST_F(ThreadSafeQueueTest, ShutdownWithoutClearKeepsElements) {
    safeQueue.push(1);
    safeQueue.push(2);
    
    EXPECT_TRUE(safeQueue.shutDown(false)); // clear = false
    EXPECT_FALSE(safeQueue.isEmpty());

    EXPECT_EQ(*safeQueue.pop(), 1);
    EXPECT_EQ(*safeQueue.pop(), 2);
}

TEST_F(ThreadSafeQueueTest, ClearEmptyQueue) {
    EXPECT_FALSE(safeQueue.clear()); 
    EXPECT_TRUE(safeQueue.isEmpty());
}

TEST_F(ThreadSafeQueueTest, ClearNonEmptyQueue) {
    safeQueue.push(1);
    safeQueue.push(2);
    
    EXPECT_TRUE(safeQueue.clear());
    EXPECT_TRUE(safeQueue.isEmpty());
}

TEST_F(ThreadSafeQueueTest, BlockingPopTimeout) {
    
    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        safeQueue.push(42);
    });
    
    auto result = safeQueue.pop(); 
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
    
    t.join();
}

TEST_F(ThreadSafeQueueTest, ConcurrentPushPopWithThreadPool) {
    constexpr int NUM_TASKS = 8;
    constexpr int ITEMS_PER_TASK = 500;

    ThreadPool pool(4); 
    pool.start();

    std::atomic<int> pushed{0};
    std::atomic<int> popped{0};
    std::mutex resultsMutex;
    std::vector<int> results;

    for (int t = 0; t < NUM_TASKS / 2; ++t) {
        pool.submit([&]() {
            for (int i = 0; i < ITEMS_PER_TASK; ++i) {
                safeQueue.push(pushed++);
            }
        });
    }

    for (int t = 0; t < NUM_TASKS / 2; ++t) {
        pool.submit([&]() {
            while (true) {
                auto item = safeQueue.pop();
                if (item.has_value()) {
                    std::lock_guard<std::mutex> lock(resultsMutex);
                    results.push_back(*item);
                    popped++;
                } else if (safeQueue.isShutdown()) {
                    break;
                }
            }
        });
    }

    while (pushed.load() < (NUM_TASKS / 2) * ITEMS_PER_TASK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    safeQueue.shutDown(); 
    pool.stop();  

    EXPECT_EQ(pushed.load(), (NUM_TASKS / 2) * ITEMS_PER_TASK);
    EXPECT_EQ(popped.load(), (NUM_TASKS / 2) * ITEMS_PER_TASK);
    EXPECT_TRUE(safeQueue.isEmpty());
    EXPECT_TRUE(safeQueue.isShutdown());
}

TEST(ThreadPoolTest, BasicTaskExecution) {
    ThreadPool pool(2);
    pool.start();

    auto f1 = pool.submit([]() { return 42; });
    auto f2 = pool.submit([]() { return std::string("hello"); });

    EXPECT_EQ(f1.get(), 42);
    EXPECT_EQ(f2.get(), "hello");

    pool.stop();
}

TEST(ThreadPoolTest, ConcurrentTasks) {
    ThreadPool pool(4);
    pool.start();

    constexpr int N = 100;
    std::atomic<int> counter{0};

    std::vector<std::future<void>> futures;
    for (int i = 0; i < N; ++i) {
        futures.push_back(pool.submit([&counter]() { counter++; }));
    }

    for (auto &f : futures) f.get();

    EXPECT_EQ(counter.load(), N);

    pool.stop();
}

TEST(ThreadPoolTest, TaskThrowsException) {
    ThreadPool pool(2);
    pool.start();

    auto f = pool.submit([]() { throw std::runtime_error("test"); });

    EXPECT_THROW(f.get(), std::runtime_error);

    pool.stop();
}

TEST(ThreadPoolTest, SubmitAfterShutdownThrows) {
    ThreadPool pool(2);
    pool.start();
    pool.stop();

    EXPECT_THROW(pool.submit([]() {}), std::runtime_error);
}

TEST(ThreadPoolTest, IntegrationWithQueue) {
    ThreadSafeQueue<int> q;
    ThreadPool pool(4);
    pool.start();

    for (int i = 0; i < 50; ++i) {
        pool.submit([i, &q]() { q.push(i); });
    }

    std::atomic<int> sum{0};
    for (int i = 0; i < 50; ++i) {
        pool.submit([&q, &sum]() {
            auto val = q.pop();
            if (val.has_value()) sum += *val;
        });
    }

    pool.stop();

    EXPECT_EQ(sum.load(), (49 * 50) / 2);
}

TEST(ThreadPoolTest, CurrentThreadPool) {
    ThreadPool pool(2);
    pool.start();
    
    auto f = pool.submit([]() {
        return ThreadPool::Current() != nullptr;
    });
    
    EXPECT_TRUE(f.get());
    
    EXPECT_EQ(ThreadPool::Current(), nullptr);
}

TEST(ThreadPoolTest, TaskExecutionOrder) {
    ThreadPool pool(1); 
    pool.start();
    
    std::vector<int> results;
    std::mutex m;
    
    for (int i = 0; i < 10; ++i) {
        pool.submit([i, &results, &m]() {
            std::lock_guard<std::mutex> lock(m);
            results.push_back(i);
        });
    }
    
    pool.stop();

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(results[i], i);
    }
}

TEST(ThreadPoolTest, StopMultipleTimes) {
    ThreadPool pool(2);
    pool.start();
    pool.stop();
    EXPECT_NO_THROW(pool.stop()); 
    EXPECT_NO_THROW(pool.stop());
}

TEST(ThreadPoolTest, DestructorWithoutStop) {
    {
        ThreadPool pool(2);
        pool.start();
        pool.submit([]() { 
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        });
    }
    SUCCEED(); 
}

TEST(ThreadPoolTest, SubmitBeforeStart) {
    ThreadPool pool(2);
    EXPECT_THROW(pool.submit([]() {}), std::runtime_error);
}

TEST(ThreadPoolTest, TaskWithParameters) {
    ThreadPool pool(2);
    pool.start();
    
    auto f = pool.submit([](int a, int b) { 
        return a + b; 
    }, 5, 7); 
    
    EXPECT_EQ(f.get(), 12);
    pool.stop();
}
