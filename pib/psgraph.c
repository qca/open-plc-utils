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
 *   psgraph.c - print PIB prescaler graph;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <limits.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibscalers.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define PSGRAPH_VERBOSE (1 << 0)
#define PSGRAPH_SILENCE (1 << 1)

/*====================================================================*
 *
 *   void int6x00Prescalers (struct _file_ * pib, unsigned scale);
 *
 *
 *--------------------------------------------------------------------*/

static void int6x00Prescalers (struct _file_ * pib, unsigned scale)

{
	unsigned index = 0;
	if (lseek (pib->file, INT_PRESCALER_OFFSET, SEEK_SET) != INT_PRESCALER_OFFSET)
	{
		error (1, errno, "Can't seek %s", pib->name);
	}
	for (index = 0; index < INT_PRESCALER_LENGTH; index++)
	{
		uint32_t value;
		if (read (pib->file, &value, sizeof (value)) != sizeof (value))
		{
			error (1, errno, "can't read %s", pib->name);
		}
		printf (" %6.3f %04d %6d ", INDEX_TO_FREQ (index), index, value);
		while (value > scale)
		{
			printf ("#");
			value -= scale;
		}
		printf ("\n");
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv [])

{
	static char const * optv [] =
	{
		"s:",
		"file [> stdout]",
		"print PIB prescaler graph",
		"s n\tscale",
		(char const *) (0)
	};
	struct _file_ pib;
	unsigned count = 0;
	unsigned scale = 10;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch (c)
		{
		case 's':
			scale = (unsigned)(uintspec (optarg, 1, UCHAR_MAX));
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc > 1)
	{
		error (1, ENOTSUP, "Only one file is permitted");
	}
	if ((argc) && (* argv))
	{
		pib.name = * argv;
		if ((pib.file = open (pib.name, O_BINARY|O_RDONLY)) == -1)
		{
			error (1, errno, "Can't open %s", pib.name);
		}
		count = pibscalers (&pib);
		if (count == AMP_CARRIERS)
		{
			error (1, ENOTSUP, "AR7x00 PIB Format");
		}
		else if (count == INT_CARRIERS)
		{
			int6x00Prescalers (&pib, scale);
		}
		else
		{
			error (1, ENOTSUP, "Unknown PIB Format");
		}
		close (pib.file);
	}
	return (0);
}

