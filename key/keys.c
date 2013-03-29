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
 *   keys.c - Encryption Key Data and Functions;
 *
 *   keys.h
 *
 *   Tabulate default Atheros pass phrases and their DAK, NMK and 
 *   NID for search and conversion purposes; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef KEYS_SOURCE
#define KEYS_SOURCE

#include "../key/keys.h"

const struct key keys [KEYS] = 

{
	{
		"none/secret",
		{
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00
		},
		{
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00
		}
	},
	{
		"HomePlugAV",
		{
			0x68,
			0x9F,
			0x07,
			0x4B,
			0x8B,
			0x02,
			0x75,
			0xA2,
			0x71,
			0x0B,
			0x0B,
			0x57,
			0x79,
			0xAD,
			0x16,
			0x30
		},
		{
			0x50,
			0xD3,
			0xE4,
			0x93,
			0x3F,
			0x85,
			0x5B,
			0x70,
			0x40,
			0x78,
			0x4D,
			0xF8,
			0x15,
			0xAA,
			0x8D,
			0xB7
		}
	},
	{
		"HomePlugAV0123",
		{
			0xF0,
			0x84,
			0xB4,
			0xE8,
			0xF6,
			0x06,
			0x9F,
			0xF1,
			0x30,
			0x0C,
			0x9B,
			0xDB,
			0x81,
			0x23,
			0x67,
			0xFF
		},
		{
			0xB5,
			0x93,
			0x19,
			0xD7,
			0xE8,
			0x15,
			0x7B,
			0xA0,
			0x01,
			0xB0,
			0x18,
			0x66,
			0x9C,
			0xCE,
			0xE3,
			0x0D
		}
	}
};


#endif

