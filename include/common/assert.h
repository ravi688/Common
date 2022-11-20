
#pragma once

#include <common/debug.h>

#define ASSERT(...) debug_assert(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define ASSERT_WRN(...) debug_assert_wrn(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define ASSERT_CALLED_ONCE(...) static int __FUNCTION__##count = 0; __FUNCTION__##count++; ASSERT(DESCRIPTION(__FUNCTION__##count < 2), "The Function \"%s\" has been called more than once", __FUNCTION__)
#define ASSERT_NOT_IMPLEMENTED(...) ASSERT(DESCRIPTION(false), "Function \"%s\" isn't implemented yet but you're still trying to call it", __FUNCTION__)


#define DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))

#define _ASSERT(assertion) ASSERT(DESCRIPTION(assertion), #assertion)
#define _ASSERT_WRN(assertion) ASSERT_WRN(DESCRIPTION(assertion), #assertion)

#ifdef GLOBAL_DEBUG
#	define assert(...) ASSERT(__VA_ARGS__)
#	define assert_wrn(...) ASSERT_WRN(__VA_ARGS__)
#	define assert_called_once(...) ASSERT_CALLED_ONCE(__VA_ARGS__)
#	define assert_not_implemented(...) ASSERT_NOT_IMPLEMENTED(__VA_ARGS__)
#	define _assert(...) _ASSERT(__VA_ARGS__)
# 	define _assert_wrn(...) _ASSERT_WRN(__VA_ARGS__)
#else
#	define assert(...)
#	define assert_wrn(...)
#	define assert_called_once(...)
#	define assert_not_implemented(...)
#	define _assert(...)
#	define _assert_wrn(...)
#endif
