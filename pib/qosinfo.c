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
 *   qosinfo.c - print qos information;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
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
 *   program constants;
 *--------------------------------------------------------------------*/

#define TM_VERBOSE (1 << 0)
#define TM_SILENCE (1 << 1)

#define OFFSET 0x020C
#define LENGTH 0x0800

/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"",
		"file [file] [...] [> stdout]",
		"print qos information",
		(char const *) (0)
	};

#ifndef __GNUC__
#pragma pack (push, 1)
#endif

	struct __packed QoS 
	{
		uint8_t UniCastPriority;
		uint8_t McastPriority;
		uint8_t IGMPPriority;
		uint8_t AVStreamPriority;
		uint32_t PriorityTTL [4];
		uint8_t EnableVLANOver;
		uint8_t EnableTOSOver;
		uint16_t RSVD1;
		uint16_t VLANPrioMatrix;
		uint16_t TOSPrecMatrix;
		uint8_t NumberOfConfigEntries;
		struct 
		{
			uint8_t one;
			uint8_t two;
		}
		AggregateConfigEntries [8];
		uint8_t CustomAggregationParameters [384];
		uint8_t RSVD2 [1619];
	}
	QoS;

#ifndef __GNUC__
#pragma pack (pop)
#endif

	file_t fd;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argc) && (* argv)) 
	{
		if ((fd = open (* argv, O_BINARY|O_RDONLY)) == -1) 
		{
			error (0, errno, "Can't open %s", * argv);
		}
		else if (lseek (fd, OFFSET, SEEK_SET) != OFFSET) 
		{
			error (0, errno, "Can't seek %s", * argv);
			close (fd);
		}
		else if (read (fd, &QoS, sizeof (QoS)) != sizeof (QoS)) 
		{
			error (0, errno, "Can't read %s", * argv);
			close (fd);
		}
		else 
		{
			for (c = 0; c < 8; c++) 
			{
				unsigned VLAN = (QoS.VLANPrioMatrix >> (c * 2)) & 0x03;
				unsigned TOS = (QoS.TOSPrecMatrix >> (c * 2)) & 0x03;
				printf ("VLAN %d TOS %d\n", VLAN, TOS);
			}
			for (c = 0; c < 4; c++) 
			{
				printf ("TTL [%d]=%d\n", c, QoS.PriorityTTL [c]);
			}
			for (c = 0; c < 8; c++) 
			{
				printf ("AggregateConfigEntries [%d] %02x %02X\n", c, QoS.AggregateConfigEntries [c].one, QoS.AggregateConfigEntries [c].two);
			}
			printf ("QoS %d %04X\n", sizeof (QoS), sizeof (QoS));
			close (fd);
		}
		argc--;
		argv++;
	}
	return (0);
}

