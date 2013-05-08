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
 *   signed pibfile2 (struct _file_ const * file);
 *
 *   pib.h
 *
 *   open a panther/lynx PIB file and validate it by 
 *   checking file size, checksum and selected internal parameters; 
 *   return a file descriptor on success; terminate the program on 
 *   error;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PIBFILE2_SOURCE
#define PIBFILE2_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

signed pibfile2 (struct _file_ const * file) 

{
	struct nvm_header2 nvm_header;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	unsigned module = 0;
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	do 
	{
		if (read (file->file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, NVM_HDR_CANTREAD, file->name, module);
		}
		if (LE16TOH (nvm_header.MajorVersion) != 1) 
		{
			error (1, errno, NVM_HDR_VERSION, file->name, module);
		}
		if (LE16TOH (nvm_header.MinorVersion) != 1) 
		{
			error (1, errno, NVM_HDR_VERSION, file->name, module);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			error (1, errno, NVM_HDR_CHECKSUM, file->name, module);
		}
		if (LE32TOH (nvm_header.PrevHeader) != origin) 
		{
			error (1, errno, NVM_HDR_LINK, file->name, module);
		}
		if (LE32TOH (nvm_header.ImageType) == NVM_IMAGE_PIB) 
		{
			if (fdchecksum32 (file->file, LE32TOH (nvm_header.ImageLength), nvm_header.ImageChecksum)) 
			{
				error (1, errno, NVM_IMG_CHECKSUM, file->name, module);
			}
			return (0);
		}
		if (fdchecksum32 (file->file, LE32TOH (nvm_header.ImageLength), nvm_header.ImageChecksum)) 
		{
			error (1, errno, NVM_IMG_CHECKSUM, file->name, module);
		}
		origin = offset;
		offset = LE32TOH (nvm_header.NextHeader);
		module++;
	}
	while (~nvm_header.NextHeader);
	return (-1);
}


#endif

