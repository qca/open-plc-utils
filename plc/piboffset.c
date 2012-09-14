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

