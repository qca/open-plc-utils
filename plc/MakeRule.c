/*====================================================================*
 *   
 *   Copyright (c) 2013 Qualcomm Atheros Inc.
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
 *   signed MakeRule (struct plc * plc, struct rule * rule);
 *
 *   plc.h
 *
 *   This plugin for program plcrule adds or removes a temporary
 *   or permanent network classification rule to a device using a
 *   VS_CLASSIFICATION message;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MAKERULE_SOURCE
#define MAKERULE_SOURCE

#include "../tools/error.h"

#include "../plc/rules.h"
#include "../plc/plc.h"

signed MakeRule (struct plc * plc, struct MMERule * rule) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_classification_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		struct MMERule rule;
	}
	* request = (struct vs_classification_request *)(message);
	struct __packed vs_classification_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* confirm = (struct vs_classification_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Set MakeRules Rules");
	memset (message, 0, sizeof (* message));
	EthernetHeader (& request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (& request->qualcomm, 0, (VS_CLASSIFICATION | MMTYPE_REQ));
	plc->packetsize = sizeof (struct vs_classification_request);
	memcpy (& request->rule, rule, sizeof (request->rule));
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMME (plc, 0, (VS_CLASSIFICATION | MMTYPE_CNF)) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	Confirm (plc, "Setting ...");
	return (0);
}

#endif

