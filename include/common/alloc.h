#pragma once

/*
 Switches:
 COMMON_ALLOC_DEBUG_MORE
 */

#include <stdarg.h>
#include <common/platform.h>
#include <common/defines.h>
#include <common/debug.h>
#include <memory.h>
#	include <stdlib.h>
#	include <malloc.h> // malloc_usable_size for linux, and _msize for windows

#	ifdef PLATFORM_WINDOWS
#		ifndef malloc_usable_size
#			define malloc_usable_size(ptr) _msize(ptr)
#		endif /* if not defined malloc_usable_size */
#	endif /* PLATFORM_WINDOWS */

#	ifdef PLATFORM_LINUX
#		ifndef _aligned_malloc
			/* NOTE: size and alignment are swapped with each other on the right */
#			define _aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#		endif /* _aligned_malloc */
#		ifndef _aligned_free
#			define _aligned_free(...) free(__VA_ARGS__)
#		endif /* _aligned_free */
#		ifndef _aligned_realloc
#			include <memory.h>  // memcpy
#			define _aligned_realloc(...) aligned_realloc(__VA_ARGS__)
			static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool is_aligned(void* ptr, size_t align) { return (CAST_TO(u64, ptr) % align) == 0; }
			static CAN_BE_UNUSED_FUNCTION void* aligned_realloc(void* old_ptr, size_t size, size_t align)
			{
				void* ptr = realloc(old_ptr, size);
				if(!is_aligned(ptr, align))
				{
					size_t old_size = malloc_usable_size(old_ptr);
					ptr = _aligned_malloc(size, align);
					if(old_ptr != NULL)
						memcpy(ptr, old_ptr, size < old_size ? size : old_size);
					_aligned_free(old_ptr);
				}
				return ptr;
			}
#		endif
#	endif /* PLATFORM_LINUX */

	/* malloc */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* _std_malloc(size_t size) { _com_assert(size != 0); return malloc(size); }
	#else /* GLOBAL_RELEASE */
	#	define _std_malloc(size) malloc(size)
	#endif /* GLOBAL_DEBUG */

	/* realloc */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* _std_realloc(void* old_ptr, size_t size) { _debug_assert_wrn__(size != 0); return realloc(old_ptr, size); }
	#else /* GLOBAL_RELEASE */
	#	define _std_realloc(old_ptr, size) realloc(old_ptr, size)
	#endif /* GLOBAL_DEBUG */

	/* free */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void _std_free(void* ptr){ _com_assert(ptr != NULL); free(ptr); }
	#else /* GLOBAL_RELEASE */
	#	define _std_free(ptr) free(ptr)
	#endif /* GLOBAL_DEBUG */

	/* aligned_malloc */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* _std_aligned_malloc(size_t size, size_t align) { _com_assert(align != 0); return _aligned_malloc(size, align); }
	#else /* GLOBAL_RELEASE */
	#	define _std_aligned_malloc(size, align) _aligned_malloc(size, align)
	#endif /* GLOBAL_DEBUG */

	/* aligned_realloc */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* _std_aligned_realloc(void* old_ptr, size_t size, size_t align) { _com_assert(align != 0); return _aligned_realloc(old_ptr, size, align); }
	#else /* GLOBAL_RELEASE */
	#	define _std_aligned_realloc(old_ptr, size, align) _aligned_realloc(old_ptr, size, align)
	#endif /* GLOBAL_DEBUG */

	/* aligned_free */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void _std_aligned_free(void* ptr) { _com_assert(ptr != NULL); _aligned_free(ptr); }
	#else /* GLOBAL_RELEASE */
	#	define _std_aligned_free(ptr) _aligned_free(ptr)
	#endif /* GLOBAL_DEBUG */

	/* alloca */
	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* __std_alloca_forward(void* ptr, size_t size) { _com_assert(size != 0); return ptr; }
		#define _std_alloca(size) __std_alloca_forward(alloca(size), size)
	#else /* GLOBAL_RELEASE */
	#	define _std_alloca(size) alloca(size)
	#endif /* GLOBAL_DEBUG */

	#ifdef GLOBAL_DEBUG
		static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 _std_malloc_usable_size(void* ptr) { _com_assert(ptr != NULL); return malloc_usable_size(ptr); }
	#else /* GLOBAL_RELEASE */
	#	define _std_malloc_usable_size(ptr) CAST_TO(u32, malloc_usable_size(ptr))
	#endif /* GLOBAL_DEBUG */

#	define alloc_init(x)
#	define alloc_terminate()
#   define add_alloc(basePtr, size) basePtr
#	define heap_alloc(size) _std_malloc(size)
#   define heap_realloc(old_ptr, size) _std_realloc(old_ptr, size)
#   define heap_aligned_alloc(size, align) _std_aligned_malloc(size, align)
#   define heap_aligned_realloc(old_ptr, size, align) _std_aligned_realloc(old_ptr, size, align)
#	define stack_alloc(size) _std_alloca(size)
#	define stack_free(basePtr) 
#	define heap_free(basePtr) _std_free(basePtr)
#   define heap_aligned_free(basePtr) _std_aligned_free(basePtr)
#   define heap_silent_free(basePtr) _std_free(basePtr)
#   define heap_silent_aligned_free(basePtr) _std_aligned_free(basePtr)
#	define heap_sizeof(type) sizeof(type)
#	define heap_usable_size(basePtr) _std_malloc_usable_size(basePtr)
#   define get_element(type, validPtr, index) (validPtr)[index]
#   define get_element_ptr(type, validPtr, index) (&(validPtr)[index])
#   define MEM_CHECK(ptr)
#   define var(type, id) type id
#   define val(id) id
#   define ptr(id) &id

#define stack_new(type) ((type*)stack_alloc(sizeof(type)))
#define stack_newv(type, count) ((type*)stack_alloc(sizeof(type) * (count)))
#define heap_new(type) ((type*)heap_alloc(sizeof(type)))
#define heap_newv(type, count) ((type*)heap_alloc(sizeof(type) * (count)))


static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* _debug_memset(void* ptr, int value, size_t size)  { _com_assert(ptr != NULL); return memset(ptr, value, size); }

#define _debug_memcpy(dst, src, size) __debug_memcpy((void*)(dst), (void*)(src), size)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void __debug_memcpy(void* dst, void* src, size_t size) { _com_assert((size == 0) || ((dst != NULL) && (src != NULL))); memcpy(dst, src, size); }

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void __memcopy(void* const dst, const void* const src, u32 dst_size, u32 src_size, u32 requested_size, u32 count, const u32 line, const char* const function, const char* const file)
{
    #ifdef GLOBAL_DEBUG
    if((src_size < requested_size) || (dst_size < requested_size))
        debug_assert(line, function, file, DESCRIPTION(false), 
            "memcopy failed! either src element size or dst element size is less than the requested element size");
    else if((src_size > requested_size) && (dst_size > requested_size))
        debug_log("[Performance Warning] ", line, function, file, 
            "src element size and dst element size both are larger than the requested size");
    #endif
    _debug_memcpy(dst, src, requested_size * count);
}

#define memcopy_void(dst, src, size) _debug_memcpy(dst, src, size)
#define memcopy(dst, src, type) __memcopy(dst, src, sizeof(*(src)), sizeof(*(dst)), sizeof(type), 1, __LINE__, __FUNCTION__, __FILE__)
#define memcopyv(dst, src, type, count) __memcopy(dst, src, sizeof(*(src)), sizeof(*(dst)), sizeof(type), count, __LINE__, __FUNCTION__, __FILE__)

/* NOTE: DESCRIPTION is already defined in common/assert.h but we can't include common/assert.h as it leads to conflicts with renderer/assert.h */
#ifndef DESCRIPTION
#   define DESCRIPTION(bool_value) (CAST_TO(u64, (bool_value)) | (1ULL << 16))
#endif

#ifdef COMMON_ALLOC_DEBUG_MORE
/* sets the memory zero; and checks if the 'actual_size' is equal to the 'requested_size' */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* __memzero(void* const ptr, const u32 actual_size, const u32 requested_size, const u32 line, const char* function, const char* file)
{
    #ifdef GLOBAL_DEBUG
    if(actual_size > requested_size)
        debug_assert(line, function, file, DESCRIPTION(false), 
            "actual size %u is not equal to the size of the type you passed which is %u", actual_size, requested_size);
    else if(actual_size < requested_size)
        debug_log("[Performance Warning] ", line, function, file, "actual size of the object is %u but you allocated more than that which is %u", actual_size, requested_size);
    #endif

    return memset(ptr, 0, actual_size);
}
#endif /* MEMORY_ALLOCATION_SAFETY_LEVEL_2 or MEMORY_ALLOCATION_SAFETY_LEVEL_1 */

/* sets the memory zero; and checks if the size of the type of the pointer is equal to the size of the type */
#ifdef COMMON_ALLOC_DEBUG_MORE
#   define memzero(ptr, type) __memzero(ptr, sizeof(*(ptr)), sizeof(type), __LINE__, __FUNCTION__, __FILE__)
#else
#   define memzero(ptr, type) unsafe_memzero(ptr, type)
#endif
/* vector version of memzero */
#ifdef COMMON_ALLOC_DEBUG_MORE
#   define memzerov(ptr, type, count) __memzero(ptr, sizeof(*(ptr)) * (count), sizeof(type) * (count), __LINE__, __FUNCTION__, __FILE__)
#else
#   define memzerov(ptr, type, count) unsafe_memzerov(ptr, type, count)
#endif
/* sets the memory zero; but it doesn't check if the size of the type of the pointer is equal to the size of the type */
#define unsafe_memzero(ptr, type) _debug_memset(ptr, 0, sizeof(type))
/* vector version of memzero */
#define unsafe_memzerov(ptr, type, count) _debug_memset(ptr, 0, sizeof(type) * (count))

#   define __safe_memset(ptr, value, size) _debug_memset(ptr, value, size)
#   define safe_memzero(ptr, type) memzero(ptr, type)
#   define safe_memzerov(ptr, type, count) memzerov(ptr, type, count)
