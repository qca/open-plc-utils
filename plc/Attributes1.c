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
 *   signed Attributes1 (struct plc * plc);
 *   
 *   plc.h
 *
 *   This plugin for program plc requests device attributes using
 *   a VS_OP_ATTRIBUTES message; attributes are pre-parsed versions
 *   of information returned by VS_SW_VER and other messages; 
 * 
 *   The VS_OP_ATTRIBUTES message structure changed between FW 3.3.4
 *   and 3.3.5 and fields are ocassionally appended to the end; you  
 *   should not use this message for operational systems because the
 *   format may change again; it was originally intended for PTS use
 *   only;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef ATTRIBUTES1_SOURCE
#define ATTRIBUTES1_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/memory.h"
#include "../tools/format.h"
#include "../tools/error.h"
#include "../tools/flags.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

signed Attributes1 (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_op_attributes_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint32_t COOKIE;
		uint8_t RTYPE;
	}
	* request = (struct vs_op_attributes_request *) (message);
	struct __packed vs_op_attributes_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint16_t MSTATUS;
		uint32_t COOKIE;
		uint8_t RTYPE;
		uint16_t MLENGTH;
		uint8_t MBUFFER [1024];
	}
	* confirm = (struct vs_op_attributes_confirm *) (message);
	struct __packed attributes 
	{
		uint8_t HARDWARE [16];
		uint8_t SOFTWARE [16];
		uint32_t FWVERSION [5];
		uint8_t RELEASEDATE [8];
		uint8_t RELEASETYPE [12];
		uint8_t SDRAMTYPE;
		uint8_t RESERVED;
		uint8_t LINEFREQ;
		uint32_t SDRAMSIZE;
		uint8_t AUTHMODE;
	}
	* attributes = (struct attributes *)(confirm->MBUFFER);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Fetch Device Attributes");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_OP_ATTRIBUTES | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->COOKIE = HTOLE32 (plc->cookie);
	request->RTYPE = 0;
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_OP_ATTRIBUTES | MMTYPE_CNF)) > 0) 
	{
		static char const * frequency [] = 
		{
			"Unknown Frequency",
			"50Hz",
			"60Hz",
		};
		static char const * zero_cross [] = 
		{
			"Not yet detected",
			"Detected",
			"Missing",
		};
		char string [512];
		size_t length = 0;
		if (confirm->MSTATUS) 
		{
			Failure (plc, PLC_WONTDOIT);
			continue;
		}

#if 0
#if defined (__GNUC__)
#warning "Debug code active in module Attributes1"
#endif

		fprintf (stderr, "HARDWARE=[%-16.16s]\n", attributes->HARDWARE);
		fprintf (stderr, "SOFTWARE=[%-16.16s]\n", attributes->SOFTWARE);
		fprintf (stderr, "FIRMWARE MAJOR VERSION=(%d)\n", LE32TOH (attributes->FWVERSION [0]));
		fprintf (stderr, "FIRMWARE MINOR VERSION=(%d)\n", LE32TOH (attributes->FWVERSION [1]));
		fprintf (stderr, "FIRMWARE [2]=(%d)\n", LE32TOH (attributes->FWVERSION [2]));
		fprintf (stderr, "FIRMWARE [3]=(%d)\n", LE32TOH (attributes->FWVERSION [3]));
		fprintf (stderr, "FIRMWARE [4]=(%d)\n", LE32TOH (attributes->FWVERSION [4]));
		fprintf (stderr, "RELEASE DATE=[%-8.8s]\n", attributes->RELEASEDATE);
		fprintf (stderr, "RELEASE TYPE=[%-12.12s]\n", attributes->RELEASETYPE);
		fprintf (stderr, "SDRAM SIZE=[0x%08X]\n", LE32TOH (attributes->SDRAMSIZE));
		fprintf (stderr, "SDRAM TYPE=[0x%02X]\n", attributes->SDRAMTYPE);
		fprintf (stderr, "RESERVED=[0x%02X]\n", attributes->RESERVED);
		fprintf (stderr, "LINE FREQUENCY=[0x%02X]\n", attributes->LINEFREQ);
		fprintf (stderr, "AUTHORIZATION MODE=[0x%02X]\n", attributes->AUTHMODE);

#endif

		length += snprintf (string + length, sizeof (string) - length, "%s", attributes->HARDWARE);
		length += snprintf (string + length, sizeof (string) - length, "-%s", attributes->SOFTWARE);
		length += snprintf (string + length, sizeof (string) - length, "-%d", LE32TOH (attributes->FWVERSION [0]));
		length += snprintf (string + length, sizeof (string) - length, "-%d", LE32TOH (attributes->FWVERSION [1]));
		length += snprintf (string + length, sizeof (string) - length, "-%04d", LE32TOH (attributes->FWVERSION [2]));
		length += snprintf (string + length, sizeof (string) - length, "-%02d", LE32TOH (attributes->FWVERSION [3]));
		length += snprintf (string + length, sizeof (string) - length, "-%04d", LE32TOH (attributes->FWVERSION [4]));
		length += snprintf (string + length, sizeof (string) - length, "-%-8.8s", attributes->RELEASEDATE);
		length += snprintf (string + length, sizeof (string) - length, "-%s", attributes->RELEASETYPE);
		length += snprintf (string + length, sizeof (string) - length, "-%c", attributes->SDRAMTYPE);
		length += snprintf (string + length, sizeof (string) - length, " (%dmb) ", LE32TOH (attributes->SDRAMSIZE));
		length += snprintf (string + length, sizeof (string) - length, " (0x%02X) %s %s ", attributes->LINEFREQ, frequency [attributes->LINEFREQ & 0x03], zero_cross [(attributes->LINEFREQ & 0x0C) >> 2]);
		length += snprintf (string + length, sizeof (string) - length, " 0x%02X", attributes->AUTHMODE);
		Display (plc, "%s", string);
	}
	return (0);
}


#endif

