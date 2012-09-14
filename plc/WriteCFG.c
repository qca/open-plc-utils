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
 *   signed WriteCFG (struct plc * plc);
 *
 *   plc.h
 *
 *   This plugin for program plc writes the contents of an SDRAM
 *   confirmation file to a device using a VS_SET_SDRAM message; the
 *   CFG file in struct plc must be opened before this function is
 *   called; the bootloader must be running for this to work; 
 *   
 *   the VS_SET_SDRAM message is recognized by the INT600 BootLoader
 *   only; the INT6400 BootLoader recognizes it but does nothing with
 *   it;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WRITECFG_SOURCE
#define WRITECFG_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../ram/sdram.h"

int WriteCFG (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_set_sdram_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		struct config_ram config_ram;
		uint32_t CHECKSUM;
	}
	* request = (struct vs_set_sdram_request *) (message);
	struct __packed vs_set_sdram_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* confirm = (struct vs_set_sdram_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Write Configuration Applet from %s", plc->CFG.name);
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
	QualcommHeader (&request->qualcomm, 0, (VS_SET_SDRAM | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (lseek (plc->CFG.file, 0, SEEK_SET)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->CFG.name);
		return (-1);
	}
	if (read (plc->CFG.file, &request->config_ram, sizeof (struct config_ram)) != sizeof (struct config_ram)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTREAD, plc->CFG.name);
		return (-1);
	}
	if (read (plc->CFG.file, &request->CHECKSUM, sizeof (request->CHECKSUM)) != sizeof (request->CHECKSUM)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, "can't read %s checksum", plc->CFG.name);
		return (-1);
	}
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMME (plc, 0, (VS_SET_SDRAM | MMTYPE_CNF)) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	Confirm (plc, "Written");
	return (0);
}


#endif

