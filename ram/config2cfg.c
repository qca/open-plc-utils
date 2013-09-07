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
 *   config2cfg.c - convert a .config file to a .cfg file;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#define _GETOPT_H

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/number.h"
#include "../tools/flags.h"
#include "../tools/memory.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../ram/sdram.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/error.c"
#include "../tools/checksum32.c"
#include "../tools/todigit.c"
#include "../tools/hexencode.c"
#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv [])

{
	static char const * optv [] =
	{
		"qv",
		"file [file] [...]",
		"convert ASCII SDRAM configuration files (DM) to binary (toolkit) format",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	struct config_ram config_ram;
	char string [(sizeof (config_ram) << 1) + 1];
	uint32_t checksum;
	flag_t flags = (flag_t)(0);
	signed state = 1;
	signed fd;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch ((char) (c))
		{
		case 'q':
			_setbits (flags, SDRAM_SILENCE);
			break;
		case 'v':
			_setbits (flags, SDRAM_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argc-- > 0) && (* argv != (char const *)(0)))
	{

#if 0

		char const * pathname;
		char const * filename;
		char const * extender;
		for (pathname = filename = * argv; *pathname; pathname++)
		{
			if ((*pathname == '/') || (*pathname == '\\'))
			{
				filename = pathname + 1;
			}
		}
		for (pathname = extender = filename; *pathname; pathname++)
		{
			if (*pathname == '.')
			{
				extender = pathname;
			}
		}
		if (extender == filename)
		{
			extender = pathname;
		}

#endif

		if ((fd = open (* argv, O_BINARY|O_RDONLY)) == -1)
		{
			error (0, errno, "can't open %s for input", * argv);
			state = 1;
		}
		else if (read (fd, &string, sizeof (string)) < (ssize_t) (sizeof (string) - 1))
		{
			error (0, errno, "can't read %s", * argv);
			state = 1;
		}
		else
		{
			close (fd);
			if (hexencode ((uint8_t *) (&config_ram), sizeof (config_ram), string) == sizeof (config_ram))
			{
				error (1, errno, "%s is suspect", * argv);
			}
			checksum = checksum32 (&config_ram, sizeof (config_ram), 0);
			if ((fd = open (* argv, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1)
			{
				error (1, errno, "can't open %s for output", * argv);
			}
			write (fd, &config_ram, sizeof (config_ram));
			write (fd, &checksum, sizeof (checksum));
		}
		close (fd);
		argv++;
	}
	return (state);
}

