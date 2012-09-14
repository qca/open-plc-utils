/*====================================================================*
 *
 *   void putoptv(char const *help[]);
 *
 *   putoptv.h
 *
 *   print program information on stdout; informtion is stored as an
 *   ordered string vector; string indexes are defined in getopt.h;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef PUTOPTV_SOURCE
#define PUTOPTV_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"

void putoptv (char const * optv []) 

{
	extern char const *program_name;
	size_t index;
	printf ("\n");
	printf (" purpose: %s\n\n", optv [PUTOPTV_I_PURPOSE]);
	printf (" command: %s [options] %s\n\n", program_name, optv [PUTOPTV_I_COMMAND]);
	printf (" options: [%s%c%c]\n\n", optv [PUTOPTV_I_OPTIONS], GETOPTV_C_VERSION, GETOPTV_C_SUMMARY);
	for (index = PUTOPTV_I_DETAILS; optv [index] != (char *) (0); index++) 
	{
		printf (" %c%s\n", GETOPTV_C_OPTION, optv [index]);
	}
	printf (" %c%c\tversion information\n", GETOPTV_C_OPTION, GETOPTV_C_VERSION);
	printf (" %c%c\thelp summary\n", GETOPTV_C_OPTION, GETOPTV_C_SUMMARY);
	printf ("\n");
	return;
}


#endif

