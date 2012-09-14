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

#define _GETOPT_H

/*====================================================================*"
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../ether/channel.h"
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
#include "../pib/pib.h"
#include "../plc/plc.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../plc/BootDevice1.c"
#include "../plc/BootDevice2.c"
#include "../plc/BootFirmware1.c"
#include "../plc/BootFirmware2.c"
#include "../plc/BootParameters1.c"
#include "../plc/BootParameters2.c"
#include "../plc/chipset.c"
#include "../plc/Confirm.c"
#include "../plc/Devices.c"
#include "../plc/ExecuteApplets.c"
#include "../plc/ExecuteApplets1.c"
#include "../plc/ExecuteApplets2.c"
#include "../plc/Failure.c"
#include "../plc/FlashDevice.c"
#include "../plc/FlashDevice1.c"
#include "../plc/FlashDevice2.c"
#include "../plc/FlashSoftloader.c"
#include "../plc/FlashFirmware.c"
#include "../plc/FlashParameters.c"
#include "../plc/FlashNVM.c"
#include "../plc/HostActionResponse.c"
#include "../plc/InitDevice.c"
#include "../plc/InitDevice1.c"
#include "../plc/InitDevice2.c"
#include "../plc/ModuleSpec.c"
#include "../plc/ModuleSession.c"
#include "../plc/ModuleWrite.c"
#include "../plc/ModuleCommit.c"
#include "../plc/NVMSelect.c"
#include "../plc/Request.c"
#include "../plc/ResetDevice.c"
#include "../plc/ReadMME.c"
#include "../plc/SendMME.c"
#include "../plc/ReadFirmware1.c"
#include "../plc/ReadParameters.c"
#include "../plc/ReadParameters1.c"
#include "../plc/ReadParameters2.c"
#include "../plc/LocalDevices.c"
#include "../plc/WaitForReset.c"
#include "../plc/WaitForStart.c"
#include "../plc/WriteMEM.c"
#include "../plc/WriteNVM.c"
#include "../plc/WritePIB.c"
#include "../plc/WriteExecuteApplet2.c"
#include "../plc/WriteExecuteFirmware.c"
#include "../plc/WriteExecuteFirmware1.c"
#include "../plc/WriteExecuteFirmware2.c"
#include "../plc/WriteExecuteParameters.c"
#include "../plc/WriteExecuteParameters1.c"
#include "../plc/WriteExecuteParameters2.c"
#include "../plc/WriteExecutePIB.c"
#include "../plc/Platform.c"
#include "../plc/PLCTopology.c"
#include "../plc/PLCTopologyPrint.c"
#include "../plc/WriteFirmware1.c"
#include "../plc/WriteFirmware2.c"
#include "../plc/StartFirmware1.c"
#include "../plc/StartFirmware2.c"
#include "../plc/PLCSelect.c"
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
#include "../mme/EthernetHeader.c"
#include "../mme/QualcommHeader.c"
#include "../mme/UnwantedMessage.c"
#include "../mme/MMECode.c"
#ifdef AR7x00
#include "../mme/FragmentHeader.c"
#endif
#endif

#ifndef MAKEFILE
#include "../key/keys.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmfile.c"
#include "../nvm/nvmfile1.c"
#include "../nvm/nvmfile2.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibfile.c"
#include "../pib/pibfile1.c"
#include "../pib/pibfile2.c"
#include "../pib/pibpeek1.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define MESSAGE "Starting PLC Host Server\n"
#define SOCKETNAME "/tmp/socket"

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

static bool done = false;

/*====================================================================*
 *  
 *   void terminate (signo_t signal);
 *
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void terminate (signo_t signal) 

{
	done = true;
	return;
}


/*====================================================================*
 *  
 *   signed opensocket (char const * socketname);
 *
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed opensocket (char const * socketname) 

{
	signed fd;
	struct sockaddr_un sockaddr_un = 
	{

#if defined (__OpenBSD__) || defined (__APPLE__)

		0,

#endif

		AF_UNIX,
		"/tmp/socket"
	};
	strncpy (sockaddr_un.sun_path, socketname, sizeof (sockaddr_un.sun_path));

#if defined (__OpenBSD__) || defined (__APPLE__)

	sockaddr_un.sun_len = SUN_LEN (&sockaddr_un);

#endif

	if (unlink (sockaddr_un.sun_path)) 
	{
		if (errno != ENOENT) 
		{
			error (1, errno, "%s", sockaddr_un.sun_path);
		}
	}
	if ((fd = socket (AF_UNIX, SOCK_DGRAM, 0)) == -1) 
	{
		error (1, errno, "%s", sockaddr_un.sun_path);
	}
	if (bind (fd, (struct sockaddr *)(&sockaddr_un), sizeof (sockaddr_un)) == -1) 
	{
		error (1, errno, "%s", sockaddr_un.sun_path);
	}
	if (chmod (sockaddr_un.sun_path, 0666) == -1) 
	{
		error (1, errno, "%s", sockaddr_un.sun_path);
	}
	return (fd);
}


/*====================================================================*
 *  
 *   signed function (struct plc * plc, char const * socket);
 *  
 *   wait indefinitely for VS_HST_ACTION messages; service the device 
 *   as directed; this function is for demonstration and experimentation
 *   only; it will stop dead - like a bug! - on error;
 *   
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

signed function (struct plc * plc, char const * socket) 

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

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

	byte buffer [3000];
	struct plctopology * plctopology = (struct plctopology *)(buffer);
	signed fd = opensocket (socket);
	char const * FactoryNVM = plc->NVM.name;
	char const * FactoryPIB = plc->PIB.name;
	signed timeout = channel->timeout;
	signed action;
	signed status;
	memset (buffer, 0, sizeof (buffer));
	write (fd, MESSAGE, strlen (MESSAGE));
	while (!done) 
	{
		channel->timeout = plc->timer;
		status = ReadMME (plc, 0, (VS_HST_ACTION | MMTYPE_IND));
		channel->timeout = timeout;
		if (status < 0) 
		{
			break;
		}
		if (status < 1) 
		{
			PLCTopology (channel, message, plctopology);
			PLCTopologyPrint (plctopology);
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
			if (BootDevice2 (plc)) 
			{
				return (-1);
			}
			if (_anyset (plc->flags, PLC_FLASH_DEVICE)) 
			{
				FlashDevice2 (plc);
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
			if (ReadParameters (plc)) 
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
			if (ReadParameters (plc)) 
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
			if (InitDevice (plc)) 
			{
				return (-1);
			}
			continue;
		}
		if (action == 0x05) 
		{
			close (plc->NVM.file);
			if ((plc->NVM.file = open (plc->NVM.name = FactoryNVM, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", plc->NVM.name);
			}
			close (plc->PIB.file);
			if ((plc->PIB.file = open (plc->PIB.name = FactoryPIB, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", plc->PIB.name);
			}
			if (ResetDevice (plc)) 
			{
				return (-1);
			}
			continue;
		}
		error (0, ENOSYS, "Host Action 0x%02X", action);
	}
	close (fd);
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
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern struct channel channel;
	extern void terminate (signo_t);
	static char const * optv [] = 
	{
		"dFi:n:N:p:P:qs:t:vw:x",
		"-N file -P file [-S file] [-n file] [-p file]",
		"Qualcomm Atheros PLC Host Daemon",
		"d\texecute in background as daemon",

#if defined (WINPCAP) || defined (LIBPCAP)

		"i n\thost interface is (n) [" LITERAL (CHANNEL_ETHNUMBER) "]",

#else

		"i s\thost interface is (s) [" LITERAL (CHANNEL_ETHDEVICE) "]",

#endif

		"n f\tread firmware from device into file (f)",
		"N f\tfirmware file is (f)",
		"p f\tread parameters from device into file (f)",
		"P f\tparameter file is (f)",
		"q\tquiet mode",
		"s f\tbroadcast event status on socket (f) [" SOCKETNAME "]",
		"S f\tsoftloader file is (f)",
		"t n\tread timeout is (n) milliseconds [" LITERAL (CHANNEL_TIMEOUT) "]",
		"v\tverbose mode",
		"w n\twakeup every (n) seconds [" LITERAL (PLC_LONGTIME) "]",
		"x\texit on error",
		(char const *) (0)
	};

#include "../plc/plc.c"

	struct sigaction sa;
	char const * socketname = SOCKETNAME;
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
	plc.timer = PLC_LONGTIME;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'd':
			_setbits (plc.flags, PLC_DAEMON);
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
			if (nvmfile (&plc.NVM)) 
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
			if (pibfile (&plc.PIB)) 
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
		case 's':
			break;
		case 'S':
			if (!checkfilename (optarg)) 
			{
				error (1, EINVAL, "%s", optarg);
			}
			if ((plc.CFG.file = open (plc.CFG.name = optarg, O_BINARY|O_RDONLY)) == -1) 
			{
				error (1, errno, "%s", plc.CFG.name);
			}
			if (nvmfile (&plc.CFG)) 
			{
				error (1, errno, "Bad NVM file: %s", plc.CFG.name);
			}
			_setbits (plc.flags, PLC_FLASH_DEVICE);
			break;
		case 't':
			channel.timeout = (signed)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'v':
			_setbits (channel.flags, CHANNEL_VERBOSE);
			_setbits (plc.flags, PLC_VERBOSE);
			break;
		case 'w':
			plc.timer = (unsigned)(uintspec (optarg, 1, 3600));
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
		error (1, ECANCELED, "No user NVM file named");
	}
	if (plc.pib.file == -1) 
	{
		error (1, ECANCELED, "No user PIB file named");
	}
	if (plc.CFG.file == -1) 
	{
		if (_anyset (plc.flags, PLC_FLASH_DEVICE)) 
		{
			error (1, ECANCELED, "No Softloader file named");
		}
	}

#ifndef WIN32

	if (_anyset (plc.flags, PLC_DAEMON)) 
	{
		pid_t pid = fork ();
		if (pid < 0) 
		{
			error (1, errno, "Can't start daemon");
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

#endif

	openchannel (&channel);
	if (!(plc.message = malloc (sizeof (* plc.message)))) 
	{
		error (1, errno, PLC_NOMEMORY);
	}
	function (&plc, socketname);
	free (plc.message);
	closechannel (&channel);
	exit (0);
}

