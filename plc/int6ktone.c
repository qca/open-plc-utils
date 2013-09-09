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
 *   plctone.c - Atheros INT6x00 Tone Map Dump Utility
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*"
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/symbol.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/plc.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/Devices.c"
#include "../plc/Failure.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#include "../plc/SignalToNoise1.c"
#include "../plc/ToneMaps1.c"
#endif

#ifndef MAKEFILE
#include "../tools/error.c"
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/hexdump.c"
#include "../tools/hexencode.c"
#include "../tools/hexdecode.c"
#include "../tools/hexstring.c"
#include "../tools/todigit.c"
#include "../tools/synonym.c"
#endif

#ifndef MAKEFILE
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#include "../ether/channel.c"
#endif

#ifndef MAKEFILE
#include "../mme/MMECode.c"
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#endif

/*====================================================================*
 *
 *   int main (int argc, char const * argv[]);
 *
 *   parse command line, populate plc structure and perform selected
 *   operations; show help summary if asked; see getoptv and putoptv
 *   to understand command line parsing and help summary display; see
 *   plc.h for the definition of struct plc;
 *
 *   the command line accepts multiple MAC addresses and the program
 *   performs the specified operations on each address, in turn; the
 *   address order is significant but the option order is not; the
 *   default address is a local broadcast that causes all devices on
 *   the local H1 interface to respond but not those at the remote
 *   end of the powerline;
 *
 *   the default address is 00:B0:52:00:00:01; omitting the address
 *   will automatically address the local device; some options will
 *   cancel themselves if this makes no sense;
 *
 *   the default interface is eth1 because most people use eth0 as
 *   their principle network connection; you can specify another
 *   interface with -i or define environment string PLC to make
 *   that the default interface and save typing;
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv [])

{
	extern struct channel channel;
	static char const * optv [] =
	{
		"ehi:qsvx",
		"node peer [> stdout]",
		"Qualcomm Atheros INT6x00 Tone Map Dump Utility",
		"e\tredirect stderr to stdout",
		"h\tprint mean-square histogram",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"q\tquiet mode",
		"s\tcompute signal-to-noise and bits-per-carrier ratios",
		"v\tverbose mode",
		"x\texit on error",
		(char const *) (0)
	};

#include "../plc/plc.c"

	signed (* function)(struct plc *) = ToneMaps1;
	signed c;
	if (getenv (PLCDEVICE))
	{

#if defined (WINPCAP) || defined (LIBPCAP)

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
		case 'e':
			dup2 (STDOUT_FILENO, STDERR_FILENO);
			break;
		case 'h':
			_setbits (plc.flags, PLC_GRAPH);
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
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 's':
			_setbits (plc.flags, PLC_ANALYSE);
			function = SignalToNoise1;
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		case 'x':
			_setbits (plc.flags, PLC_BAILOUT);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc || !argv)
	{
		error (1, ECANCELED, "No node address given");
	}
	if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices))))
	{
		error (1, errno, PLC_BAD_MAC, * argv);
	}
	argc--;
	argv++;
	if (!argc || !argv)
	{
		error (1, ECANCELED, "No peer address given");
	}
	if (!hexencode (plc.RDA, sizeof (plc.RDA), synonym (* argv, devices, SIZEOF (devices))))
	{
		error (1, errno, PLC_BAD_MAC, * argv);
	}
	argc--;
	argv++;
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message))))
	{
		error (1, errno, PLC_NOMEMORY);
	}
	function (&plc);
	free (plc.message);
	closechannel (&channel);
	return (0);
}

