
#pragma once

#include <template_repo/defines.h>

#define DEBUG_LOG_INFO(...) debug_log("[Info] ", __LINE__, __FUNCTION__, __FILE__,  __VA_ARGS__)
#define DEBUG_LOG_WARNING(...) debug_log("[Warning] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define DEBUG_LOG_ERROR(...) debug_log("[Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
#define DEBUG_LOG_FETAL_ERROR(...) debug_log_exit("[Fetal Error] ", __LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)

#ifdef GLOBAL_DEBUG
#	define debug_log_info(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define debug_log_warning(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#	define debug_log_error(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#	define debug_log_fetal_error(...) DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#else
#	define debug_log_info(...)
#	define debug_log_warning(...)
# 	define debug_log_error(...)
#	define debug_log_fetal_error(...)
#endif

BEGIN_CPP_COMPATIBLE

#include <stdarg.h>

TEMPLATE_REPO_API void debug_assert_wrn(u32 line, const char* function, const char* file, u64 assertion, ...);
TEMPLATE_REPO_API void debug_assert(u32 line, const char* function, const char* file, u64 assertion, ...);
TEMPLATE_REPO_API void debug_logv(const char* description, u32 line, const char* function, const char* file, const char* format, va_list args);
TEMPLATE_REPO_API void debug_log(const char* description, u32 line, const char* function, const char* file, const char* format, ...);
TEMPLATE_REPO_API void debug_log_exit(const char* description, u32 line, const char* function, const char* file, const char* format, ...);

END_CPP_COMPATIBLE
