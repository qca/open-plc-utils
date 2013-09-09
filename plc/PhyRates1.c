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
 *   signed PhyRates1 (struct plc * plc);
 *
 *   An INT6x00 only version;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PHYRATES1_SOURCE
#define PHYRATES1_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed PhyRates1 (struct plc * plc)

{
	extern const byte broadcast [ETHER_ADDR_LEN];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

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
		uint8_t data [1];
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
	* networks = (struct networks *) (&confirm->data);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_NW_INFO | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_NW_INFO | MMTYPE_CNF)) > 0)
	{
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--)
		{
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS--)
			{
				if (memcmp (station->MAC, broadcast, sizeof (broadcast)))
				{
					char address [ETHER_ADDR_LEN * 3];
					hexdecode (station->MAC, sizeof (station->MAC), address, sizeof (address));
					if (_anyset (plc->flags, PLC_UNCODED_RATES))
					{
						station->AVGTX = ((station->AVGTX * 21) >> 4);
						station->AVGRX = ((station->AVGRX * 21) >> 4);
					}
					Display (plc, "%s TX %03d mbps", address, station->AVGTX);
					Display (plc, "%s RX %03d mbps", address, station->AVGRX);
					if (_anyset (plc->flags, PLC_BAILOUT))
					{
						if (!station->AVGTX || !station->AVGRX)
						{
							error ((plc->flags & PLC_BAILOUT), ECANCELED, PLC_BADVALUE);
							return (-1);
						}
					}
				}
				station++;
			}
			network = (struct network *)(station);
		}
	}
	return (0);
}


#endif

