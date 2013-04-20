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
 *   signed RemoteDeviceList2 (struct plc * plc, char const * space, char const * comma);
 *
 *   print neighbor device list on stdout; 
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef REMOTEDEVICELIST2_SOURCE
#define REMOTEDEVICELIST2_SOURCE

#include <stdio.h>

#include "../tools/number.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

signed RemoteDeviceList2 (struct plc * plc, char const * space, char const * comma) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	ssize_t packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_nw_info_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
	}
	* request = (struct vs_nw_info_request *)(message);
	struct __packed vs_nw_info_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t SUB_VERSION;
		uint8_t Reserved;
		uint16_t DATA_LEN;
		uint8_t DATA [1];
	}
	* confirm = (struct vs_nw_info_confirm *)(message);
	struct __packed station 
	{
		uint8_t MAC [ETHER_ADDR_LEN];
		uint8_t TEI;
		uint8_t Reserved [3];
		uint8_t BDA [ETHER_ADDR_LEN];
		uint16_t AVGTX;
		uint8_t COUPLING;
		uint8_t Reserved3;
		uint16_t AVGRX;
		uint16_t Reserved4;
	}
	* station;
	struct __packed network 
	{
		uint8_t NID [7];
		uint8_t Reserved1 [2];
		uint8_t SNID;
		uint8_t TEI;
		uint8_t Reserved2 [4];
		uint8_t ROLE;
		uint8_t CCO_MAC [ETHER_ADDR_LEN];
		uint8_t CCO_TEI;
		uint8_t Reserved3 [3];
		uint8_t NUMSTAS;
		uint8_t Reserved4 [5];
		struct station stations [1];
	}
	* network;
	struct __packed networks 
	{
		uint8_t Reserved;
		uint8_t NUMAVLNS;
		struct network networks [1];
	}
	* networks = (struct networks *) (confirm->DATA);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (-1);
	}
	while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
	{
		if (UnwantedMessage (message, packetsize, 1, (VS_NW_INFO | MMTYPE_CNF))) 
		{
			continue;
		}
		if (_anyset (plc->flags, PLC_BRIDGE_LIST)) 
		{
			hexout (request->ethernet.OSA, sizeof (request->ethernet.OSA), HEX_EXTENDER, 0, stdout);
			if ((space) && (*space)) 
			{
				printf ("%s", space);
			}
		}
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--) 
		{
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS-- > 0) 
			{
				if (_anyset (plc->flags, PLC_REMOTE_LIST)) 
				{
					hexout (station->MAC, sizeof (station->MAC), HEX_EXTENDER, 0, stdout);
					if ((space) && (*space)) 
					{
						printf ("%s", space);
					}
				}
				station++;
			}
			network = (struct network *)(station);
		}
		if ((comma) && (*comma)) 
		{
			printf ("%s", comma);
		}
	}
	return (0);
}


#endif

