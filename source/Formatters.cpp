#include <common/Formatters.hpp>

namespace com::formatters::source_location
{
	std::ostream& operator<<(std::ostream& stream, std::source_location loc) noexcept
	{
 		stream 	<< loc.file_name() << '('
            		<< loc.line() << ':'
            		<< loc.column() << ") `"
            		<< loc.function_name();
        return stream;
	}
}
