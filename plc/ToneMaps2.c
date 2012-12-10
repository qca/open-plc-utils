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
 *   signed ToneMaps2 (struct plc * plc);
 *
 *   amp.h
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef TONEMAPS2_SOURCE
#define TONEMAPS2_SOURCE

#include <stdio.h>
#include <stdint.h>

#include "../tools/flags.h"
#include "../tools/error.h"
#include "../tools/endian.h"
#include "../plc/plc.h"
#include "../mme/mme.h"

#ifndef SIGNALTONOISE2_SOURCE

static uint8_t const mod2bits [AMP_BITS] = 

{
	0,
	1,
	2,
	3,
	4,
	6,
	8,
	10,
	12
};


#endif

#ifndef SIGNALTONOISE2_SOURCE

static uint8_t const mod2db [AMP_BITS] = 

{
	0,
	2,
	4,
	7,
	10,
	16,
	22,
	28,
	36
};


#endif

signed ToneMaps2 (struct plc * plc) 

{
	uint8_t tonemap [PLC_SLOTS + 1][AMP_CARRIERS >> 1];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	uint16_t extent = 0;
	uint16_t carriers = AMP_CARRIERS;
	uint16_t carrier = 0;
	uint8_t slots = PLC_SLOTS;
	uint8_t slot = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_tonemap_char_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t MME_SUBVER;
		uint8_t Reserved1 [3];
		uint8_t MACADDRESS [ETHER_ADDR_LEN];
		uint8_t TMSLOT;
		uint8_t COUPLING;
	}
	* request = (struct vs_tonemap_char_request *) (message);
	struct __packed vs_tonemap_char_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t MSTATUS;
		uint8_t Reserved1;
		uint16_t MME_LEN;
		struct __packed vs_tonemap_char_header 
		{
			uint8_t MME_SUBVER;
			uint8_t Reserved2;
			uint8_t MACADDRESS [6];
			uint8_t TMSLOT;
			uint8_t COUPLING;
			uint8_t NUMTMS;
			uint8_t Reserved4;
			uint16_t TMNUMACTCARRIERS;
			uint16_t Reserved5;
			uint16_t NUMCARRIERS;
			uint16_t Reserved6;
			uint32_t Reserved7;
		}
		header;
		uint8_t MOD_CARRIER [1];
	}
	* confirm = (struct vs_tonemap_char_confirm *) (message);
	struct __packed vs_tonemap_char_fragment 
	{
		struct ethernet_std ethernet;
		struct homeplug_fmi qualcomm;
		uint8_t MOD_CARRIER [1];
	}
	* fragment = (struct vs_tonemap_char_fragment *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (tonemap, 0, sizeof (tonemap));
	for (carrier = slot = 0; slot < slots; carrier = 0, slot++) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, HOMEPLUG_MTYPE);
		FragmentHeader (&request->qualcomm, 1, (VS_TONE_MAP_CHAR | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		memcpy (request->MACADDRESS, plc->RDA, sizeof (request->MACADDRESS));
		request->TMSLOT = slot;
		request->COUPLING = plc->action;
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 1, (VS_TONE_MAP_CHAR | MMTYPE_CNF)) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (confirm->MSTATUS) 
		{
			error (1, 0, "Device refused request for slot %d: %s", slot, MMECode (VS_TONE_MAP_CHAR | MMTYPE_CNF, confirm->MSTATUS));
		}
		carriers = LE16TOH (confirm->header.TMNUMACTCARRIERS);
		slots = confirm->header.NUMTMS;
		extent = LE16TOH (confirm->MME_LEN) - sizeof (struct vs_tonemap_char_header);
		if (extent > (AMP_CARRIERS >> 1)) 
		{
			error (1, EOVERFLOW, "Too many carriers");
		}
		plc->packetsize -= sizeof (struct vs_tonemap_char_confirm);
		plc->packetsize += sizeof (confirm->MOD_CARRIER);
		if (plc->packetsize > extent) 
		{
			plc->packetsize = extent;
		}
		memcpy (&tonemap [slot] [carrier], &confirm->MOD_CARRIER, plc->packetsize);
		carrier += plc->packetsize;
		extent -= plc->packetsize;
		while (extent) 
		{
			if (ReadMME (plc, 1, (VS_TONE_MAP_CHAR | MMTYPE_CNF)) <= 0) 
			{
				error (1, errno, CHANNEL_CANTREAD);
			}
			plc->packetsize -= sizeof (struct vs_tonemap_char_fragment);
			plc->packetsize += sizeof (fragment->MOD_CARRIER);
			if (plc->packetsize > extent) 
			{
				plc->packetsize = extent;
			}
			memcpy (&tonemap [slot] [carrier], fragment->MOD_CARRIER, plc->packetsize);
			carrier += plc->packetsize;
			extent -= plc->packetsize;
		}
	}
	for (carrier = 0; carrier < carriers; carrier++) 
	{
		uint16_t scale = 0;
		uint16_t value = 0;
		uint16_t index = carrier >> 1;
		printf ("%04d", carrier);
		for (slot = 0; slot < slots; slot++) 
		{
			value = tonemap [slot][index];
			if ((carrier & 1)) 
			{
				value >>= 4;
			}
			value &= 0x0F;
			printf (",%02d", mod2bits [value]);
			value *= value;
			scale += value;
		}
		if (slots) 
		{
			scale /= slots;
		}
		printf (" %03d ", scale);
		if (_anyset (plc->flags, PLC_GRAPH)) 
		{
			while (scale--) 
			{
				printf ("#");
			}
		}
		printf ("\n");
	}
	return (0);
}


#endif

