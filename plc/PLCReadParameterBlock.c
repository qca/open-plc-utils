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
 *   signed PLCReadParameterBlock (struct channel * channel, struct message * message, void * memory, size_t extent);
 *
 *   plc.h
 *
 *   Read the PIB Image from an INT6000 using as many VS_RD_MOD
 *   messages as needed; Write image blocks to file as they are 
 *   read;
 *
 *   the objective here is to read the PIB module in 1024 byte blocks 
 *   until the module offset equals or exceeds the PIB actual; we do
 *   not know the PIB actual until we have read the header contained
 *   in the first block read from OFFSET 0;
 *
 *   MOFFSET and MLENGTH fields occupy different offsets in REQ and CNF 
 *   messages; we exploit that by initializing them using CNF message
 *   offsets then copying values into REQ message offsets before each
 *   read; this works because confirmation messages always return all 
 *   the data we request or nothing at all;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READPARAMETERBLOCK_SOURCE
#define READPARAMETERBLOCK_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../pib/pib.h" 
#include "../plc/plc.h" 

signed PLCReadParameterBlock (struct channel * channel, struct message * message, void * memory, size_t extent) 

{
	uint8_t * buffer = (uint8_t *)(memory);

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

	uint16_t length = 0;
	uint32_t offset = 0;
	signed actual = PLC_RECORD_SIZE;
	do 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
		QualcommHeader (&request->qualcomm, 0, (VS_RD_MOD | MMTYPE_REQ));
		request->MODULEID = VS_MODULE_PIB;
		request->MLENGTH = HTOLE16 (actual);
		request->MOFFSET = HTOLE32 (offset);
		if (sendpacket (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
		{
			error (1, errno, CHANNEL_CANTSEND);
		}
		if (readpacket (channel, message, sizeof (* message)) <= 0) 
		{
			error (1, errno, CHANNEL_CANTREAD);
		}
		if (UnwantedMessage (message, sizeof (* confirm), 0, (VS_RD_MOD | MMTYPE_CNF))) 
		{
			actual = PLC_RECORD_SIZE;
			offset = 0;
			continue;
		}
		if (confirm->MSTATUS) 
		{
			error (1, ECANCELED, PLC_WONTDOIT);
		}
		if (LE16TOH (confirm->MLENGTH) != actual) 
		{
			error (1, ECANCELED, PLC_ERR_LENGTH);
		}
		if (LE32TOH (confirm->MOFFSET) != offset) 
		{
			error (1, ECANCELED, PLC_ERR_OFFSET);
		}
		actual = LE16TOH (confirm->MLENGTH);
		offset = LE32TOH (confirm->MOFFSET);
		if (checksum32 (confirm->BUFFER, actual, confirm->CHKSUM)) 
		{
			error (1, ECANCELED, "Bad Packet Checksum");
		}
		if (offset == length) 
		{
			struct pib_header * pib_header = (struct pib_header *) (confirm->BUFFER);
			length = LE16TOH (pib_header->PIBLENGTH);
		}
		if ((offset + actual) > length) 
		{
			actual = length - offset;
		}
		memcpy (buffer + offset, confirm->BUFFER, actual);
		offset += actual;
		extent -= actual;
	}
	while (offset < length);
	return (offset);
}


#endif

