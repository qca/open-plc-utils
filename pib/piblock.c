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
 *   signed piblock (struct _file_ const * pib);
 *
 *   recompute and update the checksum on a thunderbolt or lightning 
 *   PIB file where the checksum is located inside the PIB; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

#ifndef PIBLOCK_SOURCE
#define PIBLOCK_SOURCE

#include <unistd.h>
#include <errno.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../pib/pib.h"

signed piblock (struct _file_ const * file) 

{
	struct simple_pib simple_pib;
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, "1 " FILE_CANTHOME, file->name);
	}
	if (read (file->file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib)) 
	{
		error (1, errno, FILE_CANTREAD, file->name);
	}
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	simple_pib.CHECKSUM = fdchecksum32 (file->file, LE16TOH (simple_pib.PIBLENGTH), simple_pib.CHECKSUM);
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, "2 " FILE_CANTHOME, file->name);
	}
	if (write (file->file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib)) 
	{
		error (1, errno, FILE_CANTSAVE, file->name);
	}
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	return (0);
}


#endif

