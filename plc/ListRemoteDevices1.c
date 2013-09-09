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
 *   signed ListRemoteDevices1 (struct plc * plc, char const * space, char const * comma);
 *
 *   print the neighbor device list on stdout;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef LISTREMOTEDEVICES1_SOURCE
#define LISTREMOTEDEVICES1_SOURCE

#include <stdio.h>

#include "../tools/number.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

signed ListRemoteDevices1 (struct plc * plc, char const * space, char const * comma)

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
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_nw_info_request *)(message);
	struct __packed vs_nw_info_confirm
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t content [1];
	}
	* confirm = (struct vs_nw_info_confirm *)(message);
	struct __packed station
	{
		uint8_t MAC [ETHER_ADDR_LEN];
		uint8_t TEI;
		uint8_t BDA [ETHER_ADDR_LEN];
		uint8_t AVGTX;
		uint8_t AVGRX;
	}
	* station;
	struct __packed network
	{
		uint8_t NID [7];
		uint8_t SNID;
		uint8_t TEI;
		uint8_t ROLE;
		uint8_t CCO_MAC [ETHER_ADDR_LEN];
		uint8_t CCO_TEI;
		uint8_t NUMSTAS;
		struct station stations [1];
	}
	* network;
	struct __packed networks
	{
		uint8_t NUMAVLNS;
		struct network networks [1];
	}
	* networks = (struct networks *) (confirm->content);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (&message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_NW_INFO | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0)
	{
		return (-1);
	}
	while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0)
	{
		if (UnwantedMessage (message, packetsize, 0, (VS_NW_INFO | MMTYPE_CNF)))
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
			while (network->NUMSTAS--)
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

