#pragma once

/* compiler identification */
#if defined(__MINGW32__)
#	define COMPILER_MINGW32	 		// MING32
#elif defined(__MINGW64__)
#	define COMPILER_MINGW64 			// MINGW64
#elif defined(__clang__)
#	define COMPILER_CLANG 				// CLANG
#elif defined(__GNUC__)
#	define COMPILER_GCC				// GCC
#elif defined(_MSC_VER)
#	define COMPILER_MSVC 				// MSVC
#endif

#if defined(COMPILER_MINGW64) || defined(COMPILER_MINGW32)
#	define COMPILER_MINGW
#endif // MINGW

#if !defined(__GNUC__)
#	error "GNU compiler is expected"
#endif /* __GNUC__ */

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#	define PLATFORM_WINDOWS
#	if defined(_WIN32) || defined(__MINGW32__)
#		define PLATFORM_WINDOWS_32_BIT
#	else
#		define PLATFORM_WINDOWS_64_BIT
#	endif
#elif defined(__linux__) || defined(__linux)
#	define PLATFORM_LINUX
#	if defined(__x86_64__) || defined(__ppc64__)
#		define PLATFORM_LINUX_64_BIT
#	else 
#		define PLATFORM_LINUX_32_BIT
#	endif
#elif defined (__APPLE__) || defined(__MACH__)
#	define PLATFORM_APPLE
#endif
