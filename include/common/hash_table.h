#pragma once

#include <common/defines.h>
#include <common/comparer.h> // comparer_t
#include <common/hash_function.h> // hash_function_t
#include <common/multi_buffer.h> 	// multi_buffer_t
#include <common/allocation_callbacks.h> // com_allocation_callbacks_t

typedef buffer_t /* sub_buffer_handle_t */ bucket_handle_list_t;

typedef struct hash_table_t
{
	/* allocation callbacks */
	com_allocation_callbacks_t allocation_callbacks;
	/* size of the key in bytes */
	u32 key_size;
	/* size of the value in bytes */
	u32 value_size;
	/* bucket count in the hash table
	 * initially the hash table would have this number of buckets
	 * and all the buckets would have a shared pool of pre-allocated memory for 'capacity' number of elements. */
	u32 bucket_count;
	/* hash function to calculate hash of a key */
	hash_function_t get_hash;
	/* key comparer to compare a pair of keys for equality */
	comparer_t is_equal;
	/* (it's special data structure to store the buckets into one continguous memory block) */
	multi_buffer_t buffer;
	/* list of sub */
	bucket_handle_list_t bucket_handles;
} hash_table_t;

typedef hash_table_t* hash_table_ptr_t;

BEGIN_CPP_COMPATIBLE

/* constructor and destructors */
// NOTE: bucket_count can never be zero, it must always be equal to or greater than 1
#define hash_table_create(Tkey, Tvalue, capacity, bucket_count, key_comparer, key_hash_function, allocation_callbacks_ptr) __hash_table_create(sizeof(Tkey), sizeof(Tvalue), capacity, bucket_count, key_comparer, key_hash_function, allocation_callbacks_ptr)
COMMON_API hash_table_t __hash_table_create(u32 key_size, u32 value_size, u32 capacity, u32 bucket_count, comparer_t key_comparer, hash_function_t key_hash_function, com_allocation_callbacks_t* allocation_callbacks_ptr);
COMMON_API void hash_table_free(hash_table_t* table);

/* clears the hash table and ready to be used again */
COMMON_API void hash_table_clear(hash_table_t* table);
/* adds a key value pair and returns pointer to the value */
COMMON_API void* hash_table_add_get(hash_table_t* table, void* key, void* value);
/* adds a key value pair */
COMMON_API void hash_table_add(hash_table_t* table, void* key, void* value);
/* removes a key value pair, returns true if success otherwise false (not found) */
COMMON_API bool hash_table_remove(hash_table_t* table, void* key);
/* returns true if the hash table contains an entry with key 'key' otherwise false */
COMMON_API bool hash_table_contains(hash_table_t* table, void* key);
/* returns the number of key value pairs in the hash table */
COMMON_API u32 hash_table_get_count(hash_table_t* table);
/* returns then number of buckets in the hash table */
static INLINE_IF_RELEASE_MODE CAN_BE_UNUSED_FUNCTION u32 hash_table_get_bucket_count(hash_table_t* table) { return table->bucket_count; }
/* returns pointer to the value by it's key, NULL if the key doesn't exists */
COMMON_API void* hash_table_get_value(hash_table_t* table, void* key);
/* visits each key value pairs in the hash table */
COMMON_API void hash_table_foreach(hash_table_t* table, void (*visitor)(void* key, void* value, void* user_data), void* user_data);
/* visits each key value pairs in the hash table until visitor returns false */
COMMON_API void hash_table_foreach_until(hash_table_t* table, bool (*visitor)(void* key, void* value, void* user_data), void* user_data);

END_CPP_COMPATIBLE
