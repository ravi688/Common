#pragma once

#include <concepts>

namespace com
{
	// The cHandleType can't be 'bool'
	template<typename T>
	concept cHandleType = std::constructible_from<T> && std::assignable_from<T&, T> && std::equality_comparable<T> && !std::same_as<T, bool>;

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

		constexpr Reference& operator=(const T& handle) noexcept
		{
			m_handle = handle;
			return *this;
		}

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


		// Enable the following set of 3 (explicit) conversion operator overloads only if handle type is of pointer type
		// That's because implicit conversion of pointer to bool is possible and automatic in C++, especially in 'if' evaluation context
		// Which also leads to ambiguous resolution with 'operator bool()''
		// Therefore, we are bette defining these functions as 'explicit'
		// Otherwise, disable them.
		constexpr explicit operator T&() noexcept requires(std::is_pointer<T>::value) { return m_handle; }
		constexpr explicit operator const T&() const noexcept requires(std::is_pointer<T>::value) { return m_handle; }
		constexpr explicit operator T() const noexcept requires(std::is_pointer<T>::value) { return m_handle; }

		// Enable the following set of 3 (implicit) conversion operator overloads only if handle type is not a pointer type
		// That's because bool is automatically and implicitly converted to integer type in C++
		// Therefore, if the destination type is an integral, so the handle type, and if no conversion operator found which convert
		// to the integer type, C++ chooses the 'operator bool()' overload. Which leads to unexpected behaviour.
		// Therefore, we define implicit conversion operators to the destination type so that C++ would priorities them first in the resolution.
		constexpr operator T&() noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		constexpr operator const T&() const noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		constexpr operator T() const noexcept requires(!std::is_pointer<T>::value) { return m_handle; }
		
		// In non-old compilers, we can use 'explicit' keyword to only allow implicit evaluation to 'bool' in if context only.
		constexpr explicit operator bool() const noexcept { return m_handle != Null; }

	};
}
