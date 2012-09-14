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
 *   signed BootParameters1 (struct plc * plc) 
 *
 *   pib.h
 *
 *   write the parameter information block to SDRAM on a thunderbolt/
 *   lightning powerline device;
 *
 *   The PIB is written to different SDRAM locations depending on the 
 *   chipset used; we use PIB major/minor version to determine where 
 *   to write the PIB but may switch to using the hardware platform
 *   code derived from the VS_SW_VER message;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef BOOTPARAMETERS1_SOURCE
#define BOOTPARAMETERS1_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/files.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../plc/plc.h"
#include "../pib/pib.h"

signed BootParameters1 (struct plc * plc) 

{
	struct pib_header pib_header;
	uint32_t offset;
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (read (plc->PIB.file, &pib_header, sizeof (pib_header)) != sizeof (pib_header)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTREAD, plc->PIB.name);
	}
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->PIB.name);
	}

#if 1

/*
 *   this code is a fix to accommodate PIB relocation in memory; it is not needed when 
 *   the PIB is stored in an NVM file since the memory address is recorded in the image 
 *   header;
 */

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

#endif

	if (plc->hardwareID < CHIPSET_AR7400) 
	{
		if (WriteMEM (plc, &plc->PIB, 0, offset, LE16TOH (pib_header.PIBLENGTH))) 
		{
			return (-1);
		}
		return (0);
	}
	if (WriteExecutePIB (plc, offset, &pib_header)) 
	{
		return (-1);
	}
	return (0);
}


#endif

