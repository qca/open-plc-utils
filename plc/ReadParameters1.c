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
 *   signed ReadParameters1 (struct plc * plc);
 *
 *   plc.h
 *
 *   Read the PIB Image from an INT6x00 or AR7x00 using as many
 *   VS_RD_MOD messages as needed; Write image blocks to file as
 *   they are read;
 *
 *   the objective here is to read the PIB module in 1024 byte blocks
 *   until the module offset equals or exceeds the PIB length; we do
 *   not know the PIB length until we have read the header contained
 *   in the first block at OFFSET 0;
 *
 *   MOFFSET and MLENGTH fields occupy different offsets in REQ and CNF
 *   messages; we exploit that by initializing them using CNF message
 *   offsets then copying values into REQ message offsets before each
 *   read; this works because confirmation messages always return all
 *   the data we request or nothing at all;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *      Lluis Paulet Gimbert <dimat.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READPARAMETERS1_SOURCE
#define READPARAMETERS1_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h"
#include "../pib/pib.h"

signed ReadParameters1 (struct plc * plc)

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
		uint8_t RESERVED1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t CHKSUM;
		uint8_t BUFFER [PLC_RECORD_SIZE];
	}
	* confirm = (struct vs_rd_mod_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint32_t extent = 0;
	uint32_t offset = 0;
	uint16_t length = PLC_RECORD_SIZE;
	Request (plc, "Read Parameters from Device");
	if (lseek (plc->pib.file, 0, SEEK_SET))
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->pib.name);
		return (1);
	}
	do
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_RD_MOD | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		request->MODULEID = VS_MODULE_PIB;
		request->MLENGTH = HTOLE16 (length);
		request->MOFFSET = HTOLE32 (offset);
		if (SendMME (plc) <= 0)
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_RD_MOD | MMTYPE_CNF)) <= 0)
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
		length = LE16TOH (confirm->MLENGTH);
		offset = LE32TOH (confirm->MOFFSET);
		if (checksum32 (confirm->BUFFER, length, confirm->CHKSUM))
		{
			error ((plc->flags & PLC_BAILOUT), ECANCELED, "Bad Packet Checksum");
			return (-1);
		}
		if (offset == extent)
		{
			struct pib_header * pib_header = (struct pib_header *) (confirm->BUFFER);
			extent = LE16TOH (pib_header->PIBLENGTH);
		}
		if ((offset + length) > extent)
		{
			length = extent - offset;
		}
		if (lseek (plc->pib.file, offset, SEEK_SET) != (signed)(offset))
		{
			error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTSEEK, plc->pib.name);
			return (-1);
		}
		if (write (plc->pib.file, confirm->BUFFER, length) != (signed)(length))
		{
			error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTSAVE, plc->pib.name);
			return (-1);
		}
		offset += length;
	}
	while (offset < extent);
	Confirm (plc, "Read %s", plc->pib.name);
	return (0);
}


#endif

