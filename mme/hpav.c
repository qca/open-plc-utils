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
 *   hpav.c - Atheros HomePlug AV Packet Monitor;
 *   
 *   print message header and/or full message content on stdout for
 *   each HomePlugAV or Atheros Vendor Specific message received by
 *   the host;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined (__linux__)
#include <net/if.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <signal.h>
#endif

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../ether/channel.h"
#include "../plc/plc.h"
#include "../mme/mme.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/hexdump.c"
#include "../tools/hexdecode.c"
#include "../tools/hexstring.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../ether/channel.c"
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/sendpacket.c"
#include "../ether/readpacket.c"
#endif

#ifndef MAKEFILE
#include "../mme/MMEPeek.c"
#include "../mme/MMEName.c"
#include "../mme/MMEMode.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define HPAV_VERBOSE (1 << 0)
#define HPAV_SILENCE (1 << 1)

#define PLCDEVICE "PLC"

/*====================================================================*
 *   
 *   int main (int argc, char * argv[]);
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	struct message message;
	static char const * optv [] = 
	{
		"i:qt:v",
		PUTOPTV_S_DIVINE,
		"Qualcomm Atheros HomePlug AV Packet Monitor",

#if defined (WINPCAP) 

		"i s\thost interface is (s) [" CHANNEL_ETHDEVICE "]",

#else

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#endif

		"q\tsuppress normal output",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_TIMEOUT) "]",
		"v\tverbose messages on stdout",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed length;
	signed c;
	if (getenv (PLCDEVICE)) 
	{

#if defined (WINPCAP) 

		channel.ifindex = atoi (getenv (PLCDEVICE));

#else

		channel.ifname = strdup (getenv (PLCDEVICE));

#endif

	}
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'i':

#if defined (WIN32)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'q':
			_setbits (flags, HPAV_SILENCE);
			break;
		case 't':
			channel.timeout = (unsigned)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'v':
			_setbits (flags, HPAV_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	openchannel (&channel);
	while ((length = readpacket (&channel, &message, sizeof (message))) >= 0) 
	{
		if (length > 0) 
		{
			if (_allclr (flags, HPAV_SILENCE)) 
			{
				MMEPeek (&message, length, stdout);
			}
			if (_anyset (flags, HPAV_VERBOSE)) 
			{
				hexdump (&message, 0, length, stdout);
			}
		}
	}
	closechannel (&channel);
	return (0);
}

