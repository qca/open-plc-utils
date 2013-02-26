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
 *   signed NetworkDevices2 (struct plc * plc, void * memory, size_t extent);
 *
 *   plc.h
 *
 *   return a list powerline network device addresses on a powerline
 *   network; the list consists of a known device plus all others on
 *   the same powerline network; the device is defined by the channel
 *   peer address and appears first in the list;
 *
 *   the device address must be explicit; it cannot be the broadcast
 *   or localcast address;
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

#ifndef NETWORKDEVICES2_SOURCE
#define NETWORKDEVICES2_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed NetworkDevices2 (struct plc * plc, void * memory, size_t extent) 

{
	extern const byte broadcast [ETHER_ADDR_LEN];
	extern const byte localcast [ETHER_ADDR_LEN];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	uint8_t * origin = (byte *)(memory);
	uint8_t * offset = (byte *)(memory);

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

	ssize_t packetsize;
	if (!memcmp (channel->peer, broadcast, sizeof (channel->peer))) 
	{
		error (1, EINVAL, "Can't use broadcast address");
	}
	if (!memcmp (channel->peer, localcast, sizeof (channel->peer))) 
	{
		error (1, EINVAL, "Can't use localcast address");
	}
	memset (memory, 0, extent);
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}
	while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
	{
		if (UnwantedMessage (message, packetsize, 0, (VS_NW_INFO | MMTYPE_CNF))) 
		{
			continue;
		}
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--) 
		{
			if (extent < sizeof (request->ethernet.OSA)) 
			{
				break;
			}
			memcpy (offset, request->ethernet.OSA, sizeof (request->ethernet.OSA));
			offset += sizeof (request->ethernet.OSA);
			extent -= sizeof (request->ethernet.OSA);
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS--) 
			{
				if (extent < sizeof (station->MAC)) 
				{
					break;
				}
				memcpy (offset, station->MAC, sizeof (station->MAC));
				offset += sizeof (station->MAC);
				extent -= sizeof (station->MAC);
				station++;
			}
			network = (struct network *)(station);
		}
	}
	return ((offset - origin) / ETHER_ADDR_LEN);
}


#endif

