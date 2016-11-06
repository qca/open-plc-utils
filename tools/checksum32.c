/*====================================================================*
 *
 *   uint32_t checksum32 (void const * memory, size_t extent, uint32_t checksum);
 *
 *   memory.h
 *
 *   compute the 32 bit checksum of a memory segment; the calculated
 *   checksum is the one's complement of the XOR of all 32-bit words;
 *   this means that extent should be an even multiple of 4-bytes or
 *   trailing bytes will be excluded from the calculation;
 *
 *   set checksum argument to 0 when the memory region does not
 *   include the previous checksum value;
 *
 *   set checksum argument to the previous checksum value when the
 *   memory region includes the previous checksum value; this will
 *   effectively cancel-out the previous checksum value;
 *
 *   Motley Tools by Charles Maier;
 *   Copyright (c) 2001-2006 by Charles Maier Associates;
 *   Licensed under the Internet Software Consortium License;
 *
 *--------------------------------------------------------------------*/

#ifndef CHECKSUM32_SOURCE
#define CHECKSUM32_SOURCE

#include <string.h>
#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"

uint32_t checksum32 (void const * vmemory, size_t extent, uint32_t checksum)

{
	uint32_t temp;
	const uint8_t *memory = vmemory;

	while (extent >= sizeof (checksum))
	{
		/* Since 'memory' might point to an unaligned address,
		 * we use memcpy to copy the data to 'temp' which is
		 * aligned. Otherwise, the checksum would be wrong in the
		 * best case, or worst-case: program is killed with SIGBUS.
		 */
		memcpy (&temp, memory, sizeof (temp));
		checksum ^= temp;
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}

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

