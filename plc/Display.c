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
 *   void Display (struct plc * plc, char const * message);
 *   
 *   plc.h
 *
 *   Inform the user that an operation suceeded; print channel name,
 *   packet source device and user defined message on stdout;
 *
 *   This function is identical to Confirm () except that a message
 *   is always printed on stdout and cannot be suppressed; 
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef DISPLAY_SOURCE
#define DISPLAY_SOURCE

#include <stdio.h>
#include <stdarg.h>

#include "../plc/plc.h"
#include "../tools/memory.h"

#ifdef __GNUC__

__attribute__ ((format (printf, 2, 3))) 

#endif

void Display (struct plc * plc, char const *format, ...) 

{
	char address [ETHER_ADDR_LEN * 3];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	hexdecode (message->ethernet.OSA, sizeof (message->ethernet.OSA), address, sizeof (address));
	printf ("%s %s ", channel->ifname, address);
	if ((format) && (*format)) 
	{
		va_list arglist;
		va_start (arglist, format);
		vprintf (format, arglist);
		va_end (arglist);
	}
	printf ("\n");
	return;
}


#endif

