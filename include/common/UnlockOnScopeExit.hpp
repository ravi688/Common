#pragma once

#include <common/Concepts.hpp> // for com::concepts::Lockable<>

namespace com
{
	template<com::concepts::Lockable T>
	class UnlockOnScopeExit
	{
	private:
		T& m_value;
	
	public:
		UnlockOnScopeExit(T& value, bool acquireLock = true) : m_value(value)
		{
			if(acquireLock)
				m_value.lock();
		}
	
		~UnlockOnScopeExit()
		{
			m_value.unlock();
		}
		
		T& value() noexcept { return m_value; }
		T& operator*() noexcept { return m_value; }
		T* operator->() noexcept { return &m_value; }
	};

	template<com::concepts::Lockable T>
	UnlockOnScopeExit<T> GetLocked(T& t)
	{
		return { t };
	}
}
