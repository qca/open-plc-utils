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
 *   signed ListLocalDevices (struct plc * plc, char const * space, char const * comma);
 *
 *   plc.h
 *
 *   print local device hardware addresses on stdout;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef LISTLOCALDEVICES_SOURCE
#define LISTLOCALDEVICES_SOURCE

#include "../tools/number.h"
#include "../plc/plc.h"

signed ListLocalDevices (struct plc * plc, char const * space, char const * comma) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	ssize_t packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_sw_ver_request *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
	QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
	if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (-1);
	}
	while ((packetsize = readpacket (channel, message, sizeof (* message))) > 0) 
	{
		if (UnwantedMessage (message, packetsize, 0, (VS_SW_VER | MMTYPE_CNF))) 
		{
			continue;
		}
		hexout (request->ethernet.OSA, sizeof (request->ethernet.OSA), HEX_EXTENDER, 0, stdout);
		if ((space) && (*space)) 
		{
			printf ("%s", space);
		}
	}
	if ((comma) && (* comma)) 
	{
		printf ("%s", comma);
	}
	return (0);
}


#endif

