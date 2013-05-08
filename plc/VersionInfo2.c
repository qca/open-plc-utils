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
 *   signed VersionInfo2 (struct plc * plc);
 *   
 *   plc.h
 * 
 *   read hardware and software version information from an INT6000
 *   using a VS_SW_VER Request Message;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef VERSIONINFO2_SOURCE
#define VERSIONINFO2_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/memory.h"
#include "../tools/symbol.h"
#include "../plc/plc.h"

signed VersionInfo2 (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t COOKIE;
	}
	* request = (struct vs_sw_ver_request *) (message);
	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICE_CLASS;
		uint8_t MVERLENGTH;
		char MVERSION [254];
		uint32_t IDENT;
		uint32_t STEPPING_NUM;
		uint32_t COOKIE;
		uint32_t RSVD [6];
	}
	* confirm = (struct vs_sw_ver_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Request Version Information");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
	request->COOKIE = HTOLE32 (plc->cookie);
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_SW_VER | MMTYPE_CNF)) > 0) 
	{

#if 0

		fprintf (stderr, "DEVICE CLASS (%d)\n", LE32TOH (confirm->MDEVICE_CLASS));
		fprintf (stderr, "VERSION LENGTH (%d)\n", LE32TOH (confirm->MVERLENGTH));
		fprintf (stderr, "VERSION STRING [%s]\n", LE32TOH (confirm->MVERSION));
		fprintf (stderr, "IDENT [0x%08X]\n", LE32TOH (confirm->IDENT));
		fprintf (stderr, "STEPPING NUMBER [0x%08X]\n", LE32TOH (confirm->STEPPING_NUM));
		fprintf (stderr, "COOKIE [0x%08X]\n", LE32TOH (confirm->COOKIE));

#endif

		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			continue;
		}
		chipset (confirm);
		Display (plc, "%s %s", chipsetname (confirm->MDEVICE_CLASS), confirm->MVERSION);
	}
	return (0);
}


#endif

