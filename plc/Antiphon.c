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
 *   signed Antiphon (struct plc * plc);
 *
 *   plc.h
 *
 *   instruct one powerline device to send trash frames to another 
 *   for a given period (in seconds) to establish source device RX 
 *   PHY rate and target device TX PHY rate; this does not work if
 *   the source device is also the local device, for some reason; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef ANTIPHON_SOURCE
#define ANTIPHON_SOURCE

#include <memory.h>
#include <errno.h>

#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

signed Antiphon (struct plc * plc, byte source [], byte target []) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_fr_lbk_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t DURATION;
		uint8_t RESERVED;
		uint16_t LENGTH;
		uint8_t PACKET [1038];
	}
	* request = (struct vs_fr_lbk_request *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	if (_allclr (plc->flags, PLC_SILENCE)) 
	{
		char sourcename [ETHER_ADDR_LEN * 3];
		char targetname [ETHER_ADDR_LEN * 3];
		hexdecode (source, ETHER_ADDR_LEN, sourcename, sizeof (sourcename));
		hexdecode (target, ETHER_ADDR_LEN, targetname, sizeof (targetname));
		fprintf (stderr, "%s %s %s\n", channel->ifname, sourcename, targetname);
	}
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, source, target, channel->type);
	QualcommHeader (&request->qualcomm, 0, 41036);
	request->DURATION = plc->timer;
	request->LENGTH = HTOLE16 (sizeof (request->PACKET));
	memset (request->PACKET, 0xA5, sizeof (request->PACKET));
	EthernetHeader (request->PACKET, target, source, ETHERTYPE_IP);
	plc->packetsize = sizeof (* request);
	if (SendMME (plc) <= 0) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}

#if 0

/*
 *	This causes a multi-device session to terminate if the device has recently
 *	been removed or powered-off; The device appears to be present but will not
 *	respond; Also, this terminates a session if the network is overloaded with
 *	traffic;
 */

	if (ReadMME (plc, 0, 41037) <= 0) 
	{
		error (1, errno, CHANNEL_CANTREAD);
	}

#endif

	sleep (plc->timer);
	return (0);
}


#endif

