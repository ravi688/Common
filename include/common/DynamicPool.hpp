#pragma once

#include <common/defines.hpp>
#include <common/assert.h> // for _com_assert
#include <common/debug.h> // for debug_log_error()

#include <functional> // for std::function
#include <utility> // for std::swap
#include <vector> // for std::vector
#include <algorithm> // for std::find

namespace com
{
	template<typename T>
	class DynamicPool
	{
	public:
		typedef std::function<T()> OnCreate;
		typedef std::function<void(T&)> OnDestroy;
		typedef std::function<void(T&)> OnReturn;
		typedef std::function<void(T&)> OnRecycle;
		typedef std::function<typename std::vector<T>::iterator(const T&)> FindValueCallback;

	private:
		std::vector<T> m_storage;
		std::size_t m_activeCount;
		OnCreate m_onCreate;
		OnDestroy m_onDestroy;
		OnReturn m_onReturn;
		OnRecycle m_onRecycle;

		typename std::vector<T>::iterator getLastActive() noexcept;

	protected:
		void put_(T value, FindValueCallback findValueCallback) noexcept;

		std::vector<T>& getStorage() { return m_storage; }

		void setOnCreate(OnCreate onCreate) { m_onCreate = onCreate; }
		void setOnDestroy(OnDestroy onDestroy) { m_onDestroy = onDestroy; }
		void setOnReturn(OnReturn onReturn) { m_onReturn = onReturn; }
		void setOnRecycle(OnRecycle onRecycle) { m_onRecycle = onRecycle; }

		DynamicPool(OnDestroy onDestroy = [](T&) { }, OnReturn onReturn = [](T&) { }, OnRecycle onRecycle = [](T&) { }) noexcept;

	public:
		DynamicPool(OnCreate onCreate, OnDestroy onDestroy = [](T&) { }, 
										OnReturn onReturn = [](T&) { }, 
										OnRecycle onRecycle = [](T&) { }, 
										bool isReturn = false, 
										std::size_t initialCount = 0) noexcept;
		DynamicPool(DynamicPool&& pool) noexcept;
		~DynamicPool() noexcept;

		DynamicPool& operator =(DynamicPool&& pool) noexcept;

		T get() noexcept;
		void put(T value) noexcept;
		// Puts all the active objects back into the pool
		// NOTE: this operation doesn't destroy the objects, it just cals OnReturn on every object
		void reclaim() noexcept;
		// Destroys all the objects
		// NOTE: it only calls OnDestroy on every object
		void clear() noexcept;
		void reserve(std::size_t count, bool isReturn = true) noexcept;

		std::size_t activeCount() const noexcept { return m_activeCount; }
		std::size_t size() noexcept { return m_storage.size(); }

		typename std::vector<T>::iterator begin() noexcept { return m_storage.begin(); }
		typename std::vector<T>::iterator end() noexcept { return m_storage.end(); }

		Iteratable<typename std::vector<T>::iterator> getActives() noexcept
		{
			auto begin = m_storage.begin();
			return { begin, std::next(begin, m_activeCount) };
		}
	};

	template<typename T>
	DynamicPool<T>::DynamicPool(OnDestroy onDestroy, OnReturn onReturn, OnRecycle onRecycle) noexcept : m_activeCount(0),
																										m_onDestroy(onDestroy),
																										m_onReturn(onReturn),
																										m_onRecycle(onRecycle) { }


	template<typename T>
	DynamicPool<T>::DynamicPool(OnCreate onCreate, OnDestroy onDestroy, OnReturn onReturn, OnRecycle onRecycle, bool isReturn, std::size_t initialCount) noexcept : 
																																				m_activeCount(0),
																																				m_onCreate(onCreate),
																																				m_onDestroy(onDestroy),
																																				m_onReturn(onReturn),
																																				m_onRecycle(onRecycle)
	{
		m_storage.reserve(initialCount);
		for(std::size_t i = 0; i < initialCount; ++i)
		{
			m_storage.push_back(onCreate());
			if(isReturn)
				m_onReturn(m_storage[i]);
		}
	}

	template<typename T>
	DynamicPool<T>::DynamicPool(DynamicPool<T>&& pool) noexcept : m_storage(std::move(pool.m_storage)),
																m_activeCount(pool.m_activeCount),
																m_onCreate(std::move(m_onCreate)),
																m_onDestroy(std::move(m_onDestroy)),
																m_onReturn(std::move(m_onReturn)),
																m_onRecycle(std::move(m_onRecycle))
	{

	}

	template<typename T>
	DynamicPool<T>::~DynamicPool() noexcept
	{
		clear();
	}

	template<typename T>
	DynamicPool<T>& DynamicPool<T>::operator=(DynamicPool<T>&& pool) noexcept
	{
		m_storage = std::move(pool);
		m_activeCount = pool.m_activeCount;
		m_onCreate = pool.m_onCreate;
		m_onDestroy = pool.m_onDestroy;
		m_onReturn = pool.m_onReturn;
		m_onRecycle = pool.m_onRecycle;
		return *this;
	}

	template<typename T>
	T DynamicPool<T>::get() noexcept
	{
		_com_assert(m_activeCount <= m_storage.size());

		// If no more inactive objects left, then create new
		if(m_activeCount == m_storage.size())
			m_storage.push_back(m_onCreate());
		// Otherwise recycle the existing one
		else
			m_onRecycle(m_storage[m_activeCount]);
		// And return
		return m_storage[m_activeCount++];
	}

	template<typename T>
	typename std::vector<T>::iterator DynamicPool<T>::getLastActive() noexcept
	{
		_com_assert(m_activeCount > 0);
		return std::next(m_storage.begin(), m_activeCount - 1);
	}

	template<typename T>
	void DynamicPool<T>::put(T value) noexcept
	{
		put_(value, [this](const T& value)
			{
				auto lastActive = getLastActive();
				return std::find(m_storage.begin(), lastActive, value);;
			});
	}

	template<typename T>
	void DynamicPool<T>::put_(T value, FindValueCallback findValueCallback) noexcept
	{
		_com_assert(m_activeCount > 0);

		// Check if such value was ever taken out of the pool
		auto lastActive = getLastActive();
		auto it = findValueCallback(value);
		if(it == m_storage.end())
		{
			com_debug_log_error("No such value ever gotten from the pool, but you're still trying to return/put back into it");
			return;
		}
		*it = std::move(value);

		// If yes then swap this value with the last active value
		std::swap(*it, *lastActive);
		m_onReturn(*lastActive);
		--m_activeCount;
	}

	template<typename T>
	void DynamicPool<T>::reclaim() noexcept
	{
		if(m_activeCount == 0)
			return;

		auto lastActiveEnd = std::next(getLastActive(), 1);
		for(auto it = m_storage.begin(); it != lastActiveEnd; it++)
			m_onReturn(*it);
		m_activeCount = 0;
	}

	template<typename T>
	void DynamicPool<T>::clear() noexcept
	{
		if(m_activeCount == 0)
			return;
		for(auto it = m_storage.begin(); it != m_storage.end(); it++)
			m_onDestroy(*it);
		m_storage.clear();
		m_activeCount = 0;
	}


	template<typename T>
	void DynamicPool<T>::reserve(std::size_t count, bool isReturn) noexcept
	{
		// For now, we don't know what should happen if poolSize shrink is requested.
		// I mean, right now we can't destroy SUTK::Container or SUTK::Renderable objects.
		// Ideally, this should call m_onDestroy for every object being discarded during the size
		// shrinking.
		_com_assert(count >= m_storage.size());

		auto diff = count - m_storage.size();
		for(std::size_t i = 0; i < diff; ++i)
		{
			m_storage.push_back(m_onCreate());
			if(isReturn)
				m_onReturn(m_storage.back());
		}
	}
}