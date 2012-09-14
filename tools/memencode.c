/*====================================================================*
 *   
 *   Copyright (c) 2011 by Qualcomm Atheros.
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
 *   unsigned memencode (void * memory, size_t extent, char const * format, char const * string);
 *
 *   memory.h
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2006-2008 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *--------------------------------------------------------------------*/

#ifndef MEMENCODE_SOURCE
#define MEMENCODE_SOURCE

#include <string.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../pib/pib.h"

static size_t memstring (void * memory, size_t extent, char const * format, char const * string, size_t length) 

{
	if (extent < length) 
	{
		error (1, ECANCELED, "Overflow at %s %s", format, string);
	}
	memset (memory, 0, length);
	strncpy (memory, string, length-1);
	return (length);
}

size_t memencode (void * memory, size_t extent, char const * format, char const * string) 

{
	if (!strcmp (format, "byte")) 
	{
		uint8_t * number = (uint8_t *)(memory);
		if (extent < sizeof (* number)) 
		{
			error (1, ECANCELED, "Overflow at %s %s", format, string);
		}
		* number = (uint8_t)(basespec (string, 0, sizeof (* number)));
		return (sizeof (* number));
	}
	if (!strcmp (format, "word")) 
	{
		uint16_t * number = (uint16_t *)(memory);
		if (extent < sizeof (* number)) 
		{
			error (1, ECANCELED, "Overflow at %s %s", format, string);
		}
		* number = (uint16_t)(basespec (string, 0, sizeof (* number)));
		* number = HTOLE16 (* number);
		return (sizeof (* number));
	}
	if (!strcmp (format, "long")) 
	{
		uint32_t * number = (uint32_t *)(memory);
		if (extent < sizeof (* number)) 
		{
			error (1, ECANCELED, "Overflow at %s %s", format, string);
		}
		* number = (uint32_t)(basespec (string, 0, sizeof (* number)));
		* number = HTOLE32 (* number);
		return (sizeof (* number));
	}
	if (!strcmp (format, "huge")) 
	{
		uint64_t * number = (uint64_t *)(memory);
		if (extent < sizeof (* number)) 
		{
			error (1, ECANCELED, "Overflow at %s %s", format, string);
		}
		* number = (uint64_t)(basespec (string, 0, sizeof (* number)));
		* number = HTOLE64 (* number);
		return (sizeof (* number));
	}
	if (!strcmp (format, "text")) 
	{
		extent = (unsigned)(strlen (string));
		memcpy (memory, string, extent);
		return (extent);
	}
	if (!strcmp (format, "data")) 
	{
		extent = (unsigned)(dataspec (string, memory, extent));
		return (extent);
	}
	if (!strcmp (format, "fill")) 
	{
		extent = (unsigned)(uintspec (string, 0, extent));
		memset (memory, ~0, extent);
		return (extent);
	}
	if (!strcmp (format, "zero")) 
	{
		extent = (unsigned)(uintspec (string, 0, extent));
		memset (memory, 0, extent);
		return (extent);
	}
	if (!strcmp (format, "skip")) 
	{
		extent = (unsigned)(uintspec (string, 0, extent));
		return (extent);
	}

#if 1

/*
 *   tr-069 specific fields that don't really belong in the PIB;
 */

	if (!strcmp (format, "adminusername") || !strcmp (format, "adminpassword") || !strcmp (format, "accessusername")) 
	{
		return (memstring (memory, extent, format, string, PIB_NAME_LEN + 1));
	}
	if (!strcmp (format, "accesspassword")) 
	{
		return (memstring (memory, extent, format, string, PIB_HFID_LEN + 1));
	}
	if (!strcmp (format, "username") || !strcmp (format, "password") || !strcmp (format, "url")) 
	{
		return (memstring (memory, extent, format, string, PIB_TEXT_LEN + 1));
	}

#endif

#if 1

/*
 *   HPAV specific fields that belong in the PIB;
 */

	if (!strcmp (format, "hfid")) 
	{
		return (memstring (memory, extent, format, string, PIB_HFID_LEN));
	}
	if (!strcmp (format, "mac")) 
	{
		return (bytespec (string, memory, ETHER_ADDR_LEN));
	}
	if (!strcmp (format, "key")) 
	{
		return (bytespec (string, memory, PIB_KEY_LEN));
	}

#endif

	error (1, ENOTSUP, "%s", format);
	return (0);
}


#endif

