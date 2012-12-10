/*====================================================================*
 *   
 *   Copyright (c) 2011 by Qualcomm Atheros.
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
 *   nvram.c - NVRAM type information;
 * 
 *   nvram.h
 *
 *   nvram codes for display purposes;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef NVRAM_SOURCE
#define NVRAM_SOURCE

#include "../tools/types.h"
#include "../tools/symbol.h"
#include "../ram/nvram.h"

#define NVRAM_TYPES (sizeof (nvram_types) / sizeof (struct _type_))

static const struct _type_ nvram_types [] = 

{
	{
		0x00000005,
		"500KB"
	},
	{
		0x00000010,
		"1MB"
	},
	{
		0x00000011,
		"M25P20"
	},
	{
		0x00000012,
		"M25P40"
	},
	{
		0x00000013,
		"M25P80"
	},
	{
		0x00000014,
		"M25P16_ES"
	},
	{
		0x00000015,
		"M25P32_ES"
	},
	{
		0x00000016,
		"M25P64_ES"
	},
	{
		0x00000046,
		"AT26DF161"
	},
	{
		0x00000146,
		"AT26DF161A"
	},
	{
		0x00001020,
		"M25P05A"
	},
	{
		0x00001120,
		"M25P10A"
	},
	{
		0x00001402,
		"S25FL016AOLMFI01"
	},
	{
		0x00001420,
		"M25P80"
	},
	{
		0x00001520,
		"M25P16"
	},
	{
		0x00001532,
		"FM25S16/FM25Q16"
	},
	{
		0x00001571,
		"M25PX16"
	},
	{
		0x00001620,
		"M25P32"
	},
	{
		0x00001632,
		"FM25Q32"
	},
	{
		0x00001720,
		"M25P64"
	},
	{
		0x00004125,
		"SST25VF016B"
	}
};

char const * NVRAMName (uint16_t NVRAMTYPE) 

{
	return (typename (nvram_types, NVRAM_TYPES, NVRAMTYPE, "Unknown NVRAM Type"));
}


#endif

