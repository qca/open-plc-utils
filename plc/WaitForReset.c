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
 *   signed WaitForReset (struct plc * plc, char string [], size_t length);
 *
 *   plc.h
 *
 *   poll the device using VS_SW_VER messages until no confirmation 
 *   messages are received indicating that the firmware has stopped 
 *   running; return 0 if the firmware stops within the allotted time
 *   or -1 if it does not or if transmission errors occur;
 *
 *   this function cannot distinguish between a software reset and 
 *   a power failure;
 *
 *   retry is number of times to poll the device before returniung
 *   an error to indicate that the device failed to reset; timer is
 *   is the time between poll attempts;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WAITFORRESET_SOURCE
#define WAITFORRESET_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <sys/time.h>

#include "../plc/plc.h"
#include "../tools/timer.h"
#include "../tools/error.h"

signed WaitForReset (struct plc * plc, char string [], size_t length) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_sw_ver_request *) (message);
	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* confirm = (struct vs_sw_ver_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (string, 0, length);
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_SW_VER | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		if (!plc->packetsize) 
		{
			memcpy (string, confirm->MVERSION, confirm->MVERLENGTH);
			return (0);
		}
	}
	return (-1);
}


#endif

