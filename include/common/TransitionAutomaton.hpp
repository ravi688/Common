#include <common/defines.hpp> // for com::Lerp template function

#include <chrono> // for timing
#include <initializer_list> // for std::initializer_list
#include <unordered_map> // for std::unordered_map

namespace com
{
	template<typename T, typename FactorType = f32>
	struct GetLerpFuncProto
	{
		typedef T (*type)(T start, T end, FactorType t);
	};

	template<typename T, typename GetLerpFuncProto<T>::type LerpFunc = Lerp<T>>
	class TransitionAutomaton
	{
		std::unordered_map<u32, T> m_stateValue;

		bool m_isRunning;

		// in seconds
		f32 m_transitionDelay;
		T m_prevValue;
		// Color of the rect at any given point of time.
		T m_currentValue;
		// Final Color being chased during the course of transition.
		T m_destValue;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_prevTime;

	public:
		TransitionAutomaton(std::initializer_list<std::pair<const u32, T>> stateValuePairs) noexcept : m_stateValue(stateValuePairs),
																											m_isRunning(false),
																											m_transitionDelay(0.3f)
		{
			_com_assert(stateValuePairs.size() > 0);
			if(stateValuePairs.size() > 0)
			{
				m_currentValue = stateValuePairs.begin()->second;
				m_prevValue = m_currentValue;
				m_destValue = m_currentValue;
			}
		}
		bool isRunning() noexcept
		{
			return m_isRunning;
		}
		void update() noexcept
		{
			if(m_currentValue.equalsApprox(m_destValue))
			{
				m_currentValue = m_destValue;
				m_isRunning = false;
			}
			else
			{
				auto currTime = std::chrono::high_resolution_clock::now();
				f32 timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - m_prevTime).count() * 0.001f;
				m_currentValue = LerpFunc(m_prevValue, m_destValue, timeElapsed / m_transitionDelay);
			}
		}

		T getValue() const noexcept { return m_currentValue; }

		void set(u32 state, T color) noexcept
		{
			m_stateValue[state] = color;
		}

		void setTransitionDelay(f32 transitionDelay) noexcept { m_transitionDelay = transitionDelay; }
		void setDefault(u32 state) noexcept
		{
			auto it = m_stateValue.find(state);
			_com_assert(it != m_stateValue.end());
			m_currentValue = it->second;
			m_prevValue = m_currentValue;
		}
		void transitionTo(u32 state) noexcept
		{
			m_isRunning = true;
			auto it = m_stateValue.find(state);
			_com_assert(it != m_stateValue.end());
			m_destValue = it->second;
			m_prevValue = m_currentValue;
			m_prevTime = std::chrono::high_resolution_clock::now();
		}
	};
}
