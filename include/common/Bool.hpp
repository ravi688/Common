#pragma once

#include <common/defines.h> // for u8, WARNING: Do not include common/defines.hpp here because that header file itself depends on this header file.
#include <ostream> // for std::ostream

namespace com
{
	class Bool
	{
	public:
		static constexpr Bool False() noexcept { Bool _bool; _bool.m_value = 0u; return _bool; }
		static constexpr Bool True() noexcept { Bool _bool; _bool.m_value = 1u; return _bool; }
	private:
		u8 m_value;
	public:
		constexpr Bool() noexcept : m_value(0) { }
		constexpr Bool(const Bool& _bool) noexcept : m_value(_bool.m_value) { }
		constexpr explicit Bool(bool value) noexcept : m_value(value ? 1u : 0u) { }

		constexpr Bool& operator=(Bool rhs) noexcept { m_value = rhs.m_value; return *this; }
		constexpr Bool operator&&(const Bool& rhs) const noexcept
		{
			return Bool(static_cast<bool>(*this) && static_cast<bool>(rhs));
		}
		constexpr Bool operator||(const Bool& rhs) const noexcept
		{
			return Bool(static_cast<bool>(*this) || static_cast<bool>(rhs));
		}
		constexpr Bool operator==(const Bool& rhs) const noexcept
		{
			return Bool(static_cast<bool>(*this) == static_cast<bool>(rhs));
		}
		constexpr Bool operator!=(const Bool& rhs) const noexcept
		{
			return Bool(static_cast<bool>(*this) != static_cast<bool>(rhs));
		}
		constexpr Bool operator^(const Bool& rhs) const noexcept
		{
			return Bool(static_cast<bool>(*this) ^ static_cast<bool>(rhs));
		}

		constexpr Bool operator!() const noexcept
		{
			return Bool(!static_cast<bool>(*this));
		}

		// Only allow automatic conversion in if(<Bool>) context
		constexpr explicit operator bool() const noexcept { return (m_value == 0u) ? false : true; }
	};

	std::ostream& operator <<(std::ostream& stream, com::Bool _bool) noexcept;

	static constexpr Bool False = Bool::False();
	static constexpr Bool True = Bool::True();
}
