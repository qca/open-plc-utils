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
 *   int EmulateHost64 (struct plc * plc);
 *  
 *   plc.h
 *
 *   wait indefinitely for VS_HST_ACTION messages; service the device 
 *   as directed; this function is for demonstration and experimentation
 *   only; it will stop dead - like a bug! - on error;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef EMULATEHOST64_SOURCE
#define EMULATEHOST64_SOURCE

#include <unistd.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../ether/channel.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/files.h"

#include "../nvm/nvm.h"
#include "../pib/pib.h"

signed EmulateHost64 (struct plc * plc) 

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

	char const * NVM = plc->NVM.name;
	char const * PIB = plc->PIB.name;
	signed timeout = channel->timeout;
	signed status;
	signed action;
	Request (plc, "Waiting for Host Action");
	while (1) 
	{
		channel->timeout = plc->timer;
		status = ReadMME (plc, 0, (VS_HST_ACTION | MMTYPE_IND));
		channel->timeout = timeout;
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
			action = indicate->MACTION;
			memcpy (channel->peer, indicate->ethernet.OSA, sizeof (channel->peer));
			channel->timeout = timeout;
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
			error (0, ENOSYS, "Host Action 0x%02X", action);
		}
	}
	return (0);
}


#endif

