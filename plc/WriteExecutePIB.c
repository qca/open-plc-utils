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
 *   signed WriteExecutePIB (struct plc * plc, uint32_t offset, struct pib_header * header);
 *
 *   plc.h
 *
 *   write parameters to SDRAM using VS_WRITE_AND_EXECUTE_APPLET; the memory
 *   offset depends on the firmware release;
 *
 *   plc->PIB must be positioned to the start of the PIB file or this
 *   function will not work;
 *
 *   pass in the PIB header since it contains the PIB file length and
 *   checksum needed by the VS_WRITE_AND_EXECUTE_APPLET request;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WRITEEXECUTEPIB_SOURCE
#define WRITEEXECUTEPIB_SOURCE

#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed WriteExecutePIB (struct plc * plc, uint32_t offset, struct pib_header * header) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_write_execute_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t CLIENT_SESSION_ID;
		uint32_t SERVER_SESSION_ID;
		uint32_t FLAGS;
		uint8_t ALLOWED_MEM_TYPES [8];
		uint32_t TOTAL_LENGTH;
		uint32_t CURR_PART_LENGTH;
		uint32_t CURR_PART_OFFSET;
		uint32_t START_ADDR;
		uint32_t CHECKSUM;
		uint8_t RESERVED2 [8];
		uint8_t IMAGE [PLC_MODULE_SIZE];
	}
	* request = (struct vs_write_execute_request *) (message);
	struct __packed vs_write_execute_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t MSTATUS;
		uint32_t CLIENT_SESSION_ID;
		uint32_t SERVER_SESSION_ID;
		uint32_t FLAGS;
		uint8_t ALLOWED_MEM_TYPES [8];
		uint32_t TOTAL_LENGTH;
		uint32_t CURR_PART_LENGTH;
		uint32_t CURR_PART_OFFSET;
		uint32_t START_ADDR;
		uint32_t CHECKSUM;
		uint8_t RESERVED2 [8];
		uint32_t CURR_PART_ABSOLUTE_ADDR;
		uint32_t ABSOLUTE_START_ADDR;
	}
	* confirm = (struct vs_write_execute_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint32_t length = PLC_MODULE_SIZE;
	uint32_t extent = LE32TOH (header->PIBLENGTH);
	Request (plc, "Write %s (0) (%08X:%d)", plc->PIB.name, offset, extent);
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_WRITE_AND_EXECUTE_APPLET | MMTYPE_REQ));
		if (length > extent) 
		{
			length = extent;
		}
		if (read (plc->PIB.file, request->IMAGE, length) != (signed)(length)) 
		{
			error (1, errno, FILE_CANTREAD, plc->PIB.name);
		}
		request->CLIENT_SESSION_ID = HTOLE32 (plc->cookie);
		request->SERVER_SESSION_ID = HTOLE32 (0);
		request->FLAGS = HTOLE32 (PLC_MODULE_ABSOLUTE);
		request->ALLOWED_MEM_TYPES [0] = 1;
		request->TOTAL_LENGTH = header->PIBLENGTH;
		request->CURR_PART_LENGTH = HTOLE32 (length);
		request->CURR_PART_OFFSET = HTOLE32 (offset);
		request->START_ADDR = HTOLE32 (0);
		request->CHECKSUM = header->CHECKSUM;
		plc->packetsize = sizeof (* request);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_WRITE_AND_EXECUTE_APPLET | MMTYPE_CNF)) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			return (-1);
		}
		if (LE32TOH (confirm->CURR_PART_LENGTH) != length) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_LENGTH);
			return (-1);
		}
		if (LE32TOH (confirm->CURR_PART_OFFSET) != offset) 
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

