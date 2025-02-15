#pragma once

#include <bufferlib/buffer.h>
#include <common/defines.h>
#include <common/comparer.h> // comparer_t
#include <common/id_generator.h>

typedef struct sub_buffer_t
{
	/* offset at which the contents of this sub buffer starts in the master buffer */
	buf_ucount_t ptr;
	/* maximum elements which this sub pass can hold without resize
	 * it changes (doubles) as more elements are pushed into this sub buffer. */
	buf_ucount_t capacity;
	/* current number of elements in the sub buffer,
	 * layed out starting at 'offset' in the master buffer. */
	buf_ucount_t count;
	/* set to true if multi_buffer_sub_buffer_destroy() is called for this sub buffer */
	bool is_free;
} sub_buffer_t;

typedef buffer_t /* sub_buffer_t */ sub_buffer_list_t;

typedef struct multi_buffer_t
{
	BUFFER buffer;			// contains contiguous memory for all the sub-buffers
	id_generator_t id_gen;
	sub_buffer_list_t sub_buffers;
} multi_buffer_t;

typedef id_generator_id_type_t sub_buffer_handle_t;
#define SUB_BUFFER_HANDLE_INVALID ID_GENERATOR_ID_TYPE_MAX

BEGIN_CPP_COMPATIBLE

// constructors and destructors
COMMON_API void multi_buffer_create(u32 element_size, u32 capacity, multi_buffer_t* out_buffer);
COMMON_API void multi_buffer_free(multi_buffer_t* buffer);

// getters
COMMON_API buf_ucount_t multi_buffer_get_count(multi_buffer_t* buffer);
COMMON_API buf_ucount_t multi_buffer_get_capacity(multi_buffer_t* buffer);
COMMON_API buf_ucount_t multi_buffer_get_combined_sub_buffers_count(multi_buffer_t* multi_buffer);
COMMON_API buf_ucount_t multi_buffer_get_sub_buffer_count(multi_buffer_t* buffer);


// logic functions
COMMON_API void multi_buffer_clear(multi_buffer_t* buffer);
COMMON_API void multi_buffer_flatcopy_to(multi_buffer_t* buffer, void* dst_ptr);

// sub buffer

// constructors and destructors
COMMON_API sub_buffer_handle_t multi_buffer_sub_buffer_create(multi_buffer_t* buffer, buf_ucount_t capacity);
COMMON_API void multi_buffer_sub_buffer_destroy(multi_buffer_t* buffer, sub_buffer_handle_t handle); /*TODO*/
#define sub_buffer_create multi_buffer_sub_buffer_create
#define sub_buffer_destroy multi_buffer_sub_buffer_destroy

// logic functions
COMMON_API void multi_buffer_sub_buffer_push(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
COMMON_API void multi_buffer_sub_buffer_push_n(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value, u32 max_size);
COMMON_API void multi_buffer_sub_buffer_push_back(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
COMMON_API void multi_buffer_sub_buffer_clear(multi_buffer_t* buffer, sub_buffer_handle_t handle);
COMMON_API buf_ucount_t multi_buffer_sub_buffer_find_index_of(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal);
COMMON_API bool multi_buffer_sub_buffer_remove(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal);
#define sub_buffer_push multi_buffer_sub_buffer_push
#define sub_buffer_push_n multi_buffer_sub_buffer_push_n
#define sub_buffer_push_back multi_buffer_sub_buffer_push_back
#define sub_buffer_clear multi_buffer_sub_buffer_clear
#define sub_buffer_find_index_of multi_buffer_sub_buffer_find_index_of
#define sub_buffer_remove multi_buffer_sub_buffer_remove

// setters
COMMON_API void multi_buffer_sub_buffer_set_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value);
COMMON_API void multi_buffer_sub_buffer_set_at_n(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value, buf_ucount_t max_size);
#define sub_buffer_set_at multi_buffer_sub_buffer_set_at
#define sub_buffer_set_at_n multi_buffer_sub_buffer_set_at_n

// getters
COMMON_API buf_ucount_t multi_buffer_sub_buffer_get_count(multi_buffer_t* buffer, sub_buffer_handle_t handle);
COMMON_API buf_ucount_t multi_buffer_sub_buffer_get_capacity(multi_buffer_t* buffer, sub_buffer_handle_t handle);
COMMON_API void multi_buffer_sub_buffer_get_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* out_value);
COMMON_API void* multi_buffer_sub_buffer_get_ptr_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index);
#define sub_buffer_get_count multi_buffer_sub_buffer_get_count
#define sub_buffer_get_capacity multi_buffer_sub_buffer_get_capacity
#define sub_buffer_get_at multi_buffer_sub_buffer_get_at
#define sub_buffer_get_ptr_at multi_buffer_sub_buffer_get_ptr_at
#define sub_buffer_get_ptr(buffer, handle) sub_buffer_get_ptr_at(buffer, handle, 0)

END_CPP_COMPATIBLE
