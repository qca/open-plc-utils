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
 *   void nvmpeek1 (void const * memory);
 *
 *   print a memory resident firmware image header on stdout;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVMPEEK1_SOURCE
#define NVMPEEK1_SOURCE

#include <stdio.h>

#include "../tools/memory.h"
#include "../tools/format.h"
#include "../nvm/nvm.h"

void nvmpeek1 (void const * memory) 

{
	extern char const * nvm_imagetypes [];
	extern char const * nvm_platforms [];
	struct nvm_header1 * nvm_header = (struct nvm_header1 *)(memory);
	char const * string = "Unknown";
	char platform [100];
	printf ("\tHeader Version = 0x%08X-%02X\n", LE32TOH (nvm_header->HEADERVERSION), nvm_header->HEADERMINORVERSION);
	printf ("\tHeader Checksum = 0x%08X\n", LE32TOH (nvm_header->HEADERCHECKSUM));
	printf ("\tHeader Next = 0x%08X\n", LE32TOH (nvm_header->NEXTHEADER));
	printf ("\tFlash Address = 0x%08X\n", LE32TOH (nvm_header->IMAGEROMADDR));
	printf ("\tImage Address = 0x%08X\n", LE32TOH (nvm_header->IMAGEADDRESS));
	printf ("\tEntry Address = 0x%08X\n", LE32TOH (nvm_header->ENTRYPOINT));
	printf ("\tImage Checksum = 0x%08X\n", LE32TOH (nvm_header->IMAGECHECKSUM));
	printf ("\tImage Size = 0x%08X (%d)\n", LE32TOH (nvm_header->IMAGELENGTH), LE32TOH (nvm_header->IMAGELENGTH));
	if (LE32TOH (nvm_header->IMAGETYPE) < NVM_IMAGETYPES) 
	{
		string = nvm_imagetypes [LE32TOH (nvm_header->IMAGETYPE)];
	}
	printf ("\tImage Type = %s\n", string);
	strfbits (platform, sizeof (platform), nvm_platforms, "|", LE16TOH (nvm_header->IGNOREMASK));
	printf ("\tImage Omit = %s\n", platform);
	return;
}


#endif

