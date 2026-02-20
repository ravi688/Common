
#pragma once

#include <common/defines.h>

#ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define debug_assert_wrn com_debug_assert_wrn
#	define debug_assert com_debug_assert
#	define debug_logv com_debug_logv
#	define debug_log com_debug_log
#	define __debug_log_exit __com_debug_log_exit
#	define debug_log_break com_debug_log_break
#endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM

#ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define DEBUG_LOG_INFO(...) debug_log("[Info] ", __LINE__, __FUNCTION__, __FILE__,  __VA_ARGS__)
#	define DEBUG_LOG_WARNING(...) debug_log("[Warning] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#	define DEBUG_LOG_ERROR(...) debug_log("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#	define DEBUG_LOG_ERROR_EXIT(...) __debug_log_exit("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#	define DEBUG_LOG_FETAL_ERROR(...) debug_log_break("[Fetal Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#define COM_DEBUG_LOG_INFO(...) com_debug_log("[Info] ", __LINE__, __FUNCTION__, __FILE__,  __VA_ARGS__)
#define COM_DEBUG_LOG_WARNING(...) com_debug_log("[Warning] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define COM_DEBUG_LOG_ERROR(...) com_debug_log("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define COM_DEBUG_LOG_ERROR_EXIT(...) __com_debug_log_exit("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define COM_DEBUG_LOG_FETAL_ERROR(...) com_debug_log_break("[Fetal Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)

#ifdef COMMON_DEBUG
#	ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#		define debug_log_info(...) DEBUG_LOG_INFO(__VA_ARGS__)
#		define debug_log_warning(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#		define debug_log_error(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#   	define debug_log_exit(...) DEBUG_LOG_EXIT(__VA_ARGS__)
#		define debug_log_fetal_error(...) DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#	endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define com_debug_log_info(...) COM_DEBUG_LOG_INFO(__VA_ARGS__)
#	define com_debug_log_warning(...) COM_DEBUG_LOG_WARNING(__VA_ARGS__)
#	define com_debug_log_error(...) COM_DEBUG_LOG_ERROR(__VA_ARGS__)
#   define com_debug_log_exit(...) COM_DEBUG_LOG_EXIT(__VA_ARGS__)
#	define com_debug_log_fetal_error(...) COM_DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#else
#	ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#		define debug_log_info(...)
#		define debug_log_warning(...)
# 		define debug_log_error(...)
#   	define debug_log_exit(...)
#		define debug_log_fetal_error(...)
#	endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define com_debug_log_info(...)
#	define com_debug_log_warning(...)
# 	define com_debug_log_error(...)
#   define com_debug_log_exit(...)
#	define com_debug_log_fetal_error(...)
#endif

#ifdef COM_VERBOSE
#	ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#		define debug_log_info_verbose(...) debug_log_info(__VA_ARGS__)
#		define debug_log_warning_verbose(...) debug_log_warning(__VA_ARGS__)
#		define debug_log_error_verbose(...) debug_log_error(__VA_ARGS__)
#		define DEBUG_LOG_INFO_VERBOSE(...) DEBUG_LOG_INFO(__VA_ARGS__)
#		define DEBUG_LOG_WARNING_VERBOSE(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#		define DEBUG_LOG_ERROR_VERBOSE(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#	endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define com_debug_log_info_verbose(...) com_debug_log_info(__VA_ARGS__)
#	define com_debug_log_warning_verbose(...) com_debug_log_warning(__VA_ARGS__)
#	define com_debug_log_error_verbose(...) com_debug_log_error(__VA_ARGS__)
#	define COM_DEBUG_LOG_INFO_VERBOSE(...) COM_DEBUG_LOG_INFO(__VA_ARGS__)
#	define COM_DEBUG_LOG_WARNING_VERBOSE(...) COM_DEBUG_LOG_WARNING(__VA_ARGS__)
#	define COM_DEBUG_LOG_ERROR_VERBOSE(...) COM_DEBUG_LOG_ERROR(__VA_ARGS__)
#else
#	ifndef PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#		define debug_log_info_verbose(...)
#		define debug_log_warning_verbose(...)
#		define debug_log_error_verbose(...)
#		define DEBUG_LOG_INFO_VERBOSE(...)
#		define DEBUG_LOG_WARNING_VERBOSE(...)
#		define DEBUG_LOG_ERROR_VERBOSE(...)
#	endif // PREFIX_MACROS_AND_QUALIFY_SYMBOLS_WITH_COM
#	define com_debug_log_info_verbose(...)
#	define com_debug_log_warning_verbose(...)
#	define com_debug_log_error_verbose(...)
#	define COM_DEBUG_LOG_INFO_VERBOSE(...)
#	define COM_DEBUG_LOG_WARNING_VERBOSE(...)
#	define COM_DEBUG_LOG_ERROR_VERBOSE(...)
#endif


BEGIN_CPP_COMPATIBLE

#include <stdarg.h>

COMMON_API void com_debug_assert_wrn(u32 line, const char* function, const char* file, u64 assertion, ...);
COMMON_API void com_debug_assert(u32 line, const char* function, const char* file, u64 assertion, ...);
COMMON_API void com_debug_logv(const char* description, u32 line, const char* function, const char* file, const char* format, va_list args);
COMMON_API void com_debug_log(const char* description, u32 line, const char* function, const char* file, const char* format, ...);
COMMON_API NO_RETURN_FUNCTION void __com_debug_log_exit(const char* description, u32 line, const char* function, const char* file, const char* format, ...);
COMMON_API void com_debug_log_break(const char* description, u32 line, const char* function, const char* file, const char* format, ...);

END_CPP_COMPATIBLE
