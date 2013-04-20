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
 *   edeu.c - Ethernet Frame Echo Utility
 *   
 *   Listen for incoming Ethernet II frames and write frame data to 
 *   stdout as a continuous binary data stream. Since frame data can 
 *   contain anything, users should send program output to a file or 
 *   pipe it through a filter program to prevent corruption of the 
 *   terminal session. If all incoming data is ASCII then directing 
 *   stdout to the console should not cause any problems.
 *   
 *   this program can be used as a data target when testing AR6405
 *   UART applications; use program edsu to send files from a remote
 *   host and this program to display or save them;
 *
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
#include "../tools/memswap.c"
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

#define EFEU_INTERFACE "PLC"
#define EFEU_ETHERTYPE ETH_P_802_2

/*====================================================================*
 *   
 *   int main (int argc, char * argv[]);
 *   
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	struct ethernet_frame frame;
	signed length;
	static char const * optv [] = 
	{
		"e:i:qt:v",
		PUTOPTV_S_DIVINE,
		"Qualcomm Atheros Ethernet Frame Echo Utility",
		"e x\tethertype is (x) [" LITERAL (EFEU_ETHERTYPE) "]",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"q\tsuppress normal output",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_FOREVER) "]",
		"v\tverbose messages on stdout",
		(char const *) (0)
	};
	signed c;
	channel.type = EFEU_ETHERTYPE;
	channel.timeout = CHANNEL_FOREVER;
	if (getenv (EFEU_INTERFACE)) 
	{

#if defined (WINPCAP) || defined (LIBPCAP)

		channel.ifindex = atoi (getenv (EFEU_INTERFACE));

#else

		channel.ifname = strdup (getenv (EFEU_INTERFACE));

#endif

	}
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'e':
			channel.type = (uint16_t)(basespec (optarg, 16, sizeof (channel.type)));
			break;
		case 'i':

#if defined (WINPCAP) || defined (LIBPCAP)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			break;
		case 't':
			channel.timeout = (signed)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
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
	while ((length = readpacket (&channel, &frame, sizeof (frame))) > 0) 
	{
		memswap (&frame.frame_dhost, &frame.frame_shost, sizeof (frame.frame_dhost));
		if (sendpacket (&channel, &frame, length) != length)
		{
			error (1, errno, CHANNEL_CANTSEND);
		}
	}
	closechannel (&channel);
	return (0);
}

