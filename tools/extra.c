/*====================================================================*
 *
 *   void extra (signed status, errno_t number, int argc, char const * arg []);
 *
 *   error.h
 *
 *   print error message plus excess argments on stdout;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef EXTRA_SOURCE
#define EXTRA_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../tools/types.h"
#include "../tools/error.h"

void extra (signed status, errno_t number, int argc, char const * argv []) 

{
	extern char const *program_name;
	if ((program_name) && (*program_name)) 
	{
		fprintf (stderr, "%s: ", program_name);
	}
	if (number) 
	{
		fprintf (stderr, "%s: ", strerror (number));
	}
	fprintf (stderr, "Excess data: ");
	while ((argc) && (* argv)) 
	{
		fprintf (stderr, "%s ", * argv);
		argv++;
		argc--;
	}
	fprintf (stderr, "\n");
	fflush (stderr);
	if (status) 
	{
		exit (status);
	}
	return;
}


#endif

