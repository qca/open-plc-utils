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

/*====================================================================*"
 *
 *   amphost.c - 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
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
#include "../plc/plc.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/chipset.c"
#include "../plc/Confirm.c"
#include "../plc/Devices.c"
#include "../plc/Failure.c"
#include "../plc/FactoryDefaults.c"
#include "../plc/FlashDevice1.c"
#include "../plc/FlashNVM.c"
#include "../plc/HostActionResponse.c"
#include "../plc/Request.c"
#include "../plc/ResetDevice.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#include "../plc/WaitForReset.c"
#include "../plc/WaitForStart.c"
#include "../plc/WriteExecuteFirmware1.c"
#include "../plc/WriteExecutePIB.c"
#include "../plc/WriteMEM.c"
#include "../plc/WriteNVM.c"
#include "../plc/WritePIB.c"
#include "../plc/ReadFirmware1.c"
#include "../plc/ReadParameters1.c"
#include "../plc/InitDevice1.c"
#include "../plc/BootDevice1.c"
#include "../plc/BootFirmware1.c"
#include "../plc/BootParameters1.c"
#include "../plc/WriteFirmware1.c"
#include "../plc/StartFirmware1.c"
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
#include "../ether/channel.c"
#endif

#ifndef MAKEFILE
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#include "../mme/MMECode.c"
#endif

#ifndef MAKEFILE
#include "../ram/sdramfile.c"
#include "../ram/sdrampeek.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmfile1.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibpeek1.c"
#include "../pib/pibfile1.c"
#endif

#ifndef MAKEFILE
#include "../key/keys.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define NEWPIB "user.pib"
#define NEWNVM "user.nvm"

/*====================================================================*
 *  
 *   signed function (struct plc * plc);
 *  
 *   plc.h
 *
 *   wait indefinitely for VS_HOST_ACTION messages; service the device 
 *   as directed; this function is for demonstration and experimentation
 *   only; it will stop dead - like a bug! - on error;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

signed function (struct plc * plc) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);
	static char const * actions [] = 
	{
		"start device",
		"store firmware",
		"store parameters",
		"update host",
		"config memory",
		"restore defaults",
		"unknown"
	};

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_host_action_ind 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MACTION;
		uint8_t MAJOR_VERSION;
		uint8_t MINOR_VERSION;
	}
	* indicate = (struct vs_host_action_ind *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	char const * FactoryNVM = plc->NVM.name;
	char const * FactoryPIB = plc->PIB.name;
	signed status;
	signed action;
	Request (plc, "Waiting for Host Action");
	while (1) 
	{
		status = ReadMME (plc, 0, (VS_HOST_ACTION | MMTYPE_IND));
		if (status < 0) 
		{
			break;
		}
		if (status > 0) 
		{
			printf ("\n");
			if (indicate->MACTION < SIZEOF (actions)) 
			{
				Confirm (plc, "Host Action Request is (%d) %s.", indicate->MACTION, actions [indicate->MACTION]);
			}
			else 
			{
				error (0, ENOTSUP, "Host Action 0x%0X", indicate->MACTION);
				continue;
			}
			action = indicate->MACTION;
			memcpy (channel->peer, indicate->ethernet.OSA, sizeof (channel->peer));
			if (HostActionResponse (plc)) 
			{
				return (-1);
			}
			if (action == 0x00) 
			{
				if (BootDevice1 (plc)) 
				{
					return (-1);
				}
				if (_anyset (plc->flags, PLC_FLASH_DEVICE)) 
				{
					FlashDevice1 (plc);
				}
				continue;
			}
			if (action == 0x01) 
			{
				close (plc->NVM.file);
				if (ReadFirmware1 (plc)) 
				{
					return (-1);
				}
				if ((plc->NVM.file = open (plc->NVM.name = plc->nvm.name, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->NVM.name);
				}
				if (ResetDevice (plc)) 
				{
					return (-1);
				}
				continue;
			}
			if (action == 0x02) 
			{
				close (plc->PIB.file);
				if (ReadParameters1 (plc)) 
				{
					return (-1);
				}
				if ((plc->PIB.file = open (plc->PIB.name = plc->pib.name, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->PIB.name);
				}
				if (ResetDevice (plc)) 
				{
					return (-1);
				}
				continue;
			}
			if (action == 0x03) 
			{
				close (plc->PIB.file);
				if (ReadParameters1 (plc)) 
				{
					return (-1);
				}
				if ((plc->PIB.file = open (plc->PIB.name = plc->pib.name, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->PIB.name);
				}
				close (plc->NVM.file);
				if (ReadFirmware1 (plc)) 
				{
					return (-1);
				}
				if ((plc->NVM.file = open (plc->NVM.name = plc->nvm.name, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->NVM.name);
				}
				if (ResetDevice (plc)) 
				{
					return (-1);
				}
				continue;
			}
			if (action == 0x04) 
			{
				if (InitDevice1 (plc)) 
				{
					return (-1);
				}
				continue;
			}
			if (action == 0x05) 
			{
				close (plc->PIB.file);
				if ((plc->PIB.file = open (plc->PIB.name = FactoryPIB, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->PIB.name);
				}
				close (plc->NVM.file);
				if ((plc->NVM.file = open (plc->NVM.name = FactoryNVM, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->NVM.name);
				}
				if (ResetDevice (plc)) 
				{
					return (-1);
				}
				continue;
			}
			if (action == 0x06)
			{
				close (plc->PIB.file);
				if (ReadParameters1 (plc)) 
				{
					return (-1);
				}
				if ((plc->PIB.file = open (plc->PIB.name = plc->pib.name, O_BINARY|O_RDONLY)) == -1) 
				{
					error (1, errno, "%s", plc->PIB.name);
				}
				continue;
			}
			error (0, ENOSYS, "Host Action 0x%02X", action);
		}
	}
	return (0);
}


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
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
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
		if ((plc.nvm.file = open (plc.nvm.name = NEWNVM, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1) 
		{
			error (1, errno, "%s", plc.nvm.name);
		}
	}
	if (plc.pib.file == -1) 
	{
		if ((plc.pib.file = open (plc.pib.name = NEWPIB, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1) 
		{
			error (1, errno, "%s", plc.pib.name);
		}
	}
	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	function (&plc);
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

