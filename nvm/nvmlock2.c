/*====================================================================*
 *
 *   void nvmlock2 (struct nvm_header2 * header);
 *
 *   example function to update an NVM image header after the image
 *   had been modified; this assumes that image immediately follows
 *   the header in memory. 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVMLOCK2_SOURCE
#define NVMLOCK2_SOURCE

#include "../tools/memory.h"
#include "../nvm/nvm.h"

void nvmlock2 (struct nvm_header2 * header) 

{
	header->ImageChecksum = checksum32 ((char *)(header) + sizeof (* header), LE32TOH (header->ImageLength), 0);
	header->HeaderChecksum = checksum32 (header, sizeof (* header), header->HeaderChecksum);
	return;
}


#endif

