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
 *   void ARPCWrite (signed fd, void const * memory, size_t extent);
 *
 *   mme.h
 *
 *   write an unformatted VS_ARPC payload on the specified output 
 *   stream; this implementation is generic; memory is the start 
 *   address of the message data (&RDATA [RDATAOFFSET]) and extent 
 *   is the data length (RDATALENGTH); the call might look like 
 *   this ...
 *
 *   ARPCWrite (fd, &ARPC->RDATA [ARPC->RDATAOFFSET], LE16TOH (ARPC->RDATALENGTH - ARPC->RDATAOFFSET));
 *
 *   ... where LE16TOH () performs 16-bit host endian conversion;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef ARPCWRITE_SOURCE
#define ARPCWRITE_SOURCE

#include <stdio.h>

#include "../tools/types.h"
#include "../tools/endian.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../mme/mme.h"

void ARPCWrite (signed fd, void const * memory, size_t extent) 

{

#if 0

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_arpc_data 
	{
		uint32_t BYPASS;
		uint16_t ARPCID;
		uint16_t DATALENGTH;
		uint8_t DATAOFFSET;
		uint8_t RESERVED [3];
		uint16_t ARGOFFSET;
		uint16_t STROFFSET;
		uint16_t ARGLENGTH;
		uint16_t STRLENGTH;
		uint8_t LIST [1];
	}
	* data = (struct vs_arpc_data *)(memory);

#ifndef __GNUC__
#pragma pack (pop)
#endif

#endif
    uint16_t length = (uint16_t)(extent);
	if (isatty (fd)) 
	{
		hexwrite (fd, &length, sizeof (length));
		hexwrite (fd, memory, length);

#if defined (_WIN32)

		write (fd, "\r", sizeof (char));

#endif

		write (fd, "\n", sizeof (char));
	}
	else 
	{
		write (fd, &length, sizeof (length));
		write (fd, memory, length);
	}
	return;
}


#endif

