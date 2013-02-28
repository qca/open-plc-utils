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
 *   signed FlashMOD (struct channel * channel, uint8_t module);
 *
 *   plc.h
 *
 *   commit downloaded firmware and/or parameters to NVRAM using a
 *   VS_MOD_NVM message; flash-less devices will attempt to upload
 *   to the local host since they have no NVRAM; the host must be
 *   ready to receive the module;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef FLASHMOD_SOURCE
#define FLASHMOD_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/error.h"
#include "../tools/memory.h"

signed FlashMOD (struct channel * channel, uint8_t module) 

{
	struct message message;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_mod_nvm_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MODULEID;
	}
	* request = (struct vs_mod_nvm_request *) (&message);
	struct __packed vs_mod_nvm_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MODULEID;
	}
	* confirm = (struct vs_mod_nvm_confirm *) (&message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	ssize_t packetsize;
	memset (&message, 0, sizeof (message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MOD_NVM | MMTYPE_REQ));
	request->MODULEID = module;
	if (sendpacket (channel, &message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}
	if ((packetsize = readpacket (channel, &message, sizeof (message))) <= 0) 
	{
		error (1, errno, CHANNEL_CANTREAD);
	}
	if (UnwantedMessage (&message, packetsize, 0, (VS_MOD_NVM | MMTYPE_CNF))) 
	{
		error (1, ECANCELED, PLC_BADFRAME);
	}
	if (confirm->MSTATUS) 
	{
		error (1, ECANCELED, PLC_WONTDOIT);
	}
	return (0);
}


#endif

