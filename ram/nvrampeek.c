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
 *   void nvrampeek(struct config_nvram * config_nvram);
 *
 *   nvram.h
 *   
 *   print an nvram configuration block; 
 *
 *   This software and documentation is the property of Atheros 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Atheros assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Atheros customers may modify and distribute the software 
 *   without obligation to Atheros. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVRAMPEEK_SOURCE
#define NVRAMPEEK_SOURCE

#include <stdio.h>

#include "../ram/nvram.h"
#include "../tools/memory.h"
#include "../tools/symbol.h"

void nvrampeek (struct config_nvram * config_nvram) 

{
	printf ("TYPE=0x%02X ", LE32TOH (config_nvram->NVRAMTYPE));
	printf ("(%s) ", NVRAMName (LE32TOH (config_nvram->NVRAMTYPE)));
	printf ("PAGE=0x%04X ", LE32TOH (config_nvram->NVRAMPAGE));
	printf ("BLOCK=0x%04X ", LE32TOH (config_nvram->NVRAMBLOCK));
	printf ("SIZE=0x%04X ", LE32TOH (config_nvram->NVRAMSIZE));
	printf ("(%d) ", LE32TOH (config_nvram->NVRAMSIZE));
	printf ("\n");
	return;
}


#endif

