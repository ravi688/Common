#include <common/dictionary.h>
#include <common/alloc.h>
#include <common/assert.h>

#include <string.h> /* for strcmp */

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(dictionary)
#else 
	static void check_pre_condition(dictionary_t* dictionary);
#endif /*GLOBAL_DEBUG*/

// constructors and destructors
COMMON_API dictionary_t __dictionary_create(u32 key_size, u32 value_size, u64 capacity, bool (*key_comparer)(void* compare_key, void* key))
{
	_com_assert(key_comparer != NULL);
	_com_assert_wrn((key_size + value_size) < 1024);
	dictionary_t dictionary;
	dictionary.buffer = buf_create(key_size + value_size, capacity, 0);
	dictionary.key_comparer = key_comparer;
	dictionary.key_size = key_size;
	dictionary.value_size = value_size;
	return dictionary;
}

COMMON_API void dictionary_free(dictionary_t* dictionary)
{
	check_pre_condition(dictionary);
	buf_free(&dictionary->buffer);
	memset(dictionary, 0, sizeof(dictionary_t));
}

// getters
COMMON_API void dictionary_get_at(dictionary_t* dictionary, u64 index, void* out_key, void* out_value)
{
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcopyv(out_key, ptr, u8, dictionary->key_size);
	memcopyv(out_value, ptr + dictionary->key_size, u8, dictionary->value_size);
}

COMMON_API void dictionary_get_value(dictionary_t* dictionary, void* key, void* out_value)
{
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	void* ptr = buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer));
	memcopyv(out_value, ptr + dictionary->key_size, u8, dictionary->value_size);
}

COMMON_API bool dictionary_try_get_value_ptr(dictionary_t* dictionary, void* key, void** out_ptr)
{
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	buf_ucount_t index = buf_find_index_of(buffer, key, dictionary->key_comparer);
	if(index == BUF_INVALID_INDEX)
		return false;
	void* ptr = buf_get_ptr_at(buffer, index);
	void* value_ptr = ptr + dictionary->key_size;
	memcopy_void(out_ptr, &value_ptr, sizeof(DREF_VOID_PTR(out_ptr)));
	return true;
}

COMMON_API bool dictionary_try_get_value(dictionary_t* dictionary, void* key, void* out_value)
{
	void* value_ptr;
	if(!dictionary_try_get_value_ptr(dictionary, key, &value_ptr))
		return false;
	memcopyv(out_value, value_ptr, u8, dictionary->value_size);
	return true;
}

COMMON_API void dictionary_get_key_at(dictionary_t* dictionary, u64 index, void* out_key)
{
	check_pre_condition(dictionary);
	memcopyv(out_key, buf_get_ptr_at(&dictionary->buffer, index), u8, dictionary->key_size);
}

COMMON_API void* dictionary_get_key_ptr_at(dictionary_t* dictionary, u64 index)
{
	check_pre_condition(dictionary);
	return buf_get_ptr_at(&dictionary->buffer, index);
}

COMMON_API void dictionary_get_value_at(dictionary_t* dictionary, u64 index, void* out_value)
{
	check_pre_condition(dictionary);
	memcopyv(out_value, buf_get_ptr_at(&dictionary->buffer, index) + dictionary->key_size, u8, dictionary->value_size);
}

COMMON_API void* dictionary_get_value_ptr_at(dictionary_t* dictionary, u64 index)
{
	check_pre_condition(dictionary);
	return buf_get_ptr_at(&dictionary->buffer, index) + dictionary->key_size;
}

COMMON_API void* dictionary_get_value_ptr(dictionary_t* dictionary, void* key)
{
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	return buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer)) + dictionary->key_size;
}

COMMON_API u64 dictionary_get_count(dictionary_t* dictionary)
{
	check_pre_condition(dictionary);
	return buf_get_element_count(&dictionary->buffer);
}

COMMON_API u64 dictionary_get_capacity(dictionary_t* dictionary)
{
	check_pre_condition(dictionary);
	return buf_get_capacity(&dictionary->buffer);
}


// setters
COMMON_API void dictionary_set_at(dictionary_t* dictionary, u64 index, void* in_key, void* in_value)
{
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcopyv(ptr, in_key, u8, dictionary->key_size);
	memcopyv(ptr + dictionary->key_size, in_value, u8, dictionary->value_size);
}

COMMON_API void dictionary_set_value(dictionary_t* dictionary, void* key, void* in_value)
{
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	void* ptr = buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer));
	memcopyv(ptr + dictionary->key_size, in_value, u8, dictionary->value_size);
}

COMMON_API void dictionary_set_value_at(dictionary_t* dictionary, u64 index, void* in_value)
{
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcopyv(ptr + dictionary->key_size, in_value, u8, dictionary->value_size);
}

// logic functions
COMMON_API void dictionary_push(dictionary_t* dictionary, void* in_key, void* in_value)
{
	check_pre_condition(dictionary);
	// NOTE: buf_push internally uses memcopyv, so here we are calling memcopyv two times more
	//		 We could have used buf_push_pseudo(buffer, 1) but it internally uses memset(bytes, 0, size) which zeros out the memory
	// TODO: So, create an another version of buf_push_pseudo leaving the memory uninitialized
	u8 bytes[dictionary->key_size + dictionary->value_size];
	memcopyv(bytes, in_key, u8, dictionary->key_size);
	memcopyv(bytes + dictionary->key_size, in_value, u8, dictionary->value_size);
	buf_push(&dictionary->buffer, bytes);
}

COMMON_API void dictionary_pop(dictionary_t* dictionary, void* out_key, void* out_value)
{
	check_pre_condition(dictionary);
	void* ptr = buf_peek_ptr(&dictionary->buffer);
	memcopyv(out_key, ptr, u8, dictionary->key_size);
	memcopyv(out_value, ptr + dictionary->key_size, u8, dictionary->value_size);
	buf_pop(&dictionary->buffer, NULL);
}

COMMON_API void dictionary_remove(dictionary_t* dictionary, void* key)
{
	check_pre_condition(dictionary);
	CAN_BE_UNUSED_VARIABLE bool result = buf_remove(&dictionary->buffer, key, dictionary->key_comparer);
	_com_assert(result == true);
}

COMMON_API bool dictionary_contains(dictionary_t* dictionary, void* key)
{
	check_pre_condition(dictionary);
	return buf_find_index_of(&dictionary->buffer, key, dictionary->key_comparer) != BUF_INVALID_INDEX;
}

COMMON_API void dictionary_clear(dictionary_t* dictionary)
{
	check_pre_condition(dictionary);
	buf_clear(&dictionary->buffer, NULL);
}

COMMON_API u64 dictionary_find_index_of(dictionary_t* dictionary, void* key)
{
	check_pre_condition(dictionary);
	return buf_find_index_of(&dictionary->buffer, key, dictionary->key_comparer);
}

COMMON_API bool dictionary_key_comparer_u16(void* v1, void* v2)
{
	return (*(u16*)v1) == (*(u16*)v2);
}

COMMON_API bool dictionary_key_comparer_u32(void* v1, void* v2)
{
	return (*(u32*)v1) == (*(u32*)v2);
}

COMMON_API bool dictionary_key_comparer_u64(void* v1, void* v2)
{
	return (*(u64*)v1) == (*(u64*)v2);
}

COMMON_API bool dictionary_key_comparer_u8(void* v1, void* v2)
{
	return (*(u8*)v1) == (*(u8*)v2);
}

COMMON_API bool dictionary_key_comparer_s16(void* v1, void* v2)
{
	return (*(s16*)v1) == (*(s16*)v2);
}

COMMON_API bool dictionary_key_comparer_s32(void* v1, void* v2)
{
	return (*(s32*)v1) == (*(s32*)v2);
}

COMMON_API bool dictionary_key_comparer_s64(void* v1, void* v2)
{
	return (*(s64*)v1) == (*(s64*)v2);
}

COMMON_API bool dictionary_key_comparer_s8(void* v1, void* v2)
{
	return (*(s8*)v1) == (*(s8*)v2);
}

COMMON_API bool dictionary_key_comparer_float(void* v1, void* v2)
{
	return (*(float*)v1) == (*(float*)v2);
}

COMMON_API bool dictionary_key_comparer_double(void* v1, void* v2)
{
	return (*(double*)v1) == (*(double*)v2);
}

COMMON_API bool dictionary_key_comparer_string(void* v1, void* v2)
{
	return strcmp(*(char**)v1, *(char**)v2) == 0;
}

COMMON_API bool dictionary_key_comparer_ptr(void* v1, void* v2)
{
	return CAST_TO(void*, DREF_TO(u64, v1)) == CAST_TO(void*, DREF_TO(u64, v2));
}


#ifdef GLOBAL_DEBUG
static void check_pre_condition(dictionary_t* dictionary)
{
	_com_assert(dictionary != NULL);
	_com_assert(dictionary->key_comparer != 0);
	_com_assert_wrn(dictionary->key_size != 0);
	_com_assert_wrn(dictionary->value_size != 0);
	_com_assert_wrn((dictionary->key_size + dictionary->value_size) < 1024);
}
#endif /*GLOBAL_DEBUG*/
