#ifndef debug_h
#define debug_h

#include <stdarg.h>
#include <stdbool.h>
#include "cc.h"

#define debug(...)	do	\
{	\
	if(is_debug_enabled())\
	{	\
		debug_real(__VA_ARGS__);	\
	}	\
}while(false)

CC_PRINTF(1,2) void debug_real(const char *fmt, ...);
bool is_debug_enabled();
void enable_debug();
void disable_debug();
void set_debug(bool val);

#endif
