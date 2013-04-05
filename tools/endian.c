/*====================================================================*
 *
 *   void endian (void * memory, size_t extent);
 *
 *   reverse the order of bytes in a multi-byte memory region; 
 *
 *   Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *   Copyright (c) 2001-2006 by Charles Maier Associates;
 *   Licensed under the Internet Software Consortium License;
 *
 *--------------------------------------------------------------------*/

#ifndef ENDIAN_SOURCE
#define ENDIAN_SOURCE

#include "../tools/memory.h"

void endian (void * memory, size_t extent) 

{
	register byte * first = (byte *)(memory);
	register byte * final = first + extent;
	while (first < final) 
	{
		register byte byte = *first;
		*first++ = *--final;
		*final = byte;
	}
	return;
}


#endif

