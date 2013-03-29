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
 *   efru.c - Ethernet Frame Read Utility
 *   
 *   print message header and/or full message content on stdout for
 *   each HomePlugAV or Atheros Vendor Specific message received by
 *   the host;
 *   
 *   this program receives raw ethernet frames and so requires root
 *   privileges; if you install it using "chmod 555" and "chown 
 *   root:root" then you must login as root to run it; otherwise, you 
 *   can install it using "chmod 4555" and "chown root:root" so that 
 *   anyone can run it; the program will refuse to run until you get
 *   thing right;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
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

#ifdef __linux__
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
#include "../tools/basespec.c"
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

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define EFRU_VERBOSE (1 << 0)
#define EFRU_SILENCE (1 << 1)

#define EFRU_INTERFACE "PLC"
#define EFRU_ETHERTYPE ETH_P_802_2

/*====================================================================*
 *   
 *   int main (int argc, char * argv[]);
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	struct message message;
	static char const * optv [] = 
	{
		"e:i:qt:v",
		PUTOPTV_S_DIVINE,
		"Qualcomm Atheros Ethernet Frame Read Utility",
		"e x\tethertype is (x) [" LITERAL (EFRU_ETHERTYPE) "]",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (s) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (n) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"q\tsuppress normal output",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_FOREVER) "]",
		"v\tverbose messages on stdout",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed length;
	signed c;
	if (getenv (EFRU_INTERFACE)) 
	{

#if defined (WINPCAP) || defined (LIBPCAP)

		channel.ifindex = atoi (getenv (EFRU_INTERFACE));

#else

		channel.ifname = strdup (getenv (EFRU_INTERFACE));

#endif

	}
	optind = 1;
	channel.type = EFRU_ETHERTYPE;
	channel.timer = CHANNEL_FOREVER;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'e':
			channel.type = (uint16_t)(basespec (optarg, 16, sizeof (channel.type)));
			break;
		case 'i':

#if defined (WIN32)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'q':
			_setbits (flags, EFRU_SILENCE);
			break;
		case 't':
			channel.timer = (signed)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'v':
			_setbits (flags, EFRU_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc) 
	{
		error (1, ECANCELED, ERROR_TOOMANY);
	}
	openchannel (&channel);
	while ((length = readpacket (&channel, &message, sizeof (message))) >= 0) 
	{
		hexdump (&message, 0, length, stdout);
	}
	closechannel (&channel);
	return (0);
}

