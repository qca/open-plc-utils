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
 *   void sdrampeek(const struct config_ram * config_ram);
 *
 *   sdram.h
 *   
 *   print SDRAM configuration on stdout in human-readable format;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit 
 *:  Copyright (c) 2006-2010 by Intellon Corporation; ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SDRAMPEEK_SOURCE
#define SDRAMPEEK_SOURCE

#include <stdio.h>

#include "../ram/sdram.h"
#include "../tools/memory.h"

void sdrampeek (struct config_ram * config_ram) 

{
	printf ("\tSIZE=0x%08X (%dmb)\n", LE32TOH (config_ram->SDRAMSIZE), LE32TOH (config_ram->SDRAMSIZE)>>20);
	printf ("\tCONF=0x%08X\n", LE32TOH (config_ram->SDRAMCONF));
	printf ("\tTIM0=0x%08X\n", LE32TOH (config_ram->SDRAMTIM0));
	printf ("\tTIM1=0x%08X\n", LE32TOH (config_ram->SDRAMTIM1));
	printf ("\tCNTRL=0x%08X\n", LE32TOH (config_ram->SDRAMCNTRL));
	printf ("\tREF=0x%08X\n", LE32TOH (config_ram->SDRAMREF));
	printf ("\tCLOCK=0x%08X\n", LE32TOH (config_ram->MACCLOCK));
	return;
}


#endif

