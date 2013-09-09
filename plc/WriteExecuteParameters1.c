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
 *   signed WriteExecuteParameters1 (struct plc * plc, unsigned module, const struct nvm_header1 * nvm_header);
 *
 *   plc.h
 *
 *   write parameters to SDRAM using VS_WRITE_AND_EXECUTE_APPLET;
 *
 *   we pass in the PIB image header because that is how the caller
 *   located the PIB image in the file but we must write the entire
 *   file into SDRAM at the address found in the PIB image header;
 *   consequenctly, we have to rewind the file position and compute
 *   the actual file size;
 *
 *   this implementeation will probably never be used because we do
 *   not distribute parameter files in old image file format; but if
 *   we then this is it;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef WRITEEXECUTEPARAMETERS1_SOURCE
#define WRITEEXECUTEPARAMETERS1_SOURCE

#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed WriteExecuteParameters1 (struct plc * plc, unsigned module, const struct nvm_header1 * nvm_header)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct nvm_header1 nvm_manifest;
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
	uint32_t offset = LE32TOH (nvm_header->IMAGEADDRESS);
	uint32_t extent = LE32TOH (nvm_header->IMAGELENGTH);

#if 0

	Request (plc, "Write %s (%d) (%08X:%d)", plc->PIB.name, module, offset, extent);

#else

/*
 *	adjust the file extent to include the the manifest image header, the manifest image, the PIB
 *	image header and the PIB image; this should in principle, equal the PIB filesize for Panther
 *	and Lynx .pib files;
 */

	if (lseek (plc->PIB.file, 0, SEEK_SET))
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (read (plc->PIB.file, &nvm_manifest, sizeof (nvm_manifest)) != sizeof (nvm_manifest))
	{
		error (1, errno, FILE_CANTREAD, plc->PIB.name);
	}
	extent += sizeof (* nvm_header);
	extent += LE32TOH (nvm_manifest.IMAGELENGTH);
	extent += sizeof (* nvm_header);
	Request (plc, "Write %s (%d) (%08X:%d)", plc->PIB.name, module, offset, extent);

#endif

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
		request->TOTAL_LENGTH = nvm_header->IMAGELENGTH;
		request->CURR_PART_LENGTH = HTOLE32 (length);
		request->CURR_PART_OFFSET = HTOLE32 (offset);
		request->START_ADDR = nvm_header->ENTRYPOINT;
		request->CHECKSUM = nvm_header->IMAGECHECKSUM;
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

