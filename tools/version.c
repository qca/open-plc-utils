/*====================================================================*
 *
 *   void version();
 *
 *   version.h
 *
 *   print program and package version information on stdout; 
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef VERSION_SOURCE
#define VERSION_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "../tools/version.h"

void version () 

{
	extern char const * program_name;
	printf ("%s ", program_name);
	printf (PROJECT "-");
	printf (VERSION ".");
	printf (RELEASE " ");
	printf ("ISO C ");
	printf (COMPANY " ");
	printf (COMPILE "\n");
	return;
}


#endif

