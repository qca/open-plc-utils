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
 *   signed BootFirmware1 (struct plc * plc);
 *
 *   plc.h
 *
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef BOOTFIRMWARE1_SOURCE
#define BOOTFIRMWARE1_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/files.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"
#include "../nvm/nvm.h"

signed BootFirmware1 (struct plc * plc) 

{
	unsigned module = 0;
	struct nvm_header1 nvm_header;
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->NVM.name);
	}
	do 
	{
		if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, NVM_HDR_CANTREAD, plc->NVM.name, module);
		}
		if (LE32TOH (nvm_header.HEADERVERSION) != 0x60000000) 
		{
			error ((plc->flags & PLC_BAILOUT), ECANCELED, NVM_HDR_VERSION, plc->NVM.name, module);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			error ((plc->flags & PLC_BAILOUT), ECANCELED, NVM_HDR_CHECKSUM, plc->NVM.name, module);
		}

#if 0

		if (_anyset (LE16TOH (nvm_header.IGNOREMASK), (1 << (plc->hardwareID - 1)))) 
		{
			if (lseek (plc->NVM.file, LE32TOH (nvm_header.NEXTHEADER), SEEK_SET) == -1) 
			{
				error ((plc->flags & PLC_BAILOUT), errno, "Can't skip module: %s (%d)", plc->NVM.name, module);
			}
		}
		else 

#endif

		if (nvm_header.IMAGETYPE == NVM_IMAGE_FIRMWARE) 
		{
			if (plc->hardwareID < CHIPSET_AR7400) 
			{
				if (WriteFirmware1 (plc, module, &nvm_header)) 
				{
					return (-1);
				}
				if (StartFirmware1 (plc, module, &nvm_header)) 
				{
					return (-1);
				}
				break;
			}
			if (WriteExecuteFirmware1 (plc, module, &nvm_header)) 
			{
				return (-1);
			}
			break;
		}
		if (lseek (plc->NVM.file, LE32TOH (nvm_header.NEXTHEADER), SEEK_SET) == -1) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, "Can't skip module: %s (%d)", plc->NVM.name, module);
		}
		module++;
	}
	while (nvm_header.NEXTHEADER);
	return (0);
}


#endif

