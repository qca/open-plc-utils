/*====================================================================*
 *
 *   void assist (char const * name, char const * type, struct _code_ const list [], size_t size);
 *
 *   symbol.h
 *   
 *   print program name followed by an error message containing the
 *   type of name expected, the incorrect name and a list of correct
 *   name;
 *   
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef ASSIST_SOURCE
#define ASSIST_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tools/symbol.h"
#include "../tools/error.h"

void assist (char const * name, char const * type, struct _code_ const list [], size_t size) 

{
	extern char const * program_name;
	fprintf (stderr, "%s: ", program_name);
	fprintf (stderr, "%s: ", strerror (ENOTSUP));
	if (name) 
	{
#if 0
		fprintf (stderr, "Have %s '%s' but expected %s ", type, name, type);
#else
		fprintf (stderr, "Have '%s' but want ", name);
#endif
	}
	else 
	{
		fprintf (stderr, "Want %s ", type);
	}
	codelist (list, size, ",", "''", stderr);
	fprintf (stderr, ".\n");
	exit (1);
}


#endif

