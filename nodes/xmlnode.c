/*====================================================================*
 *
 *   NODE * xmlnode (NODE * root);
 *
 *   node.h
 *   
 *   create an empty node and attach it as the last child below the
 *   root node;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef XMLNODE_SOURCE
#define XMLNODE_SOURCE

#include <memory.h>

#include "../nodes/node.h"
#include "../tools/memory.h"

NODE * xmlnode (NODE * root) 

{
	NODE * node = NEW (NODE);
	memset (node, 0, sizeof (NODE));
	if (!root) 
	{
		return (node);
	}
	if (root->below) 
	{
		NODE * temp = root->below;
		while (temp->after) 
		{
			temp = temp->after;
		}
		node->above = root;
		temp->after = node;
		node->prior = temp;
		return (node);
	}
	root->below = node;
	node->above = root;
	return (node);
}


#endif

