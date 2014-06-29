/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or 
 *   without modification, are permitted (subject to the limitations 
 *   in the disclaimer below) provided that the following conditions 
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials 
 *     provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros nor the names of 
 *     its contributors may be used to endorse or promote products 
 *     derived from this software without specific prior written 
 *     permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE 
 *   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE 
 *   COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 *   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   hpavd.c - Qualcomm Atheros Homeplug AV Packet Daemon;
 *
 *   prints incoming Qualcomm Atheros HomePlug AV vendor-specific
 *   management messages on stdout in hex dump format for analysis;
 *
 *   the program can be run in foreground or background, as a daemon;
 *
 *   this program receives raw ethernet frames and so requires root
 *   privileges; if you install it using "chmod 555" and "chown
 *   root:root" then you must login as root to run it; otherwise, you
 *   can install it using "chmod 4555" and "chown root:root" so that
 *   anyone can run it; the program will refuse to run until you get
 *   thing right;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
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
#include <sys/signal.h>
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
#include "../tools/permissions.h"
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
#include "../tools/desuid.c"
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
 *    program constants;
 *--------------------------------------------------------------------*/

#define HPAVD_DAEMON (1 << 0)
#define HPAVD_VERBOSE (1 << 1)
#define HPAVD_SILENCE (1 << 2)
#define HPAVD_ERROR (1 << 3)

#define PLCDEVICE "PLC"
#define ETHDEVICE "eth1"
#define ETHNUMBER 2

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

static bool done = false;

/*====================================================================*
 *
 *   void terminate (signo_t signal);
 *
 *   terminate the program; we want to ensure an orgaized program
 *   exit so that the original Ethernet adapter state is restored;
 *
 *
 *--------------------------------------------------------------------*/

static void terminate (signo_t signal)

{
	done = true;
	return;
}


/*====================================================================*
 *
 *   int main (int argc, char * argv[]);
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv [])

{
	extern struct channel channel;
	struct message message;
	struct sigaction sa;
	static char const * optv [] =
	{
		"di:qv",
		PUTOPTV_S_DIVINE,
		"Qualcomm Atheros HomePlug AV Packet Daemon",
		"d\trun in background as daemon",
		"i s\thost interface is (s) [" ETHDEVICE "]",
		"q\tsuppress normal output",
		"v\tverbose messages on stdout",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
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
		switch ((char) (c))
		{
		case 'd':
			_setbits (flags, HPAVD_DAEMON);
			break;
		case 'i':

#if defined (WIN32)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'q':
			_setbits (flags, HPAVD_SILENCE);
			break;
		case 'v':
			_setbits (flags, HPAVD_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	openchannel (&channel);
	desuid ();
	if (_anyset (flags, HPAVD_DAEMON))
	{
		pid_t pid = fork ();
		if (pid < 0)
		{
			error (1, errno, "razzlefrats!");
		}
		if (pid > 0)
		{
			exit (0);
		}
	}
	memset (&sa, 0, sizeof (struct sigaction));
	sa.sa_handler = terminate;
	sigaction (SIGTERM, &sa, (struct sigaction *)(0));
	sigaction (SIGQUIT, &sa, (struct sigaction *)(0));
	sigaction (SIGTSTP, &sa, (struct sigaction *)(0));
	sigaction (SIGINT, &sa, (struct sigaction *)(0));
	sigaction (SIGHUP, &sa, (struct sigaction *)(0));
	while (!done)
	{
		signed length = length = readpacket (&channel, &message, sizeof (message));
		if (length > 0)
		{
			if (_allclr (flags, HPAVD_SILENCE))
			{
				MMEPeek (&message, length, stdout);
			}
			if (_anyset (flags, HPAVD_VERBOSE))
			{
				hexdump (&message, 0, length, stdout);
			}
		}
	}
	closechannel (&channel);
	return (0);
}

