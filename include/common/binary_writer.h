#pragma once

#include <common/defines.h>
#include <bufferlib/buffer.h>

/* type fo data to be written at the marked write position */
typedef enum mark_type_t
{
	/* variable number of bytes might be inserted at the marked write position */
	MARK_TYPE_UNDEFINED,
	/* 1 byte to be written at the marked write position */
	MARK_TYPE_U8,
	/* 2 bytes to be witten at the marked write position */
	MARK_TYPE_U16,
	/* 4 bytes to be witten at the marked write position */
	MARK_TYPE_U32
} mark_type_t;

typedef struct mark_entry_t
{
	/* user defined mark id */
	u32 id;
	/* position at which this mark as been made */
	u32 pos;
	/* type of the mark (it's information about memory stride in the buffer) */
	mark_type_t type;
	/* priority or rather the time stamp of this mark */
	u32 priority;
} mark_entry_t;

typedef BUFFER mark_entry_list_t;

/* binary writer is an interface to write binary data into a memory buffer conveniently */
typedef struct binary_writer_t
{
	/* 	user data (automatically resizable buffer instance, such as BUFFER)	*/
	void* user_data;
	/* 	ptr to a function which pushes a number of contiguous bytes on top of the memory buffer */
	void (*push)(void* user_data, const void* bytes, u32 size);
	/* 	ptr to a function which inserts a number of contiguous bytes somewhere in the memory buffer 
		index: the index where to to start inserting the bytes
		bytes: ptr to the contiguous bytes
		size: the number of bytes pointed by the pointer 'bytes'
	 */
	void (*insert)(void* user_data, u32 index, const void* bytes, u32 size);
	/* ptr to a function which returns internal memory buffer pointer */
	void* (*get_ptr)(void* user_data);
	/* ptr to a function which returns current position (number of elements) */
	u32 (*write_pos)(void* user_data);
	/* a table to map mark IDs to write positions */
	mark_entry_list_t mark_table;
} binary_writer_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
COMMON_API binary_writer_t* binary_writer_new();
COMMON_API binary_writer_t* binary_writer_create(
												void* user_data,
												void (*push)(void* user_data, const void* bytes, u32 size),
												void (*insert)(void* user_data, u32 index, const void* bytes, u32 size),
												void* (*get_ptr)(void* user_data),
												u32 (*write_pos)(void* user_data));
COMMON_API void binary_writer_destroy(binary_writer_t* writer);
COMMON_API void binary_writer_release_resources(binary_writer_t* writer);

/* writers */

/* 	writes a null terminated string */
COMMON_API void binary_writer_string(binary_writer_t* writer, const char* str);
/* 	writes a non-null termianted string, 
	automatically appends a null character at the end of the string */
COMMON_API void binary_writer_stringn(binary_writer_t* writer, const char* str, u32 len);
/* 	writes a u8 value */
COMMON_API void binary_writer_u8(binary_writer_t* writer, u8 v);
/* 	writes a u16 value */
COMMON_API void binary_writer_u16(binary_writer_t* writer, u16 v);
/* 	writes a u32 value */
COMMON_API void binary_writer_u32(binary_writer_t* writer, u32 v);
/* 	writes a u64 value */
COMMON_API void binary_writer_u64(binary_writer_t* writer, u64 v);
/* 	writes a s8 value */
COMMON_API void binary_writer_s8(binary_writer_t* writer, s8 v);
/* 	writes a s16 value */
COMMON_API void binary_writer_s16(binary_writer_t* writer, s16 v);
/* 	writes a s32 value */
COMMON_API void binary_writer_s32(binary_writer_t* writer, s32 v);
/* 	writes a s64 value */
COMMON_API void binary_writer_s64(binary_writer_t* writer, s64 v);
/* 	writes a f32 value */
COMMON_API void binary_writer_f32(binary_writer_t* writer, f32 v);
/* 	writes a f64 value */
COMMON_API void binary_writer_f64(binary_writer_t* writer, f64 v);
/*  writes contiguous bytes pointed by 'bytes' of length 'size' */
COMMON_API void binary_writer_write(binary_writer_t* writer, void* bytes, u32 size);

/* returns the current write position (number of bytes written till now) */
COMMON_API u32 binary_writer_pos(binary_writer_t* writer);

/* returns current offset value for the mark id 'mark_id', this function is useful to query the current offset value 
 * as it might have been changed because of insertion of data preceding it
 * NOTE: the returned value shouldn't be cached, whenever you need this value just call it again */
COMMON_API u32 binary_writer_pos_from_mark_id(binary_writer_t* writer, u32 mark_id);

/* marks the 2 bytes from the current position with mark_id to be writen later */
COMMON_API void binary_writer_u16_mark(binary_writer_t* writer, u32 mark_id);
/* writes 2 bytes from the write position marked with mark_id */
COMMON_API void binary_writer_u16_set(binary_writer_t* writer, u32 mark_id, u16 v);

/* marks the 4 bytes from the current position with mark_id to be writen later */
COMMON_API void binary_writer_u32_mark(binary_writer_t* writer, u32 mark_id);
/* writes 4 bytes from the write position marked with mark_id */
COMMON_API void binary_writer_u32_set(binary_writer_t* writer, u32 mark_id, u32 v);
/* marks the 1 byte from the current position with mark_id to be writen later */
COMMON_API void binary_writer_u8_mark(binary_writer_t* writer, u32 mark_id);
/* writes 1 byte from the write position marked with mark_id */
COMMON_API void binary_writer_u8_set(binary_writer_t* writer, u32 mark_id, u8 v);

/* marks the current position with mark_id, for variable number of bytes, to be written later */
COMMON_API void binary_writer_mark(binary_writer_t* writer, u32 mark_id);
/* unmarks the position which the mark_id was pointing, this function is used to reuse the mark_id, 
 * and should not be any offset mark ids as it might have unintended side-effects */
COMMON_API void binary_writer_unmark(binary_writer_t* writer, u32 mark_id);
/* writes the bytes pointed by 'bytes' with size 'size' from the write position marked with mark_id */
COMMON_API void binary_writer_insert(binary_writer_t* writer, u32 mark_id, const void* bytes, u32 size);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 binary_writer_get_mark_entry_count(binary_writer_t* writer) { return CAST_TO(u32, buf_get_element_count(&writer->mark_table)); }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE mark_entry_t* binary_writer_get_mark_entries(binary_writer_t* writer) { return CAST_TO(mark_entry_t*, buf_get_ptr(&writer->mark_table)); }

COMMON_API bool mark_id_comparer(void* ours, void* theirs);
COMMON_API bool mark_pos_comparer(void* ours, void* theirs);

END_CPP_COMPATIBLE
