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
 *   sdram.c - create INT6300 sdram configuration files;
 *
 *   sdram.h
 *
 *   this program writes two sdram configuration files in the current
 *   working directory; both files are for Linux toolkit programs and
 *   not the Windows Device Manager;
 *
 *   one of these sdram configuration blocks must be sent to the 
 *   INT6300 bootloader with VS_SET_SDRAM before downloading NVM 
 *   and PIB files;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#include "../tools/memory.h"
#include "../ram/sdram.h"

#ifndef MAKEFILE
#include "../tools/checksum32.c"
#endif

#define FILE1 "sdram16mb.cfg"
#define FILE2 "sdram64mb.cfg"

int main (int argc, char const * argv []) 

{
	const uint8_t sdram16mb [32] = 
	{
		0x00,
		0x00,
		0x00,
		0x01,
		0x68,
		0x2f,
		0x14,
		0x00,
		0x92,
		0xd4,
		0xe1,
		0x01,
		0xd6,
		0x83,
		0x08,
		0x00,
		0x88,
		0x32,
		0x00,
		0x00,
		0xdb,
		0x06,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};
	const uint8_t sdram64mb [32] = 
	{
		0x00,
		0x00,
		0x00,
		0x04,
		0x88,
		0x31,
		0x14,
		0x00,
		0x91,
		0xd4,
		0xe1,
		0x01,
		0xe3,
		0x2b,
		0x01,
		0x00,
		0x89,
		0x30,
		0x00,
		0x00,
		0x66,
		0x03,
		0x00,
		0x00,
		0x00,
		0x01,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};
	int fd;
	uint32_t checksum;
	if ((fd = open (FILE1, O_CREAT|O_WRONLY|O_TRUNC, 0444)) != -1) 
	{
		printf ("writing %s\n", FILE1);
		checksum = checksum32 (sdram16mb, sizeof (sdram16mb), 0);
		write (fd, sdram16mb, sizeof (sdram16mb));
		write (fd, &checksum, sizeof (checksum));
		close (fd);
	}
	if ((fd = open (FILE2, O_CREAT|O_WRONLY|O_TRUNC, 0444)) != -1) 
	{
		printf ("writing %s\n", FILE2);
		checksum = checksum32 (sdram64mb, sizeof (sdram64mb), 0);
		write (fd, sdram64mb, sizeof (sdram64mb));
		write (fd, &checksum, sizeof (checksum));
		close (fd);
	}
	return (0);
}

