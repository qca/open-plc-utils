/*====================================================================*
 *
 *   void xmltree (NODE const * node);
 *
 *   node.h
 *
 *   print node structure on stdout;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef XMLTREE_SOURCE
#define XMLTREE_SOURCE

#include <stdio.h>

#include "../nodes/node.h"
#include "../tools/format.h"

void xmltree (NODE const * node) 

{
	if (node) 
	{
		node = node->below;
	}
	while (node) 
	{
		static unsigned level = 0;
		printf ("%03d ", node->line);
		printf ("(%c) ", node->type);
		output (level, "[%s]", node->text);
		level++;
		xmltree (node);
		level--;
		node = node->after;
	}
	return;
}


#endif

