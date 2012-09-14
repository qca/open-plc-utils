/*====================================================================*
 *
 *   char * hexoffset (char buffer [], size_t length, off_t offset);
 *
 *   memory.h
 *
 *   encode buffer with with a NUL terminated string containing the 
 *   hexadecimal representation of a memory offset; the result will
 *   be padded with leading zeros;
 *
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 1982-2005 by Charles Maier for personal use;
 *;  Licensed under GNU General Public License Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef HEXOFFSET_SOURCE
#define HEXOFFSET_SOURCE

#include <memory.h>

#include "../tools/number.h"
#include "../tools/memory.h"

char * hexoffset (char buffer [], size_t length, off_t offset) 

{
	char * string = buffer + length - 1;
	memset (buffer, 0, length);
	while (string > buffer) 
	{
		*--string = DIGITS_HEX [offset & 0x0F];
		offset >>= 4;
	}
	return (buffer);
}


#endif

