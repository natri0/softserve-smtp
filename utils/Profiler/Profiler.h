#pragma once

// In the future this file should be included to precompiled headers

#include <chrono>
#include <string>
#include <thread>
#include <fstream>

namespace utils
{

class SpinLock {
public:
	SpinLock() {}

	void lock() {
		retries = 0;
		while (flag.test_and_set(std::memory_order_acquire)) {
			// spin until the lock is released
			backoff();
			retries++;
		}
	}

	void unlock() {
		flag.clear(std::memory_order_release);
	}

private:
	void backoff() {
		const int max_retries = 8;
		if (retries < max_retries) {
			std::this_thread::yield();
		}
		else {
			auto delay = std::chrono::microseconds(1 << (retries - max_retries));
			std::this_thread::sleep_for(delay);
		}
	}

	std::atomic_flag flag = ATOMIC_FLAG_INIT;
	int retries{ 0 };
};

struct ProfileResult
{
	std::string name;
	std::thread::id thread_id;
	long long start = 0;
	long long end = 0;
};

class Profiler
{
private:
	std::string m_sessionName;
	std::ofstream m_output;
	bool m_firstEntry;

	SpinLock lock;
	
public:
	Profiler();

	static Profiler& GetInstance();

	void Begin(const std::string& session_name, const std::string& filepath = "profiler_output.json");

	void End();

	void WriteProfile(const ProfileResult& result);
};

class ProfileTimer
{
private:
	std::string m_name;
	bool m_stopped;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

public:
	ProfileTimer(const std::string& name);

	~ProfileTimer();

	void Stop();

};

}

