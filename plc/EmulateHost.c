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
 *   signed EmulateHost (struct plc * plc);
 *
 *   plc.h
 *
 *   wait indefinitely for VS_HOST_ACTION messages; service requests
 *   only; it stops dead - like a bug! - on error;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef EMULATEHOST_SOURCE
#define EMULATEHOST_SOURCE

#include <unistd.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../ether/channel.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/files.h"

#include "../nvm/nvm.h"
#include "../pib/pib.h"

signed EmulateHost (struct plc * plc)

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

#if 0

	struct __packed vs_host_action_rsp
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
	}
	* response = (struct vs_host_action_rsp *) (message);

#endif

#ifndef __GNUC__
#pragma pack (pop)
#endif

	struct nvm_header1 nvm_header;
	struct pib_header pib_header;
	uint32_t offset;
	char const * PIB = plc->PIB.name;
	char const * NVM = plc->NVM.name;
	signed timer = channel->timeout;
	signed status = 0;
	Request (plc, "Waiting for Host Action");
	while (1)
	{
		channel->timeout = plc->timer;
		status = ReadMME (plc, 0, (VS_HOST_ACTION | MMTYPE_IND));
		channel->timeout = timer;
		if (status < 0)
		{
			break;
		}
		if (status > 0)
		{
			printf ("\n");
			if (indicate->MACTION < (sizeof (actions) / sizeof (char const *)))
			{
				Confirm (plc, "Host Action Request is (%d) %s.", indicate->MACTION, actions [indicate->MACTION]);
			}
			else
			{
				error (0, ENOTSUP, "Host Action 0x%0X", indicate->MACTION);
				continue;
			}
			memcpy (channel->peer, indicate->ethernet.OSA, sizeof (channel->peer));
			channel->timeout = timer;
			if (indicate->MACTION == 0x00)
			{
				unsigned module = 0;
				char firmware [PLC_VERSION_STRING];
				if (HostActionResponse (plc))
				{
					return (-1);
				}
				if (lseek (plc->PIB.file, 0, SEEK_SET))
				{
					error (1, errno, FILE_CANTHOME, plc->PIB.name);
				}
				if (read (plc->PIB.file, &pib_header, sizeof (pib_header)) != sizeof (pib_header))
				{
					error (1, errno, FILE_CANTREAD, plc->PIB.name);
				}
				if (lseek (plc->PIB.file, 0, SEEK_SET))
				{
					error (1, errno, FILE_CANTHOME, plc->PIB.name);
				}
				if (BE16TOH (*(uint16_t *)(&pib_header)) < 0x0305)
				{
					offset = LEGACY_PIBOFFSET;
				}
				else if (BE16TOH (*(uint16_t *)(&pib_header)) < 0x0500)
				{
					offset = INT6x00_PIBOFFSET;
				}
				else
				{
					offset = AR7x00_PIBOFFSET;
				}
				if (WriteMEM (plc, &plc->PIB, 0, offset, LE16TOH (pib_header.PIBLENGTH)))
				{
					return (-1);
				}
				if (lseek (plc->NVM.file, 0, SEEK_SET))
				{
					error (1, errno, FILE_CANTHOME, plc->NVM.name);
				}
				if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header))
				{
					error (1, errno, FILE_CANTREAD, plc->NVM.name);
				}
				while (nvm_header.NEXTHEADER)
				{
					lseek (plc->NVM.file, LE32TOH (nvm_header.NEXTHEADER), SEEK_SET);
					if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header))
					{
						error (1, errno, FILE_CANTREAD, plc->NVM.name);
					}
					module++;
				}
				if (WriteFirmware1 (plc, module, &nvm_header))
				{
					return (-1);
				}
				if (StartFirmware1 (plc, module, &nvm_header))
				{
					return (-1);
				}
				if (WaitForStart (plc, firmware, sizeof (firmware)))
				{
					return (-1);
				}
				if (_anyset (plc->flags, PLC_FLASH_DEVICE))
				{
					if (WriteNVM (plc))
					{
						return (-1);
					}
					if (WritePIB (plc))
					{
						return (-1);
					}
					if (FlashNVM (plc))
					{
						return (-1);
					}
				}
				continue;
			}
			if (indicate->MACTION == 0x01)
			{
				if (HostActionResponse (plc))
				{
					return (-1);
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
			if (indicate->MACTION == 0x02)
			{
				if (HostActionResponse (plc))
				{
					return (-1);
				}
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
			if (indicate->MACTION == 0x03)
			{
				if (HostActionResponse (plc))
				{
					return (-1);
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
			if (indicate->MACTION == 0x04)
			{

#if 0

/*
 *	Due to an omission in the INT6300 BootLoader, responding to this VS_HOST_ACTION
 *      indication will suppress subsequent VS_HOST_ACTION messages and the device will
 *     	not request firmware and parameters; this may be corrected on the INT6400;
 */

				if (HostActionResponse (plc))
				{
					return (-1);
				}

#endif

				if (WriteCFG (plc))
				{
					return (-1);
				}

/*
 *	At this point, one could download firmware and parameters without waiting for
 *	further  requests from the device; however, we elect to wait for them since it
 *	is 'good form'; a device should send code 0x00 within 10 seconds of this one;
 */

				continue;
			}
			if (indicate->MACTION == 0x05)
			{
				if (HostActionResponse (plc))
				{
					return (-1);
				}
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
			error (0, ENOSYS, "Host Action 0x%0X", indicate->MACTION);
		}
	}
	return (0);
}


#endif

