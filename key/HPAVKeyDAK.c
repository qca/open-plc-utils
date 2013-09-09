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
 *   void HPAVKeyDAK (uint8_t DAK [], const char * string);
 *
 *   HPAVKey.h
 *
 *   compute the HomePlugAV compliant Device Access Key (DAK) for a
 *   NUL terminated string; return the key in buffer DAK[]; the key
 *   will be HPAVKEY_DAK_LEN bytes as defined in HPAVKey.h;
 *
 *   seed the digest then salt the digest then rehash the digest 999
 *   times per HomePlug AV Specification;
 *
 *
 *   Contributor(s);
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HPAVKEYDAK_SOURCE
#define HPAVKEYDAK_SOURCE

#include <memory.h>

#include "../key/HPAVKey.h"
#include "../key/SHA256.h"

void HPAVKeyDAK (uint8_t DAK [], const char * string)

{
	struct sha256 sha256;
	uint8_t digest [SHA256_DIGEST_LENGTH];
	const uint8_t secret [] =
	{
		0x08,
		0x85,
		0x6D,
		0xAF,
		0x7C,
		0xF5,
		0x81,
		0x85
	};
	unsigned rehash = 999;
	SHA256Reset (&sha256);
	SHA256Write (&sha256, string, strlen (string));
	SHA256Write (&sha256, secret, sizeof (secret));
	SHA256Fetch (&sha256, digest);
	while (rehash--)
	{
		SHA256Reset (&sha256);
		SHA256Write (&sha256, digest, sizeof (digest));
		SHA256Fetch (&sha256, digest);
	}
	memcpy (DAK, digest, HPAVKEY_DAK_LEN);
	return;
}


#endif

