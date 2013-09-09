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
 *   chkpib2.c
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../key/HPAVKey.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/checksum32.c"
#include "../tools/fdchecksum32.c"
#include "../tools/checksum32.c"
#include "../tools/hexstring.c"
#include "../tools/hexdecode.c"
#include "../tools/strfbits.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../key/SHA256Reset.c"
#include "../key/SHA256Block.c"
#include "../key/SHA256Write.c"
#include "../key/SHA256Fetch.c"
#include "../key/HPAVKeyNID.c"
#include "../key/keys.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibpeek1.c"
#include "../pib/pibpeek2.c"
#endif

#ifndef MAKEFILE
#include "../nvm/manifest.c"
#endif

/*====================================================================*
 *
 *   signed pibimage1 (void const * memory, size_t extent, char const * filename, flag_t flags);
 *
 *   check memory-resident thunderbolt/lightning PIB image; return 0
 *   for a good image and -1 for a bad image;
 *
 *   the check performed here is not exhaustive but it is adequate;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage1 (void const * memory, size_t extent, char const * filename, flag_t flags)

{
	struct simple_pib * simple_pib = (struct simple_pib *)(memory);
	uint8_t NID [HPAVKEY_NID_LEN];
	if (_anyset (flags, PIB_VERBOSE))
	{
		printf ("------- %s -------\n", filename);
		if (pibpeek1 (memory))
		{
			if (_allclr (flags, PIB_SILENCE))
			{
				error (0, 0, PIB_BADVERSION, filename);
			}
			return (-1);
		}
	}
	if (extent != LE16TOH (simple_pib->PIBLENGTH))
	{
		if (_allclr (flags, PIB_SILENCE))
		{
			error (0, 0, PIB_BADLENGTH, filename);
		}
		return (-1);
	}
	if (checksum32 (memory, extent, 0))
	{
		if (_allclr (flags, PIB_SILENCE))
		{
			error (0, 0, PIB_BADCHECKSUM, filename);
		}
		return (-1);
	}
	HPAVKeyNID (NID, simple_pib->NMK, simple_pib->PreferredNID [HPAVKEY_NID_LEN-1] >> 4);
	if (memcmp (NID, simple_pib->PreferredNID, sizeof (NID)))
	{
		if (_allclr (flags, PIB_SILENCE))
		{
			error (0, 0, PIB_BADNID, filename);
		}
		return (-1);
	}
	return (0);
}


/*====================================================================*
 *
 *   signed pibimage2 (void const * memory, size_t extent, char const * filename, flag_t flags);
 *
 *   check memory-resident panther/lynx PIB image; return 0 for a
 *   good image and -1 for an bad image;
 *
 *   the check performed here is not exhaustive but it is adequate;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage2 (void const * memory, size_t extent, char const * filename, flag_t flags)

{
	struct simple_pib * simple_pib = (struct simple_pib *)(memory);
	struct pib_header * pib_header = (struct pib_header *)(memory);
	uint8_t NID [HPAVKEY_NID_LEN];
	if (_anyset (flags, PIB_VERBOSE))
	{
		pib_header->PIBLENGTH = HTOLE16(extent);
		printf ("------- %s -------\n", filename);
		if (pibpeek2 (memory))
		{
			if (_allclr (flags, PIB_SILENCE))
			{
				error (0, 0, PIB_BADVERSION, filename);
			}
			return (-1);
		}
		memset (pib_header, 0, sizeof (* pib_header));
	}
	HPAVKeyNID (NID, simple_pib->NMK, simple_pib->PreferredNID [HPAVKEY_NID_LEN-1] >> 4);
	if (memcmp (NID, simple_pib->PreferredNID, sizeof (NID)))
	{
		if (_allclr (flags, PIB_SILENCE))
		{
			error (0, 0, PIB_BADNID, filename);
		}
		return (-1);
	}
	return (0);
}


/*====================================================================*
 *
 *   signed pibchain2 (void const * memory, size_t extent, char const * filename, flag_t flags);
 *
 *   search a panther/lynx image chain looking for PIB images and
 *   verify each one; return 0 on success or -1 on error; errors
 *   occur due to an invalid image chain or a bad parameter block;
 *
 *   this implementation reads the parameter block from file into
 *   into memory and checks it there;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibchain2 (void const * memory, size_t extent, char const * filename, flag_t flags)

{
	struct nvm_header2 * nvm_header;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	unsigned module = 0;
	do
	{
		nvm_header = (struct nvm_header2 *)((char *)(memory) + offset);
		if (LE16TOH (nvm_header->MajorVersion) != 1)
		{
			if (_allclr (flags, NVM_SILENCE))
			{
				error (0, 0, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE16TOH (nvm_header->MinorVersion) != 1)
		{
			if (_allclr (flags, NVM_SILENCE))
			{
				error (0, 0, NVM_HDR_VERSION, filename, module);
			}
			return (-1);
		}
		if (LE32TOH (nvm_header->PrevHeader) != origin)
		{
			if (_allclr (flags, NVM_SILENCE))
			{
				error (0, 0, NVM_HDR_LINK, filename, module);
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
		if (checksum32 ((char *)(memory) + offset, LE32TOH (nvm_header->ImageLength), nvm_header->ImageChecksum))
		{
			if (_allclr (flags, NVM_SILENCE))
			{
				error (0, 0, NVM_IMG_CHECKSUM, filename, module);
			}
			return (-1);
		}
		if (LE32TOH (nvm_header->ImageType) == NVM_IMAGE_MANIFEST)
		{
			if (_anyset (flags, NVM_MANIFEST))
			{
				printf ("------- %s (%d) -------\n", filename, module);
				manifest ((char *)(memory) + offset, LE32TOH (nvm_header->ImageLength));
				return (0);
			}
		}
		else if (LE32TOH (nvm_header->ImageType) == NVM_IMAGE_PIB)
		{
			return (pibimage2 ((char *)(memory) + offset, LE32TOH (nvm_header->ImageLength), filename, flags));
		}
		offset += LE32TOH (nvm_header->ImageLength);
		extent -= LE32TOH (nvm_header->ImageLength);
		module++;
	}
	while (~nvm_header->NextHeader);
	if (extent)
	{
		if (_allclr (flags, NVM_SILENCE))
		{
			error (0, errno, NVM_HDR_LINK, filename, module);
		}
		return (-1);
	}
	error (0, 0, "%s has no PIB", filename);
	return (-1);
}


/*====================================================================*
 *
 *   signed chkpib (char const * filename, flag_t flags);
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed chkpib (char const * filename, flag_t flags)

{
	void * memory = 0;
	signed extent = 0;
	signed status;
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
			error (0, errno, FILE_CANTHOME, filename);
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
		if (_allclr (flags, NVM_SILENCE))
		{
			error (0, 0, FILE_WONTREAD, filename);
		}
		status = -1;
	}
	else if (LE32TOH (* (uint32_t *)(memory)) == 0x00010001)
	{
		status = pibchain2 (memory, extent, filename, flags);
	}
	else
	{
		status = pibimage1 (memory, extent, filename, flags);
	}
	free (memory);
	return (status);
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
		"mqv",
		"file [file] [...]",
		"Qualcomm Atheros PLC Parameter File Inspector",
		"m\tdisplay manifest",
		"q\tquiet",
		"v\tverbose messages",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed state = 0;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch (c)
		{
		case 'm':
			_setbits (flags, PIB_MANIFEST);
			break;
		case 'q':
			_setbits (flags, PIB_SILENCE);
			break;
		case 'v':
			_setbits (flags, PIB_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argc) && (* argv))
	{
		errno = 0;
		if (chkpib (* argv, flags))
		{
			state = 1;
		}
		else if (_allclr (flags, (PIB_VERBOSE | PIB_SILENCE | PIB_MANIFEST)))
		{
			printf ("%s looks good\n", * argv);
		}
		argc--;
		argv++;
	}
	return (state);
}

