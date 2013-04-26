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
 *   plcmdio.c - Atheros Powerline PHY Register Editor 
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
#include <string.h>
#include <ctype.h>

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
#include "../ram/nvram.h"
#include "../ram/sdram.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"
#include "../mme/mme.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/Devices.c"
#include "../plc/Display.c"
#include "../mme/UnwantedMessage.c"
#endif

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/hexencode.c"
#include "../tools/hexdecode.c"
#include "../tools/todigit.c"
#include "../tools/hexdump.c"
#include "../tools/hexview.c"
#include "../tools/regview32.c"
#include "../tools/synonym.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../ether/channel.c"
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#endif

#ifndef MAKEFILE
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/MMECode.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define INT6KMDIO_RELATIVE (1 << 0)
#define INT6KMDIO_ABSOLUTE (1 << 1)
#define INT6KMDIO_REGISTER (1 << 2)
#define INT6KMDIO_HEADINGS (1 << 3)
#define INT6KMDIO_WRITE (1 << 4)

/*====================================================================*
 *
 *   void function (struct channel * channel, uint32_t offset, uint32_t length, flag_t flags);
 *   
 *   
 *
 *--------------------------------------------------------------------*/

static void function (struct channel * channel, uint8_t mode, uint8_t phy, uint8_t reg, uint16_t data) 

{
	struct message message;
	signed packetsize;

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_mdio_command_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t OPERATION;
		uint8_t PHY;
		uint8_t REG;
		uint16_t DATA;
	}
	* request = (struct vs_mdio_command_request *)(&message);
	struct __packed vs_mdio_command_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint16_t DATA;
		uint8_t PHY;
		uint8_t REG;
	}
	* confirm = (struct vs_mdio_command_confirm *)(&message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (&message, 0, sizeof (message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MDIO_COMMAND | MMTYPE_REQ));
	request->OPERATION = mode;
	request->PHY = phy;
	request->REG = reg;
	request->DATA = HTOLE16 (data);
	if (sendpacket (channel, &message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) == -1) 
	{
		error (1, errno, CHANNEL_CANTSEND);
	}
	while ((packetsize = readpacket (channel, &message, sizeof (message))) > 0) 
	{
		if (UnwantedMessage (&message, packetsize, 0, (VS_MDIO_COMMAND | MMTYPE_CNF))) 
		{
			continue;
		}
		if (confirm->MSTATUS) 
		{
			error (0, 0, "%s (%0X): %s", MMECode (confirm->qualcomm.MMTYPE, confirm->MSTATUS), confirm->MSTATUS, PLC_WONTDOIT);
			continue;
		}
		printf (" phy 0x%02X", confirm->PHY);
		printf (" reg 0x%02X", confirm->REG);
		printf (" data 0x%04X", confirm->DATA);
		printf ("\n");
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *   
 *   
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	static char const * optv [] = 
	{
		"d:ei:p:qr:v",
		"[device] [...]",
		"Qualcomm Atheros 16-bit MDIO Register Editor",
		"d n\tdata value is (n) [0x0000]",
		"e\tredirect stderr to stdout",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"p n\tPHY number (n) [0xFF]",
		"q\tquiet mode",
		"r n\tregister number (n) [0xFF]",
		"v\tverbose mode",
		(char const *) (0)
	};
	uint8_t mode = 0;
	uint8_t phy = 0;
	uint8_t reg = 0;
	uint16_t data = 0;
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
		case 'd':
			mode = 1;
			data = (uint16_t)(uintspec (optarg, 0, 0xFFFF));
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
		case 'p':
			phy = (uint8_t)(uintspec (optarg, 0, 0x1F));
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			break;
		case 'r':
			reg = (uint8_t)(uintspec (optarg, 0, 0x1F));
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
	openchannel (&channel);
	if (!argc) 
	{
		function (&channel, mode, phy, reg, data);
	}
	while ((argc) && (* argv)) 
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices)))) 
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		function (&channel, mode, phy, reg, data);
		argv++;
		argc--;
	}
	closechannel (&channel);
	return (0);
}

