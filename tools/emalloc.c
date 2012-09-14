/*====================================================================*
 *
 *   void * emalloc (size_t length)
 *
 *   error.h
 *
 *   attempt to allocate memory using malloc(); return the memory address
 *   on success; print an error message on stderr and then terminate the
 *   program on failure;
 *
 *.  Motley Tools by Charlie Maier; cmaier@cmassoc.net;
 *:  modified sun 07 sep 03 by user root on host zeus;
 *;  Licensed under GNU General Public License Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef EMALLOC_SOURCE
#define EMALLOC_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../tools/error.h"

void * emalloc (size_t length) 

{
	void * memory = malloc (length);
	if (!memory) 
	{
		error (1, errno, "need %lu bytes", (long)(length));
	}
	return (memory);
}


#endif

