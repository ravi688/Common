#include <common/iterator.h>

COMMON_API void com_iterator_move_next(com_iterator_t* iterator)
{
	if(iterator->move_next != NULL)
		iterator->move_next(iterator->user_data);
}

COMMON_API void* com_iterator_get(com_iterator_t* iterator)
{
	return iterator->get(iterator->user_data);
}

COMMON_API void com_iterator_reset(com_iterator_t* iterator)
{
	if(iterator->reset != NULL)
		iterator->reset(iterator->user_data);
}
