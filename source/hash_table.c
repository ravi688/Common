#include <common/hash_table.h>
#include <common/pair.h>
#include <common/debug.h>
#include <common/assert.h>

COMMON_API hash_table_t __hash_table_create(u32 key_size, u32 value_size, u32 capacity, u32 bucket_count, comparer_t key_comparer, hash_function_t key_hash_function, com_allocation_callbacks_t* allocation_callbacks_ptr)
{
	com_assert(COM_DESCRIPTION(bucket_count >= 1), "Bucket count must always be >= 1");
	hash_table_t table =
	{
		.allocation_callbacks = allocation_callbacks_ptr ? *allocation_callbacks_ptr : com_allocation_callbacks_get_std(),
		.key_size = key_size,
		.value_size = value_size,
		.bucket_count = bucket_count,
		.get_hash = key_hash_function,
		.is_equal = key_comparer,
		.bucket_handles = buf_create_with_callbacks(allocation_callbacks_ptr, sizeof(sub_buffer_handle_t), capacity, 0)
	};

	/* create buffer to store hash table entries */
	multi_buffer_create(sizeof(void*), capacity, &table.buffer);

	/* create buckets */
	for(u32 i = 0; i < bucket_count; i++)
	{
		AUTO handle = multi_buffer_sub_buffer_create(&table.buffer, 1);
		buf_push(&table.bucket_handles, &handle);
	}

	return table;
}

COMMON_API void hash_table_free(hash_table_t* table)
{
	hash_table_clear(table);
	buf_free(&table->bucket_handles);
	multi_buffer_free(&table->buffer);
}

COMMON_API void hash_table_clear(hash_table_t* table)
{
	u32 bucket_count = buf_get_element_count(&table->bucket_handles);
	for(u32 i = 0; i < bucket_count; i++)
	{
		/* get the bucket handle */
		sub_buffer_handle_t bucket_handle;
		buf_get_at(&table->bucket_handles, i, &bucket_handle);

		/* for each key value pair in the bucket */
		u32 count = sub_buffer_get_count(&table->buffer, bucket_handle);
		for(u32 j = 0; j < count; j++)
		{
			/* free the key value pair memory block  */
			com_deallocate(&table->allocation_callbacks, DREF_VOID_PTR(sub_buffer_get_ptr_at(&table->buffer, bucket_handle, j)));
		}

		multi_buffer_sub_buffer_clear(&table->buffer, bucket_handle);
	}
}

static u32 get_bucket_index(hash_table_t* table, void* key)
{
	return table->get_hash(key) % table->bucket_count;
}

static sub_buffer_handle_t get_bucket_handle(hash_table_t* table, void* key)
{
	sub_buffer_handle_t bucket_handle;
	buf_get_at(&table->bucket_handles, get_bucket_index(table, key), &bucket_handle);
	return bucket_handle;
}

static void* create_key_value_pair(hash_table_t* table, void* key, void* value)
{
	void* buffer = com_allocate(&table->allocation_callbacks, table->key_size + table->value_size);
	memcpy(buffer, key, table->key_size);
	memcpy(buffer + table->key_size, value, table->value_size);
	return buffer;
}

typedef_pair_t(hash_table_ptr_t, void_ptr_t);

static bool is_equal(void* lhs, void* rhs)
{
	AUTO pair = CAST_TO(pair_t(hash_table_ptr_t, void_ptr_t)*, rhs);
	return pair->first->is_equal(CAST_TO(void*, DREF_TO(u8*, (void**)lhs)), pair->second);
}

COMMON_API void* hash_table_add_get(hash_table_t* table, void* key, void* value)
{	/* get handle to the bucket */
	sub_buffer_handle_t bucket_handle = get_bucket_handle(table, key);

	/* if a key with the same hash already exists then don't add */
	pair_t(hash_table_ptr_t, void_ptr_t) pair = { table, key };
	if(sub_buffer_find_index_of(&table->buffer, bucket_handle, &pair, is_equal) != BUF_INVALID_INDEX)
	{
		debug_log_warning("Failed to add key value pair as a key with the same hash already exists in the hash table");
		return NULL;
	}

	/* create key value pair in the heap memory */
	void* key_value_pair = create_key_value_pair(table, key, value);
	/* add the key value pair into the bucket */
	sub_buffer_push(&table->buffer, bucket_handle, &key_value_pair);
	return key_value_pair + table->key_size;
}

COMMON_API void hash_table_add(hash_table_t* table, void* key, void* value)
{
	hash_table_add_get(table, key, value);
}

static void** __hash_table_get_value(hash_table_t* table, void* key)
{
	AUTO bucket_handle = get_bucket_handle(table, key);
	pair_t(hash_table_ptr_t, void_ptr_t) pair = { table, key };
	AUTO index = sub_buffer_find_index_of(&table->buffer, bucket_handle, &pair, is_equal);
	/* if not found then return NULL */
	if(index == BUF_INVALID_INDEX)
		return NULL;
	/* otherwise a valid value */
	return CAST_TO(void**, sub_buffer_get_ptr_at(&table->buffer, bucket_handle, index));
}

COMMON_API bool hash_table_remove(hash_table_t* table, void* key)
{
	void** ptr = __hash_table_get_value(table, key);
	void* _ptr = NULL;
	if(ptr != NULL)
		_ptr = DREF_VOID_PTR(ptr);
	pair_t(hash_table_ptr_t, void_ptr_t) pair = { table, key };
	bool result = sub_buffer_remove(&table->buffer, get_bucket_handle(table, key), &pair, is_equal);
	if(ptr != NULL)
	{
		_COM_ASSERT(result == true);
		com_deallocate(&table->allocation_callbacks, _ptr);
	}
	return result;
}

COMMON_API bool hash_table_contains(hash_table_t* table, void* key)
{
	pair_t(hash_table_ptr_t, void_ptr_t) pair = { table, key };
	return sub_buffer_find_index_of(&table->buffer, get_bucket_handle(table, key), &pair, is_equal) != BUF_INVALID_INDEX;
}

static void accumulate(void*, void*, void* user_data)
{
	DREF_TO(u32, user_data) += 1;
}

COMMON_API u32 hash_table_get_count(hash_table_t* table)
{
	u32 count = 0;
	hash_table_foreach(table, accumulate, (void*)&count);
	return count;
}

COMMON_API void* hash_table_get_value(hash_table_t* table, void* key)
{
	void* ptr = __hash_table_get_value(table, key);
	if(ptr == NULL)
		return NULL;
	return DREF_VOID_PTR(ptr) + table->key_size;
}

COMMON_API void hash_table_foreach(hash_table_t* table, void (*visitor)(void* key, void* value, void* user_data), void* user_data)
{
	/* for each bucket */
	u32 bucket_count = buf_get_element_count(&table->bucket_handles);
	for(u32 i = 0; i < bucket_count; i++)
	{
		/* get the bucket handle */
		sub_buffer_handle_t bucket_handle;
		buf_get_at(&table->bucket_handles, i, &bucket_handle);

		/* for each key value pair in the bucket */
		u32 count = sub_buffer_get_count(&table->buffer, bucket_handle);
		for(u32 j = 0; j < count; j++)
		{
			/* visit */
			void* pair = DREF_VOID_PTR(sub_buffer_get_ptr_at(&table->buffer, bucket_handle, j));
			visitor(pair, pair + table->key_size, user_data);
		}
	}
}

COMMON_API void hash_table_foreach_until(hash_table_t* table, bool (*visitor)(void* key, void* value, void* user_data), void* user_data)
{
	/* for each bucket */
	u32 bucket_count = buf_get_element_count(&table->bucket_handles);
	for(u32 i = 0; i < bucket_count; i++)
	{
		/* get the bucket handle */
		sub_buffer_handle_t bucket_handle;
		buf_get_at(&table->bucket_handles, i, &bucket_handle);

		/* for each key value pair in the bucket */
		u32 count = sub_buffer_get_count(&table->buffer, bucket_handle);
		for(u32 j = 0; j < count; j++)
		{
			/* visit */
			void* pair = DREF_VOID_PTR(sub_buffer_get_ptr_at(&table->buffer, bucket_handle, j));
			if(!visitor(pair, pair + table->key_size, user_data))
				return;
		}
	}
}
