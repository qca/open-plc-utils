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
 *   signed nvmseek2 (signd fd, char const * filename, struct nvm_header2 * nvm_header, uint32_t imagetype);
 *
 *   nvm.h
 *
 *   search a panther/lynx PIB file for the next image of a given
 *   type; return 0 on success or -1 on failure to find another
 *   image of the given type;
 *
 *   the call must provide an image header strucuture for use while
 *   searching; on success, that structure will contain information
 *   about the image and file will be positioned to the start of
 *   the image;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef NVMSEEK2_SOURCE
#define NVMSEEK2_SOURCE

#include <unistd.h>

#include "../tools/endian.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../pib/pib.h"
#include "../nvm/nvm.h"

signed nvmseek2 (signed fd, char const * filename, struct nvm_header2 * nvm_header, uint32_t imagetype)

{
	unsigned module = 0;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	do
	{
		if (read (fd, nvm_header, sizeof (* nvm_header)) != sizeof (* nvm_header))
		{
			error (1, errno, NVM_HDR_CANTREAD, filename, module);
		}
		if (LE16TOH (nvm_header->MajorVersion) != 1)
		{
			error (1, 0, "(0) " NVM_HDR_VERSION , filename, module);
		}
		if (LE16TOH (nvm_header->MinorVersion) != 1)
		{
			error (1, 0, "(1) " NVM_HDR_VERSION, filename, module);
		}
		if (checksum32 (nvm_header, sizeof (* nvm_header), 0))
		{
			error (1, 0, NVM_HDR_CHECKSUM, filename, module);
		}
		if (LE32TOH (nvm_header->PrevHeader) != origin)
		{
			error (1, 0, NVM_HDR_LINK, filename, module);
		}
		if (LE32TOH (nvm_header->ImageType) == imagetype)
		{
			return (0);
		}
		if (fdchecksum32 (fd, LE32TOH (nvm_header->ImageLength), nvm_header->ImageChecksum))
		{
			error (1, ECANCELED, NVM_IMG_CHECKSUM, filename, module);
		}
		origin = offset;
		offset = LE32TOH (nvm_header->NextHeader);
		module++;
	}
	while (~nvm_header->NextHeader);
	if (lseek (fd, 0, SEEK_CUR) != lseek (fd, 0, SEEK_END))
	{
		error (1, errno, NVM_HDR_LINK, filename, module);
	}
	if (lseek (fd, 0, SEEK_SET))
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	return (-1);
}


#endif

