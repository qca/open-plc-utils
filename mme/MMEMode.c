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
 *   char const * MMEMode (uint16_t MMTYPE);
 *
 *   mme.h
 *
 *   Return the mneumoic associates with a HomePlug AV MME mode; All
 *   HomePlug AV message types permit four variants, as follows, but
 *   not all messages implement all four.
 *
 *      REQ unsolicited request from host to device;
 *      CNF solicited response from device to host;
 *      IND unsolicited request from device to host;
 *      RSP solicited responds from host to device;
 * 
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MMEMODE_SOURCE
#define MMEMODE_SOURCE

#include <stdint.h>

#include "../mme/mme.h"

static char const * mme_modes [] = 

{
	"REQ",
	"CNF",
	"IND",
	"RSP"
};

char const *MMEMode (uint16_t MMTYPE) 

{
	return (mme_modes [MMTYPE & MMTYPE_MODE]);
}


#endif

