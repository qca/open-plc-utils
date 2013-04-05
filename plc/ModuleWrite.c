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
 *   signed ModuleWrite (struct plc * plc, struct _file_ * file, unsigned index, struct vs_module_spec * vs_module_spec);
 *   
 *   plc.h
 *
 *   write an entire file to flash memory as a single module; write
 *   the file in 1400 byte records; allow up to 80 seconds for each 
 *   write to complete; extra time is needed because memory must be
 *   erased;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MODULEWRITE_SOURCE
#define MODULEWRITE_SOURCE

#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h" 

signed ModuleWrite (struct plc * plc, struct _file_ * file, unsigned index, struct vs_module_spec * vs_module_spec) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_module_operation_write_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		struct __packed 
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint32_t MOD_OP_SESSION_ID;
			uint8_t MODULE_IDX;
			uint16_t MODULE_ID;
			uint16_t MODULE_SUB_ID;
			uint16_t MODULE_LENGTH;
			uint32_t MODULE_OFFSET;
		}
		MODULE_SPEC;
		uint8_t MODULE_DATA [PLC_MODULE_SIZE];
	}
	* request = (struct vs_module_operation_write_request *)(message);
	struct __packed vs_module_operation_write_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		struct __packed 
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint32_t MOD_OP_SESSION_ID;
			uint8_t MODULE_IDX;
			uint16_t MODULE_ID;
			uint16_t MODULE_SUB_ID;
			uint16_t MODULE_LENGTH;
			uint32_t MODULE_OFFSET;
		}
		MODULE_SPEC;
	}
	* confirm = (struct vs_module_operation_write_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	unsigned timer = channel->timer;
	uint16_t length = PLC_MODULE_SIZE;
	uint32_t extent = vs_module_spec->MODULE_LENGTH;
	uint32_t offset = 0;
	Request (plc, "Flash %s", file->name);
	while (extent) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_MODULE_OPERATION | MMTYPE_REQ));
		plc->packetsize = sizeof (struct vs_module_operation_write_request);
		if (length > extent) 
		{
			length = extent;
		}
		if (read (file->file, request->MODULE_DATA, length) != length) 
		{
			error (1, errno, FILE_CANTREAD, file->name);
		}
		request->NUM_OP_DATA = 1;
		request->MODULE_SPEC.MOD_OP = HTOLE16 (PLC_MOD_OP_WRITE_MODULE);
		request->MODULE_SPEC.MOD_OP_DATA_LEN = HTOLE16 (sizeof (request->MODULE_SPEC) + sizeof (request->MODULE_DATA));
		request->MODULE_SPEC.MOD_OP_SESSION_ID = HTOLE32 (plc->cookie);
		request->MODULE_SPEC.MODULE_IDX = index;
		request->MODULE_SPEC.MODULE_ID = HTOLE16 (vs_module_spec->MODULE_ID);
		request->MODULE_SPEC.MODULE_SUB_ID = HTOLE16 (vs_module_spec->MODULE_SUB_ID);
		request->MODULE_SPEC.MODULE_LENGTH = HTOLE16 (length);
		request->MODULE_SPEC.MODULE_OFFSET = HTOLE32 (offset);

#if 1

		fprintf (stderr, "RESERVED 0x%08X\n", LE32TOH (request->RESERVED));
		fprintf (stderr, "NUM_OP_DATA %d\n", request->NUM_OP_DATA);
		fprintf (stderr, "MOD_OP 0x%02X\n", LE16TOH (request->MODULE_SPEC.MOD_OP));
		fprintf (stderr, "MOD_OP_DATA_LEN %d\n", LE16TOH (request->MODULE_SPEC.MOD_OP_DATA_LEN));
		fprintf (stderr, "RESERVED 0x%08X\n", LE32TOH (request->MODULE_SPEC.MOD_OP_RSVD));
		fprintf (stderr, "MODULE_ID 0x%04X\n", LE16TOH (request->MODULE_SPEC.MODULE_ID));
		fprintf (stderr, "MODULE_SUB_ID 0x%04X\n", LE16TOH (request->MODULE_SPEC.MODULE_SUB_ID));
		fprintf (stderr, "MODULE_LENGTH %d\n", LE16TOH (request->MODULE_SPEC.MODULE_LENGTH));
		fprintf (stderr, "MODULE_OFFSET 0x%08X\n", LE32TOH (request->MODULE_SPEC.MODULE_OFFSET));

#endif

		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		channel->timer = PLC_MODULE_WRITE_TIMEOUT;
		if (ReadMME (plc, 0, (VS_MODULE_OPERATION | MMTYPE_CNF)) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			channel->timer = timer;
			return (-1);
		}
		channel->timer = timer;

#if 1

		fprintf (stderr, "MSTATUS 0x%04X\n", LE16TOH (confirm->MSTATUS));
		fprintf (stderr, "ERROR_REC_CODE %d\n", LE16TOH (confirm->ERR_REC_CODE));
		fprintf (stderr, "RESERVED 0x%08X\n", LE32TOH (confirm->RESERVED));
		fprintf (stderr, "NUM_OP_DATA %d\n", confirm->NUM_OP_DATA);
		fprintf (stderr, "MOD_OP 0x%02X\n", LE16TOH (request->MODULE_SPEC.MOD_OP));
		fprintf (stderr, "MOD_OP_DATA_LEN %d\n", LE16TOH (confirm->MODULE_SPEC.MOD_OP_DATA_LEN));
		fprintf (stderr, "RESERVED 0x%08X\n", LE32TOH (confirm->MODULE_SPEC.MOD_OP_RSVD));
		fprintf (stderr, "MODULE_ID 0x%04X\n", LE16TOH (confirm->MODULE_SPEC.MODULE_ID));
		fprintf (stderr, "MODULE_SUB_ID 0x%04X\n", LE16TOH (confirm->MODULE_SPEC.MODULE_SUB_ID));
		fprintf (stderr, "MODULE_LENGTH %d\n", LE16TOH (confirm->MODULE_SPEC.MODULE_LENGTH));
		fprintf (stderr, "MODULE_OFFSET 0x%08X\n", LE32TOH (request->MODULE_SPEC.MODULE_OFFSET));

#endif

		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			return (-1);
		}
		if (LE16TOH (confirm->MODULE_SPEC.MODULE_LENGTH) != length) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_LENGTH);
			return (-1);
		}
		if (LE32TOH (confirm->MODULE_SPEC.MODULE_OFFSET) != offset) 
		{
			error ((plc->flags & PLC_BAILOUT), 0, PLC_ERR_OFFSET);
			return (-1);
		}
		extent -= length;
		offset += length;
	}
	return (0);
}

#endif

