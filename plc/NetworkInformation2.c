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
 *   signed NetworkInformation2 (struct plc * plc);
 *
 *   plc.h
 *
 *   Request network membership information for the peer device using 
 *   a VS_NW_INFO message; 
 *
 *   This function is similar to function NetworkInfo() but the output 
 *   format is different;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NETWORKINFORMATION2_SOURCE
#define NETWORKINFORMATION2_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed NetworkInformation2 (struct plc * plc) 

{
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

	memset (message, 0, sizeof (*message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}
	if (readpacket (channel, message, sizeof (* message)) <= 0) 
	{
		error (1, errno, CHANNEL_CANTREAD);
	}
	network = (struct network *)(&networks->networks);
	while (networks->NUMAVLNS--) 
	{
		char string [24];
		printf (" NID %20s", hexstring (string, sizeof (string), network->NID, sizeof (network->NID)));
		printf (" SNID %03d", network->SNID);
		printf ("\n");
		printf (" %s", (network->TEI == network->CCO_TEI)? "CCO": "STA");
		printf (" TEI %03d", network->TEI);
		printf (" MAC %17s", hexstring (string, sizeof (string), request->ethernet.OSA, sizeof (request->ethernet.OSA)));
		printf (" BDA %17s", hexstring (string, sizeof (string), request->ethernet.ODA, sizeof (request->ethernet.ODA)));
		printf ("\n");
		station = (struct station *)(&network->stations);
		while (network->NUMSTAS--) 
		{
			printf (" %s", (station->TEI == network->CCO_TEI)? "CCO": "STA");
			printf (" TEI %03d", station->TEI);
			printf (" MAC %17s", hexstring (string, sizeof (string), station->MAC, sizeof (station->MAC)));
			printf (" BDA %17s", hexstring (string, sizeof (string), station->BDA, sizeof (station->BDA)));
			printf (" TX %03d", station->AVGTX);
			printf (" RX %03d", station->AVGRX);
			printf ("\n");
			station++;
		}
		network = (struct network *)(station);
	}
	return (0);
}


#endif

