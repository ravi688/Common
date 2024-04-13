#pragma once

#if !defined(__GNUC__)
#	error "GNU compiler is expected"
#endif /* __GNUC__ */

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#	define PLATFORM_WINDOWS
#	if defined(_WIN32) || defined(__MINGW32__)
#		define PLATFORM_WINDOWS_32_BIT
#	else
#		define PLATFORM_WINDOWS_64_BIT
#	endif
#elif defined(__linux__)
#	define PLATFORM_LINUX
#	if defined(__x86_64__) || defined(__ppc64__)
#		define PLATFORM_LINUX_64_BIT
#	else 
#		define PLATFORM_LINUX_32_BIT
#	endif
#elif defined (__APPLE__) || defined(__MACH__)
#	define PLATFORM_APPLE
#endif
