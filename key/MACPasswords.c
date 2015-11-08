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

/*====================================================================*
 *
 *   void  MACPasswords (unsigned vendor, unsigned device, unsigned number, unsigned count, unsigned group, char space, flag_t flags);
 *
 *   keys.h
 *
 *   print a range of device address/password pairs on stdout; print
 *   an optional usage flag in first column for PTS compatability;
 *
 *   Contributor(s):
 *	Charles Maier
 *
 *--------------------------------------------------------------------*/

#ifndef MACPASSWORDS_SOURCE
#define MACPASSWORDS_SOURCE

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

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

static unsigned MACRand ()

{
	MACSeed *= 0x41C64E6D;
	MACSeed += 0x00003029;
	return ((unsigned)((MACSeed >> 0x10) & 0x7FFFFFFF));
}

/*====================================================================*
 *
 *   void MACPassword (unsigned device, char const charset [], unsigned limit, unsigned count, unsigned group, char space);
 *
 *   keys.h
 *
 *   Contributor(s):
 *	Charles Maier
 *
 *--------------------------------------------------------------------*/

void MACPassword (unsigned device, char const charset [], unsigned limit, unsigned count, unsigned group, char space)

{
	MACSRand (device);
	while (count--)
	{
		unsigned index = MACRand () % limit;
		putc (charset [index & limit], stdout);
		if ((count) && (group) && !(count % group))
		{
			putc (space, stdout);
		}
	}
	return;
}

/*====================================================================*
 *
 *   void  MACPasswords (unsigned vendor, unsigned device, unsigned number, unsigned count, unsigned group, char space, flag_t flags);
 *
 *   keys.h
 *
 *   print a range of device address/password pairs on stdout; print
 *   an optional usage flag in first column for PTS compatability;
 *
 *   Contributor(s):
 *	Charles Maier
 *
 *--------------------------------------------------------------------*/

void MACPasswords (unsigned vendor, unsigned device, unsigned number, unsigned count, unsigned group, char space, flag_t flags)

{
	char charset [UCHAR_MAX];
	unsigned offset = 0;
	if (vendor >> 24)
	{
		return;
	}
	if (device >> 24)
	{
		return;
	}
	if (number >> 24)
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
	while (number--)
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
		MACPassword (device, charset, sizeof (charset), count, group, space);
		putc ('\n', stdout);
		device++;
	}
	return;
}


#endif

