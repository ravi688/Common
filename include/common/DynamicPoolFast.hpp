#pragma once

#include <common/DynamicPool.hpp>

namespace com
{
	template<typename T>
	class DynamicPoolFast;

	template<typename T>
	class DynamicPoolFastElement
	{
		friend class DynamicPoolFast<T>;
	private:
		T m_value;
		std::size_t m_index;
		std::size_t getIndex() const noexcept { return m_index; }
		T& getValue() { return m_value; }
	public:
		DynamicPoolFastElement(T value, std::size_t index) : m_value(std::move(value)), m_index(index) { }

		DynamicPoolFastElement(DynamicPoolFastElement&& el) : m_value(std::move(el.m_value)), m_index(el.m_index) { }
		DynamicPoolFastElement(const DynamicPoolFastElement& el) : m_value(el.m_value), m_index(el.m_index) { }

		DynamicPoolFastElement& operator=(DynamicPoolFastElement&& el)
		{
			m_value = std::move(el.m_value);
			m_index = el.m_index;
			return *this;
		}

		DynamicPoolFastElement& operator=(const DynamicPoolFastElement& el)
		{
			m_value = el.m_value;
			m_index = el.m_index;
			return *this;
		}

		bool operator==(const DynamicPoolFastElement& rhs) const
		{
			return m_index == rhs.m_index;
		}

		T& operator*() { return m_value; }
		const T& operator*() const { return m_value; }
	};

	template<typename T>
	class DynamicPoolFast : public DynamicPool<DynamicPoolFastElement<T>>
	{
	public:
		using ElementType = DynamicPoolFastElement<T>;
		using OnCreate = std::function<T()>;
		using OnDestroy = std::function<void(T&)>;
		using OnReturn = std::function<void(T&)>;
		using OnRecycle = std::function<void(T&)>;
	private:
		std::size_t m_counter;
		OnCreate m_onCreate;
		OnDestroy m_onDestroy;
		OnReturn m_onReturn;
		OnRecycle m_onRecycle;

	public:
		DynamicPoolFast(OnCreate onCreate, OnDestroy onDestroy = [](T&) { },
											OnReturn onReturn = [](T&) { },
											OnRecycle onRecycle = [](T&) { }) : m_counter(0),
																				m_onCreate(onCreate),
																				m_onDestroy(onDestroy),
																				m_onReturn(onReturn),
																				m_onRecycle(onRecycle)
		{
			DynamicPool<DynamicPoolFastElement<T>>::setOnCreate([this]()
			{
				// NOTE: we can't do std::move(m_onCreate()) as move on temporary objects prevents copy elision
				return com::DynamicPoolFastElement<T> { m_onCreate(), m_counter++ };
			});
			DynamicPool<DynamicPoolFastElement<T>>::setOnDestroy([this](DynamicPoolFastElement<T>& el)
			{
				m_onDestroy(el.getValue());
			});
			DynamicPool<DynamicPoolFastElement<T>>::setOnReturn([this](DynamicPoolFastElement<T>& el)
			{
				m_onReturn(el.getValue());
			});
			DynamicPool<DynamicPoolFastElement<T>>::setOnRecycle([this](DynamicPoolFastElement<T>& el)
			{
				m_onRecycle(el.getValue());
			});
		}

		// Constant time complexity
		void putFast(DynamicPoolFastElement<T> el)
		{
			DynamicPool<DynamicPoolFastElement<T>>::put_(std::move(el), [this](const DynamicPoolFastElement<T>& el)
				{
					return std::next(DynamicPool<DynamicPoolFastElement<T>>::getStorage().begin(), el.getIndex());
				});
		}
	};
}