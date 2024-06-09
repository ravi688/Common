#pragma once

#include <common/defines.h>
#include <common/allocation_callbacks.h>
#include <bufferlib/buffer.h>

typedef buffer_t unreserved_id_list_t;

typedef struct id_generator_t
{
	/* it will be needed when creating the unreserved list */
	com_allocation_callbacks_t* callbacks;
	/* list of u32 types, it contains the ids which are unclaimed by calling id_generator_return() 
	 * it is lazely allocated, i.e. the buffer_t object will be created upon the very first call to id_generator_return() */
	unreserved_id_list_t* unreserved;
	/* this is used to generate new ids, by incrementing it, if unreserved id list is empty */
	u32 counter;
} id_generator_t;

BEGIN_CPP_COMPATIBLE

/* creatse id_generator_t object
 * begin: no ids will be less than this, i.e. it will be starting value for 'counter' */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE id_generator_t id_generator_create(u32 begin, com_allocation_callbacks_t* callbacks)
{
	id_generator_t generator = { };
	generator.callbacks = callbacks;
	generator.counter = begin;
	return generator;
}

/* destroys the unreserved list if it has been created, otherwise does nothing */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void id_generator_destroy(id_generator_t* generator)
{
	if(generator->unreserved != NULL)
		buf_free(generator->unreserved);
}

/* returns a unique id in the scope of this generator */
COMMON_API u32 id_generator_get(id_generator_t* generator);
/* puts the id into unreserved list so that it can be reused later, i.e. id_generator_get() might return it by looking into the list first */
COMMON_API void id_generator_return(id_generator_t* generator, u32 id);
/* resets the counter and clears the unreserved list, thus, invalidates all the ids which were returned by id_generator_get() */
COMMON_API void id_generator_reset(id_generator_t* generator, u32 begin);
