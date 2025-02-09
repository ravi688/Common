
#pragma once

#include <common/defines.h>

#define DEBUG_LOG_INFO(...) debug_log("[Info] ", __LINE__, __FUNCTION__, __FILE__,  __VA_ARGS__)
#define DEBUG_LOG_WARNING(...) debug_log("[Warning] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define DEBUG_LOG_ERROR(...) debug_log("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define DEBUG_LOG_ERROR_EXIT(...) __debug_log_exit("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define DEBUG_LOG_FETAL_ERROR(...) debug_log_break("[Fetal Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)

#ifdef COMMON_DEBUG
#	define debug_log_info(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define debug_log_warning(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#	define debug_log_error(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#   define debug_log_exit(...) DEBUG_LOG_EXIT(__VA_ARGS__)
#	define debug_log_fetal_error(...) DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#else
#	define debug_log_info(...)
#	define debug_log_warning(...)
# 	define debug_log_error(...)
#   define debug_log_exit(...)
#	define debug_log_fetal_error(...)
#endif

#ifdef COM_VERBOSE
#	define debug_log_info_verbose(...) debug_log_info(__VA_ARGS__)
#	define debug_log_warning_verbose(...) debug_log_warning(__VA_ARGS__)
#	define debug_log_error_verbose(...) debug_log_error(__VA_ARGS__)
#	define DEBUG_LOG_INFO_VERBOSE(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define DEBUG_LOG_WARNING_VERBOSE(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#	define DEBUG_LOG_ERROR_VERBOSE(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#else
#	define debug_log_info_verbose(...)
#	define debug_log_warning_verbose(...)
#	define debug_log_error_verbose(...)
#	define DEBUG_LOG_INFO_VERBOSE(...)
#	define DEBUG_LOG_WARNING_VERBOSE(...)
#	define DEBUG_LOG_ERROR_VERBOSE(...)
#endif


BEGIN_CPP_COMPATIBLE

#include <stdarg.h>

COMMON_API void debug_assert_wrn(u32 line, const char* function, const char* file, u64 assertion, ...);
COMMON_API void debug_assert(u32 line, const char* function, const char* file, u64 assertion, ...);
COMMON_API void debug_logv(const char* description, u32 line, const char* function, const char* file, const char* format, va_list args);
COMMON_API void debug_log(const char* description, u32 line, const char* function, const char* file, const char* format, ...);
COMMON_API NO_RETURN_FUNCTION void __debug_log_exit(const char* description, u32 line, const char* function, const char* file, const char* format, ...);
COMMON_API void debug_log_break(const char* description, u32 line, const char* function, const char* file, const char* format, ...);

END_CPP_COMPATIBLE
