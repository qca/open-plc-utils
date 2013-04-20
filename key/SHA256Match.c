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
 *   signed SHA256Match (signed fd, const uint8_t digest[]);
 *
 *   SHA256.h
 *
 *   search a fingerprint file for the given digest; return true 
 *   on success and false on failure; digests in the fingerprint
 *   file must be upper case because comparison is binary;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef SHA256MATCH_SOURCE
#define SHA256MATCH_SOURCE

#include <unistd.h>
#include <errno.h>

#include "../tools/number.h"
#include "../key/SHA256.h"

signed SHA256Match (signed fd, const uint8_t digest []) 

{
	char string [SHA256_DIGEST_LENGTH << 1];
	char buffer [SHA256_DIGEST_LENGTH << 1];
	signed length = 0;
	while (length < (signed)(sizeof (string))) 
	{
		string [length++] = DIGITS_HEX [(*digest >> 4) & 0x0F];
		string [length++] = DIGITS_HEX [(*digest >> 0) & 0x0F];
		digest++;
	}
	while (read (fd, buffer, sizeof (buffer)) == sizeof (buffer)) 
	{
		char c;
		if (!memcmp (string, buffer, sizeof (string))) 
		{
			return (1);
		}
		while (read (fd, &c, sizeof (c)) == sizeof (c)) 
		{
			if (c == '\n') 
			{
				break;
			}
		}
	}
	return (0);
}


#endif

