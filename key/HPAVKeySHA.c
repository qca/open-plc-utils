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
 *   void HPAVKeySHA (uint8_t digest [], const char * string);
 *
 *   HPAVKey.h
 *
 *   compute the SHA256 digest for a NUL terminated string; the 
 *   will always 256 bits (32 bytes) long for any string length;
 *
 *
 *   Contributor(s);
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HPAVKEYSHA_SOURCE
#define HPAVKEYSHA_SOURCE

#include "../key/HPAVKey.h"
#include "../key/SHA256.h"

void HPAVKeySHA (uint8_t digest [], const char * string) 

{
	struct sha256 sha256;
	SHA256Reset (&sha256);
	SHA256Write (&sha256, string, strlen (string));
	SHA256Fetch (&sha256, digest);
	return;
}


#endif

