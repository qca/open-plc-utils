/*====================================================================*
 *
 *   __bswap.c - byte swap functions;
 *
 *   endian.h
 *
 *   alternative byte-swap functions for systems without them (such 
 *   as Microsoft Windows);
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef __BSWAP_SOURCE
#define __BSWAP_SOURCE

#include <stdint.h>

#include "../tools/endian.h"

uint16_t __bswap_16 (uint16_t x) 

{
	endian (&x, sizeof (x));
	return (x);
}

uint32_t __bswap_32 (uint32_t x) 

{
	endian (&x, sizeof (x));
	return (x);
}

uint64_t __bswap_64 (uint64_t x) 

{
	endian (&x, sizeof (x));
	return (x);
}


#endif

