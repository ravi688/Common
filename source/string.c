/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <common/string.h>
#include <common/debug.h>
#include <bufferlib/buffer.h>
#include <stdarg.h>

COMMON_API const char* com_skip_whitespaces(const char* str, const char* const end)
{
	while((str < end) && isspace(*str)) str++;
	return str;
}

COMMON_API u32 __com_get_word_length(const char* str, const char* const end, const char* delimiters)
{
	u32 count = 0;
	while((str < end) && (!isspace(*str)) && (strchr(delimiters, *str) == NULL)) { str++; count++; }
	return count;
}


COMMON_API u32 com_get_word_length(const char* string, const char delimiter)
{
	u32 count = 0;
	while((!isspace(*string)) && (delimiter != (*string))) { string++; count++; }
	return count;
}

COMMON_API int com_safe_strncmp(const char* str1, const char* const str2, u32 str1_len)
{
	int result = strncmp(str1, str2, min(strlen(str2), str1_len));
	return result;
}

COMMON_API bool com_safe_strncmp_any_va(const char* str1, u32 len, u32 cmp_str_count, ...)
{
	va_list args;
	va_start(args, cmp_str_count);
	for(u32 i = 0; i < cmp_str_count; i++)
	{
		AUTO str = va_arg(args, const char*);
		if(com_safe_strncmp(str1, str, len) == 0)
		{
			va_end(args);
			return true;
		}
	}
	va_end(args);
	return false;
}

COMMON_API bool com_is_empty(const char* start, const char* const end)
{
	bool empty = true;
	while(start < end)
	{
		if(!isspace(*start))
		{
			empty = false;
			break;
		}
		++start;
	}
	return empty;
}

/* begin points to the beginning of the string, end points to the null character in the string */
static u32 com_get_line_no(const char* begin, const char* end)
{
	if((begin >= end) || ((begin != NULL) && (*begin == 0))) return 0;

	u32 line_no = 0;
	do
	{
		++line_no;
		begin = strchr(begin, '\n');
		if(begin == NULL)
			break;
		begin += 1;
	} while(begin <= end);
	return line_no;
}

COMMON_API char* com_stringf(com_allocation_callbacks_t* callbacks, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	buffer_t buffer = buf_new_with_callbacks(callbacks, char);
	buf_vprintf(&buffer, NULL, format, args);
	va_end(args);
	return CAST_TO(char*, buf_get_ptr(&buffer));
}

static bool is_in_same_line(const char* start, const char* end)
{
	while(start < end)
	{
		if(*start == '\n')
			return false;
		++start;
	}
	return true;
}

static inline const void* min_ptr(const void* ptr1, const void* ptr2) { return (ptr1 > ptr2) ? ptr2 : ptr1; }

COMMON_API char* com_remove_comments(com_allocation_callbacks_t* callbacks, char* start, const char* const end)
{
	const char* cmt_start = start;
	const char* prev_cmt_start = start;
	while((cmt_start = strstr(cmt_start, "/*")) != NULL)
	{
		/* this checks for the following type of comment combinations:
		 * ` // /* `
		 */
		const char* sl_cmt_start = strstr(prev_cmt_start, "//");
		if(sl_cmt_start != NULL && (sl_cmt_start < cmt_start) && is_in_same_line(sl_cmt_start, cmt_start + 2))
		{
			const char* new_line = strchr(sl_cmt_start, '\n');
			cmt_start = (new_line == NULL) ? end : (new_line + 1);
			continue;
		}

		const char* cmt_end = strstr(cmt_start + 2, "*/");
		if(cmt_end == NULL)
			return com_stringf(callbacks, "[Parse error] Multiline comment started at line no %" PRIu32 " is not closed, expeced '*/' at line no %" PRIu32, com_get_line_no(start, cmt_start), com_get_line_no(start, end));
		else cmt_end += 2;
		const char* new_line = NULL;
		while((new_line < cmt_end) && ((new_line = strchr(cmt_start, '\n')) != NULL))
		{
			memset((void*)cmt_start, (int)(' '), CAST_TO(const char*, min_ptr(new_line, cmt_end)) - cmt_start);
			cmt_start = new_line + 1;
		}
		cmt_start = cmt_end;
		prev_cmt_start = cmt_start;
	}

	cmt_start = start;
	while((cmt_start = strstr(cmt_start, "//")) != NULL)
	{
		const char* cmt_end = strchr(cmt_start + 2, '\n');
		/* //...EOF */
		if(cmt_end == NULL)
			cmt_end = end;
		memset((void*)cmt_start, (int)(' '), cmt_end - cmt_start);
		cmt_start = cmt_end;
	}

	/* sucess */
	return NULL;
}
