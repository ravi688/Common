#pragma once

#include <concepts>

namespace com
{
	template<typename T>
	concept HandleType = std::constructible_from<T> && std::assignable_from<T&, T> && std::equality_comparable<T>;

	template<HandleType T, T NullValue, typename... U>
	class Reference
	{
	public:
		static constexpr T Null = NullValue;
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

		constexpr operator T&() noexcept { return m_handle; }
		constexpr operator const T&() const noexcept { return m_handle; }
		constexpr operator T() const noexcept { return m_handle; }
		constexpr operator bool() const noexcept { return m_handle != Null; }

	};
}
