#pragma once

#include <bufferlib/buffer.h>
#include <common/defines.h>

#define DICTIONARY_INVALID_INDEX BUF_INVALID_INDEX

typedef struct dictionary_t
{ 
	BUFFER buffer;										// holds all the key-value pairs in a contigous block of memory
	bool (*key_comparer)(void* compare_key, void* key);	// compare_key = the key to be compared, key = the key already present in the dictionary
	u32 key_size;										// holds the size of the key
	u32 value_size;										// holds the size of the value
} dictionary_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
#define dictionary_create(Tkey, Tvalue, capacity, key_comparer) __dictionary_create(sizeof(Tkey), sizeof(Tvalue), capacity, key_comparer)
COMMON_API dictionary_t __dictionary_create(u32 key_size, u32 value_size, buf_ucount_t capacity, bool (*key_comparer)(void* compare_key, void* key));
COMMON_API void dictionary_free(dictionary_t* dictionary);

// getters
COMMON_API void dictionary_get_at(dictionary_t* dictionary, buf_ucount_t index, void* out_key, void* out_value);
COMMON_API void dictionary_get_value(dictionary_t* dictionary, void* key, void* out_value);
COMMON_API bool dictionary_try_get_value_ptr(dictionary_t* dictionary, void* key, void** out_ptr);
COMMON_API bool dictionary_try_get_value(dictionary_t* dictionary, void* key, void* out_value);
COMMON_API void dictionary_get_key_at(dictionary_t* dictionary, buf_ucount_t index, void* out_key);
COMMON_API void* dictionary_get_key_ptr_at(dictionary_t* dictionary, buf_ucount_t index);
COMMON_API void dictionary_get_value_at(dictionary_t* dictionary, buf_ucount_t index, void* out_value);
COMMON_API void* dictionary_get_value_ptr_at(dictionary_t* dictionary, buf_ucount_t index);
COMMON_API void* dictionary_get_value_ptr(dictionary_t* dictionary, void* key);
COMMON_API buf_ucount_t dictionary_get_count(dictionary_t* dictionary);
COMMON_API buf_ucount_t dictionary_get_capacity(dictionary_t* dictionary);


// setters
COMMON_API void dictionary_set_at(dictionary_t* dictionary, buf_ucount_t index, void* in_key, void* in_value);
COMMON_API void dictionary_set_value(dictionary_t* dictionary, void* key, void* in_value);
COMMON_API void dictionary_set_value_at(dictionary_t* dictionary, buf_ucount_t index, void* in_value);

// logic functions
#define dictionary_add(...) dictionary_push(__VA_ARGS__)
COMMON_API void dictionary_push(dictionary_t* dictionary, void* in_key, void* in_value);
COMMON_API void dictionary_pop(dictionary_t* dictionary, void* out_key, void* out_value);
COMMON_API void dictionary_remove(dictionary_t* dictionary, void* key);
COMMON_API bool dictionary_contains(dictionary_t* dictionary, void* key);
COMMON_API void dictionary_clear(dictionary_t* dictionary);
COMMON_API buf_ucount_t dictionary_find_index_of(dictionary_t* dictionary, void* key);

#define dictionary_key_comparer_char dictionary_key_comparer_s8
#define dictionary_key_comparer_int dictionary_key_comparer_s32
#define dictionary_key_comparer_uint dictionary_key_comparer_u32
#define dictionary_key_comparer_buf_ucount_t dictionary_key_comparer_u64
COMMON_API bool dictionary_key_comparer_u16(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_u32(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_u64(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_u8(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_s16(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_s32(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_s64(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_s8(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_float(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_double(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_string(void* v1, void* v2);
COMMON_API bool dictionary_key_comparer_ptr(void* v1, void* v2);

END_CPP_COMPATIBLE
