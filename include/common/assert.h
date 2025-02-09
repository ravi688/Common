
#pragma once

#include <common/debug.h>

#define ASSERT(...) debug_assert(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define ASSERT_WRN(...) debug_assert_wrn(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define ASSERT_CALLED_ONCE(...) static int __FUNCTION__##count = 0; __FUNCTION__##count++; ASSERT(DESCRIPTION(__FUNCTION__##count < 2), "The Function \"%s\" has been called more than once", __FUNCTION__)
#define ASSERT_NOT_IMPLEMENTED(...) ASSERT(DESCRIPTION(false), "Function \"%s\" isn't implemented yet but you're still trying to call it", __FUNCTION__)

#define COM_ASSERT(...) ASSERT(__VA_ARGS__)
#define COM_ASSERT_WRN(...) ASSERT_WRN(__VA_ARGS__)
#define COM_ASSERT_CALLED_ONCE(...) ASSERT_CALLED_ONCE(__VA_ARGS__)
#define COM_ASSERT_NOT_IMPLEMENTED(...) ASSERT_NOT_IMPLEMENTED(__VA_ARGS__)

#ifdef __cplusplus
#	define DESCRIPTION(bool_value) (CAST_TO(u64, static_cast<bool>(bool_value)) | (1ULL << 16))
#else
#	define DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))
#endif

#define COM_DESCRIPTION(...) DESCRIPTION(__VA_ARGS__)

#define _ASSERT(assertion) ASSERT(DESCRIPTION(assertion), #assertion)
#define _ASSERT_WRN(assertion) ASSERT_WRN(DESCRIPTION(assertion), #assertion)

#ifdef assert
#	undef assert
#endif 

#ifdef assert_wrn
#	undef assert_wrn
#endif

#ifdef assert_called_once
#	undef assert_called_once
#endif

#ifdef assert_not_implemented
#	undef assert_not_implemented
#endif

#ifdef _assert
#	undef _assert
#endif

#ifdef _assert_wrn
#	undef _assert_wrn
#endif

#define _COM_ASSERT(...) _ASSERT(__VA_ARGS__)
#define _COM_ASSERT_WRN(...) ASSERT_WRN(__VA_ARGS__)

#ifdef COMMON_DEBUG
#	define com_assert(...) ASSERT(__VA_ARGS__)
#	define com_assert_wrn(...) ASSERT_WRN(__VA_ARGS__)
#	define com_assert_called_once(...) ASSERT_CALLED_ONCE(__VA_ARGS__)
#	define com_assert_not_implemented(...) ASSERT_NOT_IMPLEMENTED(__VA_ARGS__)
#	define _com_assert(...) _ASSERT(__VA_ARGS__)
# 	define _com_assert_wrn(...) _ASSERT_WRN(__VA_ARGS__)
#else
#	define com_assert(...)
#	define com_assert_wrn(...)
#	define com_assert_called_once(...)
#	define com_assert_not_implemented(...)
#	define _com_assert(...)
#	define _com_assert_wrn(...)
#endif


#define assert(...) com_assert(__VA_ARGS__)
#define assert_wrn(...) com_assert_wrn(__VA_ARGS__)
#define assert_called_once(...) com_assert_called_once(__VA_ARGS__)
#define assert_not_implemented(...) com_assert_not_implemented(__VA_ARGS__)
#define _assert(...) _com_assert(__VA_ARGS__)
#define _assert_wrn(...) _com_assert_wrn(__VA_ARGS__)
