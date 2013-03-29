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
 *   chknvm2.c -
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/memory.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../ram/sdram.h"
#include "../nvm/nvm.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/checksum32.c"
#include "../tools/strfbits.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvm.c"
#include "../nvm/nvmpeek.c"
#include "../nvm/nvmpeek1.c"
#include "../nvm/nvmpeek2.c"
#include "../nvm/manifest.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define HARDWARE 0
#define SOFTWARE 1
#define VER 2
#define REV 3
#define BUILD 6
#define DATE 7

/*====================================================================*
 *
 *   unsigned string2vector (char ** vector, length, char * string, char c);
 *
 *   convert string to a vector and return vector count; split string
 *   on characer (c);
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static unsigned string2vector (char ** vector, char * string, char c) 

{
	char ** origin = vector;
	for (*vector++ = string; *string; string++) 
	{
		if (*string == c) 
		{
			*string++ = (char)(0);
			*vector++ = string--;
		}
	}
	*vector = (char *)(0);
	return ((unsigned)(vector - origin));
}


/*====================================================================*
 *
 *   void firmware (char const * filename, unsigned module, char const * memory, unsigned offset, flag_t flags);
 *
 *   locate and display information stored in a firmware image; this
 *   is magic that may change in future firmware releases;            
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void firmware (char const * filename, unsigned module, char const * memory, unsigned offset, flag_t flags) 

{
	if (_anyset (flags, NVM_FIRMWARE)) 
	{
		if ((* memory > 0x20) && (* memory < 0x7f)) 
		{
			printf ("%s (%d) %s\n", filename, module, memory);
		}
		else 
		{
			printf ("%s (%d) %s\n", filename, module, memory + offset);
		}
		return;
	}
	if (_anyset (flags, NVM_IDENTITY)) 
	{
		char * vector [16];
		char buffer [256];
		if ((* memory > 0x20) && (* memory < 0x7f)) 
		{
			strncpy (buffer, memory, sizeof (buffer));
		}
		else 
		{
			strncpy (buffer, memory + offset, sizeof (buffer));
		}
		string2vector (vector, buffer, '-');
		printf ("%s ", vector [HARDWARE]);
		printf ("%04d ", atoi (vector [BUILD]));
		printf ("%s ", vector [DATE]);
		printf ("%s.", vector [VER]);
		printf ("%s ", vector [REV]);
		printf ("%s (%d)\n", filename, module);
		return;
	}
	return;
}


/*====================================================================*
 *
 *   signed nvmimage2 (void const * memory, size_t extent, char const * filename, flag_t flags) 
 *
 *   verify a firmware image chain stored in memory; return 0 on 
 *   success or -1 on error;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed nvmimage2 (void const * memory, size_t extent, char const * filename, flag_t flags) 

{
	struct nvm_header1 * nvm_header;
	unsigned module = 0;
	uint32_t offset = 0;
	do 
	{
		nvm_header = (struct nvm_header1 *)((char *)(memory) + offset);
		if (LE32TOH (nvm_header->HEADERVERSION) != 0x60000000) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (checksum32 (nvm_header, sizeof (* nvm_header), 0)) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_HDR_CHECKSUM, filename, module);
			}
			return (-1);
		}
		offset += sizeof (* nvm_header);
		extent -= sizeof (* nvm_header);
		if (_anyset (flags, NVM_VERBOSE)) 
		{
			printf ("------- %s (%d) -------\n", filename, module);
			nvmpeek1 (nvm_header);
		}
		if (LE32TOH (nvm_header->IMAGETYPE) == NVM_IMAGE_FIRMWARE) 
		{
			firmware (filename, module, (char *)(memory) + offset, 0x70, flags);
		}
		if (checksum32 ((char *)(memory) + offset, LE32TOH (nvm_header->IMAGELENGTH), nvm_header->IMAGECHECKSUM)) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_IMG_CHECKSUM, filename, module);
			}
			return (-1);
		}
		offset += LE32TOH (nvm_header->IMAGELENGTH);
		extent -= LE32TOH (nvm_header->IMAGELENGTH);
		module++;
	}
	while (nvm_header->NEXTHEADER);
	if (extent)
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, NVM_HDR_LINK, filename, module);
		}
	}
	return ((signed)(extent));
}


/*====================================================================*
 *
 *   signed nvmchain2 (void const * memory, size_t extent, char const * filename, flag_t flags) 
 *
 *   verify a firmware image chain stored in memory; return 0 on 
 *   success or -1 on error;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed nvmchain2 (void const * memory, size_t extent, char const * filename, flag_t flags) 

{
	struct nvm_header2 * nvm_header;
	unsigned module = 0;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	uint32_t length = 0;
	do 
	{
		nvm_header = (struct nvm_header2 *)((char *)(memory) + offset);
		if (LE16TOH (nvm_header->MajorVersion) != 1) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE16TOH (nvm_header->MinorVersion) != 1) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE32TOH (nvm_header->PrevHeader) != origin) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_HDR_LINK, filename, module);
			}
			return (-1);
		}
		if (checksum32 (nvm_header, sizeof (* nvm_header), 0)) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, 0, NVM_HDR_CHECKSUM, filename, module);
			}
			return (-1);
		}
		origin = offset;
		offset += sizeof (* nvm_header);
		extent -= sizeof (* nvm_header);
		length = LE32TOH (nvm_header->ImageLength);
		if (_anyset (flags, NVM_VERBOSE)) 
		{
			printf ("------- %s (%d) -------\n", filename, module);
			nvmpeek2 (nvm_header);
		}
		if (LE32TOH (nvm_header->ImageType) == NVM_IMAGE_MANIFEST) 
		{
			if (_anyset (flags, NVM_MANIFEST)) 
			{
				printf ("------- %s (%d) -------\n", filename, module);
				manifest ((char *)(memory) + offset, length);
				return (0);
			}
		}
		if (checksum32 ((char *)(memory) + offset, length, nvm_header->ImageChecksum)) 
		{
			if (_allclr (flags, NVM_SILENCE)) 
			{
				error (0, errno, NVM_IMG_CHECKSUM, filename, module);
			}
			return (-1);
		}
		offset += length;
		extent -= length;
		module++;
	}
	while (~nvm_header->NextHeader);
	if (extent)
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, NVM_HDR_LINK, filename, module);
		}
	}
	return (0);
}


/*====================================================================*
 *
 *   signed chknvm (char const * filename, flag_t flags);
 *
 *   read .nvm file into memory and validate it; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed chknvm (char const * filename, flag_t flags) 

{
	void * memory = (void *)(0);
	signed extent = 0;
	signed status = 0;
	signed fd;
	if ((fd = open (filename, O_BINARY|O_RDONLY)) == -1) 
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, FILE_CANTOPEN, filename);
		}
		return (-1);
	}
	if ((extent = lseek (fd, 0, SEEK_END)) == -1) 
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, FILE_CANTSIZE, filename);
		}
		return (-1);
	}
	if (!(memory = malloc (extent))) 
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, FILE_CANTLOAD, filename);
		}
		return (-1);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, FILE_CANTLOAD, filename);
		}
		return (-1);
	}
	if (read (fd, memory, extent) != extent) 
	{
		if (_allclr (flags, NVM_SILENCE)) 
		{
			error (0, errno, FILE_CANTREAD, filename);
		}
		return (-1);
	}
	close (fd);
	if (LE32TOH (* (uint32_t *)(memory)) == 0x60000000) 
	{
		status = nvmimage2 (memory, extent, filename, flags);
	}
	else 
	{
		status = nvmchain2 (memory, extent, filename, flags);
	}
	free (memory);
	return (status);
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []);
 *   
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"imqrv",
		"file [file] [...]",
		"Qualcomm Atheros PLC Image File Validator",
		"i\tprint firmware identity string",
		"m\tdisplay manifest",
		"q\tsuppress messages",
		"r\tprint firmware revision string",
		"v\tverbose messages",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed state = 0;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'i':
			_setbits (flags, NVM_IDENTITY);
			break;
		case 'm':
			_setbits (flags, NVM_MANIFEST);
			break;
		case 'r':
			_setbits (flags, NVM_FIRMWARE);
			break;
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
	while ((argc) && (* argv)) 
	{
		if (chknvm (* argv, flags)) 
		{
			state = 1;
		}
		else if (_allclr (flags, (NVM_VERBOSE|NVM_SILENCE|NVM_MANIFEST|NVM_FIRMWARE|NVM_IDENTITY))) 
		{
			printf ("%s looks good\n", * argv);
		}
		argc--;
		argv++;
	}
	return (state);
}

