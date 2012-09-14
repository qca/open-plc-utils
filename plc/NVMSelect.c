/*====================================================================*
 *
 *   signed NVMSelect (struct plc * plc, signed old (struct plc *), signed new (struct plc *));
 *   
 *   plc.h
 *
 *   read the .nvm header to determine file format; rewind the file
 *   then call the right function to execute the applets; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVMSELECT_SOURCE
#define NVMSELECT_SOURCE

#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h"

signed NVMSelect (struct plc * plc, signed old (struct plc *), signed new (struct plc *)) 

{
	uint32_t version;
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->NVM.name);
	}
	if (read (plc->NVM.file, &version, sizeof (version)) != sizeof (version)) 
	{
		error (1, errno, FILE_CANTREAD, plc->NVM.name);
	}
	if (lseek (plc->NVM.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->NVM.name);
	}
	return (LE32TOH (version) == 0x60000000? old (plc): new (plc));
}


#endif

