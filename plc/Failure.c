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
 *   void Failure (struct plc * plc, char const *format, ...) 
 *
 *   error.h
 *
 *   Inform the user that an operation failed; print the channel name, 
 *   source device, error message and user defined message on stderr
 *   unless the PLC_SILENCE flags is set;
 *
 *   This function is similar to Confirm () except that the message
 *   status or result code and description is output when the code
 *   is non-zero; overtime, result codes have replaced status codes
 *   and so we look in different places for codes in some MMEs;
 *
 *   the status and result code fields appear at different offsets
 *   in different messages; consequently, check the MMTYPE to find
 *   field;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s);
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FAILURE_SOURCE
#define FAILURE_SOURCE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../plc/plc.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/memory.h"
#include "../mme/mme.h"

#ifdef __GNUC__

__attribute__ ((format (printf, 2, 3))) 

#endif

void Failure (struct plc * plc, char const *format, ...) 

{
	if (_allclr (plc->flags, PLC_SILENCE)) 
	{
		char address [ETHER_ADDR_LEN * 3];
		struct channel * channel = (struct channel *)(plc->channel);
		struct message * message = (struct message *)(plc->message);
		struct __packed header_confirm 
		{
			ethernet_std ethernet;
			qualcomm_std qualcomm;
			uint8_t MSTATUS;
		}
		* header = (struct header_confirm *)(message);
		hexdecode (header->ethernet.OSA, sizeof (header->ethernet.OSA), address, sizeof (address));
		fprintf (stderr, "%s %s ", channel->ifname, address);
		switch (LE16TOH (header->qualcomm.MMTYPE)) 
		{
		case VS_ADD_CONN | MMTYPE_CNF:
		case VS_MOD_CONN | MMTYPE_CNF:
		case VS_REL_CONN | MMTYPE_CNF:
		case VS_CON_INFO | MMTYPE_CNF:
			{
				struct __packed header_confirm 
				{
					struct ethernet_std ethernet;
					struct qualcomm_std qualcomm;
					uint32_t REQUEST;
					uint8_t MSTATUS;
				}
				* header = (struct header_confirm *)(message);
				if (header->MSTATUS) 
				{
					fprintf (stderr, "%s (0x%02X): ", MMECode (header->qualcomm.MMTYPE, header->MSTATUS), header->MSTATUS);
				}
			}
			break;
		case VS_SELFTEST_RESULTS | MMTYPE_CNF:
		case VS_FORWARD_CONFIG | MMTYPE_CNF:
			{
				struct __packed header_confirm 
				{
					struct ethernet_std ethernet;
					struct qualcomm_std qualcomm;
					uint8_t MVERSION;
					uint8_t RESULTCODE;
				}
				* header = (struct header_confirm *)(message);
				if (header->RESULTCODE) 
				{
					fprintf (stderr, "%s (0x%02X): ", MMECode (header->qualcomm.MMTYPE, header->RESULTCODE), header->RESULTCODE);
				}
			}
			break;
		case VS_DEBUG_INFO | MMTYPE_CNF:
			{
				struct __packed header_confirm 
				{
					struct ethernet_std ethernet;
					struct homeplug_fmi homeplug;
					uint8_t OUI [3];
					uint8_t MSTATUS;
				}
				* header = (struct header_confirm *)(message);
				if (header->MSTATUS) 
				{
					fprintf (stderr, "%s (0x%02X): ", MMECode (header->homeplug.MMTYPE, header->MSTATUS), header->MSTATUS);
				}
			}
			break;
		case VS_ACCESS_LEVEL_CONTROL | MMTYPE_CNF:
			{
				struct __packed header_confirm 
				{
					struct ethernet_std ethernet;
					struct qualcomm_std qualcomm;
					uint16_t RESERVED;
					uint16_t MRESPONSE;
				}
				* header = (struct header_confirm *)(message);
				if (header->MRESPONSE) 
				{
					fprintf (stderr, "%s (0x%02X): ", MMECode (header->qualcomm.MMTYPE, (uint8_t)(header->MRESPONSE)), header->MRESPONSE);
				}
			}
			break;
		default:
			if (header->MSTATUS) 
			{
				fprintf (stderr, "%s (0x%02X): ", MMECode (header->qualcomm.MMTYPE, header->MSTATUS), header->MSTATUS);
			}
			break;
		}
		if ((format) && (*format)) 
		{
			va_list arglist;
			va_start (arglist, format);
			vfprintf (stderr, format, arglist);
			va_end (arglist);
		}
		fprintf (stderr, "\n");
	}
	if (_anyset (plc->flags, PLC_BAILOUT)) 
	{
		if (_allclr (plc->flags, PLC_SILENCE)) 
		{
			error (1, 0, "Bailing Out!");
		}
		exit (1);
	}
	return;
}


#endif

