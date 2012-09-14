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
 *   signed ExecuteApplets2 (struct plc * plc);
 *
 *   plc.h
 *
 *   download and execute all modules in a new nvm image files;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef EXECUTEAPPLETS2_SOURCE
#define EXECUTEAPPLETS2_SOURCE

#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed ExecuteApplets2 (struct plc * plc) 

{
	unsigned module = 0;
	struct nvm_header2 nvm_header;
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		if (_allclr (plc->flags, PLC_SILENCE)) 
		{
			error ((plc->flags & PLC_BAILOUT), errno, FILE_CANTHOME, plc->NVM.name);
		}
		return (-1);
	}
	do 
	{
		if (read (plc->NVM.file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			if (_allclr (plc->flags, PLC_SILENCE)) 
			{
				error ((plc->flags & PLC_BAILOUT), errno, NVM_HDR_CANTREAD, plc->NVM.name, module);
			}
			return (-1);
		}
		if (LE16TOH (nvm_header.MajorVersion) != 1) 
		{
			if (_allclr (plc->flags, PLC_SILENCE)) 
			{
				error ((plc->flags & PLC_BAILOUT), 0, NVM_HDR_VERSION, plc->NVM.name, module);
			}
			return (-1);
		}
		if (LE32TOH (nvm_header.MinorVersion) != 1) 
		{
			if (_allclr (plc->flags, PLC_SILENCE)) 
			{
				error ((plc->flags & PLC_BAILOUT), 0, NVM_HDR_VERSION, plc->NVM.name, module);
			}
			return (-1);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			if (_allclr (plc->flags, PLC_SILENCE)) 
			{
				error ((plc->flags & PLC_BAILOUT), 0, NVM_HDR_CHECKSUM, plc->NVM.name, module);
			}
			return (-1);
		}
		if (WriteExecuteApplet2 (plc, module, &nvm_header)) 
		{
			return (-1);
		}
		if (_anyset (plc->flags, PLC_QUICK_FLASH)) 
		{
			break;
		}
		while (!ReadMME (plc, 0, (VS_HST_ACTION | MMTYPE_IND)));
		module++;
	}
	while (~nvm_header.NextHeader);
	return (0);
}


#endif

