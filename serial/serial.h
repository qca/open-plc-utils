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
 *   serial.h - Atheros Serial Line Definitions and declarations;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SERIAL_HEADER
#define SERIAL_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <ctype.h>

#ifndef WIN32
#include <termios.h>
#endif

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/types.h"

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#if defined (WIN32)
#       define DEVICE "com1:"
#       define PAUSE(x) Sleep (x)
#else
#       define DEVICE "/dev/ttyUSB0"
#       define PAUSE(x) usleep ((x)*1000)
#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#define UART_MODE 0
#define UART_BAUDRATE 115200
#define UART_DATABITS 8
#define UART_STOPBITS 1
#define UART_PARITY 0
#define UART_FLOWCTRL 0

#define UART_BUFFERSIZE 4096
#define UART_BLOCKSIZE 512
#define UART_PORT "PLCUART"

/*====================================================================*
 *   Atheros Serial Line Command flags;
 *--------------------------------------------------------------------*/

#define UART_SILENCE    (1 << 0)
#define UART_VERBOSE    (1 << 1)
#define UART_DEFAULT    (1 << 2)
#define UART_COMMAND    (1 << 3)
#define UART_WAKE       (1 << 4)
#define UART_RESPOND    (1 << 5)
#define UART_ATRV       (1 << 6)
#define UART_ATRPM      (1 << 7)
#define UART_ATSK1      (1 << 8)
#define UART_ATSK2      (1 << 9)
#define UART_ATZ        (1 << 10)
#define UART_ATDST1     (1 << 11)
#define UART_ATDST2     (1 << 12)
#define UART_ATNI       (1 << 13)
#define UART_ATFD       (1 << 14)
#define UART_ATPS       (1 << 15)
#define UART_ATO        (1 << 16)
#define UART_ATHSC      (1 << 17)
#define UART_ATRP       (1 << 18)
#define UART_ATWPF1     (1 << 19)
#define UART_ATWPF2     (1 << 20)
#define UART_ATWNV      (1 << 21)
#define UART_ATBSZ1     (1 << 22)
#define UART_ATBSZ2     (1 << 23)
#define UART_ATTO       (1 << 24)
#define UART_ATBR       (1 << 25)
#define UART_ATM        (1 << 26)

/*====================================================================*
 *
 *   struct command;
 *
 *   structure containing serial command buffer and two pointers; the
 *   buffer is used for both transmit and receive and shared by most
 *   function below;
 *
 *--------------------------------------------------------------------*/

typedef struct command

{
	char buffer [UART_BUFFERSIZE];
	unsigned length;
	unsigned offset;
}

COMMAND;

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#ifndef WIN32

signed baudrate (unsigned rate, speed_t * speed);

#endif

void openport (struct _file_ * port, flag_t mode);
void closeport (struct _file_ * port);

/*====================================================================*
 *   serial command line buffer functions;
 *--------------------------------------------------------------------*/

void clearcommand ();
void sendcommand (struct _file_ * port, flag_t flags);
void readcommand (struct _file_ * port, flag_t flags);
void insert (char c);
size_t readframe (signed fd, void * memory, size_t extent);
void decode (void const * memory, size_t extent);
void encode (void * memory, size_t extent);
void string (char * string);
uint64_t hextoint (unsigned bytes);
void mustbe (char c);;

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

