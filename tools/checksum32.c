/*====================================================================*
 *
 *   uint32_t checksum32 (void const * memory, size_t extent, uint32_t checksum);
 *
 *   memory.h
 *
 *   compute the 32 bit checksum of a memory segment; the checksum
 *   is the one's complement of the XOR of all 32 bit words;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef CHECKSUM32_SOURCE
#define CHECKSUM32_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"

uint32_t checksum32 (register void const * memory, register size_t extent, register uint32_t checksum) 

{

#ifdef __GNUC__

	while (extent >= sizeof (checksum)) 
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}

#else

	uint32_t * offset = (uint32_t *)(memory);
	while (extent >= sizeof (* offset)) 
	{
		checksum ^= *offset++;
		extent -= sizeof (* offset);
	}

#endif

	return (~checksum);
}


/*====================================================================*
 *   demo/test program;
 *--------------------------------------------------------------------*/

#if 0
#include <stdio.h>

int main (int argc, char const * argv []) 

{
	uint32_t data [100];
	read (0, data, sizeof (data));
	data [10] = 0;
	data [10] = checksum32 (data, sizeof (data), data [10]);
	printf ("data [10] = 0x%08x\n", data [10]);
	data [10] = checksum32 (data, sizeof (data), data [10]);
	printf ("data [10] = 0x%08x\n", data [10]);
	data [10] = checksum32 (data, sizeof (data), 0);
	printf ("data [10] = 0x%08x\n", data [10]);
	data [10] = checksum32 (data, sizeof (data), 0);
	printf ("data [10] = 0x%08x\n", data [10]);
	return (0);
}


#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

