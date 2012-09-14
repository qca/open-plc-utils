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
 *   serial.c - Atheros Serial Line Command Buffer Management;
 *
 *   serial.h
 *
 *   this module contains a serial line command buffer and functions
 *   to encode and decode it in different formats and send or receive
 *   it over the serial line;  
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SERIAL_SOURCE
#define SERIAL_SOURCE

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <errno.h>

#if defined (WIN32)
#include <Windows.h>
#endif

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../serial/serial.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"

/*====================================================================*
 *   private variables;
 *--------------------------------------------------------------------*/

struct command command;

/*====================================================================*
 *
 *   void clearcommand ();
 *
 *   serial.h
 *
 *
 *--------------------------------------------------------------------*/

void clearcommand () 

{
	extern struct command command;
	memset (&command, 0, sizeof (command));
	return;
}


/*====================================================================*
 *
 *   void sendcommand (struct _file_ * port, flag_t flags);
 *
 *   serial.h
 *
 *   echo then send the command;
 *
 *--------------------------------------------------------------------*/

void sendcommand (struct _file_ * port, flag_t flags) 

{
	extern struct command command;
	if (_anyset (flags, UART_VERBOSE)) 
	{
		write (STDERR_FILENO, command.buffer, command.length);
		write (STDERR_FILENO, "\n", sizeof (char));
	}
	if (write (port->file, command.buffer, command.length) != (signed)(command.length)) 
	{
		error (1, errno, "Can't write to %s", port->name);
	}
	clearcommand ();
	return;
}


/*====================================================================*
 *
 *   void readcommand (struct _file_ * port, flag_t flags);
 *
 *   serial.h
 *
 *   read response serial line and log the response;
 *
 *--------------------------------------------------------------------*/

void readcommand (struct _file_ * port, flag_t flags) 

{
	extern struct command command;

#if defined (WIN32) 

	PAUSE (250);
	memset (&command, 0, sizeof (command));
	command.length = read (port->file, command.buffer, sizeof (command.buffer));
	if (command.length < 0) 
	{
		error (1, errno, "Bad response from %s", port->name);
	}
	if (command.length == 0) 
	{
		error (1, errno, "No response from %s", port->name);
	}

#else

	struct timeval tv;
	fd_set rfd;
	ssize_t tmp;
	memset (&command, 0, sizeof (command));
	while (!strchr (command.buffer, '\r')) 
	{
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO (&rfd);
		FD_SET (port->file, &rfd);
		if (select (port->file + 1, &rfd, NULL, NULL, &tv) != 1) 
		{
			error (1, errno, "Read timeout");
		}
		tmp = read (port->file, command.buffer + command.length, sizeof (command.buffer) - command.length - 1);
		if (tmp < 0) 
		{
			error (1, errno, "Could not read %s", port->name);
		}
		command.length += tmp;
		command.buffer [command.length] = '\0';
	}

#endif

	if (_anyset (flags, UART_VERBOSE)) 
	{
		write (STDERR_FILENO, command.buffer, command.length);
		write (STDERR_FILENO, "\n", sizeof (char));
	}
	if (!memcmp (command.buffer, "ERROR", 5)) 
	{
		error (1, ECANCELED, "Device refused request");
	}
	return;
}


/*====================================================================*
 *
 *   void insert (char c);
 *
 *   serial.h
 *
 *   insert a character into the command buffer at the current buffer
 *   position then increment the buffer position pointer;
 *
 *--------------------------------------------------------------------*/

void insert (char c) 

{
	extern struct command command;
	if (command.length < sizeof (command.buffer)) 
	{
		command.buffer [command.length++] = c;
	}
	return;
}


/*====================================================================*
 *
 *   unsigned readframe (signed fd, void * memory, size_t extent);
 *
 *   serial.h
 *
 *   read a file and convert hexadecimal octets to binary bytes then 
 *   store them in consecutive memory locations up to a given length; 
 *   return the actual number of bytes stored;
 *
 *   digits may be consecutive or separated by white space consisting 
 *   of spaces, tabs, linefeeds, carriage returns, formfeeds or other
 *   characters such as punctuation; script-style comments are treated 
 *   as white space;
 *
 *--------------------------------------------------------------------*/

static signed fdgetc (signed fd) 

{
	char c;
	return ((read (fd, &c, sizeof (c)) == sizeof (c))? c: EOF);
}

size_t readframe (signed fd, void * memory, size_t extent) 

{
	unsigned digits = 0;
	uint8_t * origin = (uint8_t *)(memory);
	uint8_t * offset = (uint8_t *)(memory);
	signed c = EOF;
	while ((extent) && ((c = fdgetc (fd)) != EOF) && (c != ';')) 
	{
		if (isspace (c)) 
		{
			continue;
		}
		if (c == '#') 
		{
			do 
			{
				c = fdgetc (fd);
			}
			while ((c != '\n') && (c != EOF));
			continue;
		}
		if (c == '/') 
		{
			c = fdgetc (fd);
			if (c == '/') 
			{
				do 
				{
					c = fdgetc (fd);
				}
				while ((c != '\n') && (c != EOF));
				continue;
			}
			if (c == '*') 
			{
				while ((c != '/') && (c != EOF)) 
				{
					while ((c != '*') && (c != EOF)) 
					{
						c = fdgetc (fd);
					}
					c = fdgetc (fd);
				}
				continue;
			}
			continue;
		}
		if (isxdigit (c)) 
		{
			*offset = c;
			offset++;
			digits++;
			extent--;
			continue;
		}
		error (1, ENOTSUP, "Illegal hex digit '%c' (0x%02X) in source", c, c);
	}
	if (digits & 1) 
	{
		error (1, ENOTSUP, "Odd number of hex digits (%d) in source", digits);
	}
	return (offset - origin);
}


/*====================================================================*
 *
 *   void decode (void const * memory, size_t extent);
 *   
 *   serial.h
 *
 *   copy a memory region into command buffer at the current position
 *   and increment the buffer position pointer; convert bytes to hex 
 *   octets;
 *
 *--------------------------------------------------------------------*/

void decode (void const * memory, size_t extent) 

{
	extern struct command command;
	register byte * binary = (byte *)(memory);
	while ((command.length < sizeof (command.buffer)) && (extent--)) 
	{
		insert (DIGITS_HEX [(*binary >> 4) & 0x0F]);
		insert (DIGITS_HEX [(*binary >> 0) & 0x0F]);
		binary++;
	}
	return;
}


/*====================================================================*
 *
 *   void encode (void * memory, size_t extent);
 *
 *   serial.h
 *
 *   encode a memory region from the current command buffer position
 *   and increment the command buffer position pointer;
 *
 *--------------------------------------------------------------------*/

void encode (void * memory, size_t extent) 

{
	extern struct command command;
	register byte * binary = (byte *)(memory);
	unsigned digit;
	while ((command.offset < command.length) && (extent--)) 
	{
		*binary = 0;
		if ((digit = todigit (command.buffer [command.offset++])) > 0x0F) 
		{
			command.buffer [command.offset] = (char)(0);
			error (1, EINVAL, "[%s]1", command.buffer);
		}
		*binary |= digit << 4;
		if ((digit = todigit (command.buffer [command.offset++])) > 0x0F) 
		{
			command.buffer [command.offset] = (char)(0);
			error (1, EINVAL, "[%s]2", command.buffer);
		}
		*binary |= digit;
		binary++;
	}
	return;
}


/*====================================================================*
 *
 *   void string (char * string);
 *
 *   serial.h
 *
 *   extract the contents of a quoted string string from the command 
 *   buffer; it assumes that the current char is a quote character;
 *
 *   copy command buffer characters to an external string; start a the
 *   current buffer position and continue until the buffer exhausts or 
 *   a closing quote is encountered; NUL terminate the string; 
 *
 *--------------------------------------------------------------------*/

void string (char * string) 

{
	extern struct command command;
	while ((command.offset < command.length) && (command.buffer [command.offset] != '\"')) 
	{
		*string++ = command.buffer [command.offset++];
	}
	*string = (char)(0);
	return;
}


/*====================================================================*
 *
 *   uint64_t hextoint (unsigned bytes);
 *
 *   serial.h
 *
 *   this function is used to extract a hexadecimal integer string as
 *   an integer of specified length; an error occurs of the string is
 *   to long for the specified integer size in bytes;
 *
 *--------------------------------------------------------------------*/

uint64_t hextoint (unsigned bytes) 

{
	extern struct command command;
	uint64_t limit = -1;
	uint64_t value = 0;
	unsigned radix = 16;
	unsigned digit = 0;
	if (bytes < sizeof (limit)) 
	{
		limit <<= (bytes << 3);
		limit = ~limit;
	}
	while ((digit = todigit (command.buffer [command.offset])) < radix) 
	{
		value *= radix;
		value += digit;
		command.offset++;
		if (value > limit) 
		{
			command.buffer [command.offset] = (char)(0);
			error (1, EINVAL, "[%s] exceeds %d bits", command.buffer, (bytes << 3));
		}
	}
	return (value);
}


/*====================================================================*
 *
 *   void mustbe (char c);
 *
 *   serial.h
 *
 *   test the character at the current buffer position; advance the
 *   buffer position pointer and return true on match; terminate the
 *   program on mismatch or exhausted buffer;
 *
 *--------------------------------------------------------------------*/

void mustbe (char c) 

{
	extern struct command command;
	if (command.offset >= command.length) 
	{
		command.buffer [command.offset] = (char)(0);
		error (1, EINVAL, "[%s]: overflow", command.buffer);
	}
	if (command.buffer [command.offset++] != (c)) 
	{
		command.buffer [command.offset] = (char)(0);
		error (1, EINVAL, "[%s]: expecting 0x%02X", command.buffer, c);
	}
	return;
}


/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

