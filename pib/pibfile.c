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
 *   signed pibfile (struct _file_ const * file);
 *
 *   pib.h
 *
 *   confirm that pib->file is a valid PIB file; determine if the
 *   file is a panther/lynx image chain or flat file and call the
 *   correct functions;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PIBFILE_SOURCE
#define PIBFILE_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../pib/pib.h"

signed pibfile (struct _file_ const * file)

{
	uint32_t version;
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
		return (-1);
	}
	if (LE32TOH (version) == 0x00010001)
	{
		return (pibfile2 (file));
	}
	return (pibfile1 (file));
}


#endif

