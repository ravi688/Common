#pragma once

#include <common/defines.h>

BEGIN_CPP_COMPATIBLE

/*
	description: It takes a string representation of a floating point number and tells if it is a valid floating point number or not as if it were to parse using strtof
				The following pattern is checked:
				(+|-)digits.digits(e|E)(+|-)digits
	const char* str: null-terminated c string
	returns: true if the strings represents the valid floating point number, otherwise false
 */
COMMON_API CAN_BE_UNUSED_FUNCTION bool com_verify_float_str(const char* str);

END_CPP_COMPATIBLE
