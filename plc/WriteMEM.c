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
 *   signed WriteMEM (struct plc * plc, struct _file_ * file, unsigned module, uint32_t offset, uint32_t length);
 *
 *   plc.h
 *
 *   Write the contents of a file directly into device SDRAM using
 *   VS_WR_MEM messages; the bootloader must be running for this to
 *   work; the function is used to dowload firmware or test applets;
 *
 *   this function makes no attempt to validate the information sent
 *   to the device;
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

#ifndef WRITEMEM_SOURCE
#define WRITEMEM_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../tools/files.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed WriteMEM (struct plc * plc, struct _file_ * file, unsigned module, uint32_t offset, uint32_t extent) 

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
	Request (plc, "Write %s (%d) (%08X:%d)", file->name, module, offset, extent);
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_WR_MEM | MMTYPE_REQ));
		if (length > extent) 
		{
			length = extent;
		}
		if (read (file->file, request->BUFFER, length) != (signed)(length)) 
		{
			error (1, errno, FILE_CANTREAD, file->name);
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

