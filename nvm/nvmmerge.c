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

/*====================================================================*"
 *
 *   nvmmerge.c - 
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/memory.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../nvm/nvm.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/checksum32.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *
 *   void copyimage (struct _file_ * file, signed extent, signed image)
 *
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void copyimage (struct _file_ * file, signed extent, signed image) 

{
	char buffer [1024];
	signed length = sizeof (buffer);
	while (extent) 
	{
		if (length > extent) 
		{
			length = extent;
		}
		if (read (file->file, buffer, length) < length) 
		{
			error (1, errno, NVM_IMG_CANTREAD, file->name, image);
		}
		if (write (STDOUT_FILENO, buffer, length) < length) 
		{
			error (1, errno, NVM_IMG_CANTSAVE, file->name, image);
		}
		extent -= length;
	}
	return;
}


/*====================================================================*
 *
 *   void function1 (char const * filename, signed index, flag_t flags);
 *
 *   concatenate Atheros image files having the older header format;
 *
 *   the major and minor header version must be 0x6000 and 0x0000 
 *   and both header links must be 0x00000000;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function1 (struct _file_ * file, signed index, flag_t flags) 

{
	static signed image = 0;
	static uint32_t offset = 0;
	struct nvm_header1 nvm_header;
	if (read (file->file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
	{
		error (1, errno, NVM_HDR_CANTREAD, file->name, image);
	}
	if (LE32TOH (nvm_header.HEADERVERSION) != 0x60000000) 
	{
		error (1, 0, NVM_HDR_VERSION, file->name, image);
	}
	if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
	{
		error (1, 0, NVM_HDR_CHECKSUM, file->name, image);
	}
	if (nvm_header.NEXTHEADER) 
	{
		error (1, 0, NVM_HDR_LINK, file->name, image);
	}
	if (index) 
	{
		offset += sizeof (nvm_header);
		offset += LE32TOH (nvm_header.IMAGELENGTH);
		nvm_header.NEXTHEADER = HTOLE32 (offset);
	}
	nvm_header.HEADERCHECKSUM = 0;
	nvm_header.HEADERCHECKSUM = checksum32 (&nvm_header, sizeof (nvm_header), 0);
	if (write (STDOUT_FILENO, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
	{
		error (1, errno, NVM_HDR_CANTSAVE, file->name, image);
	}
	copyimage (file, LE32TOH (nvm_header.IMAGELENGTH), image);
	return;
}


/*====================================================================*
 *
 *   void function2 (char const * filename, signed index, flag_t flags);
 *
 *   concatenate Atheros image files having the newer header format;
 *
 *   the manor and minor header version must be 0x0001 and 0x0001 
 *   and both header links must be 0xFFFFFFFF;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function2 (struct _file_ * file, signed index, flag_t flags) 

{
	static signed image = 0;
	static signed origin = ~0;
	static signed offset = 0;
	struct nvm_header2 nvm_header;
	if (read (file->file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
	{
		error (1, errno, NVM_HDR_CANTREAD, file->name, image);
	}
	if (LE16TOH (nvm_header.MajorVersion) != 1) 
	{
		error (1, 0, NVM_HDR_VERSION, file->name, image);
	}
	if (LE16TOH (nvm_header.MinorVersion) != 1) 
	{
		error (1, 0, NVM_HDR_VERSION, file->name, image);
	}
	if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
	{
		error (1, 0, NVM_HDR_CHECKSUM, file->name, image);
	}
	if (~nvm_header.PrevHeader) 
	{
		error (1, 0, NVM_HDR_LINK, file->name, image);
	}
	if (~nvm_header.NextHeader) 
	{
		error (1, 0, NVM_HDR_LINK, file->name, image);
	}
	nvm_header.PrevHeader = HTOLE32 (origin);
	origin = offset;
	if (index) 
	{
		offset += sizeof (nvm_header);
		offset += LE32TOH (nvm_header.ImageLength);
		nvm_header.NextHeader = HTOLE32 (offset);
	}
	nvm_header.HeaderChecksum = 0;
	nvm_header.HeaderChecksum = checksum32 (&nvm_header, sizeof (nvm_header), 0);
	if (write (STDOUT_FILENO, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
	{
		error (1, errno, NVM_HDR_CANTSAVE, file->name, image);
	}
	copyimage (file, LE32TOH (nvm_header.ImageLength), image);
	image++;
	return;
}


/*====================================================================*
 *
 *   void function (char const * filename, signed count, flag_t flags);
 *
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function (char const * filename, signed index, flag_t flags) 

{
	uint32_t version;
	struct _file_ file = 
	{
		-1,
		filename
	};
	if (_anyset (flags, NVM_VERBOSE)) 
	{
		error (0, 0, "%s", file.name);
	}
	if ((file.file = open (file.name, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, file.name);
	}
	if (read (file.file, &version, sizeof (version)) != sizeof (version)) 
	{
		error (1, errno, FILE_CANTREAD, file.name);
	}
	if (lseek (file.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file.name);
	}
	if (LE32TOH (version) == 0x60000000) 
	{
		function1 (&file, index, flags);
	}
	else 
	{
		function2 (&file, index, flags);
	}
	close (file.file);
	return;
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []);
 *   
 *   
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"qv",
		"file [file] [...] [> file]",
		"Qualcomm Atheros PLC Firmware Image File Splicer",
		"q\tsuppress messages",
		"v\tverbose messages",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'q':
			_setbits (flags, NVM_SILENCE);
			break;
		case 'v':
			_setbits (flags, NVM_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

#ifdef WIN32

	setmode (fileno (stdin), O_BINARY);
	setmode (fileno (stdout), O_BINARY);

#endif

	while ((argc) && (* argv)) 
	{
		function (* argv, argc-1, flags);
		argv++;
		argc--;
	}
	return (0);
}

