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
 *   PLCHostBoot.c -
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PLCHOSTBOOT_SOURCE
#define PLCHOSTBOOT_SOURCE

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../tools/flags.h"
#include "../plc/plc.h"

#define MESSAGE "Starting PLC Host Server\n"
#define SOCKETNAME "/tmp/socket"

static bool done = false;
void terminate (signo_t signal)

{
	done = true;
	return;
}

static signed opensocket (char const * socketname)

{
	signed fd;
	struct sockaddr_un sockaddr_un =
	{

#ifdef __OpenBSD__

		0,

#endif

		AF_UNIX,
		"/tmp/socket"
	};
	strncpy (sockaddr_un.sun_path, socketname, sizeof (sockaddr_un.sun_path));

#ifdef __OpenBSD__

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
 *   signed PLCHostBoot (struct plc * plc, char const * socket, signed timer);
 *
 *   int6k.h
 *
 *   wait indefinitely for VS_HOST_ACTION messages; service the device
 *   only; it will stop dead - like a bug! - on error;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

signed PLCHostBoot (struct plc * plc, char const * socket, unsigned timer)

{
	byte buffer [3000];
	struct plctopology * plctopology = (struct plctopology *)(buffer);
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_host_action_ind
	{
		struct ethernet_hdr ethernet;
		struct qualcomm_hdr intellon;
		uint8_t MACTION;
		uint8_t MAJOR_VERSION;
		uint8_t MINOR_VERSION;
	}
	* indicate = (struct vs_host_action_ind *) (message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	signed fd = opensocket (socket);
	char const * NVM = plc->NVM.name;
	char const * PIB = plc->PIB.name;
	unsigned action;
	signed status;
	memset (buffer, 0, sizeof (buffer));
	write (fd, MESSAGE, strlen (MESSAGE));
	while (!done)
	{
		status = ReadMME (plc, 0, (VS_HOST_ACTION | MMTYPE_IND));
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
			if ((plc->NVM.file = open (plc->NVM.name = NVM, O_BINARY|O_RDONLY)) == -1)
			{
				error (1, errno, "%s", plc->NVM.name);
			}
			close (plc->PIB.file);
			if ((plc->PIB.file = open (plc->PIB.name = PIB, O_BINARY|O_RDONLY)) == -1)
			{
				error (1, errno, "%s", plc->PIB.name);
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
			if (ReadParameters (plc))
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
	close (fd);
	return (0);
}


#endif

