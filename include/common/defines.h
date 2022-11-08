
#pragma once


#include <stdint.h>
#include <stdbool.h>

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
#define WARN_UNUSED_RESULT_FUNCTION __attribute__((warn_unused_result))
#define INLINE_FUNCTION inline

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

#define DREF_TO(type, ptr) (*(type*)(ptr))
#define DREF(ptr) (*(ptr))
#define DEREF DREF
#define DEREF_TO DREF_TO
#define CAST_TO(to, s) ((to)(s))
#define REINTERPRET_TO(to, s) (*(to*)(&s))

#define OUT *
#define IN const *

#define BIT64(index) (1ULL << index)
#define BIT32(index) (1UL << index)
#define BIT_MASK32(count) (CAST_TO(u32, ~((~0UL) << count)))

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

#define DEG * DEG2RAD
