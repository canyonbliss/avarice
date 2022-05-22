/*
 *	avarice - The "avarice" program.
 *	Copyright (C) 2001 Scott Finneran
 *      Copyright (C) 2002 Intel Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License Version 2
 *      as published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 */

#ifndef INCLUDE_AVARICE_H
#define INCLUDE_AVARICE_H

#include <cstdarg>
#include <cstddef>

using uchar = unsigned char;

/** true iff --debug option specified **/
extern bool debugMode;

/** true if interrupts should be stepped over when stepping */
extern bool ignoreInterrupts;

/** printf 'fmt, ...' if debugMode **/
void vdebugOut(const char *fmt, va_list args);
void debugOut(const char *fmt, ...);
void debugOutBufHex(const char* prefix, const void* data, size_t data_size);

/** printf 'fmt, ...' to status file descriptor (currently stdout) **/
void vstatusOut(const char *fmt, va_list args);
void statusOut(const char *fmt, ...);

#endif // INCLUDE_AVARICE_H
