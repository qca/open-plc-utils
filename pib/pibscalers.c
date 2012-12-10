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
 *   uint16_t pibscalers (struct _file_ * pib);
 *
 *   return the number of scalers (carriers) based on the FWVERSION
 *   stored in a .pib file; this function is a fix so that older
 *   toolkit programs will work with newer chipsets;
 *
 *   old chipsets support 1155 carriers; new chipsets support 2880
 *   carriers; 
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

#ifndef PIBSCALERS_SOURCE
#define PIBSCALERS_SOURCE

#include <unistd.h>
#include <errno.h>

#include "../plc/plc.h"
#include "../pib/pib.h"
#include "../tools/error.h"

uint16_t pibscalers (struct _file_ * pib) 

{
	struct pib_header pib_header;
	if (read (pib->file, &pib_header, sizeof (pib_header)) != sizeof (pib_header)) 
	{
		error (1, errno, "%s", pib->name);
	}
	if (pib_header.FWVERSION < 0x05) 
	{
		return (INT_CARRIERS);
	}
	return (AMP_CARRIERS);
}


#endif

