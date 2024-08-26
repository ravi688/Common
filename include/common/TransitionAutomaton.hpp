#pragma once

#include <common/defines.hpp> // for com::Lerp template function

#include <chrono> // for timing
#include <initializer_list> // for std::initializer_list
#include <unordered_map> // for std::unordered_map
#include <memory> // for std::unique_ptr

#include <common/assert.h>
#include <common/Event.hpp> // for com::Event

namespace com
{
	template<typename T, typename FactorType = f32>
	struct GetLerpFuncProto
	{
		typedef T (*type)(T start, T end, FactorType t);
	};

	template<typename T>
	struct GetEqualsApproxFuncProto
	{
		typedef bool (*type)(T lhs, T rhs);
	};

	template<typename T>
	constexpr bool EqualsApprox(T lhs, T rhs) noexcept;

	template<>
	constexpr bool EqualsApprox<f32>(f32 lhs, f32 rhs) noexcept { return com::LiesBetweenInc(lhs, rhs - 0.00005f, rhs + 0.00005f); }
	template<>
	constexpr bool EqualsApprox<f64>(f64 lhs, f64 rhs) noexcept { return com::LiesBetweenInc(lhs, rhs - 0.000005, rhs + 0.000005); }

	template<typename T, 	typename GetEqualsApproxFuncProto<T>::type EqualsApproxFunc = EqualsApprox<T>, 
							typename GetLerpFuncProto<T>::type LerpFunc = Lerp<T>>
	class TransitionAutomaton
	{
	public:
		typedef u32 StateType;
		typedef com::Event<void> StateEvent;

	private:
		std::unordered_map<StateType, T> m_stateValue;
		std::unordered_map<StateType, std::unique_ptr<StateEvent>> m_stateEvents;

		bool m_isRunning;

		// in seconds
		f32 m_transitionDelay;
		T m_prevValue;
		// Color of the rect at any given point of time.
		T m_currentValue;
		// Final Color being chased during the course of transition.
		T m_destValue;
		StateType m_destState;
		StateType m_currentState;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_prevTime;

	public:
		TransitionAutomaton(std::initializer_list<std::pair<const StateType, T>> stateValuePairs) noexcept : m_stateValue(stateValuePairs),
																											m_isRunning(false),
																											m_transitionDelay(0.3f)
		{
			_com_assert(stateValuePairs.size() > 0);
			if(stateValuePairs.size() > 0)
			{
				m_currentValue = stateValuePairs.begin()->second;
				m_prevValue = m_currentValue;
				m_destValue = m_currentValue;
				m_currentState = stateValuePairs.begin()->first;
				m_destState = m_currentState;
			}
		}
		bool isRunning() const noexcept
		{
			return m_isRunning;
		}
		void update() noexcept
		{
			if(EqualsApproxFunc(m_currentValue, m_destValue))
			{
				m_currentValue = m_destValue;
				m_currentState = m_destState;
				m_isRunning = false;

				// check if the current state (just achieved) has any events
				auto it = m_stateEvents.find(m_currentState);
				if(it != m_stateEvents.end())
					// if yes, then publish it.
					it->second->publish();
			}
			else
			{
				auto currTime = std::chrono::high_resolution_clock::now();
				f32 timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - m_prevTime).count() * 0.001f;
				m_currentValue = LerpFunc(m_prevValue, m_destValue, timeElapsed / m_transitionDelay);
			}
		}

		T getValue() const noexcept { return m_currentValue; }
		StateType getState() const noexcept { return m_currentState; }

		void set(StateType state, T value) noexcept
		{
			m_stateValue[state] = value;
		}

		void setTransitionDelay(f32 transitionDelay) noexcept { m_transitionDelay = transitionDelay; }
		// This function supposed to be called just after instantiation of this class and only once.
		void setDefault(StateType state) noexcept
		{
			auto it = m_stateValue.find(state);
			_com_assert(it != m_stateValue.end());
			m_currentValue = it->second;
			m_prevValue = m_currentValue;
			m_currentState = state;
			m_destState = state;
		}
		void transitionTo(StateType state) noexcept
		{
			m_isRunning = true;
			auto it = m_stateValue.find(state);
			_com_assert(it != m_stateValue.end());
			m_destValue = it->second;
			m_prevValue = m_currentValue;
			m_prevTime = std::chrono::high_resolution_clock::now();
			m_destState = state;
		}

		StateEvent& getEvent(StateType state) noexcept
		{
			auto it = m_stateEvents.find(state);
			if(it == m_stateEvents.end())
			{
				StateEvent* event = new StateEvent();
				std::unique_ptr<StateEvent> uPtr(event);
				m_stateEvents.insert({ state, std::move(uPtr) });
				return *event;
			}
			return *(it->second);
		}
	};
}
