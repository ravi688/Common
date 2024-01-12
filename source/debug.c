
#include <common/debug.h>
#include <common/assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <common/third_party/debug_break.h>

COMMON_API void debug_assert_wrn(u32 line, const char* function, const char* file, u64 assertion, ...)
{
	if(assertion & 1ULL) return;
	va_list args;
	va_start(args, assertion);
	const char* format = "";
	if(assertion & (1ULL << 16)) format = va_arg(args, const char*);
	debug_logv("[Assertion Failed] Warning: ", line, function, file, format, args);
	va_end(args);
}

COMMON_API void debug_assert(u32 line, const char* function, const char* file, u64 assertion, ...)
{
	if(assertion & 1ULL) return;
	va_list args;
	va_start(args, assertion);
	const char* format = "";
	if(assertion & (1ULL << 16)) format = va_arg(args, const char*);
	debug_logv("[Assertion Failed] ", line, function, file, format, args);
	va_end(args);
	debug_break();
}

COMMON_API void debug_logv(const char* description, u32 line, const char* function, const char* file, const char* format, va_list args)
{
	printf("%s", description);
	vprintf(format, args);
	printf(" | %u, %s, %s\n", line, function, file);
	fflush(stdout);
}

COMMON_API void debug_log(const char* description, u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv(description, line, function, file, format, args);
	va_end(args);
}

COMMON_API void __debug_log_exit(const char* description, u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv(description, line, function, file, format, args);
	va_end(args);
	exit(0);
}

COMMON_API void debug_log_break(const char* description, u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv(description, line, function, file, format, args);
	va_end(args);
	debug_break();
}

#ifdef GLOBAL_DEBUG
COMMON_API void* __static_cast(u32 sizeof_type, u32 sizeof_source, void* source)
{
	assert(sizeof_type == sizeof_source, "Invalid static_cast. You might run into reading corrupted data.");
	return source;
}

COMMON_API void* __reinterpret_cast(u32 sizeof_type, u32 sizeof_source, void* source)
{
	assert(sizeof_type <= sizeof_source, "Invalid reinterpret_cast. You might run into reading corrupted data.");
	return source;
}
#endif /* GLOBAL_DEBUG */
