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
 *   psfp.c - prescaler finger print; 
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../tools/flags.h"
#include "../key/HPAVKey.h"
#include "../key/SHA256.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/fdchecksum32.c"
#include "../tools/hexdecode.c"
#include "../tools/hexstring.c"
#include "../tools/hexout.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibfile1.c"
#endif

#ifndef MAKEFILE
#include "../key/SHA256Reset.c"
#include "../key/SHA256Write.c"
#include "../key/SHA256Block.c"
#include "../key/SHA256Fetch.c"
#include "../key/keys.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define PSKEY_VERBOSE (1 << 0)
#define PSKEY_SILENCE (1 << 1)

/*====================================================================*
 *
 *   static signed pskey (struct _file_ * pib);
 *
 *   compute the SHA256 digest of the PIB file prescalers and print 
 *   the key with optional filename on stdout; the digest acts like 
 *   a fingerprint;
 *
 *   assume that offset and extent are for ar7400 and change to 6400
 *   offset and extent when the PIB requires it;
 *
 *
 *--------------------------------------------------------------------*/

static signed pskey (struct _file_ * pib, off_t offset, void * memory, ssize_t extent, flag_t flags) 

{
	struct sha256 sha256;
	byte digest [SHA256_DIGEST_LENGTH];
	struct pib_header pib_header;
	if (read (pib->file, &pib_header, sizeof (pib_header)) != sizeof (pib_header)) 
	{
		return (-1);
	}
	if (pib_header.FWVERSION < 0x05) 
	{
		offset = INT_PRESCALER_OFFSET;
		extent = INT_PRESCALER_LENGTH;
	}
	if (lseek (pib->file, offset, SEEK_SET) != offset) 
	{
		error (0, errno, FILE_CANTSEEK, pib->name);
		return (-1);
	}
	if (read (pib->file, memory, extent) != extent) 
	{
		error (0, errno, FILE_CANTREAD, pib->name);
		return (-1);
	}
	SHA256Reset (&sha256);
	SHA256Write (&sha256, memory, extent);
	SHA256Fetch (&sha256, digest);
	hexout (digest, sizeof (digest), '\0', 0, stdout);
	if (_allclr (flags, PSKEY_SILENCE)) 
	{
		printf (" %s", pib->name);
	}
	printf ("\n");
	return (0);
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []) 
 *   
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"lqrsv",
		"file [file] [...]",
		"print prescaler finger-print on stdout",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	struct _file_ pib;
	uint8_t buffer [INT_PRESCALER_LENGTH];
	signed state = 0;
	flag_t flags = (flag_t) (0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'q':
			_setbits (flags, PSKEY_SILENCE);
			break;
		case 'v':
			_setbits (flags, PSKEY_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argc) && (* argv)) 
	{
		pib.name = * argv;
		if ((pib.file = open (pib.name, O_BINARY|O_RDONLY)) == -1) 
		{
			error (0, errno, "Can't open: %s", pib.name);
			state = 1;
			errno = 0;
		}
		else if (pibfile1 (&pib)) 
		{
			error (0, errno, "Bad PIB: %s", pib.name);
			state = 1;
		}
		else if (pskey (&pib, INT_PRESCALER_OFFSET, buffer, sizeof (buffer), flags)) 
		{
			state = 1;
		}
		close (pib.file);
		argc--;
		argv++;
	}
	return (state);
}

