
#pragma once

#include <common/debug.h>

#define COM_ASSERT(...) com_debug_assert(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define COM_ASSERT_WRN(...) com_debug_assert_wrn(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define COM_ASSERT_CALLED_ONCE(...) static int __FUNCTION__##count = 0; __FUNCTION__##count++; COM_ASSERT(COM_DESCRIPTION(__FUNCTION__##count < 2), "The Function \"%s\" has been called more than once", __FUNCTION__)
#define COM_ASSERT_NOT_IMPLEMENTED(...) COM_ASSERT(DESCRIPTION(false), "Function \"%s\" isn't implemented yet but you're still trying to call it", __FUNCTION__)

#ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define ASSERT(...) COM_ASSERT(__VA_ARGS__)
#	define ASSERT_WRN(...) COM_ASSERT_WRN(__VA_ARGS__)
#	define ASSERT_CALLED_ONCE(...) COM_ASSERT_CALLED_ONCE(__VA_ARGS__)
#	define ASSERT_NOT_IMPLEMENTED(...) COM_ASSERT_NOT_IMPLEMENTED(__VA_ARGS__)
#endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM

#ifdef __cplusplus
#	define COM_DESCRIPTION(bool_value) (CAST_TO(u64, static_cast<bool>(bool_value)) | (1ULL << 16))
#else
#	define COM_DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))
#endif

#define _COM_ASSERT(assertion) COM_ASSERT(COM_DESCRIPTION(assertion), #assertion)
#define _COM_ASSERT_WRN(assertion) COM_ASSERT_WRN(COM_DESCRIPTION(assertion), #assertion)

#ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define DESCRIPTION(...) COM_DESCRIPTION(__VA_ARGS__)
#	define _ASSERT(assertion) _COM_ASSERT(assertion)
#	define _ASSERT_WRN(assertion) _COM_ASSERT_WRN(assertion)
#endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM

#ifdef COMMON_DEBUG
#	define com_assert(...) COM_ASSERT(__VA_ARGS__)
#	define com_assert_wrn(...) COM_ASSERT_WRN(__VA_ARGS__)
#	define com_assert_called_once(...) COM_ASSERT_CALLED_ONCE(__VA_ARGS__)
#	define com_assert_not_implemented(...) COM_ASSERT_NOT_IMPLEMENTED(__VA_ARGS__)
#	define _com_assert(...) _COM_ASSERT(__VA_ARGS__)
# 	define _com_assert_wrn(...) _COM_ASSERT_WRN(__VA_ARGS__)
#else
#	define com_assert(...)
#	define com_assert_wrn(...)
#	define com_assert_called_once(...)
#	define com_assert_not_implemented(...)
#	define _com_assert(...)
#	define _com_assert_wrn(...)
#endif

#ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define assert(...) com_assert(__VA_ARGS__)
#	define assert_wrn(...) com_assert_wrn(__VA_ARGS__)
#	define assert_called_once(...) com_assert_called_once(__VA_ARGS__)
#	define assert_not_implemented(...) com_assert_not_implemented(__VA_ARGS__)
#	define _assert(...) _com_assert(__VA_ARGS__)
#	define _assert_wrn(...) _com_assert_wrn(__VA_ARGS__)
#endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
