/*====================================================================*
 *   
 *   Copyright (c) 2011 Qualcomm Atheros Inc.
 *   
 *   Permission to use, copy, modify, and/or distribute this software 
 *   for any purpose with or without fee is hereby granted, provided 
 *   that the above copyright notice and this permission notice appear 
 *   in all copies.
 *   
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
 *   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL  
 *   THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
 *   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 *   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 *   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 *   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *   
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   char const * reword (code_t code, struct _code_ const list [], size_t size);
 *
 *   symbol.h
 *   
 *   search a _code_ list by code and return the associated name; return the
 *   corresponding name on success or NULL on failure;
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2006-2009 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef REWORD_SOURCE
#define REWORD_SOURCE

#include <unistd.h>
#include <string.h>

#include "../tools/symbol.h"

char const * reword (code_t code, struct _code_ const list [], size_t size) 

{
	struct _code_ const * item = list;
	while ((unsigned)(item - list) < size) 
	{
		if (item->code == code) 
		{
			return (item->name);
		}
		item++;
	}
	return (NULL);
}


#endif

