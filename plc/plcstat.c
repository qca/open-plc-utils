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

/*====================================================================*"
 *
 *   plcstat.c - 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*"
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
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
#include "../plc/chipset.c"
#include "../plc/Confirm.c"
#include "../plc/Display.c"
#include "../plc/Failure.c"
#include "../plc/Request.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#include "../plc/Devices.c"
#include "../plc/LinkStatistics.c"
#include "../plc/LocalDevices.c"
#include "../plc/NetworkInformation2.c"
#include "../plc/Topology2.c"
#include "../plc/Platform.c"
#include "../plc/WaitForStart.c"
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
#include "../tools/typename.c"
#endif

#ifndef MAKEFILE
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#include "../ether/channel.c"
#endif

#ifndef MAKEFILE
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/QualcommHeader1.c"
#include "../mme/UnwantedMessage.c"
#include "../mme/MMECode.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define LINKIDS 	(sizeof (linkids) / sizeof (struct _term_))
#define DIRECTIONS 	(sizeof (directions) / sizeof (struct _term_))

#define AMPSTAT_LOOP 1
#define AMPSTAT_WAIT 0

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

static const struct _term_ linkids [] = 

{
	{
		"CSMA-ALL",
		"0xFC"
	},
	{
		"CSMA-CAP0",
		"0x00"
	},
	{
		"CSMA-CAP1",
		"0x01"
	},
	{
		"CSMA-CAP2",
		"0x02"
	},
	{
		"CSMA-CAP3",
		"0x03"
	},
	{
		"CSMA-PEER",
		"0xF8"
	},
};

static const struct _term_ directions [] = 

{
	{
		"both",
		"2"
	},
	{
		"rx",
		"1"
	},
	{
		"tx",
		"0"
	}
};


/*====================================================================*
 *   
 *   void manager (struct plc * plc);
 *   
 *   perform operations in logical order despite any order specfied 
 *   on the command line; for example read PIB before writing PIB; 
 *
 *   operation order is controlled by the order of "if" statements 
 *   shown here; the entire operation sequence can be repeated with
 *   an optional pause between each iteration;
 * 
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

void manager (struct plc * plc, signed count, signed pause) 

{
	while (count--) 
	{
		if (_anyset (plc->flags, PLC_ANALYSE)) 
		{
			Topology2 (plc);
		}
		if (_anyset (plc->flags, PLC_NETWORK)) 
		{
			NetworkInformation2 (plc);
		}
		if (_anyset (plc->flags, PLC_LINK_STATS)) 
		{
			LinkStatistics (plc);
		}
		sleep (pause);
	}
	return;
}


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
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	static char const * optv [] = 
	{
		"Cd:ei:l:mp:qs:tvw:",
		"device [device] [...] [> stdout]",
		"Qualcomm Atheros AR7x00 Link Statistics",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"C\tclear statistics without reading using VS_LNK_STATS",
		"d n\tdirection (0=tx, 1=rx, 2=both) for VS_LNK_STATS",
		"e\tredirect stderr to stdout",
		"l n\tloop n times [" LITERAL (AMPSTAT_LOOP) "]",
		"s n\tLink ID for VS_LNK_STATS (see Programmer's Guide)",
		"m\tprint network membership information using VS_NW_INFO",
		"p x\tpeer node address for options -s",
		"q\tquiet mode",
		"t\tprint network topology using VS_NW_INFO with VS_SW_VER",
		"v\tverbose mode",
		"w n\twait n seconds [" LITERAL (AMPSTAT_WAIT) "]",
		(char const *) (0)
	};

#include "../plc/plc.c"

	signed loop = AMPSTAT_LOOP;
	signed wait = AMPSTAT_WAIT;
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
	plc.pushbutton = 0;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'C':
			_setbits (plc.flags, PLC_LINK_STATS);
			plc.pushbutton = 1;
			break;
		case 'd':
			_setbits (plc.flags, PLC_LINK_STATS);
			plc.module = (uint8_t)(uintspec (synonym (optarg, directions, DIRECTIONS), 0, UCHAR_MAX));
			break;
		case 'e':
			dup2 (STDOUT_FILENO, STDERR_FILENO);
			break;
		case 'i':

#if defined (WINPCAP) || defined (LIBPCAP)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'm':
			_setbits (plc.flags, PLC_NETWORK);
			break;
		case 'p':
			_setbits (plc.flags, PLC_LINK_STATS);
			if (!hexencode (plc.RDA, sizeof (plc.RDA), (char const *)(optarg))) 
			{
				error (1, errno, PLC_BAD_MAC, optarg);
			}
			break;
		case 'l':
			loop = (unsigned)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 's':
			_setbits (plc.flags, PLC_LINK_STATS);
			plc.action = (uint8_t)(uintspec (synonym (optarg, linkids, LINKIDS), 0, UCHAR_MAX));
			break;
		case 't':
			_setbits (plc.flags, PLC_ANALYSE);
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		case 'w':
			wait = (unsigned)(uintspec (optarg, 0, 3600));
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (!argc) 
	{
		manager (&plc, loop, wait);
	}
	while ((argc) && (* argv)) 
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices)))) 
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		manager (&plc, loop, wait);
		argc--;
		argv++;
	}
	free (plc.message);
	closechannel (&channel);
	return (0);
}

