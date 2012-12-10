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
 *   gpioinfo.c - print gpio Iinformation
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED.
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
#include "../tools/types.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/hexstring.c"
#include "../tools/hexdecode.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define TM_VERBOSE (1 << 0)
#define TM_SILENCE (1 << 1)

#define OFFSET 0x24BF
#define LENGTH 50

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
		"print GPIO information",
		(char const *) (0)
	};

#ifndef __GNUC__
#pragma pack (push, 1)
#endif

	typedef struct __packed EventBlock 
	{
		uint8_t EvtPriorityId;
		uint8_t EvtId;
		uint8_t BehId [3];
		uint16_t ParticipatingGPIOs;
		uint8_t EventAttributes;
		uint8_t RSVD [3];
	}
	EventBlock;

#ifndef __GNUC__
#pragma pack (pop)
#endif

	struct EventBlock EventBlockArray [50];
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
		else if (read (fd, &EventBlockArray, sizeof (EventBlockArray)) != sizeof (EventBlockArray)) 
		{
			error (0, errno, "Can't read %s", * argv);
			close (fd);
		}
		else 
		{
			for (c = 0; c < LENGTH; c++) 
			{
				struct EventBlock * EventBlock = (&EventBlockArray [c]);
				char string [10];
				printf ("EvtPriorityId %3d ", EventBlock->EvtPriorityId);
				printf ("EvtId %3d ", EventBlock->EvtId);
				printf ("BehId %s ", hexstring (string, sizeof (string), EventBlock->BehId, sizeof (EventBlock->BehId)));
				printf ("ParticipatingGPIOs %3d ", EventBlock->ParticipatingGPIOs);
				printf ("EventAttributes %3d ", EventBlock->EventAttributes);
				printf ("\n");
			}
			close (fd);
		}
		argc--;
		argv++;
	}
	return (0);
}

