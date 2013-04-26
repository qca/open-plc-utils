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
 *   getpib.c - PIB Data Extractor
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
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
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/basespec.c"
#include "../tools/todigit.c"
#include "../tools/hexout.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define SETPIB_VERBOSE (1 << 0)
#define SETPIB_SILENCE (1 << 1)
#define SETPIB_NEWLINE (1 << 2)

/*====================================================================*
 *
 *   void getmemory (byte const * memory, size_t extent, char const * object, size_t length);
 *
 *
 *--------------------------------------------------------------------*/

static void getmemory (byte const * memory, size_t extent, char const * object, size_t length) 

{
	if (length > extent) 
	{
		error (1, ECANCELED, "%s exceeds PIB length of " SIZE_T_SPEC " bytes", object, length);
	}
	hexout (memory, length, ':', 0, stdout);
	return;
}


/*====================================================================*
 *
 *   void getstring (byte const * memory, size_t extent, char const * object, size_t length);
 *
 *
 *--------------------------------------------------------------------*/

static void getstring (byte const * memory, size_t extent, char const * object, size_t length) 

{
	char const * string = (char const *)(memory);
	if (length > extent) 
	{
		error (1, ECANCELED, "%s exceeds PIB length " SIZE_T_SPEC " bytes", object, length);
	}
	while (isprint (*string) && (length--)) 
	{
		putc (*string++, stdout);
	}
	return;
}


/*====================================================================*
 *
 *   void getpib (int argc, char const * argv [], byte const * memory, size_t extent);
 *
 *   extract and print the specified data objects from memory;
 *
 *
 *--------------------------------------------------------------------*/

static void getpib (int argc, char const * argv [], byte const * memory, size_t extent) 

{
	unsigned length = 0;
	while ((argc) && (*argv)) 
	{
		char const * object = * argv;
		argc--;
		argv++;
		if (!strcmp (object, "byte")) 
		{
			uint8_t * number = (uint8_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("%u", * number);
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "word")) 
		{
			uint16_t * number = (uint16_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("%u", LE16TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "long")) 
		{
			uint32_t * number = (uint32_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("%u", LE32TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "huge")) 
		{
			uint64_t * number = (uint64_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("%llu", LE64TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}

#if 1

		else if (!strcmp (object, "xbyte")) 
		{
			uint8_t * number = (uint8_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("0x%02X", * number);
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "xword")) 
		{
			uint16_t * number = (uint16_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("0x%04X", LE16TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "xlong")) 
		{
			uint32_t * number = (uint32_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("0x%08X", LE32TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}
		else if (!strcmp (object, "xhuge")) 
		{
			uint64_t * number = (uint64_t *)(memory);
			if (sizeof (* number) > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			printf ("0x%016LX", LE64TOH (* number));
			memory += sizeof (* number);
			extent -= sizeof (* number);
		}

#endif

		else if (!strcmp (object, "mac")) 
		{
			length = ETHER_ADDR_LEN;
			if (length > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			getmemory (memory, extent, object, length);
			memory += length;
			extent -= length;
		}
		else if (!strcmp (object, "key")) 
		{
			length = PIB_KEY_LEN;
			if (length > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			getmemory (memory, extent, object, length);
			memory += length;
			extent -= length;
		}
		else if (!strcmp (object, "hfid")) 
		{
			length = PIB_HFID_LEN;
			if (length > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			getstring (memory, extent, object, length);
			memory += length;
			extent -= length;
		}

#if 1

		else if (!strcmp (object, "adminusername") || !strcmp (object, "adminpassword") || !strcmp (object, "accessusername")) 
		{
			length = PIB_NAME_LEN + 1;
			if (length > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			getstring (memory, extent, object, length);
			memory += length;
			extent -= length;
		}
		else if (!strcmp (object, "accesspassword")) 
		{
			length = PIB_HFID_LEN + 1;
			if (length > extent) 
			{
				error (1, ECANCELED, "%s exceeds PIB extent " SIZE_T_SPEC, object, extent);
			}
			getstring (memory, extent, object, length);
			memory += length;
			extent -= length;
		}
		else if (!strcmp (object, "username") || !strcmp (object, "password") || !strcmp (object, "url")) 
		{
			length = PIB_TEXT_LEN + 1;
			getstring (memory, extent, object, length);
			memory += length;
			extent -= length;
		}

#endif

		else if (!strcmp (object, "data")) 
		{
			if (!* argv) 
			{
				error (1, EINVAL, "%s needs a length", object);
			}
			length = (unsigned)(uintspec (* argv, 1, extent));
			hexout (memory, length, 0, 0, stdout);
			memory += length;
			extent -= length;
			argc--;
			argv++;
		}
		else if (!strcmp (object, "text")) 
		{
			if (!* argv) 
			{
				error (1, EINVAL, "%s needs a length", object);
			}
			length = (unsigned)(uintspec (* argv, 1, extent));
			getstring (memory, extent, object, length);
			memory += length;
			extent -= length;
			argc--;
			argv++;
		}
		else if (!strcmp (object, "skip")) 
		{
			if (!* argv) 
			{
				error (1, EINVAL, "%s needs a length", object);
			}
			length = (unsigned)(uintspec (* argv, 1, extent));
			memory += length;
			extent -= length;
			argc--;
			argv++;
			continue;
		}
		else 
		{
			error (1, ENOTSUP, "%s", object);
		}
		if ((argc) && (* argv)) 
		{
			putc (' ', stdout);
		}
	}
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
		"qvn",
		"file offset type [size]\n\n\tstandard-length types are 'byte'|'word'|'long'|'huge'|'hfid'|'mac'|'key'\n\tvariable-length types are 'data'|'text'|'skip'",
		"PIB Data Extractor",
		"n\tappend newline",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	struct _file_ file;
	struct pib_header * header;
	unsigned offset = 0;
	unsigned extent = 0;
	byte * buffer;
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	opterr = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'n':
			_setbits (flags, SETPIB_NEWLINE);
			break;
		case 'q':
			_setbits (flags, SETPIB_SILENCE);
			break;
		case 'v':
			_setbits (flags, SETPIB_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc) 
	{
		error (1, 0, "No file to read");
	}
	file.name = * argv;
	if ((file.file = open (file.name, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, file.name);
	}
	if ((extent = lseek (file.file, 0, SEEK_END)) == (unsigned)(-1)) 
	{
		error (1, errno, FILE_CANTSIZE, file.name);
	}
	if (!(buffer = malloc (extent))) 
	{
		error (1, errno, "Can't span %s", file.name);
	}
	if (lseek (file.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file.name);
	}
	if (read (file.file, buffer, extent) != (signed)(extent)) 
	{
		error (1, errno, FILE_CANTLOAD, file.name);
	}
	close (file.file);
	argc--;
	argv++;
	header = (struct pib_header *)(buffer);
	if (extent != LE16TOH (header->PIBLENGTH)) 
	{
		error (1, ECANCELED, "Bad PIB filesize: %s", file.name);
	}
	if (!argc) 
	{
		error (1, ECANCELED, "Need an offset");
	}
	offset = (uint32_t)(basespec (* argv, 16, sizeof (uint32_t)));
	if (offset > extent) 
	{
		error (1, ECANCELED, "PIB offset " SIZE_T_SPEC " exceeds PIB extent " SIZE_T_SPEC, offset, extent);
	}
	argc--;
	argv++;
	if (!argc) 
	{
		_setbits (flags, SETPIB_VERBOSE);
	}
	getpib (argc, argv, buffer + offset, extent - offset);
	if (_anyset (flags, SETPIB_NEWLINE)) 
	{
		putc ('\n', stdout);
	}
	free (buffer);
	return (0);
}

