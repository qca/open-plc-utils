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
 *   int6krule.c - Qualcomm Atheros Message Classification Utility;
 *
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
#include "../plc/rules.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/basespec.c"
#include "../tools/hexdump.c"
#include "../tools/hexencode.c"
#include "../tools/bytespec.c"
#include "../tools/ipv4spec.c"
#include "../tools/ipv6spec.c"
#include "../tools/uintspec.c"
#include "../tools/hexdecode.c"
#include "../tools/todigit.c"
#include "../tools/endian.c"
#include "../tools/error.c"
#include "../tools/synonym.c"
#include "../tools/assist.c"
#include "../tools/lookup.c"
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
#include "../plc/rules.c"
#include "../plc/ParseRule.c"
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
 *   program constants;
 *--------------------------------------------------------------------*/

#define INT6KRULE_CSPEC_VERSION 0
#define INT6KRULE_VLAN_TAG 0x00000000
#define COMMA ","
#define QUOTE "''"

/*====================================================================*
 *
 *   signed Classification (struct plc * plc, struct MMERule * rule);
 *
 *   plc.h
 *
 *   This plugin for program plcrule adds or removes a temporary
 *   or permanent network classification rule to a device using a
 *   VS_CLASSIFICATION message;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

signed Classification (struct plc * plc, struct MMERule * rule) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_classification_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		struct MMERule rule;
	}
	* request = (struct vs_classification_request *)(message);
	struct __packed vs_classification_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* confirm = (struct vs_classification_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Set Classification Rules");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_CLASSIFICATION | MMTYPE_REQ));
	plc->packetsize = sizeof (struct vs_classification_request);
	memcpy (&request->rule, rule, sizeof (request->rule));
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMME (plc, 0, (VS_CLASSIFICATION | MMTYPE_CNF)) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS) 
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	Confirm (plc, "Setting ...");
	return (0);
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv[]);
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
		"ei:qst:T:vV:",
		"action operand condition [...] control volatility [device] [...]\n\n          where a condition is: field operator value",
		"Qualcomm Atheros Stream Classification Utility",
		"e\tredirect stderr to stdout",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"q\tquiet mode",
		"s\tdisplay symbol tables",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_TIMER) "]",
		"T x\tinserted vlan tag is x [" LITERAL (INT6KRULE_VLAN_TAG) "]",
		"v\tverbose mode",
		"V n\tcspec version is n [" LITERAL (INT6KRULE_CSPEC_VERSION) "]",
		(char const *) (0)
	};

#include "../plc/plc.c"

	struct cspec cspec;
	struct MMERule rule;
	signed c;
	memset (&rule, 0, sizeof (rule));
	memset (&cspec, 0, sizeof (cspec));
	cspec.VLAN_TAG = INT6KRULE_VLAN_TAG;
	cspec.CSPEC_VERSION = INT6KRULE_CSPEC_VERSION;
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
		case 's':
			printf ("\n");
			printf (" Controls are ");
			codelist (controls, CONTROLS, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" Volatilities are ");
			codelist (volatilities, VOLATILITIES, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" Actions are ");
			codelist (actions, ACTIONS, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" Operands are ");
			codelist (operands, OPERANDS, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" Fields are ");
			codelist (fields, FIELDS, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" Operators are ");
			codelist (operators, OPERATORS, COMMA, QUOTE, stdout);
			printf (".\n");
			printf (" States  are ");
			codelist (states, STATES, COMMA, QUOTE, stdout);
			printf (".\n");
			printf ("\n");
			return (0);
		case 't':
			channel.timer = (signed)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'T':
			cspec.VLAN_TAG = (uint32_t)(basespec (optarg, 16, sizeof (cspec.VLAN_TAG)));
			cspec.VLAN_TAG = htonl (cspec.VLAN_TAG);
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		case 'V':
			cspec.CSPEC_VERSION = (uint16_t)(basespec (optarg, 10, sizeof (cspec.CSPEC_VERSION)));
			cspec.CSPEC_VERSION = HTOLE16 (cspec.CSPEC_VERSION);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (ParseRule (&argc, &argv, &rule, &cspec) == -1) 
	{
		error (1, 0, "invalid rule");
	}
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (!argc) 
	{
		Classification (&plc, &rule);
	}
	while ((argc) && (* argv)) 
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices)))) 
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		Classification (&plc, &rule);
		argc--;
		argv++;
	}
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

