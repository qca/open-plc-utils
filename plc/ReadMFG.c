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
 *   signed ReadMFG (struct plc * plc, uint16_t MMTYPE);
 *
 *   plc.h
 *
 *   read a manufacturer specific management message of the given
 *   type in platform independent manner; return the number of bytes 
 *   read, 0 on timeout or -1 on error; 
 *
 *   see SendMME for the send counterpart to this function; 
 *
 *   see ReadMME for the vendor specific version of this function;
 *
 *   readpacket behaves like the read function but there are several 
 *   readpacket functions in the toolkit and each performs raw packet 
 *   i/o differently depending on environment; they all use a channel
 *   structure;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READMFG_SOURCE
#define READMFG_SOURCE

#include <memory.h>

#include "../plc/plc.h"
#include "../ether/channel.h"
#include "../tools/memory.h" 
#include "../tools/flags.h"

signed ReadMFG (struct plc * plc, uint8_t MMV, uint16_t MMTYPE) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	while ((plc->packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
	{
		if (message->qualcomm.MMV != MMV) 
		{
			continue;
		}
		if (message->qualcomm.MMTYPE != HTOLE16 (MMTYPE)) 
		{
			continue;
		}
		break;
	}
	return (plc->packetsize);
}


#endif

