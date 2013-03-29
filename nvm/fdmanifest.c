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
 *   signed fdmanifest (signed fd, char const * filename, struct nvm_header2 * nvm_header, unsigned module);
 *
 *   given the manifest image header and a file positioned to the
 *   start of the manifest image, extract the manifest and print 
 *   it on stdout in human readable form;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FDMANIFEST_SOURCE
#define FDMANIFEST_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "../tools/endian.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../nvm/nvm.h"

signed fdmanifest (signed fd, char const * filename, struct nvm_header2 * nvm_header, unsigned module) 

{
	off_t extent = LE32TOH (nvm_header->ImageLength);
	void * memory = malloc (extent);
	if (!memory) 
	{
		error (1, 0, FILE_CANTLOAD, filename);
	}
	if (read (fd, memory, extent) != (signed)(extent)) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTSEEK, filename);
	}
	printf ("------- %s (%d) -------\n", filename, module);
	if (manifest (memory, extent)) 
	{
		error (1, 0, "Bad manifest: %s", filename);
	}
	free (memory);
	return (0);
}


#endif

