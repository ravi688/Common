#include <common/ThreadNaming.hpp>

#if defined(__linux__)
    #include <pthread.h>
#elif defined(_WIN32)
    #include <windows.h>
#endif

namespace com
{
	COMMON_API void SetThreadName(const std::string& name)
	{
	#if defined(__linux__)
	    pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
	#elif defined(_WIN32)
	    std::wstring wname(name.begin(), name.end());
	    SetThreadDescription(GetCurrentThread(), wname.c_str());
	#endif
	}
}
