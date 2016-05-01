/*
 *  Copyright (C) 2016 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
