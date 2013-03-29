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
 *   signed RxRates2 (struct plc * plc);
 *
 *   plc.h
 *
 *   An AR7x00 only version;
 *
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef RXRATES2_SOURCE
#define RXRATES2_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

signed RxRates2 (struct plc * plc) 

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
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 1, (VS_NW_INFO | MMTYPE_CNF)) > 0) 
	{
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--) 
		{
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS--) 
			{
				if (memcmp (station->MAC, broadcast, sizeof (broadcast))) 
				{
					station->AVGTX = LE16TOH (station->AVGTX);
					station->AVGRX = LE16TOH (station->AVGRX);
					if (_anyset (plc->flags, PLC_UNCODED_RATES))
					{
						station->AVGTX = ((station->AVGTX * 21) >> 4);
						station->AVGRX = ((station->AVGRX * 21) >> 4);
					}
					printf ("%d", _anyset (plc->flags, PLC_TXONLY)? station->AVGTX: station->AVGRX);
					if (network->NUMSTAS)
					{
						printf (", ");
					}
				}
				station++;
			}
			printf ("\n");
			network = (struct network *)(station);
		}
	}
	fflush (stdout);
	return (0);
}


#endif

