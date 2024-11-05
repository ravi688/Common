#include <common/Bool.hpp>

namespace com
{
	std::ostream& operator <<(std::ostream& stream, com::Bool _bool) noexcept
	{
		stream << std::boolalpha << static_cast<bool>(_bool);
		return stream;
	}
}
