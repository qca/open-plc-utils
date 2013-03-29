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
#include <string.h>
#include <limits.h>
#include <errno.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../key/keys.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/todigit.c"
#include "../tools/uintspec.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../key/MACPasswords.c"
#endif

/*====================================================================*
 *   program constants; 
 *--------------------------------------------------------------------*/

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6 /* normally defined in ethernet.h or if_ether.h */
#endif

/*====================================================================*
 *
 *   void function (const char * string, uint32_t range, unsigned count, unsigned group, unsigned space, flag_t flags) 
 * 
 *   parse an Ethernet hardware address string into vendor and device
 *   ID substrings; print a specified number of consecutive addresses
 *   and password strings having a defined letter count and grouping;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void function (const char * string, uint32_t range, unsigned count, unsigned group, unsigned space, flag_t flags) 

{
	const char * offset = string;
	uint32_t vendor = 0;
	uint32_t device = 0;
	unsigned radix = 0x10;
	unsigned width;
	unsigned digit;
	for (width = 0; width < ETHER_ADDR_LEN; width++) 
	{
		if ((digit = todigit (*offset)) < radix) 
		{
			vendor *= radix;
			vendor += digit;
			offset++;
			continue;
		}
		error (1, EINVAL, "Bad MAC Address: %s", string);
	}
	if (!vendor) 
	{
		error (1, EPERM, "Vendor ID can't be zero");
	}
	for (width = 0; width < ETHER_ADDR_LEN; width++) 
	{
		if ((digit = todigit (*offset)) < radix) 
		{
			device *= radix;
			device += digit;
			offset++;
			continue;
		}
		error (1, EINVAL, "Bad MAC Address: %s", string);
	}
	if (!device) 
	{
		error (1, EPERM, "Device ID can't be zero");
	}
	if (*offset) 
	{
		error (1, EINVAL, "Bad MAC address: %s", string);
	}
	if (range > (0x00FFFFFF - device)) 
	{
		error (1, ERANGE, "Want %d passwords but only %d left in range", range, (0x00FFFFFF - device));
	}
	MACPasswords (vendor, device, range, count, group, space, flags);
	return;
}


/*====================================================================*
 *   
 *   int main (int argc, const char * argv []);
 *
 *   generate unique password strings for a range of device hardware
 *   addresses; print paired addresses and passwords to stdout;
 *
 *   Many Atheros programs expect the user to enter a password to
 *   access a device; the password is encoded to product the 16-bit
 *   Device Access Key (DAK) stored in the PIB; 
 *
 *   Vendors must publish the device password so that end users can 
 *   reproduce the same 16-byte hexadecimal value later; a password
 *   is more user-friendly than a 16-byte hexadecimal value;
 *
 *   given a range of MAC address, this program will produce unique
 *   passwords so vendors can program devices and print labels that 
 *   ship devices;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#define DEFAULT_RANGE 1
#define DEFAULT_COUNT 16
#define DEFAULT_GROUP 4

int main (int argc, const char * argv []) 

{
	static const char * optv [] = 
	{
		"b:l:n:quv",
		"address [address] [...]",
		"Atheros device password generator",
		"b n\tbunching factor [" LITERAL (DEFAULT_GROUP) "]",
		"l n\tpassword letters [" LITERAL (DEFAULT_COUNT) "]",
		"n n\tgenerate n consecutive passwords [" LITERAL (DEFAULT_RANGE) "]",
		"q\tomit device address on output",
		"v\tprepend PTS flag on output",
		(const char *)(0)
	};
	uint32_t range = DEFAULT_RANGE;
	unsigned count = DEFAULT_COUNT;
	unsigned group = DEFAULT_GROUP;
	unsigned space = '-';
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char)(c)) 
		{
		case 'b':
			group = (unsigned)(uintspec (optarg, 0, UCHAR_MAX));
			break;
		case 'l':
			count = (unsigned)(uintspec (optarg, 12, 64));
			break;
		case 'n':
			range = (uint32_t)(uintspec (optarg, 0, 0x00FFFFFF));
			break;
		case 'q':
			_setbits (flags, PASSWORD_SILENCE);
			break;
		case 'v':
			_setbits (flags, PASSWORD_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	while ((argv) && (* argv)) 
	{
		function (* argv, range, count, group, space, flags);
		argc--;
		argv++;
	}
	return (0);
}

