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
 *   signed WritePIB (struct plc * plc);
 *   
 *   plc.h
 *
 *   write an entire .nvm file into PLC SDRAM using as many VS_WR_MEM
 *   messages as needed to complete the transfer;
 *
 *   runtime firmware must be running for this to work; the NVM file
 *   in struct plc must be opened before calling this function;
 *   
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WRITEPIB_SOURCE
#define WRITEPIB_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h" 

signed WritePIB (struct plc * plc) 

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
		uint8_t RESERVED;
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
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}
	* confirm = (struct vs_wr_mod_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint16_t length = PLC_RECORD_SIZE;
	uint32_t extent = lseek (plc->PIB.file, 0, SEEK_END);
	uint32_t offset = lseek (plc->PIB.file, 0, SEEK_SET);
	Request (plc, "Write %s to scratch", plc->PIB.name);
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_WR_MOD | MMTYPE_REQ));
		if (length > extent) 
		{
			length = extent;
		}
		if (read (plc->PIB.file, request->MBUFFER, length) != length) 
		{
			error (1, errno, FILE_CANTREAD, plc->PIB.name);
		}
		request->MODULEID = VS_MODULE_PIB;
		request->RESERVED = 0;
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
			length = PLC_RECORD_SIZE;
			offset = 0;
			continue;
		}
		if (LE32TOH (confirm->MOFFSET) != offset) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_OFFSET);
			length = PLC_RECORD_SIZE;
			offset = 0;
			continue;
		}
		extent -= length;
		offset += length;
	}
	return (0);
}


#endif

