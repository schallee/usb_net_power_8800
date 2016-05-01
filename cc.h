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

#ifndef cc_h
#define cc_h

#ifdef __GNUC__
#       define CC_UNUSED_VAR          __attribute__((__unused__))
#       define CC_PRINTF(fmt,first)   __attribute__((format(printf, fmt, first)))
#       define CC_NO_RETURN           __attribute__((noreturn))
#else
#       define CC_UNUSED_VAR  
#       define CC_PRINTF(fmt,first)
#       define CC_NO_RETURN
#endif

#endif
