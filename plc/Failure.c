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
 *
 *   Contributor(s);
 *      Charles Maier <cmaier@qca.qualcomm.com>
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
		case VS_CONN_ADD | MMTYPE_CNF:
		case VS_CONN_MOD | MMTYPE_CNF:
		case VS_CONN_REL | MMTYPE_CNF:
		case VS_CONN_INFO | MMTYPE_CNF:
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

