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
 *   keys.h -
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

#ifndef KEYS_HEADER
#define KEYS_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdint.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/types.h"
#include "../key/HPAVKey.h"

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define PASSWORD_SILENCE (1 << 0)
#define PASSWORD_VERBOSE (1 << 1)

#define DAK0 "00000000000000000000000000000000"
#define DAK1 "689F074B8B0275A2710B0B5779AD1630"
#define DAK2 "F084B4E8F6069FF1300C9BDB812367FF"

#define NMK0 "00000000000000000000000000000000"
#define NMK1 "50D3E4933F855B7040784DF815AA8DB7" 
#define NMK2 "B59319D7E8157BA001B018669CCEE30D"

#define KEYS 3

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

typedef struct key 

{
	char const * phrase;
	uint8_t DAK [HPAVKEY_DAK_LEN];
	uint8_t NMK [HPAVKEY_NMK_LEN];
}

key;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

extern const struct key keys [];

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

void MACPasswords (uint32_t vendor, uint32_t device, unsigned count, unsigned alpha, unsigned group, unsigned space, flag_t flags);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

