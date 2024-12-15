#include <common/utility.h>

#include <string.h>
#include <ctype.h>

COMMON_API CAN_BE_UNUSED_FUNCTION bool com_verify_float_str(const char* str)
{
	/* Following is the pattern for floating point number which is being checked here:
	 * (+|-)digits.digits(e|E)(+|-)digits(f)
	 */
	u32 len = strlen(str);
	u32 i= 0;
	if((i < len) && ((str[i] == '+') || (str[i] == '-'))) ++i;
	while((i < len) && isdigit(str[i])) ++i;
	if((i < len) && (str[i] == '.')) ++i;
	while((i < len) && isdigit(str[i])) ++i;
	if((i < len) && ((str[i] == 'e') || (str[i] == 'E'))) ++i;
	if((i < len) && ((str[i] == '+') || (str[i] == '-'))) ++i;
	while((i < len) && isdigit(str[i])) ++i;
	if((i < len) && (str[i] == 'f')) ++i;
	return i == len;
}
