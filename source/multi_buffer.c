#include <common/multi_buffer.h>
#include <common/assert.h>
#include <common/alloc.h>

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(multi_buffer)
#	define check_handle(multi_buffer, handle)
#	define check_index(sub_buffer, index)
#else
	static void check_pre_condition(multi_buffer_t* multi_buffer);
	static void check_handle(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle);
#	define check_index(sub_buffer, index) assert((index) < (sub_buffer)->count)
#endif /*GLOBAL_DEBUG*/

inline static sub_buffer_t* get_sub_buffer(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_handle(multi_buffer, handle);
	return (sub_buffer_t*)buf_get_ptr_at(&multi_buffer->sub_buffers, handle);
}

inline static buf_ucount_t get_master_index(sub_buffer_t* sub_buffer, buf_ucount_t index)
{
	check_index(sub_buffer, index);
	return sub_buffer->ptr + index;
}

// constructors and destructors
COMMON_API void multi_buffer_create(u32 element_size, u32 capacity, multi_buffer_t* out_multi_buffer)
{
	_com_assert(out_multi_buffer != NULL);
	out_multi_buffer->buffer = buf_create(element_size, capacity, 0);
	out_multi_buffer->id_gen = id_generator_create(0, NULL);
	out_multi_buffer->sub_buffers = buf_create(sizeof(sub_buffer_t), 1, 0);
}

COMMON_API void multi_buffer_free(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	buf_free(&multi_buffer->buffer);
	id_generator_destroy(&multi_buffer->id_gen);
	buf_free(&multi_buffer->sub_buffers);
}

// getters
COMMON_API buf_ucount_t multi_buffer_get_count(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	return buf_get_element_count(&multi_buffer->buffer);
}

COMMON_API buf_ucount_t multi_buffer_get_capacity(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	return buf_get_capacity(&multi_buffer->buffer);
}

COMMON_API buf_ucount_t multi_buffer_get_combined_sub_buffers_count(multi_buffer_t* multi_buffer)
{
	buf_ucount_t total_count = 0;
	u32 sub_buffer_count = buf_get_element_count(&multi_buffer->sub_buffers);
	for(u32 i = 0; i < sub_buffer_count; i++)
	{
		AUTO sub_buffer = buf_get_ptr_at_typeof(&multi_buffer->sub_buffers, sub_buffer_t, i);
		if(sub_buffer->is_free)
			continue;
		total_count += sub_buffer->count;
	}
	return total_count;
}

static void multi_buffer_verify(multi_buffer_t* multi_buffer)
{
	buf_ucount_t total_count = 0;
	u32 sub_buffer_count = buf_get_element_count(&multi_buffer->sub_buffers);
	for(u32 i = 0; i < sub_buffer_count; i++)
	{
		AUTO sub_buffer = buf_get_ptr_at_typeof(&multi_buffer->sub_buffers, sub_buffer_t, i);
		total_count += sub_buffer->capacity;
	}
	_com_assert(total_count == multi_buffer->buffer.element_count);
}

COMMON_API buf_ucount_t multi_buffer_get_sub_buffer_count(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	_com_assert(buf_get_element_count(&multi_buffer->sub_buffers) >= id_generator_get_returned_count(&multi_buffer->id_gen));
	return buf_get_element_count(&multi_buffer->sub_buffers) - id_generator_get_returned_count(&multi_buffer->id_gen);
}

// logic functions
COMMON_API void multi_buffer_clear(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	buf_clear(&multi_buffer->buffer, NULL);
	id_generator_reset(&multi_buffer->id_gen, 0);
	buf_clear(&multi_buffer->sub_buffers, NULL);
}
COMMON_API void multi_buffer_flatcopy_to(multi_buffer_t* multi_buffer, void* dst_ptr)
{
	u32 total_count = buf_get_element_count(&multi_buffer->sub_buffers);
	void* ptr = buf_get_ptr(&multi_buffer->buffer);
	u32 stride = buf_get_element_size(&multi_buffer->buffer);
	for(u32 i = 0; i < total_count; i++)
	{
		AUTO sub_buffer = buf_get_ptr_at_typeof(&multi_buffer->sub_buffers, sub_buffer_t, i);
		if(sub_buffer->is_free)
			continue;
		u32 num_bytes = sub_buffer->count * stride;
		if(num_bytes == 0) continue;
		memcopyv(dst_ptr, ptr + sub_buffer->ptr * stride, u8, num_bytes);
		dst_ptr += num_bytes;
	}
}

// sub buffer

// constructors and destructors
COMMON_API sub_buffer_handle_t multi_buffer_sub_buffer_create(multi_buffer_t* multi_buffer, buf_ucount_t capacity)
{
	check_pre_condition(multi_buffer);
	BUFFER* sub_buffers = &multi_buffer->sub_buffers;
	BUFFER* buffer = &multi_buffer->buffer;

	sub_buffer_t* sub_buffer;
	AUTO id = id_generator_get(&multi_buffer->id_gen);
	if(id < buf_get_element_count(sub_buffers))
	{
		sub_buffer = buf_get_ptr_at_typeof(sub_buffers, sub_buffer_t, id);
		if(capacity > sub_buffer->capacity)
		{
			buf_insert_pseudo(buffer, sub_buffer->ptr, capacity - sub_buffer->capacity);
			sub_buffer->capacity = capacity;
		}
	}
	else
	{
		/* the base offset of this sub buffer should be after the 'combined sub buffers capacity' including the free ones */
		buf_ucount_t base_offset = 0;
		u32 sub_buffer_count = buf_get_element_count(sub_buffers);
		for(u32 i = 0; i < sub_buffer_count; i++)
		{
			AUTO sub_buffer = buf_get_ptr_at_typeof(&multi_buffer->sub_buffers, sub_buffer_t, i);
			base_offset += sub_buffer->capacity;
		}
		// create a new sub_buffer_t instance
		buf_push_pseudo(sub_buffers, 1);
		sub_buffer = buf_peek_ptr(sub_buffers);
		sub_buffer->is_free = true;
		sub_buffer->ptr = base_offset;
		if(capacity != 0)
			// extend the master buffer to fit the newly created sub_buffer's capacity
			buf_push_pseudo(buffer, capacity);
		sub_buffer->capacity = capacity;
	}
	_com_assert(sub_buffer->is_free);
	sub_buffer->is_free = false;
	// set the element count in the newly created sub_buffer to zero
	sub_buffer->count = 0;
	// return the index of the newly created sub_buffer in the sub_buffers buffer as a handle to it
	multi_buffer_verify(multi_buffer);
	return id;
}

COMMON_API void multi_buffer_sub_buffer_destroy(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	_com_assert(!sub_buffer->is_free);
	id_generator_return(&multi_buffer->id_gen, handle);
	sub_buffer->is_free = true;
	sub_buffer->count = 0;
	multi_buffer_verify(multi_buffer);
}

// logic functions
COMMON_API void multi_buffer_sub_buffer_push(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value)
{
	multi_buffer_sub_buffer_push_n(multi_buffer, handle, in_value, multi_buffer->buffer.element_size);
}

COMMON_API void multi_buffer_sub_buffer_push_n(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, u32 max_size)
{
	check_pre_condition(multi_buffer);
	BUFFER* sub_buffers = &multi_buffer->sub_buffers;
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);

	// resize the master buffer if needed
	buf_ucount_t prev_capacity = sub_buffer->capacity;
	sub_buffer->capacity = (sub_buffer->capacity == 0) ? 1 : sub_buffer->capacity;
	if(sub_buffer->capacity < (sub_buffer->count + 1))
		sub_buffer->capacity <<= 1; 		// multiply by 2
	if(sub_buffer->capacity > prev_capacity)
	{
		buf_ucount_t diff = sub_buffer->capacity - prev_capacity;
		buf_insert_pseudo(&multi_buffer->buffer, sub_buffer->ptr + sub_buffer->count, diff);
		// update the pointers of the sub_buffers to the new location
		for(buf_ucount_t i = handle + 1; i < buf_get_element_count(sub_buffers); i++)
			((sub_buffer_t*)buf_get_ptr_at(sub_buffers, i))->ptr += diff;
	}

	// push the value
	buf_set_at_n(&multi_buffer->buffer, sub_buffer->ptr + sub_buffer->count, in_value, max_size);
	sub_buffer->count++;
	multi_buffer_verify(multi_buffer);
}

COMMON_API void multi_buffer_sub_buffer_clear(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	void* bytes = buf_get_ptr(&multi_buffer->buffer);
	memset(bytes + sub_buffer->ptr * buf_get_element_size(&multi_buffer->buffer), 0, buf_get_element_size(&multi_buffer->buffer) * sub_buffer->count);
	sub_buffer->count = 0;
	multi_buffer_verify(multi_buffer);
}

COMMON_API buf_ucount_t multi_buffer_sub_buffer_find_index_of(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal)
{
	check_pre_condition(multi_buffer);
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	if(sub_buffer->count == 0)
		return BUF_INVALID_INDEX;
	void* ptr = buf_get_ptr_at(&multi_buffer->buffer, get_master_index(sub_buffer, 0));
	u32 element_size = buf_get_element_size(&multi_buffer->buffer);
	for(u32 i = 0; i < sub_buffer->count; i++, ptr += element_size)
		if(is_equal(ptr, in_value))
			return CAST_TO(buf_ucount_t, i);
	return BUF_INVALID_INDEX;
}

COMMON_API bool multi_buffer_sub_buffer_remove(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal)
{
	check_pre_condition(multi_buffer);
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	if(sub_buffer->count == 0)
		return false;
	void* ptr = buf_get_ptr_at(&multi_buffer->buffer, get_master_index(sub_buffer, 0));
	u32 element_size = buf_get_element_size(&multi_buffer->buffer);
	for(u32 i = 0; i < sub_buffer->count; i++, ptr += element_size)
		if(is_equal(ptr, in_value))
		{
			sub_buffer->count--;
			u32 remaining_count = sub_buffer->count - i;
			while(remaining_count > 0)
			{
				memcpy(ptr, ptr + element_size, element_size);
				ptr += element_size;
				--remaining_count;
			}
			return true;
		}

	/* failed to remove, as the value doesn't exist in the buffer */
	return false;
}

// getters
COMMON_API buf_ucount_t multi_buffer_sub_buffer_get_count(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	return get_sub_buffer(multi_buffer, handle)->count;
}

COMMON_API buf_ucount_t multi_buffer_sub_buffer_get_capacity(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	return get_sub_buffer(multi_buffer, handle)->capacity;
}

COMMON_API void multi_buffer_sub_buffer_get_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* out_value)
{
	_com_assert(out_value != NULL);
	check_pre_condition(multi_buffer);
	buf_get_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index), out_value);
}

COMMON_API void* multi_buffer_sub_buffer_get_ptr_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index)
{
	check_pre_condition(multi_buffer);
	return buf_get_ptr_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index));
}

// setters
COMMON_API void multi_buffer_sub_buffer_set_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value)
{
	_com_assert(in_value != NULL);
	check_pre_condition(multi_buffer);
	buf_set_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index), in_value);
}
COMMON_API void multi_buffer_sub_buffer_set_at_n(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value, buf_ucount_t max_size)
{
	_com_assert(in_value != NULL);
	check_pre_condition(multi_buffer);
	buf_set_at_n(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index), in_value, max_size);
}

#ifdef GLOBAL_DEBUG
static void check_pre_condition(multi_buffer_t* multi_buffer)
{
	_com_assert(multi_buffer != NULL);
}

static void check_handle(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	_com_assert(handle < buf_get_element_count(&multi_buffer->sub_buffers));
	_com_assert(handle != SUB_BUFFER_HANDLE_INVALID);
}
#endif /*GLOBAL_DEBUG*/
