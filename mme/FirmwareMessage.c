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
 *   signed FirmwareMessage (void const * memory);
 *
 *   mme.h
 *
 *   intercept and print Qualcomm Atheros vendor-specific VS_ARPC 
 *   messages on stdout; this message type is used for diagnostic 
 *   reporting and should not appear in released firmware;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FIRMWAREMESSAGE_SOURCE
#define FIRMWAREMESSAGE_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/memory.h" 
#include "../tools/endian.h" 
#include "../mme/mme.h"

signed FirmwareMessage (void const * memory) 

{
	const struct message * message = (const struct message *)(memory);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	static struct qualcomm_std header_arpc = 
	{
		0,
		0,
		{
			0x00,
			0xB0,
			0x52
		}
	};
	struct __packed vs_arpc_indicate 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint16_t RDATALENGTH;
		uint8_t RDATAOFFSET;
		uint8_t RDATA [1];
	}
	* indicate = (struct vs_arpc_indicate *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	header_arpc.MMTYPE = HTOLE16 (VS_ARPC | MMTYPE_IND);
	if (!memcmp (&indicate->qualcomm, &header_arpc, sizeof (header_arpc))) 
	{
		ARPCPrint (stderr, &indicate->RDATA [indicate->RDATAOFFSET], LE16TOH (indicate->RDATALENGTH) - indicate->RDATAOFFSET);
		return (-1);
	}
	return (0);
}


#endif

