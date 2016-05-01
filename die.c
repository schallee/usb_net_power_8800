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

#include "die.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void die_generic(const char* errstr, const char *fmt, va_list args)
{
	vfprintf(stderr, fmt, args);
	if(errstr)
		fprintf(stderr, ": %s\n", errstr);
	else
		fputc('\n', stderr);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

void die(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	die_generic(NULL, fmt, args);
	va_end(args);
}


void die_usb(int usb_err, const char *fmt, ...)
{
	va_list args;
#	ifndef HAVE_LIBUSB_ERROR_NAME
		char buf[22];	/* long enough for uint64_t */
#	endif

	va_start(args, fmt);
#	ifdef HAVE_LIBUSB_ERROR_NAME
		die_generic(libusb_error_name(usb_err), fmt, args);
#	else
		snprintf(buf, sizeof(buf), "%d", usb_err);
		die_generic(buf, fmt, args);
#	endif
	va_end(args);
}

void die_std(const char *fmt, ...)
{
	va_list args;
	int std_err = errno;

	va_start(args, fmt);
	die_generic(strerror(std_err), fmt, args);
	va_end(args);
}

