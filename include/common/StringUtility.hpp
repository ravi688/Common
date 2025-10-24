#pragma once

#include <string>
#include <string_view>

namespace com
{
	template <typename... Args>
	std::string string_join(Args&&... args)
	{
    	std::size_t size = (std::string_view(args).size() + ... + 0);
    	std::string result;
    	result.reserve(size);
    	(result.append(args), ...);
    	return result;
	}
}
