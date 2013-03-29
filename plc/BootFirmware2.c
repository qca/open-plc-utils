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
 *   signed BootFirmware2 (struct plc * plc) 
 *
 *   plc.h
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef BOOTFIRMWARE2_SOURCE
#define BOOTFIRMWARE2_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/files.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"
#include "../nvm/nvm.h"

signed BootFirmware2 (struct plc * plc) 

{
	unsigned module = 0;
	struct nvm_header2 nvm_header;
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->NVM.name);
	}
	do 
	{
		if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, NVM_HDR_CANTREAD, plc->NVM.name, module);
		}
		if (LE16TOH (nvm_header.MajorVersion) != 1) 
		{
			error (1, ECANCELED, NVM_HDR_VERSION, plc->NVM.name, module);
		}
		if (LE16TOH (nvm_header.MinorVersion) != 1) 
		{
			error (1, ECANCELED, NVM_HDR_VERSION, plc->NVM.name, module);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			error (1, ECANCELED, NVM_HDR_CHECKSUM, plc->NVM.name, module);
		}

#if 0

		if (_allclr (LE16TOH (nvm_header.ExecuteMask), (1 << (plc->hardwareID - 1)))) 
		{
			if (lseek (plc->NVM.file, LE32TOH (nvm_header.NextHeader), SEEK_SET) == -1) 
			{
				error (1, errno, "Can't skip module: %s (%d)", plc->NVM.name, module);
			}
		}
		else 

#endif

		if (LE32TOH (nvm_header.ImageType) == NVM_IMAGE_FIRMWARE) 
		{
			if (WriteExecuteFirmware2 (plc, module, &nvm_header)) 
			{
				return (-1);
			}
			break;
		}
		if (lseek (plc->NVM.file, LE32TOH (nvm_header.NextHeader), SEEK_SET) == -1) 
		{
			error (1, errno, "Can't skip module: %s (%d)", plc->NVM.name, module);
		}
		module++;
	}
	while (~nvm_header.NextHeader);
	return (0);
}


#endif

