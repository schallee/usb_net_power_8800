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
