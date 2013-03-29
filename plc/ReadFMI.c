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
 *   signed ReadFMI (struct plc * plc, uint8_t MMV, uint16_t MMTYPE);
 *
 *   plc.h
 *
 *   read a fragmented message and return a pointer to a buffer that
 *   contains the concatenated message fragments; The buffer address
 *   is returned in plc->content; the calling function must free the
 *   buffer when done;
 *
 *   buffer length is computed from the number of fragments returned
 *   in the FMI field of the first fragment; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READFMI_SOURCE
#define READFMI_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed ReadFMI (struct plc * plc, uint8_t MMV, uint16_t MMTYPE) 

{
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed frame 
	{
		struct ethernet_std ethernet;
		struct homeplug_fmi homeplug;
		uint8_t content [ETHERMTU - sizeof (struct homeplug_fmi)];
	}
	* frame = (struct frame *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	byte * content = (byte *)(0);
	unsigned count;
	unsigned index;
	if (ReadMME (plc, MMV, MMTYPE) <= 0) 
	{
		return (-1);
	}
	index = ((frame->homeplug.FMID >> 0) & 0x0F);
	count = ((frame->homeplug.FMID >> 4) & 0x0F);

#if 0

	printf ("index=%d count=%d batch=%d\n", index, count, frame->homeplug.FMSN);

#endif

	content = plc->content = calloc ((count + 1), sizeof (frame->content));
	if (!content) 
	{
		return (-1);
	}
	memcpy (content, frame->content, sizeof (frame->content));
	content += sizeof (frame->content);
	while (index < count) 
	{
		if (ReadMME (plc, MMV, MMTYPE) <= 0) 
		{
			free (plc->content);
			plc->content = NULL;
			return (-1);
		}
		index = ((frame->homeplug.FMID >> 0) & 0x0F);
		count = ((frame->homeplug.FMID >> 4) & 0x0F);
		memcpy (content, frame->content, sizeof (frame->content));
		content += sizeof (frame->content);
	}

#if 0

	hexdump (plc->content, 0, content - (byte *)(plc->content), stdout);

#endif

	return ((signed)(content - (byte *)(plc->content)));
}


#endif

