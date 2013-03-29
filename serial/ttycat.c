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
 *   ttycat.c - serial port test program;
 *
 *   write one or more files to a serial device;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../serial/serial.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/efreopen.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *   program warnings;
 *--------------------------------------------------------------------*/

#if defined (WIN32)
#error "This program does not support Windows platforms"
#endif

/*====================================================================*
 *
 *   signed copy (signed ifd, signed ofd, void * memory, signed extent) 
 *
 *   copy ifd to ofd using a buffer of specified size;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

static signed copy (signed ifd, signed ofd, void * memory, signed extent) 

{
	while ((extent = read (ifd, memory, extent)) > 0) 
	{
		if (write (ofd, memory, extent) < extent) 
		{
			return (-1);
		}
	}
	return (0);
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"s:",
		PUTOPTV_S_FUNNEL,
		"copy one or more files to a  serial device",
		"s n\tline speed is (n) [115200]",
		(char const *)(0)
	};
	struct termios restore;
	struct termios current;
	speed_t speed = B115200;
	byte buffer [512];
	signed c;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 's':
			if (baudrate (uintspec (optarg, 0, UINT_MAX), &speed)) 
			{
				error (1, 0, "could not set baud rate");
			}
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!isatty (STDOUT_FILENO)) 
	{
		error (1, ENOTSUP, "stdout must be a serial line device");
	}
	tcflush (STDOUT_FILENO, TCIFLUSH);
	tcgetattr (STDOUT_FILENO, &restore);
	memset (&current, 0, sizeof (current));
	current.c_cflag = speed | CS8 | CLOCAL | CREAD;
	current.c_iflag = IGNPAR;
	current.c_oflag = 0;
	current.c_lflag = 0;
	current.c_cc [VTIME] = 0;
	current.c_cc [VMIN] = 5;
	tcsetattr (STDOUT_FILENO, TCSANOW, &current);
	if (!argc) 
	{
		copy (STDIN_FILENO, STDOUT_FILENO, buffer, sizeof (buffer));
	}
	while ((argc) && (* argv)) 
	{
		if (efreopen (* argv, "rb", stdin)) 
		{
			copy (STDIN_FILENO, STDOUT_FILENO, buffer, sizeof (buffer));
		}
		argc--;
		argv++;
	}
	tcsetattr (STDOUT_FILENO, TCSANOW, &restore);
	exit (0);
}

