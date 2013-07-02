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
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
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
#include "../tools/hexstring.c"
#include "../tools/hexdecode.c"
#include "../tools/error.c"
#include "../tools/emalloc.c"
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
	struct sigaction sa;
	struct ifreq ifreq; 
	struct sockaddr_ll sockaddr = 
	{
		PF_PACKET,
		htons (ETH_P_HPAV),
		0x0000,
		ARPHRD_ETHER,
		PACKET_OTHERHOST,
		ETHER_ADDR_LEN,
		{
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00
		}
	};
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
	uint8_t packet [ETHER_MAX_LEN];
	flag_t state = (flag_t)(0);
	flag_t flags = (flag_t)(0);
	sock_t fd = -1;
	signed c;
	memset (&ifreq, 0, sizeof (ifreq));
	memcpy (ifreq.ifr_name, ETHDEVICE, sizeof (ETHDEVICE));
	if (getenv (PLCDEVICE)) 
	{
		memcpy (ifreq.ifr_name, getenv (PLCDEVICE), sizeof (ifreq.ifr_name));
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
			memcpy (ifreq.ifr_name, optarg, sizeof (ifreq.ifr_name));
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
	if (geteuid ()) 
	{
		error (1, EPERM, ERROR_NOTROOT);
	}
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
	if ((fd = socket (sockaddr.sll_family, SOCK_RAW, sockaddr.sll_protocol)) == -1) 
	{
		error (1, errno, "Can't create socket for %s", ifreq.ifr_name);
	}
	if (ioctl (fd, SIOCGIFFLAGS, &ifreq) < 0) 
	{
		error (1, errno, "Can't read %s device state", ifreq.ifr_name);
	}
	state = ifreq.ifr_flags;
	_setbits (ifreq.ifr_flags, (IFF_UP | IFF_BROADCAST));
	_clrbits (ifreq.ifr_flags, (IFF_MULTICAST | IFF_ALLMULTI | IFF_PROMISC));
	if (ioctl (fd, SIOCSIFFLAGS, &ifreq) < 0) 
	{
		error (1, errno, "Can't change %s device state", ifreq.ifr_name);
	}
	if (ioctl (fd, SIOCGIFINDEX, &ifreq) == -1) 
	{
		error (1, errno, "Can't get %s interface index", ifreq.ifr_name);
	}
	sockaddr.sll_ifindex = ifreq.ifr_ifindex;
	if (ioctl (fd, SIOCGIFHWADDR, &ifreq) == -1) 
	{
		error (1, errno, "Can't get %s hardware address", ifreq.ifr_name);
	}
	memcpy (sockaddr.sll_addr, ifreq.ifr_ifru.ifru_hwaddr.sa_data, sizeof (sockaddr.sll_addr));
	if (bind (fd, (struct sockaddr *) (&sockaddr), sizeof (struct sockaddr_ll)) == -1) 
	{
		error (1, errno, "Can't bind socket to %s", ifreq.ifr_name);
	}
	while (!done) 
	{
		signed length = recvfrom (fd, packet, sizeof (packet), 0, (struct sockaddr *) (0), (socklen_t *)(0));
		if (length > 0) 
		{
			if (_allclr (flags, HPAVD_SILENCE)) 
			{
				MMEPeek (&packet, length, stdout);
			}
			if (_anyset (flags, HPAVD_VERBOSE)) 
			{
				hexdump (&packet, 0, length, stdout);
			}
		}
	}
	ifreq.ifr_flags = state;
	if (ioctl (fd, SIOCSIFFLAGS, &ifreq) < 0) 
	{
		error (1, errno, "Can't restore %s device state", ifreq.ifr_name);
	}
	close (fd);
	return (0);
}

