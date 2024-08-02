#pragma once

#include <common/defines.h>
#include <common/assert.h>

#define COM_STATIC_STRING_N_T(SIZE) static_string_##SIZE##_t
#define COM_TYPEDEF_STATIC_STRING_N(SIZE) typedef struct COM_STATIC_STRING_N_T(SIZE) { char data[SIZE]; } COM_STATIC_STRING_N_T(SIZE)

#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h> /* string functions */

BEGIN_CPP_COMPATIBLE

#define COM_STATIC_STRING_N(SIZE) \
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE COM_STATIC_STRING_N_T(SIZE) static_string_##SIZE(const char* str) \
{ \
	_com_assert(strlen(str) < SIZE); \
	COM_STATIC_STRING_N_T(SIZE) sstr; \
	strncpy(sstr.data, str, SIZE); \
	return sstr; \
}

#define COM_STATIC_STRING_N_N(SIZE) \
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE COM_STATIC_STRING_N_T(SIZE) static_string_##SIZE##_n(const char* str, u32 len) \
{ \
	_com_assert(len < SIZE); \
	COM_STATIC_STRING_N_T(SIZE) sstr; \
	memcpy(sstr.data, str, len); \
	sstr.data[len] = 0; \
	return sstr; \
}

#define COM_STATIC_STRING_N_CPY(SIZE) \
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const char* static_string_##SIZE##_cpy(COM_STATIC_STRING_N_T(SIZE)* dst, const COM_STATIC_STRING_N_T(SIZE)* src) \
{ \
	_com_assert(strlen(src->data) < SIZE); \
	return strncpy(dst->data, src->data, SIZE); \
}

#define COM_STATIC_STRING_N_CMP(SIZE) \
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE int static_string_##SIZE##_cmp(const COM_STATIC_STRING_N_T(SIZE)* str1, const COM_STATIC_STRING_N_T(SIZE)* str2) \
{ \
	_com_assert(strlen(str1->data) < SIZE); \
	_com_assert(strlen(str2->data) < SIZE); \
	return strncmp(str1->data, str2->data, SIZE); \
}

#ifdef __STDC_LIB_EXT1__
#	define com_strlen_s(str, max_size) CAST_TO(u32, strlen_s(str, max_size))
#else
	COMMON_API u32 com_strlen_s(const char* str, u32 max_size);
#endif

#define COM_STATIC_STRING_N_LEN(SIZE) \
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 static_string_##SIZE##_len(const COM_STATIC_STRING_N_T(SIZE)* str) \
{ \
	return com_strlen_s(str->data, SIZE); \
}

#define COM_STATIC_STRING_N_EXPAND(SIZE) \
COM_TYPEDEF_STATIC_STRING_N(SIZE); \
COM_STATIC_STRING_N(SIZE) \
COM_STATIC_STRING_N_N(SIZE) \
COM_STATIC_STRING_N_CPY(SIZE) \
COM_STATIC_STRING_N_CMP(SIZE) \
COM_STATIC_STRING_N_LEN(SIZE) \


/* static_string_16_t
 * static_string_16_cpy
 * static_string_16_cmp  */
COM_STATIC_STRING_N_EXPAND(16);
/* static_string_32_t
 * static_string_32_cpy
 * static_string_32_cmp  */
COM_STATIC_STRING_N_EXPAND(32);
/* static_string_64_t
 * static_string_64_cpy
 * static_string_64_cmp  */
COM_STATIC_STRING_N_EXPAND(64);
/* static_string_128_t
 * static_string_128_cpy
 * static_string_128_cmp  */
COM_STATIC_STRING_N_EXPAND(128);
/* static_string_256_t
 * static_string_256_cpy
 * static_string_256_cmp  */
COM_STATIC_STRING_N_EXPAND(256);
/* static_string_512_t
 * static_string_512_cpy
 * static_string_512_cmp  */
COM_STATIC_STRING_N_EXPAND(512);

END_CPP_COMPATIBLE