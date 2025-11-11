#pragma once

namespace utils
{

class SpinLock {
private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

public:
	SpinLock() {}

	void lock() {
		while (m_flag.test_and_set(std::memory_order_acquire)) {
			// spin until the lock is released
		}
	}

	void unlock() {
		m_flag.clear(std::memory_order_release);
	}

};

}
