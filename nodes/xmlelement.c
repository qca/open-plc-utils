/*====================================================================*
 *
 *   struct node * xmlelement (struct node const * node, char const * name);
 *
 *   node.h
 *   
 *   search node for the named element node; return the node address;
 *   
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *   
 *--------------------------------------------------------------------*/

#ifndef XMLELEMENT_SOURCE
#define XMLELEMENT_SOURCE

#include <string.h>

#include "../nodes/node.h"

struct node const * xmlelement (struct node const * node, char const * name) 

{
	if (node) 
	{
		node = node->below;
	}
	while (node) 
	{
		if (node->type == NODE_ELEM) 
		{
			struct node const * temp;
			if (!strcmp (node->text, name)) 
			{
				break;
			}
			temp = xmlelement (node, name);
			if (temp) 
			{
				return (temp);
			}
		}
		node=node->after;
	}
	return (node);
}


#endif

