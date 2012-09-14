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
 *   signed WaitForAssoc2 (struct plc * plc);
 *
 *   plc.h
 *
 *   poll the device using VS_NW_INFO messages and examine confirmation
 *   messages until the device reports that a network exists and has at
 *   least one station present on that network; return 0 if the device
 *   associates within the allotted time or -1 if it does not or if 
 *   transmission errors occur;
 *
 *   retry is number of times to poll the device before returing an 
 *   error to indicate that the device di not associate in the allotted
 *   time; timer is the time between poll attempts;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WAITFORASSOC2_SOURCE
#define WAITFORASSOC2_SOURCE

#include <unistd.h>
#include <stdint.h>
#include <memory.h>
#include <sys/time.h>

#include "../plc/plc.h"
#include "../tools/timer.h"
#include "../tools/error.h"

signed WaitForAssoc2 (struct plc * plc) 

{
	extern byte broadcast [ETHER_ADDR_LEN];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

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

	Request (plc, "Wait for Assoc");
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
		FragmentHeader (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 1, (VS_NW_INFO | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_START_TIMER);
		}
		if (plc->packetsize) 
		{
			network = (struct network *)(&networks->networks);
			while (networks->NUMAVLNS--) 
			{
				station = (struct station *)(&network->stations);
				while (network->NUMSTAS--) 
				{
					if (memcmp (station->MAC, broadcast, sizeof (broadcast))) 
					{
						return (0);
					}
					station++;
				}
				network = (struct network *)(station);
			}
		}
	}
	return (-1);
}


#endif

