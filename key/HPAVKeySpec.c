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
 *   void HPAVKeySpec (const char * string);
 *   
 *   HPAVKey.h
 *   
 *   confirm that a string is a legal HomePlug AV pass phrase; return
 *   on success; exit the program on failure; legal pass phrases have 
 *   12 to 64 characters ranging from 0x20 thru 0x7F;
 *
 *   this function is intended to check pass phrases entered from the
 *   command line as arguments therefore it explains why it failed;
 *   
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s);
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HPAVKEYSPEC_SOURCE
#define HPAVKEYSPEC_SOURCE

#include <ctype.h>

#include "../tools/error.h"
#include "../key/HPAVKey.h"

void HPAVKeySpec (const char * string) 

{
	const char * sp = string;
	while (isprint (*sp)) 
	{
		sp++;
	}
	if (*sp) 
	{
		error (1, ENOTSUP, "Phrase \"%s\" has illegal characters", string);
	}
	if ((sp - string) < HPAVKEY_PHRASE_MIN) 
	{
		error (1, ENOTSUP, "Phrase \"%s\" less than %d characters", string, HPAVKEY_PHRASE_MIN);
	}
	if ((sp - string) > HPAVKEY_PHRASE_MAX) 
	{
		error (1, ENOTSUP, "Phrase \"%s\" more than %d characters", string, HPAVKEY_PHRASE_MAX);
	}
	return;
}


#endif

