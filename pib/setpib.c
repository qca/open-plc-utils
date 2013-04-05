/*====================================================================*
 *   
 *   Copyright (c) 2011 Qualcomm Atheros Inc.
 *   
 *   Permission to use, copy, setpib, and/or distribute this software 
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
 *   setpib.c -
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*"
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../pib/pib.h"
#include "../nvm/nvm.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/basespec.c"
#include "../tools/dataspec.c"
#include "../tools/bytespec.c"
#include "../tools/todigit.c"
#include "../tools/hexdump.c"
#include "../tools/hexpeek.c"
#include "../tools/fdchecksum32.c"
#include "../tools/checksum32.c"
#include "../tools/memencode.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmseek2.c"
#endif

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define SETPIB_VERBOSE (1 << 0)
#define SETPIB_SILENCE (1 << 1)
#define SETPIB_HEADERS (1 << 2)
#define SETPIB_CHANGED (1 << 3)
#define SETPIB_WINDOW 32

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

static flag_t flags = (flag_t)(0);

/*====================================================================*
 *
 *   signed modify (void * memory, size_t extent, int argc, char const * argv [], unsigned window) 
 *
 *   apply a series of edits to a memory region; edits are specified
 *   in string vector argv [] which must follow rules understood by 
 *   function memencode(); this function merely walks the vector and
 *   deals with error and display options;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed modify (void * memory, size_t extent, int argc, char const * argv [], unsigned window) 

{
	uint32_t origin;
	uint32_t offset;
	if (!argc) 
	{
		error (1, ENOTSUP, "Need an offset");
	}
	origin = offset = (size_t)(basespec (* argv, 16, sizeof (offset)));
	if (offset >= extent) 
	{
		error (1, ECANCELED, "Offset %X exceeds file length of " SIZE_T_SPEC, offset, extent);
	}
	argc--;
	argv++;
	if (!argc) 
	{
		_setbits (flags, SETPIB_VERBOSE);
	}
	while ((argc > 1) && (* argv)) 
	{
		_setbits (flags, SETPIB_CHANGED);
		offset += (unsigned)(memencode ((byte *)(memory) + offset, extent - offset, argv [0], argv [1]));
		argc -= 2;
		argv += 2;
	}
	if (argc) 
	{
		error (1, ECANCELED, "object %s needs a value", *argv);
	}
	if (_anyset (flags, SETPIB_VERBOSE)) 
	{
		hexpeek (memory, origin, offset, extent, window, stdout);
	}
	return (0);
}


/*====================================================================*
 *
 *   signed pibimage1 (signed fd, char const * filename, int argc, char const * argv [], unsigned window);
 * 
 *   read an entire flat parameter file into memory, edit it, save 
 *   it and display it;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage1 (signed fd, char const * filename, int argc, char const * argv [], unsigned window) 

{
	off_t extent;
	void * memory;
	if ((extent = lseek (fd, 0, SEEK_END)) == -1) 
	{
		error (1, errno, FILE_CANTSIZE, filename);
	}
	if (!(memory = malloc (extent))) 
	{
		error (1, errno, FILE_CANTLOAD, filename);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (read (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (modify (memory, extent, argc, argv, window)) 
	{
		error (1, errno, FILE_CANTEDIT, filename);
	}
	if (_anyset (flags, SETPIB_CHANGED)) 
	{
		struct pib_header * pib_header = (struct pib_header *)(memory);
		pib_header->CHECKSUM = checksum32 (memory, extent, pib_header->CHECKSUM);
		if (write (fd, memory, extent) != extent) 
		{
			error (1, errno, FILE_CANTSAVE, filename);
		}
		if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
		{
			error (1, errno, FILE_CANTHOME, filename);
		}
	}
	free (memory);
	close (fd);
	return (0);
}


/*====================================================================*
 *
 *   signed pibimage2 (signed fd, char const * filename, int argc, char const * argv [], unsigned window);
 * 
 *   read an entire flat parameter file into memory, edit it, save 
 *   it and display it;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage2 (signed fd, char const * filename, struct nvm_header2 * nvm_header, int argc, char const * argv [], unsigned window) 

{
	void * memory;
	off_t extent = LE32TOH (nvm_header->ImageLength);
	if (!(memory = malloc (extent))) 
	{
		error (1, errno, FILE_CANTLOAD, filename);
	}
	if (read (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (modify (memory, extent, argc, argv, window)) 
	{
		error (1, errno, FILE_CANTEDIT, filename);
	}
	if (_anyset (flags, SETPIB_CHANGED)) 
	{
		nvm_header->ImageChecksum = checksum32 (memory, extent, 0);
		if (write (fd, memory, extent) != extent) 
		{
			error (1, errno, FILE_CANTSAVE, filename);
		}
		if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
		{
			error (1, errno, FILE_CANTHOME, filename);
		}
		nvm_header->HeaderChecksum = checksum32 (nvm_header, sizeof (* nvm_header), nvm_header->HeaderChecksum);
		if (lseek (fd, (off_t)(0) - sizeof (* nvm_header), SEEK_CUR) == -1) 
		{
			error (1, errno, FILE_CANTHOME, filename);
		}
		if (write (fd, nvm_header, sizeof (* nvm_header)) != sizeof (* nvm_header)) 
		{
			error (1, errno, FILE_CANTSAVE, filename);
		}
		if (lseek (fd, (off_t)(0) - sizeof (* nvm_header), SEEK_CUR) == -1) 
		{
			error (1, errno, FILE_CANTHOME, filename);
		}
	}
	free (memory);
	return (0);
}


/*====================================================================*
 *
 *   signed function (int argc, char const * argv [], unsigned window);
 *
 *   call an appropriate parameter edit function based on the file 
 *   header;
 *
 *   older parameter files are flat with their own header; newer ones
 *   are image chains where one of image contains the parameter block;
 *
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed function (int argc, char const * argv [], unsigned window) 

{
	uint32_t version;
	char const * filename = * argv;
	signed status;
	signed fd;
	if ((fd = open (filename, O_BINARY|O_RDWR)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, filename);
	}
	if (read (fd, &version, sizeof (version)) != sizeof (version)) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	argc--;
	argv++;
	if (LE32TOH (version) == 0x00010001) 
	{
		struct nvm_header2 nvm_header;
		if (!nvmseek2 (fd, filename, &nvm_header, NVM_IMAGE_PIB)) 
		{
			status = pibimage2 (fd, filename, &nvm_header, argc, argv, window);
		}
	}
	else 
	{
		status = pibimage1 (fd, filename, argc, argv, window);
	}
	close (fd);
	return (status);
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *   
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"qvw:x",
		"file base [type data] [type data] [...]\n\n\tstandard-length types are 'byte'|'word'|'long'|'huge'|'hfid'|'mac'|'key'\n\tvariable-length types are 'data'|'text'|'fill'|'skip'",
		"Qualcomm Atheros PIB File Editor",
		"q\tquiet mode",
		"v[v]\tverbose mode",
		"w n\twindow size is (n) [" LITERAL (SETPIB_WINDOW) "]",
		"x\trepair checksum",
		(char const *) (0)
	};
	unsigned window = SETPIB_WINDOW;
	signed c;
	optind = 1;
	opterr = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'q':
			_setbits (flags, SETPIB_SILENCE);
			break;
		case 'v':
			if (_anyset (flags, SETPIB_VERBOSE)) 
			{
				_setbits (flags, SETPIB_HEADERS);
			}
			_setbits (flags, SETPIB_VERBOSE);
			break;
		case 'w':
			window = (unsigned)(uintspec (optarg, 0, UINT_MAX));
			_setbits (flags, SETPIB_VERBOSE);
			break;
		case 'x':
			_setbits (flags, SETPIB_CHANGED);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if ((argc) && (* argv)) 
	{
		function (argc, argv, window);
	}
	return (0);
}

