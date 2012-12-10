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

/*====================================================================*
 *
 *   int64host.c - Atheros INT6400 Host Emulator
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
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
#include "../plc/plc.h"
#include "../ram/nvram.h"
#include "../ram/sdram.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/BootDevice1.c"
#include "../plc/BootFirmware1.c"
#include "../plc/BootParameters1.c"
#include "../plc/chipset.c"
#include "../plc/Confirm.c"
#include "../plc/Devices.c"
#include "../plc/Failure.c"
#include "../plc/FlashNVM.c"
#include "../plc/HostActionResponse.c"
#include "../plc/InitDevice1.c"
#include "../plc/EmulateHost64.c"
#include "../plc/Request.c"
#include "../plc/ResetDevice.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#include "../plc/WaitForReset.c"
#include "../plc/WaitForStart.c"
#include "../plc/WriteExecuteFirmware1.c"
#include "../plc/WriteExecutePIB.c"
#include "../plc/StartFirmware1.c"
#include "../plc/WriteFirmware1.c"
#include "../plc/WriteMEM.c"
#include "../plc/WriteNVM.c"
#include "../plc/WritePIB.c"
#include "../plc/WriteCFG.c"
#include "../plc/ReadFirmware1.c"
#include "../plc/ReadParameters1.c"
#include "../plc/ModuleRead.c"
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
#include "../tools/error.c"
#include "../tools/strfbits.c"
#include "../tools/typename.c"
#endif

#ifndef MAKEFILE
#include "../ether/openchannel.c"
#include "../ether/closechannel.c"
#include "../ether/readpacket.c"
#include "../ether/sendpacket.c"
#include "../ether/setchanneltimeout.c"
#include "../ether/channel.c"
#endif

#ifndef MAKEFILE
#include "../mme/MMECode.c"
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#endif

#ifndef MAKEFILE
#include "../key/keys.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmfile1.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibfile1.c"
#include "../pib/pibpeek1.c"
#endif

/*====================================================================*
 *   
 *   int main (int argc, char const * argv[]);
 *   
 *   parse command line, populate plc structure and perform selected 
 *   operations; show help summary when asked; see getoptv and putoptv
 *   to understand command line parsing and help summary display; see
 *   plc.h for the definition of struct plc; 
 *
 *   the default interface is eth1 because most people use eth0 as 
 *   their principle network connection; you can specify another 
 *   interface with -i or define environment string PLC to make
 *   that the default interface and save typing;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	static char const * optv [] = 
	{
		"Fi:n:N:p:P:qt:vx",
		"-N file -P file [-n file] [-p file]",
		"Qualcomm Atheros INT6400 Host Emulator",
		"F[F]\tflash (force) NVRAM using VS_MOD_NVM",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"n f\tread NVM from device into file using VS_RD_MOD",
		"N f\twrite NVM file to device using VS_WR_MEM",
		"p f\tread PIB from device into file using VS_RD_MOD",
		"P f\twrite PIB file to device using VS_WR_MEM",
		"q\tquiet mode",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_TIMER) "]",
		"v\tverbose mode",
		"x\texit on error",
		(char const *) (0)
	};

#include "../plc/plc.c"

	signed c;
	plc.timer = 10000;
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
		case 'F':
			_setbits (plc.module, (VS_MODULE_MAC | VS_MODULE_PIB));
			if (_anyset (plc.flags, PLC_FLASH_DEVICE)) 
			{
				_setbits (plc.module, VS_MODULE_FORCE);
			}
			_setbits (plc.flags, PLC_FLASH_DEVICE);
			break;
		case 'i':

#if defined (WINPCAP) || defined (LIBPCAP)

			channel.ifindex = atoi (optarg);

#else

			channel.ifname = optarg;

#endif

			break;
		case 'N':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.NVM.file = open (plc.NVM.name = optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", plc.NVM.name);
			}
			if (nvmfile1 (&plc.NVM)) 
			{
				error (1, errno, "Bad NVM file: %s", plc.NVM.name);
			}
			_setbits (plc.flags, PLC_WRITE_MAC);
			break;
		case 'n':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.nvm.file = open (plc.nvm.name = optarg, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1) 
			{
				error (1, errno, "%s", plc.nvm.name);
			}
			break;
		case 'P':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.PIB.file = open (plc.PIB.name = optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", plc.PIB.name);
			}
			if (pibfile1 (&plc.PIB)) 
			{
				error (1, errno, "Bad PIB file: %s", plc.PIB.name);
			}
			_setbits (plc.flags, PLC_WRITE_PIB);
			break;
		case 'p':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.pib.file = open (plc.pib.name = optarg, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1) 
			{
				error (1, errno, "%s", plc.pib.name);
			}
			break;
		case 'q':
			_setbits (channel.flags, CHANNEL_SILENCE);
			_setbits (plc.flags, PLC_SILENCE);
			break;
		case 't':
			channel.timer = (signed)(uintspec (optarg, 0, UINT_MAX));
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

/*
 *      cancel operation if the user omitted a file or entered to extra argments 
 *      on the command line;
 */

	if (argc) 
	{
		error (1, ENOTSUP, ERROR_TOOMANY);
	}
	if (plc.NVM.file == -1) 
	{
		error (1, ECANCELED, "No host NVM file named");
	}
	if (plc.PIB.file == -1) 
	{
		error (1, ECANCELED, "No host PIB file named");
	}
	if (plc.nvm.file == -1) 
	{
		error (1, ECANCELED, "No user NVM file named");
	}
	if (plc.pib.file == -1) 
	{
		error (1, ECANCELED, "No user PIB file named");
	}
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	EmulateHost64 (&plc);
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

