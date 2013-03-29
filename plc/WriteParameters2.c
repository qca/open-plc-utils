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
 *   signed WriteParameters2 (struct plc * plc, unsigned module, const struct nvm_header2 * nvm_header);
 *
 *   plc.h
 *
 *   use VS_WR_MEM message to write a parameter block into SDRAM 
 *   when the image is stored in the old image file format;
 *
 *   the boot loader must be running for this to work since runtime 
 *   firmware ignores VS_WR_MEM messages; 
 *
 *   future devices will ignore VS_WR_MEM messages so use function
 *   WriteExecuteParameters2, instead;
 *
 *   this function makes no attempt to validate the information sent
 *   to the device;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WRITEPARAMETERS2_SOURCE
#define WRITEPARAMETERS2_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../tools/files.h"
#include "../tools/error.h"
#include "../tools/memory.h"
#include "../plc/plc.h"

signed WriteParameters2 (struct plc * plc, unsigned module, const struct nvm_header2 * nvm_header) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_wr_mem_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t MOFFSET;
		uint32_t MLENGTH;
		uint8_t BUFFER [PLC_RECORD_SIZE];
	}
	* request = (struct vs_wr_mem_request *) (message);
	struct __packed vs_wr_mem_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint32_t MOFFSET;
		uint32_t MLENGTH;
	}
	* confirm = (struct vs_wr_mem_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint32_t length = PLC_RECORD_SIZE;
	uint32_t offset = LE32TOH (nvm_header->ImageAddress);
	uint32_t extent = LE32TOH (nvm_header->ImageLength);
	Request (plc, "Write %s (%d) (%08X:%d)", plc->PIB.name, module, offset, extent);
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_WR_MEM | MMTYPE_REQ));
		if (length > extent) 
		{
			length = extent;
		}
		if (read (plc->PIB.file, request->BUFFER, length) != (signed)(length)) 
		{
			error (1, errno, FILE_CANTREAD, plc->PIB.name);
		}
		request->MLENGTH = HTOLE32 (length);
		request->MOFFSET = HTOLE32 (offset);
		plc->packetsize = sizeof (* request);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_WR_MEM | MMTYPE_CNF)) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			return (-1);
		}
		if (LE32TOH (confirm->MLENGTH) != length) 
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
		offset += length;
		extent -= length;
	}
	return (0);
}


#endif

