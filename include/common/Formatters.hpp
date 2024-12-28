#include <source_location> // for std::source_location
#include <ostream> // for std::ostream

namespace com::formatters::source_location
{
	std::ostream& operator<<(std::ostream& stream, std::source_location loc) noexcept;
}
