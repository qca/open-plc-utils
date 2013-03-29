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
 *   rkey.c - random key generator;
 *
 *   generate random SHA256, device access, network membership and
 *   network identifier keys using a seed file;
 *
 *   read the seed file, increment the seed for each key generated
 *   then save the seed when done; exit the loop in an orderly way 
 *   on keyboard interrupt;          
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
 *   Contributor(s);
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#define _GETOPT_H

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../tools/flags.h"
#include "../key/HPAVKey.h"
#include "../key/SHA256.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/uintspec.c"
#include "../tools/todigit.c"
#include "../tools/strincr.c"
#include "../tools/hexout.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../key/HPAVKeyDAK.c"
#include "../key/HPAVKeyNMK.c"
#include "../key/HPAVKeyNID.c"
#include "../key/HPAVKeySHA.c"
#include "../key/HPAVKeyOut.c"
#include "../key/SHA256Reset.c"
#include "../key/SHA256Write.c"
#include "../key/SHA256Block.c"
#include "../key/SHA256Fetch.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define DEFAULT_LEVEL 0
#define DEFAULT_COUNT 1

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

static unsigned count = DEFAULT_COUNT;

/*====================================================================*
 *   
 *   void stop (signo_t signal);
 *   
 *   terminate the program; we want to ensure an organized program 
 *   exit such that the current pass phrase is saved;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

#if defined (__linux__)

static void stop (signo_t signal) 

{
	count = 0;
	return;
}


#endif

/*====================================================================*
 *   
 *   int main (int argc, const char * argv []);
 *
 *
 *
 *--------------------------------------------------------------------*/

int main (int argc, const char * argv []) 

{
	static const char * optv [] = 
	{
		"DL:MNn:oqv",
		"seedfile",
		"generate HomePlug AV compliant keys",
		"D\tDAK - Device Access Keys",
		"L n\tSecurity Level is n [" LITERAL (DEFAULT_LEVEL) "]",
		"M\tNMK - Network Membership Keys",
		"N\tNID - Network Identifier",
		"n n\tgenerate n keys [" LITERAL (DEFAULT_COUNT) "]",
		"o\tuse old seedfile value",
		"q\tquiet mode",
		"v\tverbose mode",
		(const char *)(0)
	};

#if defined (__linux__)

	struct sigaction sa;

#endif

	char phrase [HPAVKEY_PHRASE_MAX + 1];
	uint8_t digest [SHA256_DIGEST_LENGTH];
	unsigned level = DEFAULT_LEVEL;
	signed type = 0;
	signed next = 1;
	signed fd;
	flag_t flags = (flag_t)(0);
	signed c;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char)(c)) 
		{
		case 'D':
			type = HPAVKEY_DAK;
			break;
		case 'M':
			type = HPAVKEY_NMK;
			break;
		case 'N':
			type = HPAVKEY_NID;
			break;
		case 'n':
			count = (unsigned)(uintspec (optarg, 0, UINT_MAX));
			break;
		case 'L':
			level = (unsigned)(uintspec (optarg, 0, 1));
			break;
		case 'o':
			next = 0;
			break;
		case 'q':
			_setbits (flags, HPAVKEY_SILENCE);
			break;
		case 'v':
			_setbits (flags, HPAVKEY_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) 
	{
		error (1, ECANCELED, "No secret file given");
	}
	memset (phrase, 0, sizeof (phrase));
	if ((fd = open (* argv, O_BINARY|O_CREAT|O_RDWR, FILE_FILEMODE)) == -1) 
	{
		error (1, errno, "Can't open %s", * argv);
	}
	if (read (fd, phrase, sizeof (phrase) - 1) == -1) 
	{
		error (1, errno, "Can't read seedfile");
	}
	for (c = 0; c < (signed)(sizeof (phrase) - 1); c++) 
	{
		if (phrase [c] < HPAVKEY_CHAR_MIN) 
		{
			phrase [c] = HPAVKEY_CHAR_MIN;
			continue;
		}
		if (phrase [c] > HPAVKEY_CHAR_MAX) 
		{
			phrase [c] = HPAVKEY_CHAR_MAX;
			continue;
		}
	}

#if defined (__linux__)

	memset (&sa, 0, sizeof (struct sigaction));
	sa.sa_handler = stop;
	sigaction (SIGTERM, &sa, (struct sigaction *)(0));
	sigaction (SIGQUIT, &sa, (struct sigaction *)(0));
	sigaction (SIGTSTP, &sa, (struct sigaction *)(0));
	sigaction (SIGINT, &sa, (struct sigaction *)(0));
	sigaction (SIGHUP, &sa, (struct sigaction *)(0));

#endif

	while (count-- > 0) 
	{
		memset (digest, 0, sizeof (digest));
		if (next && strincr ((uint8_t *)(phrase), (size_t) (sizeof (phrase) - 1), HPAVKEY_CHAR_MIN, HPAVKEY_CHAR_MAX)) 
		{
			error (1, errno, "Can't increment seedfile");
		}
		if (type == HPAVKEY_DAK) 
		{
			HPAVKeyDAK (digest, phrase);
			HPAVKeyOut (digest, HPAVKEY_DAK_LEN, phrase, flags);
			continue;
		}
		if (type == HPAVKEY_NMK) 
		{
			HPAVKeyNMK (digest, phrase);
			HPAVKeyOut (digest, HPAVKEY_NMK_LEN, phrase, flags);
			continue;
		}
		if (type == HPAVKEY_NID) 
		{
			HPAVKeyNMK (digest, phrase);
			HPAVKeyNID (digest, digest, level);
			HPAVKeyOut (digest, HPAVKEY_NID_LEN, phrase, flags);
			continue;
		}
		HPAVKeySHA (digest, phrase);
		HPAVKeyOut (digest, HPAVKEY_SHA_LEN, phrase, flags);
	}
	if (lseek (fd, 0, SEEK_SET) == -1) 
	{
		error (1, errno, "Can't rewind seedfile");
	}
	if (write (fd, phrase, sizeof (phrase) - 1) == -1) 
	{
		error (1, errno, "Can't update seedfile");
	}
	close (fd);
	return (0);
}

