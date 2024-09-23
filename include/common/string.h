/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.h is a part of VulkanRenderer

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


#pragma once

#include <common/defines.h>
#include <common/allocation_callbacks.h>

#include <string.h>
#include <ctype.h>

BEGIN_CPP_COMPATIBLE

/* skips whitespaces (checks via 'isspace')
 * returns: 'end' or pointer to the first non-white-space character. 
 * NOTE: it is safe, as it also makes sure the iteration doesn't exceed 'end' ptr */
COMMON_API const char* com_skip_whitespaces(const char* str, const char* const end);
/* skips until either of 'isspace()' or 'delimiters' occurs
 * returns: the number of characters skipped (i.e. the word length)
 * NOTE: it is safe, as it also makes sure the iteration doesn't exceed 'end' ptr */
COMMON_API u32 __com_get_word_length(const char* str, const char* const end, const char* delimiters);
/* skips until either of 'isspace()' or 'delimiter' occurs
 * returns: the number of characters skipped (i.e. the word length) 
 * WARN: it is not safe, as it doesn't check for overrun */
COMMON_API u32 com_get_word_length(const char* string, const char delimiter);
/* safely compares two strings as follows:
 * 'str1': is assumed to have no-null characters at the end 
 * 'str2': is assumed to have a null character at the end (must have) 
 * len: is the number characters of 'str1' which can participate in the comparison 
 * returns: 'int' same as would strncmp()
 * NOTE: it is safe, as it calculates the minimium of strlen(str2) and 'len' and the compares */
COMMON_API int com_safe_strncmp(const char* str1, const char* const str2, u32 str1_len);
/* safely checks if 'str1' matches with any of the variable numbers of supplied null-terminated strings 
 * 'str1': is assumed to have no-null characters at the end
 * 'len': is the number of characters of 'str1' which can participate in the comparison with each passed null-terminated string
 * 'com_str_count': is the number of null-terminated strings passed at the end 
 * ...: must be comma separated null terminated strings. 
 * returns: true if there is an occurence of 'str1', otherwise false. */
COMMON_API bool com_safe_strncmp_any_va(const char* str1, u32 len, u32 cmp_str_count, ...);
/* checks if string defined by 'start' up to 'end' (exclusive) is empty as follows:
 * it calls 'isspace' for each character.*/
COMMON_API bool com_is_empty(const char* start, const char* const end);
COMMON_API char* com_stringf(com_allocation_callbacks_t* callbacks, const char* format, ...);
COMMON_API char* com_remove_comments(com_allocation_callbacks_t* callbacks, char* start, const char* const end);

END_CPP_COMPATIBLE
