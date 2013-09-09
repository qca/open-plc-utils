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
 *   void openport (struct _file_ * port, flag_t flags);
 *
 *   open the serial port named by port->name and set file descriptor
 *   port->file; datatype struct _file_ is define in tools/types.h;
 *
 *   this function no longer initializes port settings because there
 *   are too many differences in constants, variables and functions
 *   between Linux, OpenBSD, MacOSX and Windows to cleanly implement
 *   a single approach to serial port configuration; this means that
 *   users should manually configure a port and then leave it alone;
 *
 *   use stty on Linux systems and the Control Panel on Windows;
 *
 *   port configuration code for Linux and Windows can be enabled if
 *   needed by defining SERIAL_CONFIG at compile time; this will not
 *   restore original port setting when the port is closed;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *	Mathieu Olivari <mathieu@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef OPENPORT_SOURCE
#define OPENPORT_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#if defined (WIN32)
#	include <windows.h>
#elif defined (__linux__)
#	include <termios.h>
#elif defined (__APPLE__)
#	include <termios.h>
#	include <net/ethernet.h>
#elif defined (__OpenBSD__)
#	include <termios.h>
#else
#error "Unknown Environment"
#endif

#include "../tools/types.h"
#include "../tools/files.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../serial/serial.h"

void openport (struct _file_ * port, flag_t flags)

{

#if defined (WIN32)

	HANDLE hSerial;
	COMMTIMEOUTS timeouts;
	hSerial = CreateFile (port->name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		error (1, errno, "%s", port->name);
	}
	if (_anyset (flags, UART_DEFAULT))
	{
		DCB dcbSerial =
		{
			0
		};
		dcbSerial.DCBlength = sizeof (dcbSerial);
		if (!GetCommState (hSerial, &dcbSerial))
		{
			error (1, errno, "Can't read state: %s", port->name);
		}
		if (_anyset (flags, UART_VERBOSE))
		{
			printf ("getting %s ", port->name);
			printf ("Baud %6d ", dcbSerial.BaudRate);
			printf ("Data %d ", dcbSerial.ByteSize);
			printf ("Stop %d ", dcbSerial.StopBits);
			printf ("Parity %d\n", dcbSerial.Parity);
		}
		dcbSerial.BaudRate = CBR_115200;
		dcbSerial.ByteSize = DATABITS_8;
		dcbSerial.StopBits = ONESTOPBIT;
		dcbSerial.Parity = NOPARITY;
		if (_anyset (flags, UART_VERBOSE))
		{
			printf ("setting %s ", port->name);
			printf ("Baud %6d ", dcbSerial.BaudRate);
			printf ("Data %d ", dcbSerial.ByteSize);
			printf ("Stop %d ", dcbSerial.StopBits);
			printf ("Parity %d\n", dcbSerial.Parity);
		}
		if (!SetCommState (hSerial, &dcbSerial))
		{
			error (1, errno, "Can't save state: %s", port->name);
		}
	}
	timeouts.ReadIntervalTimeout = 10;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts (hSerial, &timeouts))
	{
		error (1, errno, "Can't set timeouts: %s", port->name);
	}
	CloseHandle (hSerial);
	if ((port->file = open (port->name, O_BINARY|O_RDWR)) == -1)
	{
		error (1, errno, "%s", port->name);
	}

#else

	if ((port->file = open (port->name, O_BINARY|O_RDWR)) == -1)
	{
		error (1, errno, "%s", port->name);
	}
	if (_anyset (flags, UART_DEFAULT))
	{
		struct termios termios;

#if 1

/*
 *	POSIX generic code;
 */

		tcgetattr (port->file, &termios);
		cfmakeraw (&termios);
		termios.c_cflag |= CS8;
		termios.c_cflag &= ~(CSTOPB);
		cfsetospeed (&termios, B115200);

#else

/*
 *	Linux specific code;
 */

		termios.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
		termios.c_iflag = IGNPAR;
		termios.c_oflag = 0;
		termios.c_lflag = 0;
		termios.c_cc [VTIME] = 0;
		termios.c_cc [VMIN] = 5;

#endif

		tcsetattr (port->file, TCSANOW, &termios);
	}

#endif

	return;
}


#endif

