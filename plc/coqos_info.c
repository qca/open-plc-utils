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
 *   coqos_info.c - Retrieve MCMSA stream info
 *
 *
 *   Contributor(s):
 *	Bill Wike <bill.wike@qca.qualcomm.com>
 *	Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *	Charles Maier <cmaier@qca.qualcomm.com>
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
#include "../plc/coqos.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/hexdump.c"
#include "../tools/hexencode.c"
#include "../tools/bytespec.c"
#include "../tools/ipv4spec.c"
#include "../tools/ipv6spec.c"
#include "../tools/hexdecode.c"
#include "../tools/todigit.c"
#include "../tools/typename.c"
#include "../tools/error.c"
#include "../tools/synonym.c"
#include "../tools/lookup.c"
#include "../tools/assist.c"
#include "../tools/codelist.c"
#endif

#ifndef MAKEFILE
#include "../plc/Confirm.c"
#include "../plc/Display.c"
#include "../plc/Failure.c"
#include "../plc/ReadMME.c"
#include "../plc/Request.c"
#include "../plc/SendMME.c"
#include "../plc/Devices.c"
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#endif

#ifndef MAKEFILE
#include "../ether/channel.c"
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#endif

#ifndef MAKEFILE
#include "../mme/MMECode.c"
#endif

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define CONTROL "control"

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

#define CONTROL_STA 0x00
#define CONTROL_CID 0x01
#define CONTROL_RSVD 0x02
#define CONTROL_VIC 0x03
#define CONTROL_BW 0x04
#define CONTROL_BWC 0xFF

static struct _code_ const types [] =

{
	{
		CONTROL_STA,
		"STA"
	},
	{
		CONTROL_CID,
		"CID"
	},
	{
		CONTROL_RSVD,
		"RSVD"
	},
	{
		CONTROL_VIC,
		"VIC"
	},
	{
		CONTROL_BW,
		"BW"
	},
	{
		CONTROL_BWC,
		"BWC"
	}
};


/*====================================================================*
 *
 *   signed con_info (struct plc * plc, uint8_t TYPE, uint16_t CID, uint16_t TOT_BW_USED);
 *
 *
 *
 *   Contributor(s):
 *	Bill Wike <bill.wike@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed con_info (struct plc * plc, uint8_t TYPE, uint16_t CID, uint16_t TOT_BW_USED)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_con_info_req
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t REQ_TYPE;
		uint32_t RSVD;
		uint16_t CSPEC_VER;
		uint16_t CID;
		uint16_t TOT_BW_USED;
	}
	* request = (struct vs_con_info_req *)(message);
	struct __packed vs_con_info_cnf
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t REQ_TYPE;
		uint8_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RSVD;
		uint8_t NUM_CONN;
		struct conn_info CONN_INFO [64];
	}
	* confirm = (struct vs_con_info_cnf *)(message);
	struct conn_info * conn_info = (struct conn_info*)(confirm->CONN_INFO);
	struct vs_con_info_cnf * indicate = (struct vs_con_info_cnf *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	int i;
	Request (plc, "COQOS connection information");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_CONN_INFO | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->REQ_TYPE = TYPE;
	request->CID = CID;
	request->TOT_BW_USED = TOT_BW_USED;
	request->CSPEC_VER = 0x01;
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_CONN_INFO | MMTYPE_CNF)) <= 0);
	if (confirm->MSTATUS)
	{
		Failure (plc, "Error requesting information.");
		return (-1);
	}

/* BW exceeded request and "cancel all requests" do not send indicate */

	if ((TYPE == CONTROL_BW) || (TYPE == CONTROL_BWC))
	{
		Confirm (plc, "Success");
		return (0);
	}
	Confirm (plc, "Received confirm, waiting for indicate...");
	while (ReadMME (plc, 0, (VS_CONN_INFO | MMTYPE_IND)) <= 0);
	if (indicate->MSTATUS)
	{
		Failure (plc, "Error requesting information.");
		return (-1);
	}
	printf ("Number of Connections: %d\n", (int)indicate->NUM_CONN);
	conn_info = (struct conn_info *)(indicate->CONN_INFO);
	for (i = 0; i < indicate->NUM_CONN; i++)
	{
		printf ("SNID: %02X ", conn_info->SNID);
		printf ("CID: %04X ", conn_info->CID);
		printf ("STEI: %02X ", conn_info->STEI);
		printf ("DTEI: %02X ", conn_info->DTEI);
		printf ("LLID: %02X ", conn_info->LLID);
		printf ("CSPEC_VERSION: %04X ", conn_info->CSPECVERSION);
		printf ("CONN_CAP: %02d ", conn_info->CONN_CAP);
		printf ("CONN_COQOS_PRIO: %02d ", conn_info->CONN_COQOS_PRIO);
		printf ("CONN_RATE: %d ", conn_info->CONN_RATE);
		printf ("CONN_TTL: %d ", conn_info->CONN_TTL);
		printf ("BW_USED: %d \n", conn_info->BW_USED);
		conn_info++;
	}
	return (0);
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv[]);
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv [])

{
	extern struct channel channel;
	static char const * optv [] =
	{
		"ei:qv",
		"[sta|cid|rsvd|vic|bw|bwc] [limit] [device] [...]",
		"CoQos Stream Utility",
		"e\tredirect stderr to stdout",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};

#include "../plc/plc.c"

	uint16_t cid = 0;
	uint16_t limit = 0;
	uint8_t type;
	signed code;
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
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if ((code = lookup (* argv++, types, SIZEOF (types))) == -1)
	{
		assist (*--argv, CONTROL, types, SIZEOF (types));
	}
	type = (uint8_t)(code);
	argc--;
	if (type == CONTROL_CID)
	{
		if (!argc)
		{
			error (1, errno, "Expected Connection Identifier");
		}
		if (!hexencode ((uint8_t *)(&cid), sizeof (cid), * argv++))
		{
			error (1, errno, "Invalid CID");
		}
		cid = htons (cid);
		argc--;
	}
	else if (type == CONTROL_BW)
	{
		if (!argc)
		{
			error (1, errno, "Expected Bandwidth Limit");
		}
		limit = atoi (* argv++);
		argc--;
	}
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message))))
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (!argc)
	{
		con_info (&plc, type, cid, limit);
	}
	while ((argc) && (* argv))
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices))))
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		con_info (&plc, type, cid, limit);
		argc--;
		argv++;
	}
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

