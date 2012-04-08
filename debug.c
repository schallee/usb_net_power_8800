#include "debug.h"
#include <stdio.h>

static bool debug_state=false;

bool is_debug_enabled()
{
	return debug_state;
}

void enable_debug()
{
	debug_state = true;
}

void disable_debug()
{
	debug_state = false;
}

void set_debug(bool val)
{
	debug_state = val;
}

void debug_real(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	if(debug_state)
	{
		fputs("[DEBUG] ", stderr);
		vfprintf(stderr, fmt, args);
		fputc('\n', stderr);
		fflush(stderr);
	}
	va_end(args);
}
