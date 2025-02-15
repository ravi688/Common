#pragma once

#include <common/defines.h>

/* allocation callbacks made during the compilation process
 * one can initialize them with his/her user-defined allocators */
typedef struct com_allocation_callbacks_t
{
	/* pointer to the user data which might be needed by 'allocate', 'reallocate', or 'deallocate' functions */
	void* user_data;
	/* called when allocation of certain size is needed */
	void* (*allocate)(void* user_data, u32 size, u32 align);
	/* called when reallocation of certain size is needed */
	void* (*reallocate)(void* user_data, void* old_ptr, u32 size, u32 align);	
	/* called when deallocation is needed for a memory block pointed by 'ptr' */
	void (*deallocate)(void* user_data, void* ptr);
} com_allocation_callbacks_t;

BEGIN_CPP_COMPATIBLE

COMMON_API com_allocation_callbacks_t com_allocation_callbacks_get_std();

COMMON_API void* com_call_allocate_aligned(com_allocation_callbacks_t* callbacks, u32 size, u32 align);
COMMON_API void* com_call_reallocate_aligned(com_allocation_callbacks_t* callbacks, void* old_ptr, u32 size, u32 align);
#define com_allocate_obj_init(callbacks, type) CAST_TO(type*, com_memzero_forward(com_call_allocate(callbacks, sizeof(type)), sizeof(type)))
#include <string.h> /* for memset */
CAN_BE_UNUSED_FUNCTION static INLINE_IF_RELEASE_MODE void* com_memzero_forward(void* ptr, u32 size) { memset(ptr, 0, size); return ptr; }
#define com_allocate_obj(callbacks, type) com_call_allocate_obj(callbacks, type)
#define com_call_allocate_obj(callbacks, type) CAST_TO(type*, com_call_allocate(callbacks, sizeof(type)))
#define com_allocate(callbacks, size) com_call_allocate(callbacks, size)
#define com_allocate_aligned(callbacks, size, align) com_call_allocate_aligned(callbacks, size, align)
CAN_BE_UNUSED_FUNCTION static INLINE_IF_RELEASE_MODE void* com_call_allocate(com_allocation_callbacks_t* callbacks, u32 size){ return com_call_allocate_aligned(callbacks, size, U32_MAX); }
CAN_BE_UNUSED_FUNCTION static INLINE_IF_RELEASE_MODE void* com_call_reallocate(com_allocation_callbacks_t* callbacks, void* old_ptr, u32 size) { return  com_call_reallocate_aligned(callbacks, old_ptr, size, U32_MAX); }
#define com_deallocate(callbacks, ptr) com_call_deallocate(callbacks, ptr)
COMMON_API void com_call_deallocate(com_allocation_callbacks_t* callbacks, void* ptr);

#include <bufferlib/buffer.h>
// NOTE: param 'callbacks' is allowed to be NULL, in which case com_allocation_callbacks_get_std() will be used
COMMON_API buffer_t buf_create_with_callbacks(com_allocation_callbacks_t* callbacks, u32 element_size, u32 capacity, u32 offset);
// NOTE: param 'callbacks' is allowed to be NULL, in which case com_allocation_callbacks_get_std() will be used
COMMON_API BUFFER* BUFcreate_with_callbacks(com_allocation_callbacks_t* callbacks, void* buffer, u32 element_size, u32 capacity, u32 offset);
// NOTE: param 'callbacks' is allowed to be NULL, in which case com_allocation_callbacks_get_std() will be used
#define buf_new_with_callbacks(callbacks, type) buf_create_with_callbacks(callbacks, sizeof(type), 0, 0)

END_CPP_COMPATIBLE
