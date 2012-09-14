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
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
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

