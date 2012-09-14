/*====================================================================*
 *
 *   void hexpeek (void const * memory, size_t origin, size_t offset, size_t extent, size_t window, FILE * fp);
 *
 *   memory.h
 *
 *   print a hexadecimal dump of a memory region on stdout within a 
 *   window that precedes and follows the region;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>

#include "../tools/memory.h"

void hexpeek (void const * memory, size_t origin, size_t offset, size_t extent, size_t window, FILE * fp) 

{
	if (offset == origin) 
	{
		offset++;
	}
	hexdump (memory, window < origin? origin - window: 0, offset + window < extent? offset + window: extent, fp);
	return;
}

