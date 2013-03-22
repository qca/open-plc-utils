/*====================================================================*
 *   
 *   Copyright (c) 2011 by Qualcomm Atheros.
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
 *   pibcomp.c - Qualcomm Atheros Parameter Information Block Compare Utility
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *   Contributor(s):
 *      Nathan Houghton <nathan.houghton@qca.qualcomm.com>
 *	Charles Maier <charles.maier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/number.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/chars.h"
#include "../tools/sizes.h"
#include "../tools/files.h"
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
#include "../tools/hexoffset.c"
#include "../tools/error.c"
#include "../tools/checksum32.c"
#include "../tools/fdchecksum32.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmseek2.c"
#endif

/*====================================================================*
 *   
 *   void function (char const * filename [], flag_t flags);
 *   
 *   read object definitions from stdin and use them to compare two 
 *   files; dump only those objects that differ on stdout;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

static void function (char const * filename [], flag_t flags) 

{
	unsigned file;
	unsigned object = 0;
	unsigned lineno = 1;
	signed fd [2];
	signed length = 0;
	off_t origin [2];
	off_t offset [2];
	off_t extent [2];
	char memory [_ADDRSIZE+1];
	char symbol [_NAMESIZE];
	char string [_LINESIZE];
	char * sp;
	signed c;
	offset [0] = 0;
	offset [1] = 0;
	for (file = 0; file < SIZEOF (fd); file++) 
	{
		uint32_t version;
		if ((fd [file] = open (filename [file], O_BINARY|O_RDONLY)) == -1) 
		{
			error (1, errno, "%s", filename [file]);
		}
		if (read (fd [file], &version, sizeof (version)) != sizeof (version)) 
		{
			error (1, errno, FILE_CANTREAD, filename [file]);
		}
		if ((extent [file] = lseek (fd [file], 0, SEEK_END)) == (off_t)(-1)) 
		{
			error (1, 0, FILE_CANTSIZE, filename [file]);
		}
		if ((origin [file] = lseek (fd [file], 0, SEEK_SET))) 
		{
			error (1, errno, FILE_CANTHOME, filename [file]);
		}
		if (LE32TOH (version) == 0x60000000) 
		{
			error (1, ENOTSUP, "%s is not a PIB file", filename [file]);
		}
		if (LE32TOH (version) == 0x00010001) 
		{
			struct nvm_header2 nvm_header;
			if (nvmseek2 (fd [file], filename [file], &nvm_header, NVM_IMAGE_PIB)) 
			{
				error (1, ENOTSUP, "%s is not a PIB file", filename [file]);
			}
			origin [file] = lseek (fd [file], 0, SEEK_CUR);
		}
	}
	if (origin [0] != origin [1]) 
	{
		error (1, EINVAL, "PIBs have different offsets");
	}
	while ((c = getc (stdin)) != EOF) 
	{
		if ((c == '#') || (c == ';')) 
		{
			do 
			{
				c = getc (stdin);
			}
			while (nobreak (c));
			lineno++;
			continue;
		}
		if (isspace (c)) 
		{
			if (c == '\n') 
			{
				lineno++;
			}
			continue;
		}
		length = 0;
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
		while (isblank (c)) 
		{
			c = getc (stdin);
		}
		if (c == '[') 
		{
			*sp++ = (char)(c);
			c = getc (stdin);
			while (isblank (c)) 
			{
				c = getc (stdin);
			}
			while (isdigit (c)) 
			{
				*sp++ = (char)(c);
				c = getc (stdin);
			}
			while (isblank (c)) 
			{
				c = getc (stdin);
			}
			*sp = (char)(0);
			if (c != ']') 
			{
				error (1, EINVAL, "Have '%s' but need ']' on line %d", symbol, lineno);
			}
			*sp++ = (char)(c);
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

			char * buffer [2];
			buffer [0] = (char *)(emalloc (length));
			buffer [1] = (char *)(emalloc (length));

#else

			byte buffer [2][length];

#endif

			if ((read (fd [0], buffer [0], length) == length) && (read (fd [1], buffer [1], length) == length)) 
			{
				if (memcmp (buffer [0], buffer [1], length)) 
				{
					if (!object++) 
					{
						for (c = 0; c < _ADDRSIZE + 65; c++) 
						{
							putc ('-', stdout);
						}
						putc ('\n', stdout);
					}
					printf ("%s %d %s\n", hexoffset (memory, sizeof (memory), offset [0]), length, symbol);
					for (c = 0; c < _ADDRSIZE; c++) 
					{
						putc ('-', stdout);
					}
					printf (" %s\n", filename [0]);
					hexview (buffer [0], offset [0], length, stdout);
					for (c = 0; c < _ADDRSIZE; c++) 
					{
						putc ('-', stdout);
					}
					printf (" %s\n", filename [1]);
					hexview (buffer [1], offset [1], length, stdout);
					for (c = 0; c < _ADDRSIZE + 65; c++) 
					{
						putc ('-', stdout);
					}
					putc ('\n', stdout);
				}
			}

#if defined (WIN32)

			free (buffer [0]);
			free (buffer [1]);

#endif

		}
		offset [0] += length;
		offset [1] += length;
		lineno++;
	}
	if (_allclr (flags, PIB_SILENCE)) 
	{
		offset [0] += origin [0];
		offset [1] += origin [1];
		for (file = 0; file < SIZEOF (extent); file++) 
		{
			if (offset [file] < extent [file]) 
			{
				error (0, 0, "%s exceeds definition by " OFF_T_SPEC " bytes", filename [file], extent [file] - offset [file]);
			}
			if (offset [file] > extent [file]) 
			{
				error (0, 0, "definition exceeds %s by " OFF_T_SPEC " bytes", filename [file], offset [file] - extent [file]);
			}
		}
		if (extent [0] > extent [1]) 
		{
			error (0, 0, "%s exceeds %s by " OFF_T_SPEC " bytes", filename [0], filename [1], extent [0] - extent [1]);
		}
		if (extent [1] > extent [0]) 
		{
			error (0, 0, "%s exceeds %s by " OFF_T_SPEC " bytes", filename [1], filename [0], extent [1] - extent [0]);
		}
	}
	close (fd [0]);
	close (fd [1]);
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
		"f:qv",
		"file1 file2",
		"Qualcomm Atheros Parameter Information Block Compare Utility",
		"f f\tobject definition file",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *)(0)
	};
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
				error (1, errno, "%s", optarg);
			}
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
	if (argc != 2) 
	{
		error (1, 0, "Need two files to compare.");
	}
	function (argv, flags);
	return (0);
}

