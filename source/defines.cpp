#include <common/defines.hpp>

namespace com
{
	std::filesystem::path trim_front(const std::filesystem::path& path) noexcept
	{
	    if(path.begin() == path.end())
	        return { };
	    auto it = path.begin();
	    std::size_t len = it->generic_string().length();
	    len += 1;
	    auto str = path.generic_string();
	    str.erase(0, len);
	    return { str };
	}
}