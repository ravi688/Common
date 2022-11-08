
#include <template_repo/debug.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

TEMPLATE_REPO_API void debug_assert_wrn(u32 line, const char* function, const char* file, u64 assertion, ...)
{
	if(assertion & 1ULL) return;
	va_list args;
	va_start(args, assertion);
	const char* format = "";
	if(assertion & (1ULL << 16)) format = va_arg(args, const char*);
	debug_logv("[Assertion Failed] ", line, function, file, format, args);
	va_end(args);
}

TEMPLATE_REPO_API void debug_assert(u32 line, const char* function, const char* file, u64 assertion, ...)
{
	if(assertion & 1ULL) return;
	va_list args;
	va_start(args, assertion);
	const char* format = "";
	if(assertion & (1ULL << 16)) format = va_arg(args, const char*);
	debug_logv("[Assertion Failed] ", line, function, file, format, args);
	va_end(args);
	exit(0);
}

TEMPLATE_REPO_API void debug_logv(const char* description, u32 line, const char* function, const char* file, const char* format, va_list args)
{
	printf("%s", description);
	vprintf(format, args);
	printf(" | %u, %s, %s\n", line, function, file);
}

TEMPLATE_REPO_API void debug_log(const char* description, u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv(description, line, function, file, format, args);
	va_end(args);
}

TEMPLATE_REPO_API void debug_log_exit(const char* description, u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv(description, line, function, file, format, args);
	va_end(args);
	exit(0);
}
