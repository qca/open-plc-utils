/*====================================================================*
 *
 *   void clr32bitmap (uint32_t * map, unsigned bit);
 *
 *   flags.h  
 *
 *   clear the given bit in an 32-bit flagword array; macro _bits () 
 *   is defined in flags.h;
 *
 *.  Motley Tools by Charlie Maier; cmaier@cmassoc.net;
 *:  modified sun 07 sep 03 by user root on host zeus;
 *;  Licensed under GNU General Public License Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef CLR32BITMAP_SOURCE
#define CLR32BITMAP_SOURCE

#include "../tools/flags.h"
#include "../tools/endian.h"

void clr32bitmap (uint32_t * map, unsigned bit) 

{
	map [bit / _bits (* map)] &= ~ HTOLE32 (1 << (bit % _bits (* map)));
	return;
}


#endif

