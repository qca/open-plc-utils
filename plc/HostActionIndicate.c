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
 *   signed HostActionIndicate (struct plc * plc);
 *   
 *   plc.h
 * 
 *   This test plugin requests come action from a local host using a 
 *   VS_HOST_ACTION indication message; the message must be sent to a 
 *   host, not a device; it can be used to verify that your host 
 *   application correctly responds to device requests;
 *
 *   this function is provided so that you can emulate a device boot
 *   sequence by prompting a listening host to download firmware; it
 *   has no practical purpose other than development and test;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HOSTACTIONINDICATE_SOURCE
#define HOSTACTIONINDICATE_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/error.h"
#include "../tools/memory.h"

signed HostActionIndicate (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_host_action_ind 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MACTION;
		uint8_t MAJOR_VERSION;
		uint8_t MINOR_VERSION;
	}
	* indicate = (struct vs_host_action_ind *) (message);
	struct __packed vs_host_action_rsp 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* response = (struct vs_host_action_rsp *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Start Host Action");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&indicate->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&indicate->qualcomm, 0, (VS_HOST_ACTION | MMTYPE_IND));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	indicate->MACTION = plc->action;
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_HOST_ACTION | MMTYPE_RSP)) > 0) 
	{
		if (response->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			continue;
		}
		Confirm (plc, "Start.");
	}
	return (0);
}


#endif

