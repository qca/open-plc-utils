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
 *   signed GetProperty (struct plc * plc, struct property * property);
 *   
 *   plc.h
 *
 *   interrogte PLC devices for a property using the VS_GET_PROPERTY
 *   message and display the property value on stdout as a series of 
 *   binary, decimal or hexadecimal bytes or an ASCII string;
 *
 *   the current implementation retrieves properties by number, not
 *   by name;
 * 
 *--------------------------------------------------------------------*/

#ifndef GETPROPERTY_SOURCE
#define GETPROPERTY_SOURCE

#include <ctype.h>
#include <stdint.h>
#include <memory.h>

#include "../tools/error.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../plc/plc.h"

signed GetProperty (struct plc * plc, struct plcproperty * plcproperty) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_get_property_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t COOKIE;
		uint8_t DATA_FORMAT;
		uint8_t PROP_FORMAT;
		uint8_t RESERVED [2];
		uint32_t PROP_VERSION;
		uint32_t PROP_LENGTH;
		uint8_t PROP_NUMBER;
	}
	* request = (struct vs_get_property_request *) (message);
	struct __packed vs_get_property_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t MSTATUS;
		uint32_t COOKIE;
		uint8_t DATA_FORMAT;
		uint8_t RESERVED [3];
		uint32_t DATA_LENGTH;
		uint32_t DATA_BUFFER [1];
	}
	* confirm = (struct vs_get_property_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Get Property");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_GET_PROPERTY | MMTYPE_REQ));
	request->COOKIE = HTOLE32 (plc->cookie);
	request->DATA_FORMAT = plcproperty->DATA_FORMAT;
	request->PROP_FORMAT = plcproperty->PROP_FORMAT;
	request->PROP_VERSION = HTOLE32 (plcproperty->PROP_VERSION);
	request->PROP_LENGTH = HTOLE32 (plcproperty->PROP_LENGTH);
	request->PROP_NUMBER = HTOLE32 (plcproperty->PROP_NUMBER);
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_GET_PROPERTY | MMTYPE_CNF)) > 0) 
	{
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			continue;
		}
		if (plcproperty->DATA_FORMAT == PLC_FORMAT_BIN) 
		{
			binout (confirm->DATA_BUFFER, confirm->DATA_LENGTH, ' ', '\n', stdout);
			continue;
		}
		if (plcproperty->DATA_FORMAT == PLC_FORMAT_HEX) 
		{
			hexout (confirm->DATA_BUFFER, confirm->DATA_LENGTH, ' ', '\n', stdout);
			continue;
		}
		if (plcproperty->DATA_FORMAT == PLC_FORMAT_DEC) 
		{
			decout (confirm->DATA_BUFFER, confirm->DATA_LENGTH, ' ', '\n', stdout);
			continue;
		}
		if (plcproperty->DATA_FORMAT == PLC_FORMAT_ASC) 
		{
			chrout (confirm->DATA_BUFFER, confirm->DATA_LENGTH, '.', '\n', stdout);
			continue;
		}
	}
	return (0);
}


#endif

