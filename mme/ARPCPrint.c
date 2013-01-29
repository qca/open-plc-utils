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
 *   void ARPCPrint (FILE * fp, void const * memory, size_t extent);
 *
 *   mme.h
 *
 *   print formatted VS_ARPC payload on the specified output stream;
 *   this implementation is generic; memory is the start address of
 *   the message data (&RDATA [RDATAOFFSET]) and extent is the data 
 *   length (RDATALENGTH); the call might look like this ...
 *
 *   ARPCPrint (fp, &ARPC->RDATA [ARPC->RDATAOFFSET], LE16TOH (ARPC->RDATALENGTH) - ARPC->RDATAOFFSET);
 *
 *   ... where LE16TOH () performs 16-bit host endian conversion;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *	Florian Fainelli <f.fainelli@gmail.com>
 *
 *--------------------------------------------------------------------*/

#ifndef ARPCPRINT_SOURCE
#define ARPCPRINT_SOURCE

#include <stdio.h>
#include <stddef.h>

#include "../tools/types.h"
#include "../tools/endian.h"
#include "../tools/memory.h"
#include "../mme/mme.h"

void ARPCPrint (FILE * fp, void const * memory, size_t extent) 

{

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

	uint32_t * argp = (uint32_t *)(&data->LIST [LE16TOH (data->ARGOFFSET)]);
	uint16_t argc = LE16TOH (data->ARGLENGTH) >> 2;
	while (argc--) 
	{
		*argp = LE32TOH (*argp);
		argp++;
	}

#if defined (__UCLIBC__) || defined (__FREESCALE__) 

/*
 *	This is a temporary fix so that we can compile program plctest for the PL16; A better solution is needed;
 */

	argp = (uint32_t *)(&data->LIST [LE16TOH (data->ARGOFFSET)]);
	fprintf (fp, (char *)(&data->LIST [LE16TOH (data->STROFFSET)]), argp [0], argp [1], argp [2], argp [3], argp [4], argp [5], argp [6], argp [7], argp [8], argp [9], argp [10], argp [11], argp [12], argp [13], argp [14], argp [15], argp [16], argp [17], argp [18], argp [19], argp [20], argp [21], argp [22], argp [23], argp [24], argp [25], argp [26], argp [27], argp [28], argp [29], argp [30], argp [31], argp [32], argp [33], argp [34], argp [35], argp [36], argp [37], argp [38], argp [39]);

#else

	vfprintf (fp, (char *)(&data->LIST [LE16TOH (data->STROFFSET)]), (void *)(&data->LIST [LE16TOH (data->ARGOFFSET)]));

#endif

	fprintf (fp, "\n");
	fflush (fp);
	return;
}


#endif

