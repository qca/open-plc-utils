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
 *   signed PushButton (struct plc * plc);
 *
 *   plc.h
 *
 *   THis plugin for program plc emulates pushbutton functionality
 *   using a MS_PB_ENC message;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PUSHBUTTON_SOURCE
#define PUSHBUTTON_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/memory.h"
#include "../plc/plc.h"

signed PushButton (struct plc * plc)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed ms_pb_enc_request
	{
		struct ethernet_std ethernet;
		struct homeplug_std homeplug;
		uint8_t PBACTION;
	}
	* request = (struct ms_pb_enc_request *) (message);
	struct __packed ms_pb_enc_confirm
	{
		struct ethernet_std ethernet;
		struct homeplug_std homeplug;
		uint8_t MSTATUS;
		uint8_t AVLNSTAT;
	}
	* confirm = (struct ms_pb_enc_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	if (plc->pushbutton == 1)
	{
		Request (plc, "Join Network");
	}
	if (plc->pushbutton == 2)
	{
		Request (plc, "Leave Network");
	}
	if (plc->pushbutton == 3)
	{
		Request (plc, "Fetch Network Status");
	}
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	request->homeplug.MMV = 0;
	request->homeplug.MMTYPE = HTOLE16 (MS_PB_ENC | MMTYPE_REQ);
	request->PBACTION = plc->pushbutton;
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMFG (plc, 0, (MS_PB_ENC | MMTYPE_CNF)) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS)
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	if (plc->pushbutton == 1)
	{
		Confirm (plc, "Joining ...");
		return (0);
	}
	if (plc->pushbutton == 2)
	{
		Confirm (plc, "Leaving ...");
		return (0);
	}
	if (plc->pushbutton == 3)
	{
		Confirm (plc, "Membership Status %d", confirm->AVLNSTAT);
		return (0);
	}
	Failure (plc, "Unexpected pushbutton action code");
	return (-1);
}


#endif

