#include <common/debug.hpp>

namespace com
{
	std::ostream& operator<<(std::ostream& stream, const std::source_location& location) noexcept
	{
		 stream
			<< location.file_name() << '('
			<< location.line() << ':'
			<< location.column() << ") `"
			<< location.function_name() << "` ";
		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, source_loc value) noexcept
	{
		stream << value.m_loc;
		return stream;
	}
}