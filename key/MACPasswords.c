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
 *   void  MACPasswords (uint32_t vendor, uint32_t device, unsigned number, unsigned count, unsigned group, unsigned space, flag_t flags);
 *
 *   keys.h
 *
 *   print a range of device address/password pairs on stdout; print 
 *   an optional usage flag in the first column for PTS compatability;
 *
 *   vendor is the 24-bit OUI expressed as an integer; device is the 
 *   24-bit starting unit address expressed as an integer; number is 
 *   the number of address/password pairs to generate; count is the 
 *   number of letters in the password excluding delimiters; 
 *
 *   passwords consists of letters arranged in groups separated by
 *   spaces; count is the number of letters; group is the number of 
 *   letters in each group; space is the character that separates 
 *   each group;
 *   
 *   vendor is used to seed the random number generator and create 
 *   a character set having the 256 random upper case letters used 
 *   for all vendor passwords; most letters will appear more than 
 *   once in the character set;
 *
 *   device is used to seed the random number generator and select
 *   count random letters from the character set until the password 
 *   has been constructed;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MACPASSWORDS_SOURCE
#define MACPASSWORDS_SOURCE

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <netinet/in.h>

#include "../tools/types.h"
#include "../tools/flags.h"
#include "../key/keys.h"

static uint64_t MACSeed = 0;
static uint64_t MACSRand (uint64_t seed) 

{
	uint64_t temp = MACSeed;
	MACSeed = seed;
	return (temp);
}

static uint32_t MACRand () 

{
	MACSeed *= 0x41C64E6D;
	MACSeed += 0x00003029;
	return ((uint32_t)((MACSeed >> 0x10) & 0x7FFFFFFF));
}

static void MACPassword (uint32_t device, char const charset [], unsigned count, unsigned alpha, unsigned group, char space) 

{
	unsigned letter = 0;
	MACSRand (device);
	while (letter < alpha) 
	{
		unsigned offset = MACRand () % count;
		if ((letter) && (group) && !(letter%group)) 
		{
			putc (space, stdout);
		}
		putc (charset [offset], stdout);
		letter++;
	}
	return;
}

void MACPasswords (uint32_t vendor, uint32_t device, unsigned count, unsigned alpha, unsigned group, unsigned space, flag_t flags) 

{
	char charset [UCHAR_MAX + 1];
	unsigned offset = 0;
	if (vendor >> 24) 
	{
		return;
	}
	if (device >> 24) 
	{
		return;
	}
	if (count >> 24) 
	{
		return;
	}
	MACSRand (vendor);
	while (offset < sizeof (charset)) 
	{
		unsigned c = MACRand () % (SCHAR_MAX + 1);
		if (isupper (c)) 
		{
			charset [offset++] = c;
		}
	}
	while (count--) 
	{
		if (_anyset (flags, PASSWORD_VERBOSE)) 
		{
			putc ('0', stdout);
			putc (' ', stdout);
		}
		if (_allclr (flags, PASSWORD_SILENCE)) 
		{
			printf ("%06X", vendor & 0x00FFFFFF);
			printf ("%06X", device & 0x00FFFFFF);
			putc (' ', stdout);
		}
		MACPassword (device, charset, sizeof (charset), alpha, group, space);
		putc ('\n', stdout);
		device++;
	}
	return;
}


#endif

