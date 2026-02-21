#pragma once

#include <common/defines.hpp> // for COMMON_API etc.

#include <limits> // for std::numeric_limits<>
#include <mutex> // for std::mutex
#include <condition_variable> // for std::condition_variable
#include <deque> // for std::deque

namespace com
{
	// Thread-safe producer consumer buffer
	template<typename T>
	class COMMON_API ProducerConsumerBuffer
	{
	private:
		std::deque<T> m_storage;
		std::mutex m_mutex;
		std::condition_variable m_pushCV;
		std::condition_variable m_popCV;
		std::size_t m_maxCount;

		// Transfer-of-lock-ownership
		std::unique_lock<std::mutex> acquirePushLock()
		{
			std::unique_lock<std::mutex> ulock(m_mutex);
			m_pushCV.wait(ulock, [this]() { return m_storage.size() < m_maxCount; });
			return ulock;
		}

		void releasePushLock(std::unique_lock<std::mutex> lock)
		{
			lock.unlock();
			m_popCV.notify_all();
		}

	public:
		// The internal buffer will expand unboundedly by default
		ProducerConsumerBuffer(std::size_t maxCount = std::numeric_limits<std::size_t>::max()) : m_maxCount(maxCount) { }

		void push(T&& value)
		{
			auto lock = acquirePushLock();
			m_storage.push_back(std::move(value));
			releasePushLock(std::move(lock));
		}

		void push(const T& value)
		{
			auto lock = acquirePushLock();
			m_storage.push_back(value);
			releasePushLock(std::move(lock));
		}

		T pop()
		{
			std::unique_lock<std::mutex> ulock(m_mutex);
			m_popCV.wait(ulock, [this]() { return m_storage.size() > 0; });
			auto p = std::move(m_storage.front());
			m_storage.pop_front();
			m_pushCV.notify_all();
			return p;
		}
	};
}