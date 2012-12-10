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
 *   uint32_t piboffset (struct _file_ * file) 
 *
 *   The PIB is written to different SDRAM locations depending on 
 *   the chipset used; we use the PIN major/minor version here to 
 *   determine where to write the PIB but probably should switch 
 *   to using the hardware platform code derived from the 
 *   VS_SW_VER message;
 *
 *--------------------------------------------------------------------*/

#include <stdlib.h>

#include "../plc/plc.h"
#include "../pib/pib.h"

uint32_t piboffset (struct _file_ * file) 

{
	struct pib_header pib_header;
	uint32_t offset = 0;
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	if (read (file->file, &pib_header, sizeof (pib_header)) < sizeof (pib_header)) 
	{
		error (1, errno, FILE_CANTREAD, file->name);
	}
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
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
	return (offset);
}

