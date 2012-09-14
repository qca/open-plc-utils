/*====================================================================*
 *
 *   void endian (void * memory, size_t extent);
 *
 *   reverse the order of bytes in a multi-byte memory region; 
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
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

