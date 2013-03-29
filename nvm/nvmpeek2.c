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
 *   void nvmpeek2 (void const * memory);
 *
 *   print a memory resident firmware image header on stdout;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVMPEEK2_SOURCE
#define NVMPEEK2_SOURCE

#include <stdio.h>

#include "../tools/memory.h"
#include "../tools/format.h"
#include "../nvm/nvm.h"

void nvmpeek2 (void const * memory) 

{
	extern char const * nvm_imagetypes [];
	extern char const * nvm_platforms [];
	struct nvm_header2 * nvm_header = (struct nvm_header2 *)(memory);
	char const * string = "Unknown";
	char platform [100];
	printf ("\tHeader Version = 0x%04x-0x%04x\n", LE16TOH (nvm_header->MajorVersion), nvm_header->MinorVersion);
	printf ("\tHeader Checksum = 0x%08X\n", LE32TOH (nvm_header->HeaderChecksum));
	printf ("\tHeader Next = 0x%08X\n", LE32TOH (nvm_header->NextHeader));
	printf ("\tHeader Prev = 0x%08X\n", LE32TOH (nvm_header->PrevHeader));
	printf ("\tFlash Address = 0x%08X\n", LE32TOH (nvm_header->ImageNvmAddress));
	printf ("\tImage Address = 0x%08X\n", LE32TOH (nvm_header->ImageAddress));
	printf ("\tEntry Address = 0x%08X\n", LE32TOH (nvm_header->EntryPoint));
	printf ("\tEntry Version = 0x%04X\n", LE16TOH (nvm_header->AppletEntryVersion));
	printf ("\tImage Checksum = 0x%08X\n", LE32TOH (nvm_header->ImageChecksum));
	printf ("\tImage ModuleID = 0x%04X\n", LE32TOH (nvm_header->ModuleID));
	printf ("\tImage ModuleSubID = 0x%04X\n", LE32TOH (nvm_header->ModuleSubID));
	printf ("\tImage Size = 0x%08X (%d)\n", LE32TOH (nvm_header->ImageLength), LE32TOH (nvm_header->ImageLength));
	if (LE32TOH (nvm_header->ImageType) < NVM_IMAGETYPES) 
	{
		string = nvm_imagetypes [LE32TOH (nvm_header->ImageType)];
	}
	printf ("\tImage Type = %s\n", string);
	strfbits (platform, sizeof (platform), nvm_platforms, "|", LE16TOH (nvm_header->ExecuteMask));
	printf ("\tImage Exec = %s\n", platform);

#if 0

	printf ("\tReserved = 0x%04X\n", LE16TOH (nvm_header->Reserved0));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved1));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved2));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved3));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved4));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved5));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved6));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved7));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved8));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved9));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved10));
	printf ("\tReserved = 0x%08X\n", LE32TOH (nvm_header->Reserved11));

#endif

	return;
}


#endif

