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

#define _GETOPT_H

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"
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
#include "../tools/hexout.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../key/HPAVKeyDAK.c"
#include "../key/HPAVKeyNMK.c"
#include "../key/HPAVKeyNID.c"
#include "../key/HPAVKeySHA.c"
#include "../key/HPAVKeyOut.c"
#include "../key/SHA256.c"
#endif

/*====================================================================*
 *
 *   void generate (signed type, signed level, flag_t flags);
 *
 *   read pass phrases from stdin, compute the digest for each and
 *   print both on stdout; ignore illegal pass phrases;
 *
 *   a pass phrase consists of consecutive ASCII characters in the
 *   range 0x20 through 0x7F; other characters are noise and serve
 *   to delimit the phrase; phrases less than HPAVKEY_PHRASE_MIN characters
 *   or more than HPAVKEY_PHRASE_MAX characters are also illegal;
 *
 *   effectively, each text line is a candidate phrase where spaces
 *   are legal and significant; tabs characters are illegal and act
 *   as line breaks;
 *
 *   detected errors are reported along with the input line number;
 *
 *
 *--------------------------------------------------------------------*/

void generate (signed class, signed level, flag_t flags)

{
	uint8_t digest [SHA256_DIGEST_LENGTH];
	char phrase [BUFSIZ];
	char * sp = phrase;
	unsigned line = 1;
	signed c = getc (stdin);
	while (c != EOF)
	{
		if (!isprint (c))
		{
			if (c == '\n')
			{
				line++;
			}
			c = getc (stdin);
			continue;
		}
		sp = phrase;
		while (isprint (c))
		{
			if ((sp - phrase) < (signed)(sizeof (phrase) - 1))
			{
				*sp++ = c;
			}
			c = getc (stdin);
		}
		if ((c != '\r') && (c != '\n') && (c != EOF))
		{
			error (0, ENOTSUP, "Illegal characters on line %d", line);
			continue;
		}
		*sp = (char)(0);
		if (_anyset (flags, HPAVKEY_ENFORCE))
		{
			if ((sp - phrase) < HPAVKEY_PHRASE_MIN)
			{
				error (0, ENOTSUP, "Less than %d characters on line %d", HPAVKEY_PHRASE_MIN, line);
				continue;
			}
			if ((sp - phrase) > HPAVKEY_PHRASE_MAX)
			{
				error (0, ENOTSUP, "More than %d characters on line %d", HPAVKEY_PHRASE_MAX, line);
				continue;
			}
		}
		if (class == HPAVKEY_DAK)
		{
			HPAVKeyDAK (digest, phrase);
			HPAVKeyOut (digest, HPAVKEY_DAK_LEN, phrase, flags);
			continue;
		}
		if (class == HPAVKEY_NMK)
		{
			HPAVKeyNMK (digest, phrase);
			HPAVKeyOut (digest, HPAVKEY_NMK_LEN, phrase, flags);
			continue;
		}
		if (class == HPAVKEY_NID)
		{
			HPAVKeyNMK (digest, phrase);
			HPAVKeyNID (digest, digest, level);
			HPAVKeyOut (digest, HPAVKEY_NID_LEN, phrase, flags);
			continue;
		}
		HPAVKeySHA (digest, phrase);
		HPAVKeyOut (digest, HPAVKEY_SHA_LEN, phrase, flags);
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, const char * argv []);
 *
 *
 *--------------------------------------------------------------------*/

#define DEFAULT_LEVEL 0

int main (int argc, const char * argv [])

{
	static const char * optv [] =
	{
		"DeL:MNqv",
		"file [file] [...]",
		"HomePlug AV key generator",
		"D\tconvert password to Device Access Key",
		"e\tenforce HomePlug AV password rules",
		"M\tconvert password to Network Membership Key",
		"N\tconvert password to Network Identification Key",
		"L n\tSecurity Level is (n) [" LITERAL (DEFAULT_LEVEL) "]",
		"q\tquiet mode",
		"v\tverbose mode",
		(const char *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed type = 0;
	signed level = DEFAULT_LEVEL;
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1)
	{
		switch ((char) (c))
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
		case 'L':
			level = (signed)(uintspec (optarg, 0, 1));
			break;
		case 'q':
			_setbits (flags, HPAVKEY_SILENCE);
			break;
		case 'v':
			_setbits (flags, HPAVKEY_VERBOSE);
			break;
		case 'e':
			_setbits (flags, HPAVKEY_ENFORCE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc)
	{
		generate (type, level, flags);
	}
	while ((argc) && (* argv))
	{
		if (!freopen (* argv, "r", stdin))
		{
			error (0, errno, "%s", * argv);
		}
		else
		{
			generate (type, level, flags);
		}
		argv++;
		argc--;
	}
	return (0);
}

