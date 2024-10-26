#pragma once

#include <common/defines.hpp> // for COMMON_API
#include <common/Concepts.hpp> // for com::Number

#include <chrono> // for timing

namespace com
{
	template<Number NumberType, class Period = std::milli>
	class TimeTaken
	{
	private:
		NumberType& m_elapsedTime;
		std::chrono::time_point<std::chrono::steady_clock> m_start;
	public:
		TimeTaken(NumberType& elapsedTime) noexcept : m_elapsedTime(elapsedTime), m_start(std::chrono::steady_clock::now()) { }
		~TimeTaken() noexcept
		{
			auto end = std::chrono::steady_clock::now();
			m_elapsedTime = std::chrono::duration_cast<std::chrono::duration<NumberType, Period>>(end - m_start).count();
		}
	};
}