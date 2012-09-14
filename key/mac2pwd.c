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

#define _GETOPT_H

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../key/keys.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/todigit.c"
#include "../tools/uintspec.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../key/MACPasswords.c"
#endif

/*====================================================================*
 *   program constants;  
 *--------------------------------------------------------------------*/

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6 
#endif

/*====================================================================*
 *
 *   void function (const char * file, unsigned count, unsigned bunch, flag_t flags)
 *
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

static void function (const char * file, unsigned count, unsigned group, unsigned space, flag_t flags) 

{
	unsigned line = 1;
	unsigned radix = 0x10;
	unsigned width;
	unsigned digit;
	signed c = getc (stdin);
	while (c != EOF) 
	{
		uint32_t vendor = 0;
		uint32_t device = 0;
		while (isspace (c)) 
		{
			if (c == '\n') 
			{
				line++;
			}
			c = getc (stdin);
		}
		if ((c == '#') || (c == ';')) 
		{
			do 
			{
				c = getc (stdin);
			}
			while ((c != '\n') && (c != EOF));
			continue;
		}
		for (width = 0; width < ETHER_ADDR_LEN; width++) 
		{
			if ((digit = todigit (c)) < radix) 
			{
				vendor *= radix;
				vendor += digit;
				c = getc (stdin);
				continue;
			}
			error (1, EINVAL, "%s: line %d: Illegal vendor", file, line);
		}
		if (!vendor) 
		{
			error (1, EPERM, "%s: line %d: Vendor can't be zero", file, line);
		}
		for (width = 0; width < ETHER_ADDR_LEN; width++) 
		{
			if ((digit = todigit (c)) < radix) 
			{
				device *= radix;
				device += digit;
				c = getc (stdin);
				continue;
			}
			error (1, EINVAL, "%s: line %d: Illegal device", file, line);
		}
		if (!device) 
		{
			error (1, EPERM, "%s: line %d: Device can't be zero", file, line);
		}
		while (isspace (c)) 
		{
			if (c == '\n') 
			{
				line++;
			}
			c = getc (stdin);
		}
		MACPasswords (vendor, device, 1, count, group, space, flags);
	}
	return;
}


/*====================================================================*
 *   
 *   int main (int argc, const char * argv []);
 *
 *   read one or more text files containing device address strings 
 *   and print a stream of address/password pairs; device addresses 
 *   must be separated by white space; 
 *
 *   This software and documentation is the property of Atheros 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Atheros assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Atheros customers may modify and distribute the software 
 *   without obligation to Atheros. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#define DEFAULT_COUNT 16
#define DEFAULT_GROUP 4

int main (int argc, const char * argv []) 

{
	static const char * optv [] = 
	{
		"b:l:qs:uv",
		PUTOPTV_S_FUNNEL,
		"Atheros device password generator",
		"b n\tbunching factor [" LITERAL (DEFAULT_GROUP) "]",
		"l n\tpassword letters [" LITERAL (DEFAULT_COUNT) "]",
		"q\tomit device address on output",
		"v\tprepend PTS flag on output",
		(const char *)(0)
	};
	unsigned count = DEFAULT_COUNT;
	unsigned group = DEFAULT_GROUP;
	unsigned space = '-';
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char)(c)) 
		{
		case 'b':
			group = (unsigned)(uintspec (optarg, 0, UCHAR_MAX));
			break;
		case 'l':
			count = (unsigned)(uintspec (optarg, 12, 64));
			break;
		case 'q':
			_setbits (flags, PASSWORD_SILENCE);
			break;
		case 'u':
		case 'v':
			_setbits (flags, PASSWORD_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc) 
	{
		function ("stdin", count, group, space, flags);
	}
	while ((argv) && (* argv)) 
	{
		if (!freopen (* argv, "rb", stdin)) 
		{
			error (1, EINVAL, "Can't open %s", * argv);
		}
		function (* argv, count, group, space, flags);
		argc--;
		argv++;
	}
	return (0);
}

