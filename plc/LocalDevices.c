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
 *   unsigned LocalDevices (struct channel * channel,  struct message * message, void * memory, size_t extent);
 *   
 *   plc.h
 * 
 *   populate a memory region with consecutive Ethernet addresses; 
 *   the addresses belong to Atheros powerline devices connected to
 *   the host interface specified in struct channel; each bridge may 
 *   be connected to one or more HomePlug AV devices via powerline;
 *
 *   each powerline bridge normally belongs to a different powerline 
 *   network but is is possible for multiple bridges to belong to the 
 *   same powerline network thus leading to confusing configurations;
 *
 *   use function NetworkDevices() to discover all devices on each
 *   powerline network;
 *
 *   although this function accepts a channel structure, it ignores
 *   the channel peer address and sends a VS_SW_VER request message
 *   to the Local Management Address; this causes all local devices
 *   to respond; the list is a collection of source addresses taken
 *   from all responses;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef NETWORKBRIDGES_SOURCE
#define NETWORKBRIDGES_SOURCE

#include <memory.h>
#include <errno.h>

#include "../plc/plc.h"
#include "../ether/channel.h"
#include "../tools/types.h"
#include "../tools/error.h"

unsigned LocalDevices (struct channel const * channel, struct message * message, void * memory, size_t extent) 

{
	extern const byte localcast [ETHER_ADDR_LEN];
	struct vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_sw_ver_request *)(message);
	uint8_t * origin = (uint8_t *)(memory);
	uint8_t * offset = (uint8_t *)(memory);
	ssize_t packetsize;
	memset (memory, 0, extent);
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, localcast, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (0);
	}
	while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
	{
		if (UnwantedMessage (message, packetsize, 0, (VS_SW_VER | MMTYPE_CNF))) 
		{
			continue;
		}
		if (extent >= sizeof (message->ethernet.OSA)) 
		{
			memcpy (offset, message->ethernet.OSA, sizeof (message->ethernet.OSA));
			offset += sizeof (message->ethernet.OSA);
			extent -= sizeof (message->ethernet.OSA);
		}
	}
	return ((unsigned)(offset - origin) / ETHER_ADDR_LEN);
}


#endif

