#pragma once

#include <concepts>

namespace com
{
	template<typename T>
	concept cHandleType = std::constructible_from<T> && std::assignable_from<T&, T> && std::equality_comparable<T>;

	template<cHandleType T, T NullValue, typename... U>
	class Reference
	{
	public:
		static constexpr T Null = NullValue;
		typedef T HandleType;
	protected:
		T m_handle;
	public:
		constexpr Reference() noexcept : m_handle(Null) { }
		constexpr Reference(T handle) noexcept : m_handle(handle) { }
		constexpr Reference(const Reference& reference) noexcept : m_handle(reference.m_handle) { }

		constexpr Reference& operator =(const Reference& reference) noexcept
		{
			m_handle = reference.m_handle;
			return *this;
		}

		constexpr bool operator ==(const Reference reference) const noexcept
		{
			return m_handle == reference.m_handle;
		}

		constexpr bool operator !=(const Reference reference) const noexcept
		{
			return !operator==(reference);
		}

		constexpr const T& getHandle() const noexcept { return m_handle; }


		constexpr explicit operator T&() noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		constexpr explicit operator const T&() const noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		constexpr explicit operator T() const noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		
		constexpr operator bool() const noexcept { return m_handle != Null; }

	};
}
