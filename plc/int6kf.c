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
 *   int6kf.c - Atheros Powerline Device Flash Utility;
 *
 *   this program sends and receives raw ethernet frames and so needs
 *   root priviledges; if you install it using "chmod 555" and "chown 
 *   root:root" then you must login as root to run it; otherwise, you 
 *   can install it using "chmod 4555" and "chown root:root" so that 
 *   anyone can run it; the program will refuse to run until you get
 *   things right;
 *   
 *   This software and documentation is the property of Atheros 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Atheros assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Atheros customers may modify and distribute the software 
 *   without obligation to Atheros. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *   
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <charles.maier@atheros.com>
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
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../ram/nvram.h"
#include "../ram/sdram.h"
#include "../nvm/nvm.h"
#include "../plc/plc.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/chipset.c"
#include "../plc/Confirm.c"
#include "../plc/Devices.c"
#include "../plc/Display.c"
#include "../plc/Failure.c"
#include "../plc/FlashNVM.c"
#include "../plc/ReadMME.c"
#include "../plc/Request.c"
#include "../plc/SendMME.c"
#include "../plc/StartDevice1.c"
#include "../plc/FlashDevice1.c"
#include "../plc/UpgradeDevice1.c"
#include "../plc/StartFirmware1.c"
#include "../plc/WriteFirmware1.c"
#include "../plc/WriteCFG.c"
#include "../plc/WriteMEM.c"
#include "../plc/WriteNVM.c"
#include "../plc/WritePIB.c"
#include "../plc/WaitForReset.c"
#include "../plc/WaitForStart.c"
#endif

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/checkfilename.c"
#include "../tools/hexdecode.c"
#include "../tools/hexstring.c"
#include "../tools/todigit.c"
#include "../tools/hexdump.c"
#include "../tools/checksum32.c"
#include "../tools/fdchecksum32.c"
#include "../tools/typename.c"
#include "../tools/strfbits.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../ram/sdramfile.c"
#include "../ram/sdrampeek.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvm.c"
#include "../nvm/nvmfile1.c"
#include "../nvm/nvmpeek1.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibfile1.c"
#include "../pib/pibpeek1.c"
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
#include "../mme/HomePlugHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#include "../mme/MMECode.c"
#endif

#ifndef MAKEFILE
#include "../key/keys.c"
#endif

/*====================================================================*
 *   
 *   int main (int argc, const char * argv[]);
 *   
 *   parse command line, populate int6k structure and perform selected 
 *   operations; show help summary when asked; see getoptv and putoptv
 *   to understand command line parsing and help summary display; see
 *   int6k.h for the definition of struct int6k; 
 *
 *   the default interface is eth1 because most people use eth0 as 
 *   their principle network connection; you can specify another 
 *   interface with -i or define environment string PLC to make
 *   that the default interface and save typing;
 *   
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *--------------------------------------------------------------------*/

int main (int argc, const char * argv []) 

{
	extern struct channel channel;
	static const char *optv [] = 
	{
		"C:i:eFN:p:P:qt:vx",
		"-C file -P file -N file",
		"Atheros Powerline Device Flash Utility for INT6300",
		"C f\twrite CFG file to device using VS_SET_SDRAM",
		"e\tredirect stderr messages to stdout",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface number [2]",

#else

		"i s\thost interface name [" CHANNEL_ETHDEVICE "]",

#endif

		"F[F]\tflash [force] NVRAM after firmware start using VS_MOD_NVM",
		"N f\twrite NVM file to device using VS_WR_MEM",
		"P f\twrite PIB file to device using VS_WR_MEM",
		"q\tquiet mode",

#if defined (WINPCAP) || defined (LIBPCAP)

		"t n\tread capture time is (n) milliseconds [50]",

#else

		"t n\tread timeout is (n) milliseconds [50]",

#endif

		"v\tverbose mode",
		"x\texit on error",
		(const char *) (0) 
	};

#include "../plc/plc.c"

	char firmware [PLC_VERSION_STRING];
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
	opterr = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'C':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.CFG.file = open (optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", optarg);
			}
			if (sdramfile (plc.CFG.file, optarg, plc.flags)) 
			{
				error (1, ECANCELED, "CFG file %s is corrupt", optarg);
			}
			_setbits (plc.flags, PLC_SDRAM_CONFIG);
			plc.CFG.name = optarg;
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
		case 'F':
			_setbits (plc.module, PLC_MODULE_NVM_PIB);
			if (_anyset (plc.flags, PLC_FLASH_DEVICE)) 
			{
				_setbits (plc.module, VS_MODULE_FORCE);
			}
			_setbits (plc.flags, PLC_FLASH_DEVICE);
			break;
		case 'N':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.NVM.file = open (optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", optarg);
			}
			plc.NVM.name = optarg;
			if (nvmfile1 (&plc.NVM)) 
			{
				error (1, errno, "Bad NVM file: %s", plc.NVM.name);
			}
			_setbits (plc.flags, PLC_WRITE_MAC);
			break;
		case 'P':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.PIB.file = open (optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", optarg);
			}
			plc.PIB.name = optarg;
			if (pibfile1 (&plc.PIB)) 
			{
				error (1, errno, "Bad PIB file: %s", plc.PIB.name);
			}
			_setbits (plc.flags, PLC_WRITE_PIB);
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 't':
			channel.timer = (unsigned)(uintspec (optarg, 0, UINT_MAX));
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
	if (argc) 
	{
		error (1, ECANCELED, "Too many arguments");
	}
	if (plc.CFG.file == -1) 
	{
		error (1, ECANCELED, "No CFG file specified");
	}
	if (plc.PIB.file == -1) 
	{
		error (1, ECANCELED, "No PIB file specified");
	}
	if (plc.NVM.file == -1) 
	{
		error (1, ECANCELED, "No NVM file specified");
	}
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (struct message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	if (WaitForStart (&plc, firmware, sizeof (firmware))) 
	{
		Failure (&plc, "Device must be connected");
		return (-1);
	}
	if (plc.hardwareID > CHIPSET_INT6300) 
	{
		Failure (&plc, "Device must be %s or earlier; try using int6kboot.", chipsetname (CHIPSET_INT6300));
		return (-1);
	}
	if (strcmp (firmware, "BootLoader")) 
	{
		Failure (&plc, "Bootloader must be running");
		return (-1);
	}
	if (!StartDevice1 (&plc)) 
	{
		if (_anyset (plc.flags, PLC_FLASH_DEVICE)) 
		{
			UpgradeDevice1 (&plc);
		}
	}
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

