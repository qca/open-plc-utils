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
 *   pib2xml.c 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/endian.h"
#include "../tools/format.h"
#include "../tools/files.h"
#include "../tools/chars.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../nodes/node.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/hexview.c"
#include "../tools/output.c"
#include "../tools/error.c"
#include "../tools/checksum32.c"
#include "../tools/fdchecksum32.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmseek2.c"
#endif

#ifndef MAKEFILE
#include "../nodes/xmlschema.c"
#endif

/*====================================================================*
 *   
 *   void pibdump (signed fd, char const * filename, char const * schema, unsigned extent, flag_t flags);
 *   
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

static void pibdump (signed fd, char const * filename, char const * schema, unsigned extent, flag_t flags) 

{
	unsigned offset = 0;
	signed indent = 0;
	signed length = 0;
	unsigned lineno = 1;
	char symbol [0x0100];
	char string [0x0400];
	char * sp;
	signed c;
	output (indent++, "<%s xmlns:xsi='%s' xsi:noNamespaceSchemaLocation='%s'>", DATA_OBJECT, XML_NAMESPACE, schema);
	while ((c = getc (stdin)) != EOF) 
	{
		if ((c == '#') || (c == ';')) 
		{
			do 
			{
				c = getc (stdin);
			}
			while (nobreak (c));
		}
		if (isspace (c)) 
		{
			if (c == '\n') 
			{
				lineno++;
			}
			continue;
		}
		while (isdigit (c)) 
		{
			length *= 10;
			length += c - '0';
			c = getc (stdin);
		}
		while (isblank (c)) 
		{
			c = getc (stdin);
		}
		sp = symbol;
		if (isalpha (c) || (c == '_')) 
		{
			do 
			{
				*sp++ = (char)(c);
				c = getc (stdin);
			}
			while (isident (c));
		}
		*sp = (char)(0);
		while (isblank (c)) 
		{
			c = getc (stdin);
		}
		if (c == '[') 
		{

#if 0

			*sp++ = (char)(c);

#endif

			c = getc (stdin);
			while (isblank (c)) 
			{
				c = getc (stdin);
			}
			while (isdigit (c)) 
			{

#if 0

				*sp++ = (char)(c);

#endif

				c = getc (stdin);
			}
			while (isblank (c)) 
			{
				c = getc (stdin);
			}
			if (c != ']') 
			{
				error (1, EINVAL, "Have '%c' but need ']'", c);
			}

#if 0

			*sp++ = (char)(c);

#endif

			c = getc (stdin);
		}
		*sp = (char)(0);
		while (isblank (c)) 
		{
			c = getc (stdin);
		}
		sp = string;
		while (nobreak (c)) 
		{
			*sp++ = (char)(c);
			c = getc (stdin);
		}
		*sp = (char)(0);
		if (length > 0) 
		{

#if defined (WIN32)

			byte * buffer = (byte *)(emalloc (length));

#else

			byte buffer [length];

#endif

			if (read (fd, buffer, length) == length) 
			{
				output (indent++, "<%s name='%s'>", DATA_MEMBER, symbol);

#if 0

				if (*string) 
				{
					output (indent++, "<text>");
					output (indent, "%s", string);
					output (indent--, "</text>");
				}

#endif

				output (indent++, "<%s>", DATA_OFFSET);
				output (indent, "%04X", offset);
				output (indent--, "</%s>", DATA_OFFSET);
				output (indent++, "<%s>", DATA_LENGTH);
				output (indent, "%d", length);
				output (indent--, "</%s>", DATA_LENGTH);
				output (indent++, "<%s>", DATA_MEMORY);
				for (c = 0; c < indent; c++) 
				{
					printf ("\t");
				}
				for (c = 0; c < length; c++) 
				{
					printf ("%02X", buffer [c]);
				}
				printf ("\n");
				output (indent--, "</%s>", DATA_MEMORY);
				output (indent--, "</%s>", DATA_MEMBER);
			}

#if defined (WIN32)

			free (buffer);

#endif

		}
		offset += length;
		length = 0;
	}
	output (indent--, "</%s>", DATA_OBJECT);
	if (_allclr (flags, PIB_SILENCE)) 
	{
		if (offset != extent) 
		{
			error (0, 0, "file %s is %d not %d bytes", filename, extent, offset);
		}
	}
	return;
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []);
 *   
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"f:x",
		"file [file] [...]",
		"Qualcomm Atheros Parameter File XML Dump Utility",
		"f f\tobject definition file",
		"x\tprint XML schema on stdout and exit",
		(char const *)(0)
	};
	char const * schema = DATA_SCHEMA;
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'f':
			if (!freopen (optarg, "rb", stdin)) 
			{
				error (1, errno, "Can't open %s", optarg);
			}
			break;
		case 'x':
			xmlschema ();
			return (0);
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
	if (argc > 1) 
	{
		error (1, ENOTSUP, ERROR_TOOMANY);
	}

#if defined (WIN32)

	setmode (STDOUT_FILENO, O_BINARY);

#endif

	if ((argc) && (* argv)) 
	{
		signed fd;
		uint32_t version;
		if ((fd = open (*argv, O_BINARY | O_RDONLY)) == -1) 
		{
			error (1, errno, "%s", *argv);
		}
		else if (read (fd, &version, sizeof (version)) != sizeof (version)) 
		{
			error (1, errno, "Can't read %s", *argv);
		}
		else if (lseek (fd, 0, SEEK_SET)) 
		{
			error (1, errno, "Can't home %s", *argv);
		}
		else if (LE32TOH (version) == 0x60000000) 
		{
			error (1, errno, "Won't read %s", *argv);
		}
		else if (LE32TOH (version) == 0x00010001) 
		{
			struct nvm_header2 nvm_header;
			if (!nvmseek2 (fd, *argv, &nvm_header, NVM_IMAGE_PIB)) 
			{
				pibdump (fd, *argv, schema, LE32TOH (nvm_header.ImageLength), flags);
			}
		}
		else 
		{
			struct simple_pib pib_header;
			if (read (fd, &pib_header, sizeof (pib_header)) != sizeof (pib_header)) 
			{
				error (1, errno, "Can't read %s", *argv);
			}
			if (lseek (fd, 0, SEEK_SET)) 
			{
				error (1, errno, "Can't home %s", *argv);
			}
			pibdump (fd, *argv, schema, LE16TOH (pib_header.PIBLENGTH), flags);
		}
		close (fd);
	}
	return (0);
}

