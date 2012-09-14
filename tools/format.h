/*====================================================================*
 *
 *   format.h - format function definitions and declarations;;
 *   
 *   this file is a subset of the original that includes only those
 *   definitions and declaration needed for toolkit programs;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef FORMAT_HEADER
#define FORMAT_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

size_t strfbits (char buffer [], size_t length, char const * operands [], char const *operator, unsigned flagword);

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

#ifdef __GNUC__

__attribute__ ((format (printf, 2, 3))) 

#endif

void output (signed indent, char const * format, ...);

#ifdef __GNUC__

__attribute__ ((format (printf, 3, 4))) 

#endif

void markup (signed fd, signed level, char const * format, ...);

/*====================================================================*
 *   end definitions;
 *--------------------------------------------------------------------*/

#endif

