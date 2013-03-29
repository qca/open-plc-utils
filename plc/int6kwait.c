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
 *   plcwait.c - Atheros Powerline Device Procrastinator;
 *
 *   wait for device events to start or finish before returning;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
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
#include <sys/time.h>

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
#include "../tools/timer.h"
#include "../tools/error.h"
#include "../plc/plc.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/hexdump.c"
#include "../tools/hexencode.c"
#include "../tools/hexdecode.c"
#include "../tools/todigit.c"
#include "../tools/checkfilename.c"
#include "../tools/synonym.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../plc/Confirm.c"
#include "../plc/Display.c"
#include "../plc/Failure.c"
#include "../plc/ReadMME.c"
#include "../plc/Request.c"
#include "../plc/SendMME.c"
#include "../plc/Devices.c"
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
#include "../mme/QualcommHeader1.c"
#include "../mme/UnwantedMessage.c"
#endif

#define RETRY 300
#define POLL 5

/*====================================================================*
 *
 *   signed ResetAndWait (struct plc * plc);
 * 
 *   plc.h
 *
 *   reset the device using a VS_RS_DEV Request message; continue to
 *   request resets each second until the device accepts the request
 *   or the wait period expires;
 *
 *--------------------------------------------------------------------*/

signed ResetAndWait (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_rs_dev_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
	}
	* request = (struct vs_rs_dev_request *) (message);
	struct __packed vs_rs_dev_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* confirm = (struct vs_rs_dev_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Reset when Ready");
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_RS_DEV | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_RS_DEV | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		if (plc->packetsize) 
		{
			if (!confirm->MSTATUS) 
			{
				Confirm (plc, "Resetting ...");
				return (0);
			}
		}
	}
	return (-1);
}


/*====================================================================*
 *
 *   signed WaitForReset (struct plc * plc, char string [], size_t length);
 *
 *   plc.h
 *
 *   send and receive VS_SW_VER messages until a confirmation is not
 *   received within channel->timer milliseconds to indicate that 
 *   the device is inactive; return 0 if the device resets within 
 *   plc->timer seconds; otherwise, return -1; 
 *
 *   this function cannot distinguish between a software reset and 
 *   hardware reset;
 *
 *--------------------------------------------------------------------*/

signed WaitForReset (struct plc * plc, char string [], size_t length) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* request = (struct vs_sw_ver_request *) (message);
	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* confirm = (struct vs_sw_ver_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (string, 0, length);
	Request (plc, "Allow %d seconds for Reset", plc->timer);
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_SW_VER | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		if (!plc->packetsize) 
		{
			if (_allset (plc->flags, (PLC_WAITFORRESET | PLC_ANALYSE))) 
			{
				Confirm (plc, "Waited %d seconds for Reset", timer);
			}
			memcpy (string, confirm->MVERSION, confirm->MVERLENGTH);
			return (0);
		}
	}
	if (_allset (plc->flags, (PLC_WAITFORRESET | PLC_ANALYSE))) 
	{
		Confirm (plc, "Waited %d seconds for Reset", timer);
	}
	return (-1);
}


/*====================================================================*
 *
 *   signed WaitForStart (struct plc * plc, char string [], size_t length);
 *
 *   plc.h
 *
 *   send VS_SW_VER messages until confirmation is received within
 *   channel->timer milliseconds to indicate that the device is 
 *   active; return 0 if the device responds within plc->timer 
 *   seconds; otherwise, return -1;
 *
 *   poll the device using VS_SW_VER messages until it responds or 
 *   the allotted time expires; return 0 if the device responds within
 *   the allotted time or -1 if it does not or if a transmission error
 *   occurs;
 *
 *--------------------------------------------------------------------*/

signed WaitForStart (struct plc * plc, char string [], size_t length) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* request = (struct vs_sw_ver_request *) (message);
	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		char MVERSION [PLC_VERSION_STRING];
	}
	* confirm = (struct vs_sw_ver_confirm *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Allow %d seconds for Start", plc->timer);
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (&request->qualcomm, 0, (VS_SW_VER | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_SW_VER | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		if (plc->packetsize) 
		{
			if (confirm->MSTATUS) 
			{
				Failure (plc, PLC_WONTDOIT);
				return (-1);
			}
			if (_allset (plc->flags, (PLC_WAITFORSTART | PLC_ANALYSE))) 
			{
				Confirm (plc, "Waited %d seconds for Start", timer);
			}
			strncpy (string, confirm->MVERSION, length);
			return (0);
		}
	}
	if (_allset (plc->flags, (PLC_WAITFORSTART | PLC_ANALYSE))) 
	{
		Confirm (plc, "Waited %d seconds for Start", timer);
	}
	return (-1);
}


/*====================================================================*
 *   
 *   signed WaitForAssoc (struct plc * plc);
 *
 *   plc.h
 *
 *   send and receive VS_NW_INFO messages until the device reports 
 *   that a network exists and has at least one station; return 0 if 
 *   a network forms within plc->timer seconds; otherwise, return
 *   -1;
 *
 *--------------------------------------------------------------------*/

signed WaitForAssoc (struct plc * plc) 

{
	extern const uint8_t broadcast [ETHER_ADDR_LEN];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct timeval ts;
	struct timeval tc;
	unsigned timer = 0;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_nw_info_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
	}
	* request = (struct vs_nw_info_request *)(message);
	struct __packed vs_nw_info_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t SUB_VERSION;
		uint8_t Reserved;
		uint16_t DATA_LEN;
		uint8_t DATA [1];
	}
	* confirm = (struct vs_nw_info_confirm *)(message);
	struct __packed station 
	{
		uint8_t MAC [ETHER_ADDR_LEN];
		uint8_t TEI;
		uint8_t Reserved [3];
		uint8_t BDA [ETHER_ADDR_LEN];
		uint16_t AVGTX;
		uint8_t COUPLING;
		uint8_t Reserved3;
		uint16_t AVGRX;
		uint16_t Reserved4;
	}
	* station;
	struct __packed network 
	{
		uint8_t NID [7];
		uint8_t Reserved1 [2];
		uint8_t SNID;
		uint8_t TEI;
		uint8_t Reserved2 [4];
		uint8_t ROLE;
		uint8_t CCO_MAC [ETHER_ADDR_LEN];
		uint8_t CCO_TEI;
		uint8_t Reserved3 [3];
		uint8_t NUMSTAS;
		uint8_t Reserved4 [5];
		struct station stations [1];
	}
	* network;
	struct __packed networks 
	{
		uint8_t Reserved;
		uint8_t NUMAVLNS;
		struct network networks [1];
	}
	* networks = (struct networks *) (confirm->DATA);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Allow %d seconds for Assoc", plc->timer);
	if (gettimeofday (&ts, NULL) == -1) 
	{
		error (1, errno, CANT_START_TIMER);
	}
	for (timer = 0; timer < plc->timer; timer = SECONDS (ts, tc)) 
	{
		memset (message, 0, sizeof (* message));
		EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
		plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
		if (SendMME (plc) <= 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 1, (VS_NW_INFO | MMTYPE_CNF)) < 0) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (gettimeofday (&tc, NULL) == -1) 
		{
			error (1, errno, CANT_RESET_TIMER);
		}
		if (plc->packetsize) 
		{
			network = (struct network *)(&networks->networks);
			while (networks->NUMAVLNS--) 
			{
				station = (struct station *)(&network->stations);
				while (network->NUMSTAS--) 
				{
					if (memcmp (station->MAC, broadcast, sizeof (broadcast))) 
					{
						if (_allset (plc->flags, (PLC_WAITFORASSOC | PLC_ANALYSE))) 
						{
							Confirm (plc, "Waited %d seconds for Assoc", timer);
						}
						return (0);
					}
					station++;
				}
				network = (struct network *)(station);
			}
		}
	}
	if (_allset (plc->flags, (PLC_WAITFORASSOC | PLC_ANALYSE))) 
	{
		Confirm (plc, "Waited %d seconds for Assoc", timer);
	}
	return (-1);
}


/*====================================================================*
 *   
 *   void function (struct plc * plc, char const * firmware);
 *
 *   perform operations in a logical order;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

static void function (struct plc * plc, char const * firmware) 

{
	char string [PLC_VERSION_STRING];
	if (_anyset (plc->flags, PLC_RESET_DEVICE)) 
	{
		if (ResetAndWait (plc)) 
		{
			Failure (plc, "Device did not Reset.");
		}
	}
	if (_anyset (plc->flags, PLC_WAITFORRESET)) 
	{
		if (WaitForReset (plc, string, sizeof (string))) 
		{
			Failure (plc, "Device did not Reset.");
		}
	}
	if (_anyset (plc->flags, PLC_WAITFORSTART)) 
	{
		if (WaitForStart (plc, string, sizeof (string))) 
		{
			Failure (plc, "Device did not Start.");
		}
		if ((firmware) && (*firmware) && strcmp (firmware, string)) 
		{
			Failure (plc, "Started wrong firmware");
		}
	}
	if (_anyset (plc->flags, PLC_WAITFORASSOC)) 
	{
		if (WaitForAssoc (plc)) 
		{
			Failure (plc, "Device did not Assoc.");
		}
	}
	if (plc->sleep) 
	{
		Request (plc, "Pause %d seconds", plc->sleep);
		sleep (plc->sleep);
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
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	static char const * optv [] = 
	{
		"ac:ef:i:p:qrRstvxw:",
		"device [device] [...] [> stdout]",
		"Qualcomm Atheros INT6x00 Powerline Device Procrastinator",
		"a\twait for device assoc",
		"c n\tpolling retry count [" LITERAL (RETRY) "]",
		"e\tredirect stderr to stdout",
		"f s\tconfirm firmware is revision s",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"p n\tpoll (n) times per second [" LITERAL (POLL) "]",
		"q\tquiet mode",
		"v\tverbose mode",
		"R\treset device and wait",
		"r\twait for device reset",
		"s\twait for device start",
		"t\tshow wait times",
		"w n\twait n seconds",
		"x\texit on error",
		(char const *) (0)
	};

#include "../plc/plc.c"

	char const * firmware = "";
	unsigned retry = RETRY;
	unsigned poll = POLL;
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
		case 'a':
			_setbits (plc.flags, PLC_WAITFORASSOC);
			break;
		case 'c':
			retry = (unsigned)(uintspec (optarg, 1, UINT_MAX));
			break;
		case 'e':
			dup2 (STDOUT_FILENO, STDERR_FILENO);
			break;
		case 'f':
			firmware = optarg;
			break;
		case 'i':

#if defined (WINPCAP) || defined (LIBPCAP)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'p':
			poll = (unsigned)(uintspec (optarg, 1, 50));
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 'r':
			_setbits (plc.flags, PLC_WAITFORRESET);
			break;
		case 'R':
			_setbits (plc.flags, PLC_RESET_DEVICE);
			break;
		case 's':
			_setbits (plc.flags, PLC_WAITFORSTART);
			break;
		case 't':
			_setbits (plc.flags, PLC_ANALYSE);
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		case 'w':
			plc.sleep = (unsigned)(uintspec (optarg, 0, 3600));
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
	plc.timer = retry / poll;
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (!argc) 
	{
		function (&plc, firmware);
	}
	while ((argc) && (* argv)) 
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices)))) 
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		function (&plc, firmware);
		argv++;
		argc--;
	}
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

