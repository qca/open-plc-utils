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
 *   signed ModuleSession (struct plc * plc, unsigned modules, struct vs_module_spec * vs_module_spec);
 *
 *   Establish a module download session; the session expires after
 *   30 minutes; the session is lost if no modules are committed in
 *   that time; the timeout is set to 10 seconds so that the device
 *   has enouth time to reply;
 *
 *   array vs_module_spec contains information about each module in
 *   this session;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef MODULESESSION_SOURCE
#define MODULESESSION_SOURCE

#include "../tools/error.h"
#include "../plc/plc.h"

signed ModuleSession (struct plc * plc, unsigned modules, struct vs_module_spec * vs_module_spec)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_module_operation_start_request
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t RESERVED1;
		uint8_t NUM_OP_DATA;
		struct __packed
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint32_t MOD_OP_SESSION_ID;
			uint8_t NUM_MODULES;
		}
		MODULE_SPEC;
		struct vs_module_spec MOD_OP_SPEC [10];
	}
	* request = (struct vs_module_operation_start_request *)(message);
	struct __packed vs_module_operation_start_confirm
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
			uint8_t NUM_MODULES;
		}
		MODULE_SPEC;
		struct __packed
		{
			uint16_t MOD_STATUS;
			uint16_t ERR_REC_CODE;
		}
		MOD_OP_DATA [1];
	}
	* confirm = (struct vs_module_operation_start_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	unsigned timer = channel->timeout;
	struct vs_module_spec * request_spec = (struct vs_module_spec *)(&request->MOD_OP_SPEC);
	Request (plc, "Start Module Write Session");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MODULE_OPERATION | MMTYPE_REQ));
	plc->packetsize = sizeof (* request);
	request->NUM_OP_DATA = 1;
	request->MODULE_SPEC.MOD_OP = HTOLE16 (PLC_MOD_OP_START_SESSION);
	request->MODULE_SPEC.MOD_OP_DATA_LEN = HTOLE16 ((uint16_t)(sizeof (request->MODULE_SPEC)) + modules * sizeof (struct vs_module_spec));
	request->MODULE_SPEC.MOD_OP_SESSION_ID = HTOLE32 (plc->cookie);
	request->MODULE_SPEC.NUM_MODULES = modules;
	while (modules--)
	{
		request_spec->MODULE_ID = HTOLE16 (vs_module_spec->MODULE_ID);
		request_spec->MODULE_SUB_ID = HTOLE16 (vs_module_spec->MODULE_SUB_ID);
		request_spec->MODULE_LENGTH = HTOLE32 (vs_module_spec->MODULE_LENGTH);
		request_spec->MODULE_CHKSUM = vs_module_spec->MODULE_CHKSUM;
		vs_module_spec++;
		request_spec++;
	}
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	channel->timeout = PLC_MODULE_REQUEST_TIMEOUT;
	if (ReadMME (plc, 0, (VS_MODULE_OPERATION | MMTYPE_CNF)) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		channel->timeout = timer;
		return (-1);
	}
	channel->timeout = timer;
	if (confirm->MSTATUS)
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	return (0);
}


#endif

