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
 *   signed WriteMOD (struct channel * channel, uint8_t module, void const * memory, size_t extent);
 *   
 *   plc.h
 *
 *   This plugin writes a memory resident NVM image to device SDRAM
 *   using as many VS_WR_MOD messages as needed to complete transfer; 
 *   runtime firmware must be running for this to work; the NVM file
 *   in struct plc must be opened before calling this function;
 *   
 *   This function is functionally similar to WriteNVM but downloads
 *   the image from memory instead of reading the image from a file;
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

#ifndef WRITEMOD_SOURCE
#define WRITEMOD_SOURCE

#include <unistd.h>
#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h" 
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/files.h"

signed WriteMOD (struct plc * plc, uint8_t module, void const * memory, size_t extent) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_wr_mod_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MODULEID;
		uint8_t MACCESS;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t MCHKSUM;
		uint8_t MBUFFER [PLC_RECORD_SIZE];
	}
	* request = (struct vs_wr_mod_request *) (message);
	struct __packed vs_wr_mod_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MODULEID;
		uint8_t MACCESS;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}
	* confirm = (struct vs_wr_mod_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint32_t length = PLC_RECORD_SIZE;
	uint32_t offset = 0;
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_WR_MOD | MMTYPE_REQ));
		if (length > extent) 
		{
			length = (signed)(extent);
		}
		memcpy (request->MBUFFER, (byte *)(memory) + offset, length);
		request->MODULEID = module;
		request->MACCESS = 0;
		request->MLENGTH = HTOLE16 (length);
		request->MOFFSET = HTOLE32 (offset);
		request->MCHKSUM = checksum32 (request->MBUFFER, length, 0);
		plc->packetsize = sizeof (* request);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_WR_MOD | MMTYPE_CNF)) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			return (-1);
		}
		if (LE16TOH (confirm->MLENGTH) != length) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_LENGTH);
			return (-1);
		}
		if (LE32TOH (confirm->MOFFSET) != offset) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_OFFSET);
			return (-1);
		}
		offset += length;
		extent -= length;
	}
	return (0);
}


#endif

