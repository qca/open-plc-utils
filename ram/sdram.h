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
 *   sdram.h - SDRAM definitions and declarations;
 *
 *   the first section if an NVM file contains SDRAM parameters used
 *   by the bootloader;
 *
 *.  Intellon Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SDRAM_HEADER
#define SDRAM_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdint.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/types.h"
#include "../tools/flags.h"

/*====================================================================*
 *   constants; 
 *--------------------------------------------------------------------*/

#define SDRAM_SILENCE (1 << 0)
#define SDRAM_VERBOSE (1 << 1)
#define SDRAM_BASENAME (1 << 2)

/*====================================================================*
 *   variables; 
 *--------------------------------------------------------------------*/

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed config_ram 

{
	uint32_t SDRAMSIZE;
	uint32_t SDRAMCONF;
	uint32_t SDRAMTIM0;
	uint32_t SDRAMTIM1;
	uint32_t SDRAMCNTRL;
	uint32_t SDRAMREF;
	uint32_t MACCLOCK;
	uint32_t SDRAMRSVD;
}

config_ram;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

void sdrampeek (struct config_ram * config_ram);
void sdramtext (struct config_ram * config_ram);
int sdramfile (int fd, char const *title, flag_t flags);
int sdramfileA (int fd, char const *title, flag_t flags);

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#endif

