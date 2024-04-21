
#pragma once


#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <float.h>
#include <stddef.h> // NULL
#include <common/platform.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;
typedef u32 uint;

typedef u8* u8_ptr_t;
typedef u16* u16_ptr_t;
typedef u32* u32_ptr_t;
typedef u64* u64_ptr_t;
typedef s8* s8_ptr_t;
typedef s16* s16_ptr_t;
typedef s32* s32_ptr_t;
typedef s64* s64_ptr_t;
typedef f32* f32_ptr_t;
typedef f64* f64_ptr_t;
typedef void* void_ptr_t;

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define S8_MAX INT8_MAX
#define S8_MIN INT8_MIN
#define S16_MAX INT16_MAX
#define S16_MIN INT16_MIN
#define S32_MAX INT32_MAX
#define S32_MIN INT32_MIN
#define S64_MAX INT64_MAX
#define S64_MIN INT64_MIN

#define F32_MIN FLT_MIN
#define F32_MAX FLT_MAX
#define F64_MIN DBL_MIN
#define F64_MAX DBL_MAX

#define DEPRECATED_FUNCTION __attribute__((deprecated))
#define REMOVED_FUNCTION __attribute__((unavailable))
#define FORCE_INLINE_FUNCTION inline __attribute__((always_inline))
#define NO_RETURN_FUNCTION __attribute__((noreturn))
#define COLD_FUNCTION __attribute__((cold))
#define HOT_FUNCTION __attribute__((hot))
#define CONST_FUNCTION __attribute__((const))
#define PURE_FUNCTION __attribute__((pure))
#define NON_NULL_RETURN_FUNCTION __attribute__((returns_nonnull))
#define CONSTRUCTOR_FUNCTION __attribute__((constructor))
#define DESTRUCTOR_FUNCTION __attribute__((destructor))
#define UNUSED_FUNCTION __attribute__((unused))
#define UNUSED_VARIABLE __attribute__((unused))
#define WARN_UNUSED_RESULT_FUNCTION __attribute__((warn_unused_result))
#define INLINE_FUNCTION inline
#define UNREACHABLE() __builtin_unreachable()

#define DEPRECATED DEPRECATED_FUNCTION
#define INLINE INLINE_FUNCTION
#define FORCE_INLINE FORCE_INLINE_FUNCTION
#define CONSTRUCTOR CONSTRUCTOR_FUNCTION
#define DESTRUCTOR DESTRUCTOR_FUNCTION
#define HOT HOT_FUNCTION
#define COLD COLD_FUNCTION
#define AUTO __auto_type

#ifdef COMMON_STATIC_LIBRARY
#	define COMMON_API
#elif COMMON_DYNAMIC_LIBRARY
#	define COMMON_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define COMMON_API __declspec(dllexport)
#else
#	define COMMON_API
#endif

#ifdef __cplusplus
#	define BEGIN_CPP_COMPATIBLE extern "C" {
#	define END_CPP_COMPATIBLE }
#else
#	define BEGIN_CPP_COMPATIBLE
#	define END_CPP_COMPATIBLE
#endif

#ifdef GLOBAL_DEBUG
#	define DEBUG_BLOCK(x) x
#else
#	define DEBUG_BLOCK(x)
#endif /*GLOBAL_DEBUG*/

#define DREF_TO(type, ptr) (*(type*)(ptr))
#define DREF(ptr) (*(ptr))
#define DEREF DREF
#define DEREF_TO DREF_TO
#define CAST_TO(to, s) ((to)(s))
#define REINTERPRET_TO(to, s) (*(to*)(&(s)))

/* REINTERPRET: coerces the data in 'source' into 'target', where 'source' and 'target' hold the data.
 * Use REINTERPRET when you're sure that target's size is less than or equal to that of source */
#define REINTERPRET(target, source) REINTERPRET_TO(target, source)

/* SIZEOF_NON_PTR_TYPE: returns the size of non pointer type when a pointer type is passed */
#define SIZEOF_NON_PTR_TYPE(ptr_type) sizeof(DREF(CAST_TO(ptr_type, NULL)))

/* STATIC_CAST: casts one type of pointer to another where the corresponding non-pointer types have the same size.
 * Use STATIC_CAST when you're sure both the pointers point to same sized objects
 * If the size mistmatched, it throws an error at runtime. */
#ifdef GLOBAL_DEBUG
#   define STATIC_CAST(type, source) CAST_TO(type, __static_cast(SIZEOF_NON_PTR_TYPE(type), sizeof(DREF(source)), source))
    COMMON_API void* __static_cast(u32 sizeof_type, u32 sizeof_source, void* source);
#else
#   define STATIC_CAST(target, source) CAST_TO(target, source)
#endif /* GLOBAL_DEBUG */

/* REINTERPRET_CAST: casts on type of pointer to another where the corresponding non-pointer types may not have the
                     same size. However the size of the non-pointer type of the target must be less than or equal to that of source.
 * Use REINTERPRET_CAST when you're sure the size of the non-pointer type of the target is less than or equal to that of source. */
#ifdef GLOBAL_DEBUG
#   define REINTERPRET_CAST(type, source) CAST_TO(type, __reinterpret_cast(SIZEOF_NON_PTR_TYPE(type), sizeof(DREF(source)), source))
#   define REINTERPRET_CONST_CAST(type, source) CAST_TO(type, __reinterpret_const_cast(SIZEOF_NON_PTR_TYPE(type), sizeof(DREF(source)), source))
    COMMON_API const void* __reinterpret_const_cast(u32 sizeof_type, u32 sizeof_source, const void* source);
    COMMON_API void* __reinterpret_cast(u32 sizeof_type, u32 sizeof_source, void* source);
#else
#   define REINTERPRET_CAST(type, source) CAST_TO(type, source)
#endif /* GLOBAL_DEBUG */

#define __COM_OUT__ *
#define __COM_IN__ const *

#ifndef COMMON_HINT_OUT_IN_ALREADY_DEFINED
#   define OUT __COM_OUT__
#   define IN __COM_IN__
#endif

#define BIT64(index) (1ULL << index)
#define BIT32(index) (1UL << index)
#define BIT16(index) (CAST_TO(u16, 1U) << index)
#define BIT8(index) (CAST_TO(u8, 1U) << index)
#define BIT_MASK32(count) (CAST_TO(u32, ~((~0UL) << count)))

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

#define DEG * DEG2RAD

#define ALIGN_AS(alignment) __attribute__((aligned(alignment)))
#define DERIVE_FROM(type) type __base
#define BASE(ptr) (&((ptr)->__base))
#define BIT16_PACK8(v1, v2) ((CAST_TO(u16, v1) << 8) | CAST_TO(u16, v2))
#define BIT16_UNPACK8(bits, index) CAST_TO(u8, (((bits) >> (CAST_TO(u32, index) << 3)) & 0xFFUL))
#define BIT32_PACK16(v1, v2) ((CAST_TO(u32, v1) << 16) | CAST_TO(u32, v2))
#define BIT32_UNPACK16(bits, index) CAST_TO(u16, (((bits) >> (CAST_TO(u32, index) << 4)) & 0xFFFFUL))
#define BIT32_PACK8(v1, v2, v3, v4) BIT32_PACK16(BIT16_PACK8(v1, v2), BIT16_PACK8(v3, v4))
#define BIT32_UNPACK8(bits, index) CAST_TO(u8, (((bits) >> (CAST_TO(u32, index) << 3)) & 0xFFU))
#define BIT64_PACK32(v1, v2) ((CAST_TO(u64, v1) << 32) | CAST_TO(u64, v2))
#define BIT64_UNPACK32(bits, index) CAST_TO(u32, (((bits) >> (CAST_TO(u32, index) << 5)) & 0xFFFFFFFFUL))


#define HAS_FLAG(flags, flag) (((flags) & (flag)) == (flag))

#ifdef COMMON_PORTABLE_STDLIB
    #ifdef PLATFORM_WINDOWS
    #   define strtoull(...) _strtoui64(__VA_ARGS__)
    #elif defined(PLATFORM_LINUX)
    #   define _strtoui64(...) strtoull(__VA_ARGS__)
    #endif
#endif
