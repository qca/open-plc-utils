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
 *   mme.c - Atheros vendor-specific message code and name printer;
 *
 *   print vendor-specific mesage codes and names and with associated
 *   error codes and error text on stdout in various formats; options 
 *   are HTML, CSV and plain text;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/number.h"
#include "../tools/format.h"
#include "../tools/error.h"
#include "../tools/flags.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/error.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/output.c"
#endif

#ifndef MAKEFILE
#include "../mme/MMEName.c"
#include "../mme/MMEMode.c"
#endif

#include "../mme/MMECode.c"

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define MME_VERBOSE	(1 << 0)
#define MME_SILENCE	(1 << 1)
#define MME_TOCSV	(1 << 2)
#define MME_TOHTML	(1 << 3)

/*====================================================================*
 *   
 *   int main (int argc, char * argv[]);
 *   
 *   print vendor-specific message codes and names with associated
 *   error codes and text on stdout; output options are HTML, CSV 
 *   and plain text;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

#define DEFAULT_COLUMN 40

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"cht:",
		PUTOPTV_S_DIVINE,
		"Atheros vendor-specific message enumerator",
		"c\toutput CSV table",
		"h\toutput HTML table",
		"t n\ttab to column (n) [" LITERAL (DEFAULT_COLUMN) "]",
		(char const *) (0)
	};
	signed column = DEFAULT_COLUMN;
	signed index;
	uint16_t type;
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'c':
			_setbits (flags, MME_TOCSV);
			break;
		case 'h':
			_setbits (flags, MME_TOHTML);
			break;
		case 't':
			column = (signed)(uintspec (optarg, 0, 255));
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc) 
	{
		error (1, ENOTSUP, ERROR_TOOMANY);
	}
	if (_anyset (flags, MME_TOHTML)) 
	{
		signed margin = 2;
		output (margin++, "<table class='mme'>");
		output (margin++, "<tr class='mme'>");
		output (margin, "<th class='type'>Type</th>");
		output (margin, "<th class='name'>Name</th>");
		output (margin, "<th class='code'>Code</th>");
		output (margin, "<th class='text'>Text</th>");
		output (margin--, "</tr>");
		for (index = 0; index < (signed)(SIZEOF (mme_codes)); index++) 
		{
			type = mme_codes [index].type;
			output (margin++, "<tr class='mme'>");
			output (margin, "<td class='type'>0x%04X</td>", type);
			output (margin, "<td class='name'>%s.%s</td>", MMEName (type), MMEMode (type));
			output (margin, "<td class='code'>0x%02X</td>", mme_codes [index].code);
			output (margin, "<td class='text'>%s</td>", mme_codes [index].text);
			output (margin--, "</tr>");
		}
		output (margin--, "</table>");
		return (0);
	}
	if (_anyset (flags, MME_TOCSV)) 
	{
		printf ("Name,Type,Code,Text\n");
		for (index = 0; index < (signed)(SIZEOF (mme_codes)); index++) 
		{
			type = mme_codes [index].type;
			printf ("0x%04X,", type);
			printf ("%s.%s,", MMEName (type), MMEMode (type));
			printf ("0x%02X,", mme_codes [index].code);
			printf ("\"%s\"\n", mme_codes [index].text);
		}
		return (0);
	}
	for (index = 0; index < (signed)(SIZEOF (mme_codes)); index++) 
	{
		signed indent = column;
		type = mme_codes [index].type;
		indent -= printf ("0x%04X ", type);
		indent -= printf ("%s.%s ", MMEName (type), MMEMode (type));
		while (indent-- > 0) 
		{
			putc (' ', stdout);
		}
		printf ("0x%02X ", mme_codes [index].code);
		printf ("\"%s\"\n", mme_codes [index].text);
	}
	return (0);
}

