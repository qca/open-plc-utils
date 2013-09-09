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
 *   ttysig.c - Serial Line Signal Controller;
 *
 *   This program is for Linux only;
 *
 *   Contributor(s):
 *	Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   kernel header files;
 *--------------------------------------------------------------------*/

#include <sys/ioctl.h>

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/number.h"
#include "../tools/error.h"

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

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define SERIAL_PORT "/dev/ttyS0"
#define TTYSIG_SET_DTR       (1 << 0)
#define TTYSIG_SET_RTS       (1 << 1)
#define TTYSIG_READ_STATUS   (1 << 2)
#define TTYSIG_INTERACTIVE   (1 << 3)
#define TTYSIG_NOPROMPT      (1 << 4)

void set_status (int fd, int flag, int value)

{
	int status;
	if (ioctl (fd, TIOCMGET, &status) == -1) error (1, errno, "TIOCMGET failed");
	if (value) status |= flag;
	else status &= ~flag;
	if (ioctl (fd, TIOCMSET, &status) == -1) error (1, errno, "TIOCMSET failed");
}

void print_status (int fd)

{
	int status;
	if (ioctl (fd, TIOCMGET, &status) == -1) error (1, errno, "TIOCMGET failed");
	printf ("--> DTR: %s\n", (status & TIOCM_DTR)? "+V": "-V");
	printf ("--> RTS: %s\n", (status & TIOCM_RTS)? "+V": "-V");
	printf ("<-- CTS: %s\n", (status & TIOCM_CTS)? "+V": "-V");
	printf ("<-- DSR: %s\n", (status & TIOCM_DSR)? "+V": "-V");
	printf ("<-- DCD: %s\n", (status & TIOCM_CD)? "+V": "-V");
	printf ("<-- RI : %s\n", (status & TIOCM_RI)? "+V": "-V");
}

void comment (void)

{
	int c;
	while ((c = getchar ()) != EOF)
	{
		if (c == '\n')
		{
			ungetc (c, stdin);
			break;
		}
	}
}

int number (char *buf, int *val)

{
	char *p;
	while (isspace (*buf)) ++buf;
	if (!isdigit (*buf))
	{
		error (0, 0, "\"%s\" is not a number", buf);
		return (-1);
	}
	*val = atoi (buf);
	p = buf;
	while (isdigit (*buf)) ++buf;
	if (*buf != '\0')
	{
		error (0, 0, "\"%s\" is not a number", p);
		return (-1);
	}
	return (0);
}

void interactive (int fd, flag_t flags)

{
	char buf [32];
	int i,
	c;
	int value;
	char *p;
	for (; ; )
	{
		if (!_anyset (flags, TTYSIG_NOPROMPT))
		{
			printf ("command (D #, R #, e, r, s, q): ");
			fflush (stdout);
		}
		i = 0;
		while ((c = getchar ()) != EOF)
		{
			if (c == '#')
			{
				comment ();
				continue;
			}
			if (c == '\n') break;
			if (i == sizeof (buf) - 1) error (1, 0, "input too large");
			buf [i++] = c;
		}
		if (c == EOF) return;
		if (i == 0) continue;
		buf [i] = '\0';
		switch (buf [0])
		{
		case 'D':
			if (number (buf + 1, &value)) break;
			set_status (fd, TIOCM_DTR, value);
			break;
		case 'e':
			p = buf + 1;
			if (*p == ' ') ++p;
			printf ("%s\n", p);
			fflush (stdout);
			break;
		case 'R':
			if (number (buf + 1, &value)) break;
			set_status (fd, TIOCM_RTS, value);
			break;
		case 'r':
			print_status (fd);
			break;
		case 's':
			if (number (buf + 1, &value)) break;
			sleep (value);
			break;
		case 'q':
		case 'Q':
			return;
			break;
		case '\0':
			break;
		default:
			error (0, 0, "invalid command");
		}
	}
}

int main (int argc, char const ** argv)

{
	int fd;
	static char const * optv [] =
	{
		"s:D:IR:rqv",
		"[ttysig script filename]",
		"Serial Line Signal Controller",
		"D n\tset DTR (0 = -V, 1 = +V) at startup",
		"I\tInteractive mode",
		"R n\tset RTS (0 = -V, 1 = +V) at startup",
		"r\tread current RTS/DTR values at startup",
		"s f\tserial port is (f) [" SERIAL_PORT "]",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	signed c;
	optind = 1;
	flag_t flags = 0;
	uint8_t dtr_value;
	uint8_t rts_value;
	char *device = SERIAL_PORT;
	int input = -1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch ((char) (c))
		{
		case 'D':
			dtr_value = uintspec (optarg, 0, 1);
			_setbits (flags, TTYSIG_SET_DTR);
			break;
		case 'I':
			_setbits (flags, TTYSIG_INTERACTIVE);
			break;
		case 'R':
			rts_value = uintspec (optarg, 0, 1);
			_setbits (flags, TTYSIG_SET_RTS);
			break;
		case 'r':
			_setbits (flags, TTYSIG_READ_STATUS);
			break;
		case 's':
			device = optarg;
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc == 1)
	{
		input = open (* argv, O_RDONLY);
		if (input == -1) error (1, errno, "%s", * argv);
		if (dup2 (input, STDIN_FILENO) == -1) error (1, errno, "%s", * argv);
		_setbits (flags, TTYSIG_INTERACTIVE | TTYSIG_NOPROMPT);
	}
	else if (argc > 0) error (1, 0, "Invalid arguments");
	fd = open (device, O_RDONLY);
	if (fd == -1) error (1, errno, "could not open %s", device);
	if (_anyset (flags, TTYSIG_SET_DTR)) set_status (fd, TIOCM_DTR, dtr_value);
	if (_anyset (flags, TTYSIG_SET_RTS)) set_status (fd, TIOCM_RTS, rts_value);
	if (_anyset (flags, TTYSIG_READ_STATUS)) print_status (fd);
	if (_anyset (flags, TTYSIG_INTERACTIVE)) interactive (fd, flags);
	close (fd);
	return (0);
}

