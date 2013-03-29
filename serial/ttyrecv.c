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
 *   ttyrecv.c 
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
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <sys/time.h>
#include <sys/types.h>

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/error.h"
#include "../tools/putoptv.h"
#include "../tools/getoptv.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../serial/serial.h"

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
#endif

#ifndef MAKEFILE
#include "../serial/baudrate.c"
#endif

/*====================================================================*
 *   program constants;   
 *--------------------------------------------------------------------*/

#define SERIAL_PORT "/dev/ttyS0"

#define TTYRECV_VERBOSE (1 << 0)

/*====================================================================*
 *
 *   double ttyrecv (int ifd, int ofd, size_t time, size_t chunk_size, flag_t flags) ;
 *
 *--------------------------------------------------------------------*/

static double ttyrecv (int ifd, int ofd, size_t time, size_t chunk_size, flag_t flags) 

{
	char *buf;
	char *p;
	ssize_t r;
	ssize_t w;
	size_t bytes_read;
	struct timeval tv_start,
	tv_now,
	tv_result;
	struct timeval tv_timeout;
	double bytes_sec;
	fd_set rfd;
	buf = malloc (chunk_size);
	if (buf == NULL) 
	{
		error (1, errno, "could not allocate memory");
	}
	tcflush (ifd, TCIFLUSH);
	FD_ZERO (&rfd);
	FD_SET (ifd, &rfd);
	tv_timeout.tv_sec = 5;
	tv_timeout.tv_usec = 0;
	if (select (ifd + 1, &rfd, NULL, NULL, &tv_timeout) != 1) 
	{
		error (1, errno, "timed out waiting for data");
	}
	if (gettimeofday (&tv_start, NULL) == -1) 
	{
		error (1, errno, "could not get time");
	}
	if (_anyset (flags, TTYRECV_VERBOSE)) 
	{
		fprintf (stderr, "Started receive timer.\n");
	}
	bytes_read = 0;
	do 
	{
		FD_ZERO (&rfd);
		FD_SET (ifd, &rfd);
		tv_timeout.tv_sec = 1;
		tv_timeout.tv_usec = 0;
		if (select (ifd + 1, &rfd, NULL, NULL, &tv_timeout) == 1) 
		{
			r = read (ifd, buf, chunk_size);
			if (r == -1) 
			{
				error (1, 0, "could not read");
			}
			bytes_read += r;
			if (ofd != -1) 
			{
				p = buf;
				while (r) 
				{
					w = write (ofd, p, r);
					if (w == -1) 
					{
						error (1, errno, "could not write");
					}
					p += w;
					r -= w;
				}
			}
		}
		if (gettimeofday (&tv_now, NULL) == -1) 
		{
			error (1, errno, "could not get time");
		}
		timersub (&tv_now, &tv_start, &tv_result);
	}
	while (tv_result.tv_sec < (signed)(time));
	bytes_sec = bytes_read / (tv_result.tv_sec + tv_result.tv_usec / 1000000.0);
	free (buf);
	return (bytes_sec * 8 / 1000.0);
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"cl:rs:t:qv",
		"",
		"Serial Line Rate Tester",
		"c\tconsume received data (do not output to stdout)",
		"l f\tserial port is (f) [" SERIAL_PORT "]",
		"r\tprint the receive data rate to stdout",
		"s n\tport speed [ 115200 ]",
		"t n\treceive for (n) seconds [ 10 ]",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	struct termios termios;
	char * line = SERIAL_PORT;
	double rate = 0;
	speed_t speed = B115200;
	size_t time = 10;
	size_t chunk_size = 256;
	flag_t flags = 0;
	signed consume = 0;
	signed rflag = 0;
	signed fd;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'c':
			consume = 1;
			break;
		case 'r':
			rflag = 1;
			break;
		case 'l':
			line = optarg;
			break;
		case 's':
			if (baudrate (uintspec (optarg, 0, UINT_MAX), &speed)) 
			{
				error (1, 0, "could not set baud rate");
			}
			break;
		case 't':
			time = uintspec (optarg, 0, SIZE_MAX);
			break;
		case 'v':
			_setbits (flags, TTYRECV_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	fd = open (line, O_RDWR | O_NONBLOCK);
	if (fd == -1) 
	{
		error (1, errno, "could not open %s", line);
	}
	if (tcgetattr (fd, &termios) == -1) 
	{
		error (1, errno, "could not get tty attributes");
	}
	cfmakeraw (&termios);
	if (cfsetspeed (&termios, speed) == -1) 
	{
		error (1, errno, "could not set tty speed");
	}
	if (tcsetattr (fd, TCSANOW, &termios) == -1) 
	{
		error (1, errno, "could not set tty attributes");
	}
	if (!consume) 
	{
		rate = ttyrecv (fd, STDOUT_FILENO, time, chunk_size, flags);
	}
	else 
	{
		rate = ttyrecv (fd, -1, time, chunk_size, flags);
	}
	if (rflag) 
	{
		fprintf (stderr, "%.02f Kbps\n", rate);
	}
	close (fd);
	return (0);
}

