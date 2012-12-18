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
 *   PLCTopology.c
 *   
 *   plc.h
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef PLCTOPOLOGY_SOURCE
#define PLCTOPOLOGY_SOURCE

#include <memory.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/symbol.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

/*====================================================================*
 *
 *   signed PLCPlatform (struct channel * channel, struct plcstation * station);
 * 
 *   populate plcstation structure with hardware and firmware version
 *   string using a VS_SW_VER message;
 *
 *   extern char const * chipset [] contains chipset name strings in
 *   order of chipset code but function chipset() must be called to 
 *   insert the true code into the confirmation message because some
 *   chipsets return the wrong code; alien technology and voodoo are
 *   needed;
 *
 *--------------------------------------------------------------------*/

static signed PLCPlatform (struct channel * channel, struct plcstation * plcstation) 

{
	struct message message;
	ssize_t packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_sw_ver_request *) (&message);
	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICE;
		uint8_t MLENGTH;
		char MSTRING [0x80];
	}
	* confirm = (struct vs_sw_ver_confirm *) (&message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (&message, 0, sizeof (message));
	EthernetHeader (&request->ethernet, plcstation->MAC, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
	if (sendpacket (channel, &message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) > 0) 
	{
		while ((packetsize = readpacket (channel, &message, sizeof (message))) > 0) 
		{
			if (!UnwantedMessage (&message, packetsize, 0, (VS_SW_VER | MMTYPE_CNF))) 
			{
				chipset (confirm);
				strncpy (plcstation->hardware, chipsetname (confirm->MDEVICE), sizeof (plcstation->hardware));
				strncpy (plcstation->firmware, confirm->MSTRING, sizeof (plcstation->firmware));
				return (0);
			}
		}
	}
	return (-1);
}


/*====================================================================*
 *
 *   signed PLCIdentity (struct channel * channel, struct plcstation * station);
 * 
 *   populate plcstation structure with the device identity using a
 *   VS_MFG_STRING message; 
 *
 *--------------------------------------------------------------------*/

static signed PLCIdentity (struct channel * channel, struct plcstation * plcstation) 

{
	struct message message;
	ssize_t packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_mfg_string_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_mfg_string_request *) (&message);
	struct __packed vs_mfg_string_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MLENGTH;
		char MSTRING [0x40];
	}
	* confirm = (struct vs_mfg_string_confirm *) (&message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (&message, 0, sizeof (message));
	EthernetHeader (&request->ethernet, plcstation->MAC, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MFG_STRING | MMTYPE_REQ));
	if (sendpacket (channel, &message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) > 0) 
	{
		while ((packetsize = readpacket (channel, &message, sizeof (message))) > 0) 
		{
			if (!UnwantedMessage (&message, packetsize, 0, (VS_MFG_STRING | MMTYPE_CNF))) 
			{
				strncpy (plcstation->identity, confirm->MSTRING, sizeof (plcstation->identity));
				return (0);
			}
		}
	}
	return (-1);
}


/*====================================================================*
 *
 *   signed PLCTopology (struct channel * channel, struct message * message, struct plctopology * plctopology) 
 * 
 *   populate a plctopology structure with network information; the 
 *   logic this is elusive due to the way the VS_NW_INFO message is
 *   structured;
 *
 *   the host can have many interfaces and each interface can have 
 *   many powerline devices connected to it; each powerline device
 *   can bridge to an independent powerline network having a unique
 *   set of member devices; alternately, some powerline devices can  
 *   be members of the same powerline network;
 *
 *   INT6x00 chipsets have an 8-bit PHY rate while AR7x00 chipsets
 *   have a 16-bit PHY rate; this means that AR7x00 PHY rates need
 *   endian conversion;
 *
 *--------------------------------------------------------------------*/

signed PLCTopology (struct channel * channel, struct message * message, struct plctopology * plctopology) 

{
	ssize_t packetsize;

#if defined(INT6x00)

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

#elif defined (AR7x00)

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

#else
#error "Unspecified Atheros chipset"
#endif

	struct plcnetwork * plcnetwork = (struct plcnetwork *)(&plctopology->plcnetwork);
	struct plcstation * plcstation;
	byte bridges [255] [ETHER_ADDR_LEN];
	unsigned bridge = LocalDevices (channel, message, bridges, (size_t)(sizeof (bridges)));
	while (bridge--) 
	{
		memset (message, 0, sizeof (* message));
		memcpy (channel->peer, bridges [bridge], sizeof (channel->peer));

#if defined (INT6x00)

		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_NW_INFO | MMTYPE_REQ));

#elif defined (AR7x00)

		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		FragmentHeader (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));

#else
#error "Unspecified Atheros chipset"
#endif

		if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
		{
			error (1, errno, CHANNEL_CANTSEND);
		}
		while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
		{

#if defined (INT6x00)

			if (UnwantedMessage (confirm, packetsize, 0, (VS_NW_INFO | MMTYPE_CNF))) 

#elif defined (AR7x00)

			if (UnwantedMessage (confirm, packetsize, 1, (VS_NW_INFO | MMTYPE_CNF))) 

#else
#error "Unspecified Atheros chipset"
#endif

			{
				continue;
			}
			network = (struct network *)(&networks->networks);
			plcstation = (struct plcstation *)(&plcnetwork->plcstation);
			memset (plcnetwork, 0, sizeof (* plcnetwork));
			memset (plcstation, 0, sizeof (* plcstation));
			plcstation->LOC = !memcmp (confirm->ethernet.OSA, bridges [bridge], sizeof (confirm->ethernet.OSA));
			plcstation->CCO = !memcmp (confirm->ethernet.OSA, network->CCO_MAC, sizeof (confirm->ethernet.OSA));
			plcstation->TEI = network->TEI;
			memcpy (plcstation->MAC, confirm->ethernet.OSA, sizeof (plcstation->MAC));
			memcpy (plcstation->BDA, confirm->ethernet.ODA, sizeof (plcstation->BDA));
			PLCPlatform (channel, plcstation);
			plcnetwork->plcstations++;
			plcstation++;
			while (networks->NUMAVLNS--) 
			{
				station = (struct station *)(&network->stations);
				while (network->NUMSTAS--) 
				{
					memset (plcstation, 0, sizeof (* plcstation));
					plcstation->LOC = !memcmp (station->BDA, channel->host, sizeof (station->BDA));
					plcstation->CCO = !memcmp (station->MAC, network->CCO_MAC, sizeof (station->MAC));
					plcstation->TEI = station->TEI;
					memcpy (plcstation->MAC, station->MAC, sizeof (plcstation->MAC));
					memcpy (plcstation->BDA, station->BDA, sizeof (plcstation->BDA));

#if defined (INT6x00)

					plcstation->TX = station->AVGTX;
					plcstation->RX = station->AVGRX;

#elif defined (AR7x00)

					plcstation->TX = LE16TOH (station->AVGTX);
					plcstation->RX = LE16TOH (station->AVGRX);

#else
#error "Unspecified Atheros chipset"
#endif

					PLCPlatform (channel, plcstation);
					PLCIdentity (channel, plcstation);
					plcnetwork->plcstations++;
					plcstation++;
					station++;
				}
				plctopology->plcnetworks++;
				plcnetwork = (struct plcnetwork *)(plcstation);
				network = (struct network *)(station);
			}
		}
	}
	return (0);
}


/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

