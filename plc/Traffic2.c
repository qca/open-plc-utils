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
 *   signed Traffic2 (struct plc * plc);
 *   
 *   generate bi-directional traffic between a local device and all
 *   remote devices on a powerline network; remote devices need not
 *   be connected; no traffic is generated between remote devices;
 *
 *   this function is similar to NetworkTraffic but takes less time
 *   since not all powerline paths are exercised and those that are
 *   can be exercised in parallel; 
 *
 *   this function is for AR7x00 chipsets due to chipset design
 *   changes; use Traffic1 for INT6x00 chipsets; 
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

#ifndef TRAFFIC2_SOURCE
#define TRAFFIC2_SOURCE

#include <memory.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../mme/mme.h"
#include "../plc/plc.h"

signed Traffic2 (struct plc * plc) 

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

	byte bridgelist [255] [ETHER_ADDR_LEN];
	unsigned bridges = LocalDevices (channel, message, bridgelist, sizeof (bridgelist));
	while (bridges--) 
	{
		byte devicelist [255] [ETHER_ADDR_LEN];
		unsigned devices = 0;
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, bridgelist [bridges], channel->host, channel->type);
		QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
		plc->packetsize = ETHER_MIN_LEN - ETHER_CRC_LEN;
		if (SendMME (plc) <= 0) 
		{
			error (0, errno, CHANNEL_CANTSEND);
			continue;
		}
		if (ReadMME (plc, 1, (VS_NW_INFO | MMTYPE_CNF)) <= 0) 
		{
			error (0, errno, CHANNEL_CANTREAD);
			continue;
		}
		memcpy (devicelist [devices++], request->ethernet.OSA, sizeof (devicelist [0]));
		network = (struct network *)(&networks->networks);
		while (networks->NUMAVLNS--) 
		{
			station = (struct station *)(&network->stations);
			while (network->NUMSTAS--) 
			{
				memcpy (devicelist [devices++], station->MAC, sizeof (devicelist [0]));
				station++;
			}
			network = (struct network *)(station);
		}
		while (devices-- > 1) 
		{
			Transmit (plc, devicelist [0], devicelist [devices]);
			Antiphon (plc, devicelist [devices], devicelist [0]);
		}
	}
	return (0);
}


#endif

