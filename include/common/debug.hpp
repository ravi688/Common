#pragma once

#include <common/defines.hpp>
#include <common/debug.h>

#include <ostream> // for std::ostream
#include <source_location> // for std::source_location

namespace com
{
	std::ostream& operator<<(std::ostream& stream, const std::source_location& location) noexcept;

	class source_loc
	{
		friend std::ostream& operator<<(std::ostream& stream, source_loc loc) noexcept;
	private:
		std::source_location m_loc;
	public:
		constexpr source_loc(std::source_location loc = std::source_location::current()) : m_loc(loc) { }
	};

	std::ostream& operator<<(std::ostream& stream, source_loc value) noexcept;

	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE  std::ostream& cout(const std::source_location& location = std::source_location::current()) noexcept
	{
		return operator<<(std::cout, location);
	}
}
