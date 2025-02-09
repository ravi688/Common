#pragma once

#if (defined _WIN32 || defined __CYGWIN__) && defined(__GNUC__)
#	define COMMON_IMPORT_API __declspec(dllimport)
#	define COMMON_EXPORT_API __declspec(dllexport)
#else
#	define COMMON_IMPORT_API __attribute__((visibility("default")))
#	define COMMON_EXPORT_API __attribute__((visibility("default")))
#endif

#ifdef COMMON_BUILD_STATIC_LIBRARY
#	define COMMON_API
#elif defined(COMMON_BUILD_DYNAMIC_LIBRARY)
#	define COMMON_API COMMON_EXPORT_API
#elif defined(COMMON_USE_DYNAMIC_LIBRARY)
#	define COMMON_API COMMON_IMPORT_API
#elif defined(COMMON_USE_STATIC_LIBRARY)
#	define COMMON_API
#else
#	define COMMON_API
#endif
