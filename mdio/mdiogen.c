/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted (subject to the limitations
 *   in the disclaimer below) provided that the following conditions
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 *   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE
 *   COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 *   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *--------------------------------------------------------------------*/

/*====================================================================*"
 *
 *   mdiogen.c -
 *
 *
 *   Contributor(s):
 *      Charles Maier
 *      Nathaniel Houghton
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/number.h"
#include "../tools/memory.h"
#include "../tools/endian.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../mdio/mdio.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/error.c"
#endif

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

const char * program_name;

/*====================================================================*
 *
 *   void regview (void const * memory, uint16_t offset, uint16_t length, FILE *fp);
 *
 *   memory.h
 *
 *   print memory as a binary dump showing absolute memory offsets and
 *   16-bit binary register maps; sample output looks like:
 *
 *      ADDR DATA 5432-1098-7654-3210
 *      0000 0000 0000-0000-0000-0000
 *      0002 0000 0000-0000-0000-0000
 *      0004 0000 0000-0000-0000-0000
 *      0008 0000 0000-0000-0000-0000
 *      0010 0000 0000-0000-0000-0000
 *
 *
 *--------------------------------------------------------------------*/

static void regview (void const * memory, uint16_t offset, uint16_t extent, FILE *fp)

{
	uint16_t * origin = (uint16_t *)(memory);
	printf ("ADDR DATA 5432-1098-7654-3210\n");
	while (extent >= sizeof (* origin))
	{
		signed bit = sizeof (* origin) << 3;
		printf ("%04u ", offset);
		printf ("%04X ", LE16TOH (*origin));
		while (bit--)
		{
			putc (((* origin >> bit) & 1)? '1': '0', fp);
			if ((bit) && !(bit%4))
			{
				putc ('-', fp);
			}
		}
		putc ('\n', fp);
		offset += sizeof (* origin);
		extent -= sizeof (* origin);
		origin++;
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, const char * argv []);
 *
 *   this is a basic program that produces an MDIO program block
 *   using macros defined in mdio.h; the output filename is fixed;
 *
 *   declare your MDIO program by editing array program [] based on
 *   the MDIO program rules defined in the Atheros PLC Firmware TRM
 *   under the description of the VS_MOD_OP;
 *
 *
 *--------------------------------------------------------------------*/

#define MDIO_PROGRAM "mdio.bin"

int main (int argc, const char * argv [])

{
	uint16_t program [] =
	{

#if 1

		MDIO16_START (1, 0, 9),
		MDIO16_INSTR (1, 1, 0x18, 0x00, 2),
		0x0000,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x10, 0x16, 2),
		0x003F,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x10, 0x17, 2),
		0x7E3F,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x18, 0x00, 2),
		0x0000,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x10, 0x02, 2),
		0x0700,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x10, 0x03, 2),
		0x0000,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x18, 0x00, 2),
		0x0000,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x14, 0x00, 2),
		0x007D,
		0xFFFF,
		MDIO16_INSTR (1, 1, 0x14, 0x01, 2),
		0x0000,
		0xFFFF,

#else

		MDIO16_START (1, 0, 24),
		MDIO32_INSTR (0x2c, 0x7e3f003f, 0xffffffff),
		MDIO32_INSTR (0x8, 0x500, 0xffffffff),
		MDIO32_INSTR (0x100, 0x7d, 0xffffffff),
		MDIO32_INSTR (0x30, 0x19f005f2, 0xffffffff),
		MDIO32_INSTR (0x200, 0x7d, 0xffffffff),
		MDIO32_INSTR (0x300, 0x7d, 0xffffffff),
		MDIO32_INSTR (0x400, 0x7d, 0xffffffff),
		MDIO32_INSTR (0x500, 0x7d, 0xffffffff),

#endif

	};
	signed fd;
	program_name = * argv;
	if (--argc)
	{
		error (1, ECANCELED, "Too many command line arguments");
	}
	regview (&program, 0, sizeof (program), stdout);
	if ((fd = open (MDIO_PROGRAM, O_CREAT|O_WRONLY|O_TRUNC, FILE_FILEMODE)) == -1)
	{
		error (1, errno, "Can't open %s", MDIO_PROGRAM);
	}
	if (write (fd, program, sizeof (program)) != sizeof (program))
	{
		unlink (MDIO_PROGRAM);
		error (1, errno, "Can't write %s", MDIO_PROGRAM);
	}
	if (sizeof (program) % sizeof (uint32_t))
	{
		uint32_t zeros = 0;
		ssize_t count = sizeof (uint32_t) - sizeof (program) % sizeof (uint32_t);
		if (write (fd, &zeros, count) != count)
		{
			unlink (MDIO_PROGRAM);
			error (1, errno, "Can't write %s", MDIO_PROGRAM);
		}
	}
	close (fd);
	return (0);
}

