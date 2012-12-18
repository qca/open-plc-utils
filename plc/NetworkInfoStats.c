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
 *   signed NetworkInfoStats (struct plc * plc);
 *
 *   plc.h
 *
 *   Request network membership information from the peer device using
 *   the VS_NW_INFO_STATS message; 
 *
 *   This function is similar to function NetworkInformation() but the
 *   output format is different;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NETWORKINFOSTATS_SOURCE
#define NETWORKINFOSTATS_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"

signed NetworkInfoStats (struct plc * plc) 

{
	extern char const * StationRole [STATIONROLES];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

#if defined(INT6x00)

	struct __packed vs_ns_info_stats_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t FIRST_TEI;
	}
	* request = (struct vs_ns_info_stats_request *)(message);
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
		uint8_t ACCESS uint8_t NEIGHBOR_NETWORKS;
		uint8_t CCO_TEI;
		uint8_t NUMSTAS;
		struct station stations [1];
	}
	* network;
	struct __packed vs_ns_info_stats_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t FIRST_TEI;
		uint8_t IN_AVLN;
		struct network network;
	}
	* confirm = (struct vs_ns_info_stats_confirm *)(message);

#elif defined (AR7x00)

	struct __packed vs_ns_info_stats_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t MME_SUBVER;
		uint8_t RESERVED [3];
		uint8_t FIRST_TEI;
		UINT8_6 NUM_STAS;
	}
	* request = (struct vs_ns_info_stats_request *)(message);
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
		uint8_t Reserved1;
		uint8_t SNID;
		uint8_t TEI;
		uint8_t Reserved2 [2];
		uint8_t ROLE;
		uint8_t CCO_MAC [ETHER_ADDR_LEN];
		uint8_t ACCESS;
		uint8_t NEIGHBOR_NETWORKS;
		uint8_t CCO_TEI;
		uint8_t Reserved3 [7];
		uint8_t NUMSTAS;
		uint8_t Reserved4 [5];
		struct station stations [1];
	}
	* network;
	struct __packed vs_ns_info_stats_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t SUB_VERSION;
		uint8_t Reserved;
		uint16_t DATA_LEN;
		uint8_t FIRST_TEI;
		uint8_t NUM_STAS;
		uin16_t Reserved;
		uint8_t IN_AVLN;
		struct network network;
	}
	* confirm = (struct vs_ns_info_stats_confirm *)(message);

#else
#error "Unspecified Atheros chipset"
#endif

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Fetch Network Information");
	memset (message, 0, sizeof (* message));

#if defined (INT6x00)

	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_NW_INFO_STATS | MMTYPE_REQ));

#elif defined (AR7x00)

	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	FragmentHeader (&request->qualcomm, 1, (VS_NW_INFO_STATS | MMTYPE_REQ));

#else
#error "Unspecified Atheros chipset"
#endif

	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}

#if defined (INT6x00)

	while (ReadMME (plc, 0, (VS_NW_INFO_STATS | MMTYPE_CNF)) > 0) 

#elif defined (AR7x00)

	while (ReadMME (plc, 1, (VS_NW_INFO_STATS | MMTYPE_CNF)) > 0) 

#else
#error "Unspecified Atheros chipset"
#endif

	{
		char string [24];
		Confirm (plc, "Found %d Network(s)\n", networks->NUMAVLNS);
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--) 
		{
			printf ("\tnetwork->NID = %s\n", hexstring (string, sizeof (string), network->NID, sizeof (network->NID)));
			printf ("\tnetwork->SNID = %d\n", network->SNID);
			printf ("\tnetwork->TEI = %d\n", network->TEI);
			printf ("\tnetwork->ROLE = 0x%02X (%s)\n", network->ROLE, StationRole [network->ROLE]);
			printf ("\tnetwork->CCO_DA = %s\n", hexstring (string, sizeof (string), network->CCO_MAC, sizeof (network->CCO_MAC)));
			printf ("\tnetwork->CCO_TEI = %d\n", network->CCO_TEI);
			printf ("\tnetwork->STATIONS = %d\n", network->NUMSTAS);
			printf ("\n");
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS--) 
			{
				char * TX = "";
				char * RX = "";
				printf ("\t\tstation->MAC = %s\n", hexstring (string, sizeof (string), station->MAC, sizeof (station->MAC)));
				printf ("\t\tstation->TEI = %d\n", station->TEI);
				printf ("\t\tstation->BDA = %s\n", hexstring (string, sizeof (string), station->BDA, sizeof (station->BDA)));

#if defined (AR7x00)

				station->AVGTX = LE16TOH (station->AVGTX);
				station->AVGRX = LE16TOH (station->AVGRX);
				TX = ((station->COUPLING >> 4) & 0x0F)? "Alternate": "Primary";
				RX = ((station->COUPLING >> 0) & 0x0F)? "Alternate": "Primary";

#endif

				printf ("\t\tstation->AvgPHYDR_TX = %03d mbps %s\n", station->AVGTX, TX);
				printf ("\t\tstation->AvgPHYDR_RX = %03d mbps %s\n", station->AVGRX, RX);
				printf ("\n");
				station++;
			}
			network = (struct network *)(station);
		}
	}
	return (0);
}


#endif

