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
 *   plcID.c - Qualcomm Atheros Powerline Device Identity
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
#include <limits.h>
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
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/hexencode.c"
#include "../tools/hexdecode.c"
#include "../tools/todigit.c"
#include "../tools/hexdump.c"
#include "../tools/hexout.c"
#include "../tools/error.c"
#include "../tools/synonym.c"
#include "../tools/checksum32.c"
#endif

#ifndef MAKEFILE
#include "../plc/Display.c"
#include "../plc/Devices.c"
#include "../plc/Failure.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#endif

#ifndef MAKEFILE
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
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
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define PLCID_DAK 0
#define PLCID_NMK 1
#define PLCID_MAC 2
#define PLCID_MFG 3
#define PLCID_USR 4
#define PLCID_NET 5

/*====================================================================*
 *
 *   signed function (struct plc * plc, signed newline, signed key);
 *
 *   plc.h
 *
 *   read start of parameter chain from the device using a single
 *   VS_MODULE_OPERATION message; search parameter chain for PIB and
 *   print requested key on stdout;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed function (struct plc * plc, signed newline, signed key)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	struct nvm_header2 * nvm_header;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	signed module = 0;
	char * filename = "device";

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_module_operation_read_request
	{
		struct ethernet_hdr ethernet;
		struct qualcomm_hdr qualcomm;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		struct __packed
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint16_t MODULE_ID;
			uint16_t MODULE_SUB_ID;
			uint16_t MODULE_LENGTH;
			uint32_t MODULE_OFFSET;
		}
		MODULE_SPEC;
	}
	* request = (struct vs_module_operation_read_request *)(message);
	struct __packed vs_module_operation_read_confirm
	{
		struct ethernet_hdr ethernet;
		struct qualcomm_hdr qualcomm;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		struct __packed
		{
			uint16_t MOD_OP;
			uint16_t MOD_OP_DATA_LEN;
			uint32_t MOD_OP_RSVD;
			uint16_t MODULE_ID;
			uint16_t MODULE_SUB_ID;
			uint16_t MODULE_LENGTH;
			uint32_t MODULE_OFFSET;
		}
		MODULE_SPEC;
		uint8_t MODULE_DATA [PLC_MODULE_SIZE];
	}
	* confirm = (struct vs_module_operation_read_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (&message, 0, sizeof (message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MODULE_OPERATION | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	request->NUM_OP_DATA = 1;
	request->MODULE_SPEC.MOD_OP = HTOLE16 (0);
	request->MODULE_SPEC.MOD_OP_DATA_LEN = HTOLE16 (sizeof (request->MODULE_SPEC));
	request->MODULE_SPEC.MOD_OP_RSVD = HTOLE32 (0);
	request->MODULE_SPEC.MODULE_ID = HTOLE16 (PLC_MODULEID_PARAMETERS);
	request->MODULE_SPEC.MODULE_SUB_ID = HTOLE16 (0);
	request->MODULE_SPEC.MODULE_LENGTH = HTOLE16 (PLC_MODULE_SIZE);
	request->MODULE_SPEC.MODULE_OFFSET = HTOLE32 (0);
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	if (ReadMME (plc, 0, (VS_MODULE_OPERATION | MMTYPE_CNF)) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
		return (-1);
	}
	if (confirm->MSTATUS)
	{
		Failure (plc, PLC_WONTDOIT);
		return (-1);
	}
	do
	{
		nvm_header = (struct nvm_header2 *)(&confirm->MODULE_DATA [offset]);
		if (LE16TOH (nvm_header->MajorVersion) != 1)
		{
			if (_allclr (plc->flags, PLC_SILENCE))
			{
				error (0, errno, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE16TOH (nvm_header->MinorVersion) != 1)
		{
			if (_allclr (plc->flags, PLC_SILENCE))
			{
				error (0, errno, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE32TOH (nvm_header->PrevHeader) != origin)
		{
			if (_allclr (plc->flags, PLC_SILENCE))
			{
				error (0, errno, NVM_HDR_LINK, filename, module);
			}
			return (-1);
		}
		if (checksum32 (nvm_header, sizeof (* nvm_header), 0))
		{
			error (0, 0, NVM_HDR_CHECKSUM, filename, module);
			return (-1);
		}
		origin = offset;
		offset += sizeof (* nvm_header);
		if (LE32TOH (nvm_header->ImageType) == NVM_IMAGE_PIB)
		{
			struct simple_pib * pib = (struct simple_pib *)(&confirm->MODULE_DATA [offset]);
			if (key == PLCID_MAC)
			{
				hexout (pib->MAC, sizeof (pib->MAC), HEX_EXTENDER, 0, stdout);
			}
			else if (key == PLCID_DAK)
			{
				hexout (pib->DAK, sizeof (pib->DAK), HEX_EXTENDER, 0, stdout);
			}
			else if (key == PLCID_NMK)
			{
				hexout (pib->NMK, sizeof (pib->NMK), HEX_EXTENDER, 0, stdout);
			}
			else if (key == PLCID_MFG)
			{
				pib->MFG [PIB_HFID_LEN - 1] = (char)(0);
				printf ("%s", pib->MFG);
			}
			else if (key == PLCID_USR)
			{
				pib->USR [PIB_HFID_LEN - 1] = (char)(0);
				printf ("%s", pib->USR);
			}
			else if (key == PLCID_NET)
			{
				pib->NET [PIB_HFID_LEN - 1] = (char)(0);
				printf ("%s", pib->NET);
			}
			if (_anyset (plc->flags, PLC_NEWLINE))
			{
				putc (newline, stdout);
			}
			break;
		}
		if (checksum32 (&confirm->MODULE_DATA [offset], LE32TOH (nvm_header->ImageLength), nvm_header->ImageChecksum))
		{
			if (_allclr (plc->flags, PLC_SILENCE))
			{
				error (0, errno, NVM_IMG_CHECKSUM, filename, module);
			}
			return (-1);
		}
		offset += LE32TOH (nvm_header->ImageLength);
		module++;
	}
	while (~nvm_header->NextHeader);
	return (0);
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
		"Ac:Dei:MnNqSUv",
		"device",
		"Qualcomm Atheros Powerline Device Identity",
		"A\tEthernet address (MAC)",
		"c c\tcharacter delimiter is (c)",
		"D\tDevice Access Key (DAK)",
		"e\tredirect stderr to stdout",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"M\tNetwork Membership Key (NMK)",
		"n\tappend newline on output",
		"N\tnetwork HFID",
		"q\tquiet mode",
		"S\tmanufacturer HFID",
		"U\tuser HFID",
		"v\tverbose mode",
		(char const *) (0)
	};

#include "../plc/plc.c"

	signed newline = '\n';
	signed key = PLCID_DAK;
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
		case 'A':
			key = PLCID_MAC;
			break;
		case 'c':
			newline = * optarg;
			break;
		case 'D':
			key = PLCID_DAK;
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
		case 'M':
			key = PLCID_NMK;
			break;
		case 'n':
			_setbits (plc.flags, PLC_NEWLINE);
			break;
		case 'N':
			key = PLCID_NET;
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 'S':
			key = PLCID_MFG;
			break;
		case 'U':
			key = PLCID_USR;
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
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message))))
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (!argc)
	{
		function (&plc, newline, key);
	}
	while ((argc) && (* argv))
	{
		if (!hexencode (channel.peer, sizeof (channel.peer), synonym (* argv, devices, SIZEOF (devices))))
		{
			error (1, errno, PLC_BAD_MAC, * argv);
		}
		function (&plc, newline, key);
		argv++;
		argc--;
	}
	free (plc.message);
	closechannel (&channel);
	return (0);
}

