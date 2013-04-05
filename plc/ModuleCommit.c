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
 *   signed ModuleCommit (struct plc * plc, uint32_t options);
 *
 *   commit previously downloaded modules to NVM; this action closes
 *   the current module download session; the timer is temporarily
 *   set to 10 seconds so that the operation can complete;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef MODULECOMMIT_SOURCE
#define MODULECOMMIT_SOURCE

#include "../tools/error.h"
#include "../plc/plc.h"

signed ModuleCommit (struct plc * plc, uint32_t options) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_module_operation_commit_request 
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
			uint32_t COMMIT_CODE;
		}
		request;
		uint8_t RSVD [20];
	}
	* request = (struct vs_module_operation_commit_request *)(message);
	struct __packed vs_module_operation_commit_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED1;
		uint8_t NUM_OP_DATA;
		struct __packed 
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint32_t MOD_OP_SESSION_ID;
			uint32_t COMMIT_CODE;
			uint8_t NUM_MODULES;
		}
		request;
		struct __packed 
		{
			uint16_t MOD_STATUS;
			uint16_t ERR_REC_CODE;
		}
		MOD_OP_DATA [1];
	}
	* confirm = (struct vs_module_operation_commit_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	unsigned timer = channel->timer;
	Request (plc, "Close Session");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MODULE_OPERATION | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->NUM_OP_DATA = 1;
	request->request.MOD_OP = HTOLE16 (PLC_MOD_OP_CLOSE_SESSION);
	request->request.MOD_OP_DATA_LEN = HTOLE16 (sizeof (request->request) + sizeof (request->RSVD));
	request->request.MOD_OP_SESSION_ID = HTOLE32 (plc->cookie);
	request->request.COMMIT_CODE = HTOLE32 (options);
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
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	return (0);
}


#endif

