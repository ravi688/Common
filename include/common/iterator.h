#pragma once

#include <common/defines.h>

typedef void (*com_iterator_move_next_callback_t)(void* user_data);
#define COM_ITERATOR_MOVE_NEXT_CALLBACK(fnptr) CAST_TO(com_iterator_move_next_callback_t, fnptr)
typedef void* (*com_iterator_get_callback_t)(void* user_data);
#define COM_ITERATOR_GET_CALLBACK(fnptr) CAST_TO(com_iterator_get_callback_t, fnptr)
typedef void (*com_iterator_reset_callback_t)(void* user_data);
#define COM_ITERATOR_RESET_CALLBACK(fnptr) CAST_TO(com_iterator_reset_callback_t, fnptr)

typedef struct com_iterator_t
{
	void* user_data;
	com_iterator_move_next_callback_t move_next;
	com_iterator_get_callback_t get;
	com_iterator_reset_callback_t reset;
} com_iterator_t;

BEGIN_CPP_COMPATIBLE

COMMON_API void com_iterator_move_next(com_iterator_t* iterator);

COMMON_API void* com_iterator_get(com_iterator_t* iterator);

COMMON_API void com_iterator_reset(com_iterator_t* iterator);

END_CPP_COMPATIBLE