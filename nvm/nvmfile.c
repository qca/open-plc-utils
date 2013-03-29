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
 *   signed nvmfile (struct _file_ const * file);
 *
 *   nvm.h
 *
 *   open an NVM file and validate it by walking the header and image
 *   chain and validating all checksums; rewind valid files; return a
 *   file descriptor on success or terminate the program on error;
 *
 *   the checksum of the entire header, including header checksum, is
 *   always 0 for valid headers; similarly, the checksum of the image
 *   and image checksum is always 0 for valid images;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVMFILE_SOURCE
#define NVMFILE_SOURCE

#include "../tools/endian.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../nvm/nvm.h"

signed nvmfile (struct _file_ const * file) 

{
	uint32_t version;
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	if (read (file->file, &version, sizeof (version)) != sizeof (version)) 
	{
		error (1, errno, FILE_CANTREAD, file->name);
	}
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	if (LE32TOH (version) == 0x60000000) 
	{
		return (nvmfile1 (file));
	}
	return (nvmfile2 (file));
}


#endif

