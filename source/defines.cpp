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

	std::filesystem::path trim_back(const std::filesystem::path& path) noexcept
	{
    	if(path.empty())
    	    return path;
    	auto it = path.end();
    	it--;
    	auto len = it->generic_string().length();
    	len += 1; // path separator
    	std::string str = path.generic_string();
    	str.erase(str.length() - len, len);
    	return { str };
	}
}