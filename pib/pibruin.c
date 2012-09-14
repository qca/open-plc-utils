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
 *   pibruin.c - Atheros Classification Rule Insert Utility;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#define _GETOPT_H

/*====================================================================*"
 *   system header files;
 *--------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/symbol.h"
#include "../tools/chars.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/rules.h"
#include "../nvm/nvm.h"
#include "../pib/pib.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/lookup.c"
#include "../tools/reword.c"
#include "../tools/hexstring.c"
#include "../tools/hexdecode.c"
#include "../tools/fdchecksum32.c"
#include "../tools/checksum32.c"
#include "../tools/memout.c"
#include "../tools/assist.c"
#include "../tools/bytespec.c"
#include "../tools/basespec.c"
#include "../tools/ipv4spec.c"
#include "../tools/ipv6spec.c"
#include "../tools/endian.c"
#include "../tools/emalloc.c"
#include "../tools/todigit.c"
#include "../tools/codelist.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../pib/pibfile1.c"
#include "../pib/piblock.c"
#endif

#ifndef MAKEFILE
#include "../nvm/nvmseek2.c"
#endif

#ifndef MAKEFILE
#include "../plc/rules.c"
#include "../plc/ParseRule.c"
#endif

/*====================================================================*
 *   program constants;
 *--------------------------------------------------------------------*/

#define PIB_MAX_AUTOCONN 16
#define PIB_MAX_PRIORITY_MAPS 8

#define PIB_OFFSET1 0x0228
#define PIB_OFFSET2 0x0760

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed classifier_pib 

{
	uint32_t CR_PID;
	uint32_t CR_OPERAND;
	uint8_t CR_VALUE [16];
}

classifier_pib;
typedef struct __packed auto_connection 

{
	uint8_t MACTION;
	uint8_t MOPERAND;
	uint16_t NUM_CLASSIFIERS;
	struct classifier_pib CLASSIFIER [3];
	struct cspec cspec;
	uint8_t RSVD [14];
}

auto_connection;
typedef struct __packed classifier_priority_map 

{
	uint32_t Priority;
	struct classifier_pib CLASSIFIER;
}

classifier_priority_map;
typedef struct __packed classifiers 

{
	uint32_t priority_count;
	uint32_t autoconn_count;
	struct classifier_priority_map classifier_priority_map [PIB_MAX_PRIORITY_MAPS];
	struct auto_connection auto_connection [PIB_MAX_AUTOCONN];
}

classifiers;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *
 *   signed pibimage1 (signed fd, char const * filename, unsigned offset, struct classifiers * classifiers);
 * 
 *   read an entire flat parameter file into memory, edit it, save 
 *   it and display it;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage1 (signed fd, char const * filename, unsigned offset, struct classifiers * classifiers) 

{
	struct pib_header * pib_header;
	off_t extent;
	void * memory;
	if ((extent = lseek (fd, 0, SEEK_END)) == -1) 
	{
		error (1, errno, FILE_CANTSIZE, filename);
	}
	if (!(memory = malloc (extent))) 
	{
		error (1, errno, FILE_CANTLOAD, filename);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (read (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	memcpy ((byte *)(memory) + offset, classifiers, sizeof (*classifiers));
	pib_header = (struct pib_header *)(memory);
	pib_header->CHECKSUM = checksum32 (memory, extent, pib_header->CHECKSUM);
	if (write (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTSAVE, filename);
	}
	if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	free (memory);
	close (fd);
	return (0);
}


/*====================================================================*
 *
 *   signed pibimage2 (signed fd, char const * filename, struct nvm_header2 * nvm_header, unsigned offset, struct classifiers * classifiers);
 * 
 *   read an entire flat parameter file into memory, edit it, save 
 *   it and display it;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static signed pibimage2 (signed fd, char const * filename, struct nvm_header2 * nvm_header, unsigned offset, struct classifiers * classifiers) 

{
	void * memory;
	off_t extent = LE32TOH (nvm_header->ImageLength);
	if (!(memory = malloc (extent))) 
	{
		error (1, errno, FILE_CANTLOAD, filename);
	}
	if (read (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	memcpy ((byte *)(memory) + offset, classifiers, sizeof (*classifiers));
	nvm_header->ImageChecksum = checksum32 (memory, extent, 0);
	if (write (fd, memory, extent) != extent) 
	{
		error (1, errno, FILE_CANTSAVE, filename);
	}
	if (lseek (fd, (off_t)(0) - extent, SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	nvm_header->HeaderChecksum = checksum32 (nvm_header, sizeof (* nvm_header), nvm_header->HeaderChecksum);
	if (lseek (fd, (off_t)(0) - sizeof (* nvm_header), SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	if (write (fd, nvm_header, sizeof (* nvm_header)) != sizeof (* nvm_header)) 
	{
		error (1, errno, FILE_CANTSAVE, filename);
	}
	if (lseek (fd, (off_t)(0) - sizeof (* nvm_header), SEEK_CUR) == -1) 
	{
		error (1, errno, FILE_CANTHOME, filename);
	}
	free (memory);
	return (0);
}

/*====================================================================*
 *
 *   void make_rule (int argc, char const * argv [], struct classifiers * classifiers);
 *
 *   walk argv [] and populate classifiers according to QCA PLC
 *   classifier rules; see programmers guide on VS_CLASSIFICATION
 *   more inforamtion about classification rules; module ParseRule
 *   used here is used in other programs.
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

static void make_rule (int argc, char const * argv [], struct classifiers * classifiers) 

{
	static char const * optv [] = 
	{
		"T:V:",
		"",
		"Atheros Classification Rule Insert Utility",
		"T\ttag",
		"V\tversion",
		(char const *) (0)
	};
	struct rule rule;
	struct cspec cspec;
	signed c;
	optind = 1;
	memset (&rule, 0, sizeof (rule));
	memset (&cspec, 0, sizeof (cspec));
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'T':
			cspec.VLAN_TAG = (uint32_t)(basespec (optarg, 16, sizeof (cspec.VLAN_TAG)));
			cspec.VLAN_TAG = htonl (cspec.VLAN_TAG);
			break;
		case 'V':
			cspec.CSPEC_VERSION = (uint16_t)(basespec (optarg, 10, sizeof (cspec.CSPEC_VERSION)));
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if ((argc) && (*argv)) 
	{
		ParseRule (&argc, &argv, &rule, &cspec);
		if ((rule.NUM_CLASSIFIERS > 1) || (rule.MACTION == ACTION_STRIPTX) || (rule.MACTION == ACTION_STRIPRX) || (rule.MACTION == ACTION_TAGTX) || (classifiers->priority_count >= PIB_MAX_PRIORITY_MAPS)) 
		{
			struct auto_connection * auto_connection;
			if (classifiers->autoconn_count >= PIB_MAX_AUTOCONN) 
			{
				error (1, ENOTSUP, "Too many auto connection rules");
			}
			auto_connection = &classifiers->auto_connection [classifiers->autoconn_count];
			auto_connection->CLASSIFIER [0].CR_PID = 0xFF;
			auto_connection->CLASSIFIER [1].CR_PID = 0xFF;
			auto_connection->CLASSIFIER [2].CR_PID = 0xFF;
			auto_connection->MACTION = rule.MACTION;
			auto_connection->MOPERAND = rule.MOPERAND;
			auto_connection->NUM_CLASSIFIERS = rule.NUM_CLASSIFIERS;
			for (c = 0; c < rule.NUM_CLASSIFIERS; ++c) 
			{
				auto_connection->CLASSIFIER [c].CR_PID = rule.CLASSIFIER [c].CR_PID;
				auto_connection->CLASSIFIER [c].CR_OPERAND = rule.CLASSIFIER [c].CR_OPERAND;
				memcpy (&auto_connection->CLASSIFIER [c].CR_VALUE, &rule.CLASSIFIER [c].CR_VALUE, sizeof (auto_connection->CLASSIFIER [c].CR_VALUE));
			}
			memcpy (&auto_connection->cspec, &rule.cspec, sizeof (auto_connection->cspec));
			classifiers->autoconn_count++;
		}
		else 
		{
			struct classifier_priority_map * classifier_priority_map;
			if (classifiers->priority_count >= PIB_MAX_PRIORITY_MAPS) 
			{
				error (1, ENOTSUP, "Too many priority map rules");
			}
			classifier_priority_map = &classifiers->classifier_priority_map [classifiers->priority_count];
			classifier_priority_map->Priority = rule.MACTION;
			classifier_priority_map->CLASSIFIER.CR_PID = rule.CLASSIFIER [0].CR_PID;
			classifier_priority_map->CLASSIFIER.CR_OPERAND = rule.CLASSIFIER [0].CR_OPERAND;
			memcpy (&classifier_priority_map->CLASSIFIER.CR_VALUE, &rule.CLASSIFIER [0].CR_VALUE, sizeof (classifier_priority_map->CLASSIFIER.CR_VALUE));
			classifiers->priority_count++;
		}
	}
	return;
}


/*====================================================================*
 *
 *   size_t getargs (char const * vector [], size_t length)
 *
 *   read one line from stdin; fill vector with fields from that
 *   line; return the number of fields found; ignore blank lines 
 *   and script style comment lines; this implementation inserts
 *   a program name at vector [0] to look like a true argv [];
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

static size_t getargs (char const * vector [], size_t length) 

{
	extern char const * program_name;
	char const ** vp = vector;
	char string [1024];
	char * sp = string;
	signed c = getc (stdin);
	memset (vector, 0, length * sizeof (char const *));
	while (nobreak (c)) 
	{
		if (isspace (c)) 
		{
			do 
			{
				c = getc (stdin);
			}
			while (isspace (c));
		}
		if (c == '#') 
		{
			do 
			{
				c = getc (stdin);
			}
			while (nobreak (c));
			c = getc (stdin);
			continue;
		}
		*vp++ = program_name;
		*vp++ = sp = string;
		while (nobreak (c)) 
		{
			if (c == '#') 
			{
				do 
				{
					c = getc (stdin);
				}
				while (nobreak (c));
				break;
			}
			if (isblank (c)) 
			{
				c = (char)(0);
				*vp = sp + 1;
			}
			else if (sp == *vp) 
			{
				if ((size_t)(vp - vector) < length)
				{
					vp++;
				}
			}
			*sp++ = c;
			c = getc (stdin);
		}
		*vp = (char const *)(0);
		*sp = (char)(0);
	}
	return ((size_t)(vp - vector));
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv[]);
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	extern char const * program_name;
	static char const * optv [] = 
	{
		"eo:qv",
		"pibfile < rules",
		"Atheros Classification Rule Insert Utility",
		"e\tredirect stderr messages to stdout",
		"o x\talternate offset is (x)",
		"q\tquiet mode",
		"v\tverbose mode",
		(char const *) (0)
	};
	struct classifiers classifiers;
	char const * vector [24];
	uint32_t offset;
	uint32_t version;
	signed status;
	signed fd;
	size_t count;
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'e':
			dup2 (STDOUT_FILENO, STDERR_FILENO);
			break;
		case 'o':
			offset = (unsigned) (basespec (optarg, 16, sizeof (offset)));
			break;
		case 'q':
			_setbits (flags, PIB_SILENCE);
			break;
		case 'v':
			_setbits (flags, PIB_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	memset (&classifiers, 0, sizeof (classifiers));
	while ((count = getargs (vector, SIZEOF (vector)))) 
	{
		make_rule (count, vector, &classifiers);
	}
	while ((argc) && (*argv)) 
	{
		status = 0;
		if ((fd = open (*argv, O_BINARY|O_RDWR)) == -1) 
		{
			error (0, errno, "%s", *argv);
			status = 1;
		}
		else if (read (fd, &version, sizeof (version)) != sizeof (version)) 
		{
			error (0, errno, FILE_CANTREAD, *argv);
			status = 1;
		}
		else if (lseek (fd, 0, SEEK_SET)) 
		{
			error (0, errno, FILE_CANTHOME, *argv);
			status = 1;
		}
		else if (LE32TOH (version) == 0x00010001) 
		{
			struct nvm_header2 nvm_header;
			if (!nvmseek2 (fd, *argv, &nvm_header, NVM_IMAGE_PIB)) 
			{
				status = pibimage2 (fd, *argv, &nvm_header, offset? offset: PIB_OFFSET2, &classifiers);
			}
		}
		else 
		{
			status = pibimage1 (fd, *argv, offset? offset: PIB_OFFSET1, &classifiers);
		}
		close (fd);
		argc--;
		argv++;
	}
	exit (0);
}

