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
 *   signed Transmit (struct plc * plc, byte source [], byte target []) ;
 *
 *   send TCP/IP frames to a remote powerline device to establish 
 *   the source device TX PHY rate and remote device RX PHY rate;
 *
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Qualcomm Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *--------------------------------------------------------------------*/

#ifndef TRANSMIT_SOURCE
#define TRANSMIT_SOURCE

#include <memory.h>
#include <errno.h>
#include <sys/time.h>

#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/types.h"
#include "../tools/timer.h"
#include "../plc/plc.h"

signed Transmit (struct plc * plc, byte source [], byte target []) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;
	if (_allclr (plc->flags, PLC_SILENCE)) 
	{
		char sourcename [ETHER_ADDR_LEN * 3];
		char targetname [ETHER_ADDR_LEN * 3];
		hexdecode (source, ETHER_ADDR_LEN, sourcename, sizeof (sourcename));
		hexdecode (target, ETHER_ADDR_LEN, targetname, sizeof (targetname));
		fprintf (stderr, "%s %s %s\n", channel->ifname, sourcename, targetname);
	}
	memset (message, 0xA5, sizeof (* message));
	EthernetHeader (message, target, source, ETHERTYPE_IP);
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		if (sendpacket (channel, message, sizeof (* message)) <= 0) 
		{
			error (1, ECANCELED, CHANNEL_CANTSEND);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		SLEEP (100);
	}
	return (0);
}


#endif

