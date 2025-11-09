#ifndef LOGQUEUE_H
#define LOGQUEUE_H

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <optional>

template<typename T>
class LogQueue
{
public:

	void Push(T value)
	{
		assert(!is_closed);
		std::lock_guard guard(mutex);
		buffer.push(std::move(value));
		is_not_empty.notify_one();
	}


	std::optional<T> Pop()
	{
		std::unique_lock lock(mutex);
		while (buffer.empty())
		{
			if (is_closed) return std::nullopt;
			is_not_empty.wait(lock);
		}
		return PopLocked();
	}

	void Close()
	{
		is_closed.store(true);
		is_not_empty.notify_all();
	}

	std::queue<T> Extract()
	{
		std::lock_guard guard(mutex);
		std::queue<T> extracted = std::move(buffer);
		buffer = {};
		return std::move(extracted);
	}

	bool Empty() const {
		std::lock_guard lock(mutex);
		return buffer.empty();
	}


private:

	T PopLocked()
	{
		assert(!buffer.empty());
		T value{ std::move(buffer.front()) };
		buffer.pop();
		return value;
	}

	std::queue<T> buffer;
	mutable std::mutex mutex;
	std::condition_variable is_not_empty;
	std::atomic<bool> is_closed{ false };
};

#endif
