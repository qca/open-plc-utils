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
 *   signed DeviceIdent (struct plc * plc);
 *
 *   plc.h
 *   
 *   This plugin for program plc reads the first block of a device
 *   PIB using VS_RD_MOD and updates the plc structure with device
 *   network identity information, including MAC, DAK and NMK;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef DEVICEIDENT_SOURCE
#define DEVICEIDENT_SOURCE

#include <string.h>

#include "../plc/plc.h" 
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/memory.h"
#include "../pib/pib.h"

signed DeviceIdent (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_rd_mod_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MODULEID;
		uint8_t MACCESS;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint8_t MSECRET [16];
	}
	* request = (struct vs_rd_mod_request *) (message);
	struct __packed vs_rd_mod_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t RES [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t MCHKSUM;
		struct simple_pib IDENT;
	}
	* confirm = (struct vs_rd_mod_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_RD_MOD | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->MODULEID = VS_MODULE_PIB;
	request->MLENGTH = HTOLE16 (sizeof (confirm->IDENT));
	request->MOFFSET = 0;
	if (SendMME (plc) <= 0) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}
	while (ReadMME (plc, 0, (VS_RD_MOD | MMTYPE_CNF)) <= 0) 
	{
		plc->flags |= PLC_BAILOUT;
		Failure (plc, PLC_NODETECT);
	}
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
	}
	memcpy (plc->MAC, confirm->IDENT.MAC, sizeof (plc->MAC));
	memcpy (plc->DAK, confirm->IDENT.DAK, sizeof (plc->DAK));
	memcpy (plc->NMK, confirm->IDENT.NMK, sizeof (plc->NMK));
	return (0);
}


#endif

