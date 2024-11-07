#include <common/id_generator.h>

#include <common/assert.h>

COMMON_API id_generator_id_type_t id_generator_get(id_generator_t* generator)
{
	/* if the unreserved list can be created and there are some elements in it */
	if((generator->unreserved != NULL) && (buf_get_element_count(generator->unreserved) > 0))
	{
		/* then pop the last element (right-most element) and return it */
		id_generator_id_type_t id;
		buf_pop(generator->unreserved, &id);
		return id;
	}
	/* otherwise, return the counter value and increment it for the next unique id generation */
	return generator->counter++;
}

COMMON_API void id_generator_return(id_generator_t* generator, id_generator_id_type_t id)
{
	_com_assert(id < generator->counter);
	if(generator->unreserved == NULL)
		generator->unreserved = BUFcreate_with_callbacks(generator->callbacks, NULL, sizeof(id_generator_id_type_t), 1, 0);
	buf_push(generator->unreserved, &id);
}
COMMON_API void id_generator_reset(id_generator_t* generator, id_generator_id_type_t begin)
{
	if(generator->unreserved != NULL)
		buf_clear_fast(generator->unreserved);
	generator->counter = begin;
}

COMMON_API id_generator_id_type_t id_generator_get_returned_count(id_generator_t* generator)
{
	if(generator->unreserved != NULL)
		return buf_get_element_count(generator->unreserved);
	return 0;
}