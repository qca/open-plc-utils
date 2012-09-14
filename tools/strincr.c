/*====================================================================*
 *
 *   signed strincr (void * memory, size_t extent, byte min, byte max);
 *
 *   memory.h
 *
 *   increment a multi-byte memory region; start at min and reset at
 *   max; return -1 if all bytes are max;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef STRINCR_SOURCE
#define STRINCR_SOURCE

#include "../tools/memory.h"

signed strincr (void * memory, register size_t extent, register byte min, register byte max) 

{
	register byte * offset = (byte *)(memory);
	while (extent--) 
	{
		if (++ offset [extent] <= max) 
		{
			return (0);
		}
		offset [extent] = min;
	}
	return (-1);
}


#endif

