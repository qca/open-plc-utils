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

/*====================================================================*"
 *
 *   nvmsplit.c - 
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/


/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/memory.h"
#include "../tools/flags.h"
#include "../tools/files.h"
#include "../tools/error.h"
#include "../nvm/nvm.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/checksum32.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *
 *   void function1 (char const * filename, flag_t flags);
 *
 *   open an Atheros Qualcomm PLC firmware image file and write each 
 *   component as an independent image file;
 *
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function1 (char const * filename, flag_t flags) 

{
	struct nvm_header1 nvm_header;
	char savename [FILENAME_MAX];
	byte buffer [BUFSIZ];
	signed image = 0;
	signed offset = 0;
	signed ifd;
	signed ofd;
	char * sp;
	char * cp;
	memset (&nvm_header, 0, sizeof (nvm_header));
	for (sp = cp = strcpy (savename, filename); *cp; cp++) 
	{
		if (*cp == FILE_C_EXTENDER) 
		{
			sp = cp;
		}
	}
	if (sp == savename) 
	{
		sp = cp;
	}
	if ((ifd = open (filename, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, filename);
	}
	do 
	{
		signed extent;
		signed length;
		if (lseek (ifd, 0, SEEK_CUR) != offset) 
		{
			error (1, errno, NVM_HDR_LINK, filename, image);
		}
		if (read (ifd, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, NVM_HDR_CANTREAD, filename, image);
		}
		if (LE32TOH (nvm_header.HEADERVERSION) != 0x60000000) 
		{
			error (1, 0, NVM_HDR_VERSION, filename, image);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			error (1, 0, NVM_HDR_CHECKSUM, filename, image);
		}

#if defined (WIN32)

		sprintf (sp, "-%02d.nvm", image);

#else

		snprintf (sp, savename + sizeof (savename) - sp, "-%02d.nvm", image);

#endif

		if ((ofd = open (savename, O_BINARY|O_CREAT|O_RDWR|O_TRUNC, FILE_FILEMODE)) == -1) 
		{
			error (1, errno, FILE_CANTOPEN, savename);
		}
		if (_anyset (flags, NVM_VERBOSE)) 
		{
			error (0, 0, "%s", savename);
		}
		offset = LE32TOH (nvm_header.NEXTHEADER);
		nvm_header.NEXTHEADER = 0;
		nvm_header.HEADERCHECKSUM = 0;
		nvm_header.HEADERCHECKSUM = checksum32 (&nvm_header, sizeof (nvm_header), 0);
		if (write (ofd, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, FILE_CANTSAVE, savename);
		}
		extent = LE32TOH (nvm_header.IMAGELENGTH);
		length = sizeof (buffer);
		while (extent) 
		{
			if (length > extent) 
			{
				length = extent;
			}
			if (read (ifd, buffer, length) < length) 
			{
				error (1, errno, NVM_IMG_CANTREAD, filename, image);
			}
			if (write (ofd, buffer, length) < length) 
			{
				error (1, errno, NVM_IMG_CANTSAVE, savename, image);
			}
			extent -= length;
		}
		close (ofd);
		image++;
	}
	while (offset);
	close (ifd);
	return;
}


/*====================================================================*
 *
 *   void function2 (char const * filename, flag_t flags);
 *
 *   open an Atheros Qualcomm PLC firmware image file and write each 
 *   component as an independent image file;
 *
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function2 (char const * filename, flag_t flags) 

{
	struct nvm_header2 nvm_header;
	char savename [FILENAME_MAX];
	byte buffer [BUFSIZ];
	signed image = 0;
	signed offset = 0;
	signed ifd;
	signed ofd;
	char * sp;
	char * cp;
	memset (&nvm_header, 0, sizeof (nvm_header));
	for (sp = cp = strcpy (savename, filename); *cp; cp++) 
	{
		if (*cp == FILE_C_EXTENDER) 
		{
			sp = cp;
		}
	}
	if (sp == savename) 
	{
		sp = cp;
	}
	if ((ifd = open (filename, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, filename);
	}
	do 
	{
		signed extent;
		signed length;
		if (lseek (ifd, 0, SEEK_CUR) != offset) 
		{
			error (1, errno, NVM_HDR_LINK, filename, image);
		}
		if (read (ifd, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, NVM_HDR_CANTREAD, filename, image);
		}
		if (LE16TOH (nvm_header.MinorVersion) != 1) 
		{
			error (1, 0, NVM_HDR_VERSION, filename, image);
		}
		if (LE16TOH (nvm_header.MajorVersion) != 1) 
		{
			error (1, 0, NVM_HDR_VERSION, filename, image);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0)) 
		{
			error (1, 0, NVM_HDR_CHECKSUM, filename, image);
		}

#if defined (WIN32)

		sprintf (sp, "-%02d.nvm", image);

#else

		snprintf (sp, savename + sizeof (savename) - sp, "-%02d.nvm", image);

#endif

		if ((ofd = open (savename, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, FILE_FILEMODE)) == -1) 
		{
			error (1, errno, FILE_CANTOPEN, savename);
		}
		if (_anyset (flags, NVM_VERBOSE)) 
		{
			error (0, 0, "%s --> %s", filename, savename);
		}
		offset = LE32TOH (nvm_header.NextHeader);
		nvm_header.NextHeader = ~0;
		nvm_header.PrevHeader = ~0;
		nvm_header.HeaderChecksum = 0;
		nvm_header.HeaderChecksum = checksum32 (&nvm_header, sizeof (nvm_header), 0);
		if (write (ofd, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header)) 
		{
			error (1, errno, NVM_HDR_CANTSAVE, savename, image);
		}
		extent = LE32TOH (nvm_header.ImageLength);
		length = sizeof (buffer);
		while (extent) 
		{
			if (length > extent) 
			{
				length = extent;
			}
			if (read (ifd, buffer, length) < length) 
			{
				error (1, errno, NVM_IMG_CANTREAD, filename, image);
			}
			if (write (ofd, buffer, length) < length) 
			{
				error (1, errno, NVM_IMG_CANTSAVE, savename, image);
			}
			extent -= length;
		}
		close (ofd);
		image++;
	}
	while (~offset);
	close (ifd);
	return;
}


/*====================================================================*
 *
 *   void function (struct _file_ * file, flag_t flags);
 *
 *   open an Atheros Qualcomm PLC firmware image file and write each 
 *   component as an independent, single-image firmware file;
 *
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function (char const * filename, flag_t flags) 

{
	uint32_t version;
	signed ifd;
	if ((ifd = open (filename, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, FILE_CANTOPEN, filename);
	}
	if (read (ifd, &version, sizeof (version)) != sizeof (version)) 
	{
		error (1, errno, FILE_CANTREAD, filename);
	}
	close (ifd);
	if (LE32TOH (version) == 0x60000000) 
	{
		function1 (filename, flags);
	}
	else 
	{
		function2 (filename, flags);
	}
	return;
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []);
 *   
 *   
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"qv",
		"file [file] [...]",
		"Qualcomm Atheros PLC Firmware Image File Slicer",
		"q\tsuppress messages",
		"v\tverbose messages",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'q':
			_setbits (flags, NVM_SILENCE);
			break;
		case 'v':
			_setbits (flags, NVM_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argc) && (* argv)) 
	{
		function (* argv, flags);
		argc--;
		argv++;
	}
	return (0);
}

