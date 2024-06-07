#include <common/static_string.h>

#ifndef __STDC_LIB_EXT1__
COMMON_API u32 com_strlen_s(const char* str, u32 max_size)
{
	u32 cnt = 0;
	while((cnt < max_size) && (*str++ != 0)) { ++cnt; }
	return cnt;
}
#endif
