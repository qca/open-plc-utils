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
 *   signed HomePlugHeader (struct homeplug_std * header, uint8_t MMV, uint16_t MMTYPE);
 *
 *   mme.h
 *
 *   encode memory with a standard HomePlug AV header having HomePlug 
 *   protocol version (HOMEPLUG_MMV) and HomePlug message type (MMTYPE); 
 *   return the number of bytes actually encoded;
 *
 *
 *   Contributor(s): 
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HOMEPLUGHEADER_SOURCE
#define HOMEPLUGHEADER_SOURCE

#include <stdint.h>

#include "../mme/mme.h"
#include "../tools/endian.h"

signed HomePlugHeader (struct homeplug_std * header, uint8_t MMV, uint16_t MMTYPE) 

{
	header->MMV = MMV;
	header->MMTYPE = HTOLE16 (MMTYPE);
	return (sizeof (* header));
}


#endif

