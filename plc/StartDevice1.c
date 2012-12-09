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
 *   int StartDevice1 (struct plc * plc);
 *
 *   plc.h
 *  
 *   This int6kboot plugin initialize a device having no NVRAM or blank 
 *   or corrupted NVRAM; ensure Bootloader is running before starting; 
 *   write SDRAM configuration then NVM and PIB files to SDRAM and 
 *   start firmware execution; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef STARTDEVICE1_SOURCE
#define STARTDEVICE1_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"
#include "../plc/plc.h"

int StartDevice1 (struct plc * plc) 

{
	unsigned module = 0;
	struct nvm_header1 nvm_header;
	struct pib_header pib_header;
	uint32_t offset;
	if (WriteCFG (plc)) 
	{
		return (-1);
	}
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->NVM.name);
		return (-1);
	}
	if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTREAD, plc->NVM.name);
		return (-1);
	}
	while (nvm_header.NEXTHEADER) 
	{
		if (lseek (plc->NVM.file, LE32TOH (nvm_header.NEXTHEADER), SEEK_SET) == -1) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->NVM.name);
			return (-1);
		}
		if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTREAD, plc->NVM.name);
			return (-1);
		}
		module++;
	}
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (read (plc->PIB.file, &pib_header, sizeof (pib_header)) != (signed)(sizeof (pib_header))) 
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
	if (WriteFirmware1 (plc, module, &nvm_header)) 
	{
		return (-1);
	}
	if (StartFirmware1 (plc, module, &nvm_header)) 
	{
		return (-1);
	}
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->NVM.name);
		return (-1);
	}
	return (0);
}


#endif

