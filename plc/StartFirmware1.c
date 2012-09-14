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
 *   struct StartFirmware1 (struct plc * plc, unsigned module, const struct nvm_header1 * nvm_header) 
 *
 *   plc.h
 *
 *   This plugin starts software execution using VS_ST_MAC; despite
 *   an affirmative respond from the device, there is no guarantee
 *   that the firmware will actually start or continue to run so we
 *   wait for a response and report the result;
 *
 *   struct nvm_header1 must be little-endian order and ready to send 
 *   to the device;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef STARTSOFTWARE1_SOURCE
#define STARTSOFTWARE1_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/error.h"
#include "../plc/plc.h"

signed StartFirmware1 (struct plc * plc, unsigned module, const struct nvm_header1 * nvm_header) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_st_mac_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MODULEID;
		uint8_t RESERVED [3];
		uint32_t IMAGEBOOT;
		uint32_t IMAGELENGTH;
		uint32_t IMAGECHECKSUM;
		uint32_t IMAGESTART;
	}
	* request = (struct vs_st_mac_request *) (message);
	struct __packed vs_st_mac_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MODULEID;
	}
	* confirm = (struct vs_st_mac_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	Request (plc, "Start %s (%d) (%08X)", plc->NVM.name, module, LE32TOH (nvm_header->ENTRYPOINT));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
	QualcommHeader (&request->qualcomm, 0, (VS_ST_MAC | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->IMAGEBOOT = nvm_header->IMAGEADDRESS;
	request->IMAGELENGTH = nvm_header->IMAGELENGTH;
	request->IMAGECHECKSUM = nvm_header->IMAGECHECKSUM;
	request->IMAGESTART = nvm_header->ENTRYPOINT;
	request->MODULEID = VS_MODULE_MAC;
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMME (plc, 0, (VS_ST_MAC | MMTYPE_CNF)) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	return (0);
}


#endif

