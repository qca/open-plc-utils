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
 *   void Confirm (struct plc * plc, char const * message);
 *
 *   plc.h
 *
 *   Inform the user that an operation has completed; print the channel
 *   name, frame source address and user message on stdout unless the
 *   PLC_SILENCE flag is set;
 *
 *   this function is identical to Request () except that it prints the
 *   packet source address instead of the template destination address;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CONFIRM_SOURCE
#define CONFIRM_SOURCE

#include <stdio.h>
#include <stdarg.h>

#include "../plc/plc.h"
#include "../tools/memory.h"
#include "../tools/flags.h"

#ifdef __GNUC__

__attribute__ ((format (printf, 2, 3)))

#endif

void Confirm (struct plc * plc, char const *format, ...)

{
	if (_allclr (plc->flags, PLC_SILENCE))
	{
		char address [ETHER_ADDR_LEN * 3];
		struct channel * channel = (struct channel *)(plc->channel);
		struct message * message = (struct message *)(plc->message);
		hexdecode (message->ethernet.OSA, sizeof (message->ethernet.OSA), address, sizeof (address));
		fprintf (stderr, "%s %s ", channel->ifname, address);
		if ((format) && (*format))
		{
			va_list arglist;
			va_start (arglist, format);
			vfprintf (stderr, format, arglist);
			va_end (arglist);
		}
		fprintf (stderr, "\n");
	}
	return;
}


#endif

