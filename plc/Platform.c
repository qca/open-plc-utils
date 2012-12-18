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
 *   signed Platform (struct channel * channel, const uint8_t device []);
 *   
 *   plc.h
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef PLATFORM_SOURCE
#define PLATFORM_SOURCE

#include <memory.h>
#include <errno.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/symbol.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

signed Platform (struct channel * channel, const uint8_t device []) 

{
	struct message message;
	ssize_t packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* confirm = (struct vs_sw_ver_confirm *) (&message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (&message, 0, sizeof (message));
	EthernetHeader (&message.ethernet, device, channel->host, channel->type);
	QualcommHeader (&message.qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
	if (sendpacket (channel, &message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) > 0) 
	{
		while ((packetsize = readpacket (channel, &message, sizeof (message))) > 0) 
		{
			if (!UnwantedMessage (&message, packetsize, 0, (VS_SW_VER | MMTYPE_CNF))) 
			{
				chipset (confirm);
				printf (" %s", chipsetname (confirm->MDEVICEID));
				printf (" %s", confirm->MVERSION);
				return (0);
			}
		}
	}
	return (-1);
}


#endif

