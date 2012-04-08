#ifndef die_h
#define die_h

#include <stdarg.h>
#include "cc.h"

CC_NO_RETURN void die_generic(const char* errstr, const char *fmt, va_list args);
CC_NO_RETURN CC_PRINTF(1,2) void die(const char* fmt, ...);
CC_NO_RETURN CC_PRINTF(2,3) void die_usb(int usb_err, const char *fmt, ...);
CC_NO_RETURN CC_PRINTF(1,2) void die_std(const char *fmt, ...);

#endif
