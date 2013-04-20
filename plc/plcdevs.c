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
 *   nics.c - network interface enumerator;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <limits.h>
#include <memory.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../ether/channel.h"
#include "../ether/ether.h"
#include "../plc/plc.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/error.c"
#include "../tools/hexdecode.c"
#include "../tools/decdecode.c"
#include "../tools/hexstring.c"
#include "../tools/decstring.c"
#include "../tools/typename.c"
#include "../tools/hexdump.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../ether/hostnics.c"
#include "../ether/openchannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#include "../ether/closechannel.c"
#include "../ether/channel.c"
#endif

#ifndef MAKEFILE
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#endif

#ifndef MAKEFILE
#include "../plc/Platform.c"
#include "../plc/chipset.c"
#include "../plc/Devices.c"
#endif

/*====================================================================*
 *
 *   void enumerate ();
 *
 *   enumerate host interfaces on stdout;
 *
 *--------------------------------------------------------------------*/

static void enumerate (struct channel * channel, struct nic nic [], unsigned size) 

{
	extern const byte localcast [ETHER_ADDR_LEN];
	for (; size--; nic++) 
	{
		byte memory [ETHER_ADDR_LEN];
		char string [ETHER_ADDR_LEN * 3];
		memset (memory, 0x00, sizeof (memory));
		if (!memcmp (memory, nic->ethernet, sizeof (memory))) 
		{
			continue;
		}
		memset (memory, 0xFF, sizeof (memory));
		if (!memcmp (memory, nic->ethernet, sizeof (memory))) 
		{
			continue;
		}

#if defined (WINPCAP) || defined (LIBPCAP)

		printf (" %d", nic->ifindex);

#elif defined (__linux__) || defined (__OpenBSD__) || defined (__APPLE__)

		printf (" %s", nic->ifname);

#else
#error "Unknown environment"
#endif

		printf (" %s", hexstring (string, sizeof (string), nic->ethernet, sizeof (nic->ethernet)));
		printf (" %s", decstring (string, sizeof (string), nic->internet, sizeof (nic->internet)));

#if 0

		printf (" %s", nic->ifname);
		printf (" %s", nic->ifdesc);

#endif

		channel->ifname = nic->ifname;
		openchannel (channel);
		Platform (channel, localcast);
		closechannel (channel);
		printf ("\n");
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	static char const * optv [] = 
	{
		"qt:v",
		PUTOPTV_S_DIVINE,
		"Atheros Ethernet Interface Enumerator",
		(char const *) (0)
	};
	struct nic nics [16];
	unsigned size = hostnics (nics, sizeof (nics) / sizeof (struct nic));
	signed c;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
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
		error (1, ENOTSUP, ERROR_TOOMANY);
	}
	enumerate (&channel, nics, size);
	return (0);
}

