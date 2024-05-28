#include <common/allocation_callbacks.h>
#include <stdlib.h> /* for malloc, realloc and free */


static void* com_std_allocate(void* user_data, u32 size, u32 align) { return malloc(size); }
static void* com_std_reallocate(void* user_data, void* old_ptr, u32 size, u32 align) { return realloc(old_ptr, size); }
static void com_std_deallocate(void* user_data, void* ptr) { free(ptr); }

COMMON_API com_allocation_callbacks_t com_allocation_callbacks_get_std()
{
	return (com_allocation_callbacks_t)
	{
		.allocate = com_std_allocate,
		.reallocate = com_std_reallocate,
		.deallocate = com_std_deallocate
	};
}

COMMON_API void* com_call_allocate_aligned(com_allocation_callbacks_t* callbacks, u32 size, u32 align)
{ 
	return (callbacks != NULL) ? callbacks->allocate(callbacks->user_data, size, align) : com_allocation_callbacks_get_std().allocate(NULL, size, align); 
}
COMMON_API void* com_call_reallocate_aligned(com_allocation_callbacks_t* callbacks, void* old_ptr, u32 size, u32 align)
{ 
	return (callbacks != NULL) ? callbacks->reallocate(callbacks->user_data, old_ptr, size, align) : com_allocation_callbacks_get_std().reallocate(NULL, old_ptr, size, align);
}
COMMON_API void com_call_deallocate(com_allocation_callbacks_t* callbacks, void* ptr)
{ 
	return (callbacks != NULL) ? callbacks->deallocate(callbacks->user_data, ptr) : com_allocation_callbacks_get_std().deallocate(NULL, ptr);
}

static void* buf_malloc_callback_handler(buf_ucount_t size, void* user_data)
{
	return com_call_allocate(CAST_TO(com_allocation_callbacks_t*, user_data), size);
}

static void buf_free_callback_handler(void* ptr, void* user_data)
{
	com_call_deallocate(CAST_TO(com_allocation_callbacks_t*, user_data), ptr);
}

static void* buf_realloc_callback_handler(void* old_ptr, buf_ucount_t size, void* user_data)
{
	return com_call_reallocate(CAST_TO(com_allocation_callbacks_t*, user_data), old_ptr, size);
}

COMMON_API buffer_t buf_create_with_callbacks(com_allocation_callbacks_t* callbacks, u32 element_size, u32 capacity, u32 offset)
{
	return buf_create_a(element_size, capacity, offset, buf_malloc_callback_handler, buf_free_callback_handler, buf_realloc_callback_handler, callbacks);
}

COMMON_API BUFFER* BUFcreate_with_callbacks(com_allocation_callbacks_t* callbacks, void* buffer, u32 element_size, u32 capacity, u32 offset)
{
	return BUFcreate_a(buffer, element_size, capacity, offset, buf_malloc_callback_handler, buf_free_callback_handler, buf_realloc_callback_handler, callbacks);
}
