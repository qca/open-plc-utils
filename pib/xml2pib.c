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
 *   xml2pib.c -
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
#include "../nodes/node.h"
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
#include "../tools/todigit.c"
#include "../tools/output.c"
#include "../tools/emalloc.c"
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
#include "../nodes/xmlopen.c"
#include "../nodes/xmlnode.c"
#include "../nodes/xmlscan.c"
#include "../nodes/xmledit.c"
#include "../nodes/xmltree.c"
#include "../nodes/xmlfree.c"
#endif

/*====================================================================*
 *
 *   signed pibedit1 (struct node const * node, void * memory, size_t extent);
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibedit1 (struct node const * node, void * memory, size_t extent)

{
	struct simple_pib * simple_pib = (struct simple_pib *)(memory);
	xmledit (node, memory, extent);
	checksum32 (memory, extent, simple_pib->CHECKSUM);
	return (0);
}


/*====================================================================*
 *
 *   signed pibedit2 (char const * filename, struct node const * node, char * memory, size_t extent);
 *
 *   search a panther/lynx image chain looking for PIB images and
 *   verify each one; return 0 on success or -1 on error; errors
 *   occur due to an invalid image chain or a bad parameter block;
 *
 *   this implementation reads the parameter block from file into
 *   into memory and checks it there;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibedit2 (char const * filename, struct node const * node, char * memory, size_t extent)

{
	struct nvm_header2 * nvm_header;
	uint32_t origin = ~0;
	uint32_t offset = 0;
	unsigned length = 0;
	unsigned module = 0;
	do
	{
		nvm_header = (struct nvm_header2 *)(memory + offset);
		if (LE16TOH (nvm_header->MajorVersion) != 1)
		{
			error (1, 0, NVM_HDR_VERSION, filename, module);
		}
		if (LE16TOH (nvm_header->MinorVersion) != 1)
		{
			error (1, 0, NVM_HDR_VERSION, filename, module);
		}
		if (LE32TOH (nvm_header->PrevHeader) != origin)
		{
			error (1, 0, NVM_HDR_LINK, filename, module);
		}
		if (checksum32 (nvm_header, sizeof (* nvm_header), 0))
		{
			error (1, 0, NVM_HDR_CHECKSUM, filename, module);
		}
		origin = offset;
		offset += sizeof (* nvm_header);
		extent -= sizeof (* nvm_header);
		length = LE32TOH (nvm_header->ImageLength);
		if (checksum32 (memory + offset, length, nvm_header->ImageChecksum))
		{
			error (1, 0, NVM_IMG_CHECKSUM, filename, module);
		}
		if (LE32TOH (nvm_header->ImageType) == NVM_IMAGE_PIB)
		{
			xmledit (node, memory + offset, length);
			nvm_header->ImageChecksum = checksum32 (memory + offset, length, 0);
			nvm_header->HeaderChecksum = checksum32 (nvm_header, sizeof (* nvm_header), nvm_header->HeaderChecksum);
			return (0);
		}
		offset += length;
		extent -= length;
		module++;
	}
	while (~nvm_header->NextHeader);
	return (-1);
}


/*====================================================================*
 *
 *   signed function (char const * filename);
 *
 *   determine the type of file and call appropriate functions;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed function (char const * filename, struct node const * node)

{
	char * memory = 0;
	signed extent = 0;
	signed fd;
	if ((fd = open (filename, O_BINARY | O_RDWR)) == -1)
	{
		error (1, errno, FILE_CANTOPEN, filename);
	}
	if ((extent = lseek (fd, 0, SEEK_END)) == -1)
	{
		error (1, errno, FILE_CANTSIZE, filename);
	}
	if (!(memory = (char *)(malloc (extent))))
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
	if (LE32TOH (* (uint32_t *)(memory)) == 0x60000000)
	{
		error (1, 0, FILE_WONTREAD, filename);
	}
	if (LE32TOH (* (uint32_t *)(memory)) == 0x00010001)
	{
		pibedit2 (filename, node, memory, extent);
	}
	else
	{
		pibedit1 (node, memory, extent);
	}
	if (lseek (fd, 0, SEEK_SET))
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (write (fd, memory, extent) != extent)
	{
		error (1, errno, FILE_CANTSAVE, filename);
	}
	close (fd);
	return (0);
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
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
		"f:qv",
		"pib-file [pib-file] [...]",
		"Qualcomm Atheros PLC Parameter File Editor",
		"f f\txmlfile is (f)",
		"q\tquiet",
		"v\tverbose",
		(char const *) (0)
	};
	struct node * node = (struct node *)(0);
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch (c)
		{
		case 'f':
			node = xmlopen (optarg);
			break;
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
		function (* argv, node);
		argc--;
		argv++;
	}
	if (_anyset (flags, PIB_VERBOSE))
	{
		xmltree (node);
	}
	xmlfree (node);
	return (0);
}

