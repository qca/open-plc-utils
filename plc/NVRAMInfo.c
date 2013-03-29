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
 *   signed NVRAMInfo (struct plc * plc);
 *   
 *   plc.h
 * 
 *   This plugin for program plc requests a device NVRAM parameter 
 *   block using a VS_GET_NVM message; the information is displayed
 *   and can be used to determine the flash memory characteristics 
 *   before downloading firmware;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVRAMINFO_SOURCE
#define NVRAMINFO_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/error.h"
#include "../tools/memory.h"
#include "../tools/symbol.h"
#include "../ram/nvram.h"

signed NVRAMInfo (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_get_nvm_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_get_nvm_request *) (message);
	struct __packed vs_get_nvm_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		struct config_nvram config_nvram;
	}
	* confirm = (struct vs_get_nvm_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	struct config_nvram * config_nvram = (struct config_nvram *)(&confirm->config_nvram);
	Request (plc, "Fetch NVRAM Configuration");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_GET_NVM | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_GET_NVM | MMTYPE_CNF)) > 0) 
	{
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			continue;
		}
		Display (plc, "TYPE=0x%02X (%s) PAGE=0x%04X (%d) BLOCK=0x%04X (%d) SIZE=0x%04X (%d)", LE32TOH (config_nvram->NVRAMTYPE), NVRAMName (LE32TOH (config_nvram->NVRAMTYPE)), LE32TOH (config_nvram->NVRAMPAGE), LE32TOH (config_nvram->NVRAMPAGE), LE32TOH (config_nvram->NVRAMBLOCK), LE32TOH (config_nvram->NVRAMBLOCK), LE32TOH (config_nvram->NVRAMSIZE), LE32TOH (config_nvram->NVRAMSIZE));
	}
	return (0);
}


#endif

