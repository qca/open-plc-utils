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
 *   signed QualcommHeader1 (struct qualcomm_fmi * header, uint8_t MMV, uint16_t MMTYPE, uint16_tFMI);
 *
 *   mme.h 
 *
 *   Encode memory with an Atheros vendor specific message header 
 *   having HomePlug message protocol version (INTELLON_MMV) and 
 *   Atheros message type (MMTYPE);
 *
 *   return the number of bytes actually encoded or 0 on encode error; 
 *   the error code is stored in errno;
 *
 *   see the INT6000 Firmware Technical Reference Manual for more
 *   about MME headers and message types; the Atheros OUI is 
 *   implicit in this function;
 *
 *   MMV is the version number of the MME command set; currently, 
 *   there is only one command set for Atheros MMEs; 
 *
 *   MMTYPE indicates the desired Atheros device operation taken
 *   from the TRM; some operations are undocumented and should not
 *   be used;
 *
 *   OUI is the Organizationally Unique Identifier resgistered with
 *   the IEEE by the vendor and is a constant for Atheros Devices; 
 *
 *   There is no need to flush the header since this function writes
 *   to all locations unless there is an error; the caller may elect
 *   to flush the buffer before calling this function;
 *
 *
 *   Contributor(s): 
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FRAGMENTHEADER_SOURCE
#define FRAGMENTHEADER_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/endian.h"
#include "../mme/mme.h"

signed QualcommHeader1 (struct qualcomm_fmi * header, uint8_t MMV, uint16_t MMTYPE) 

{
	header->MMV = MMV;
	header->MMTYPE = HTOLE16 (MMTYPE);
	header->FMSN = 0;
	header->FMID = 0;
	header->OUI [0] = 0x00;
	header->OUI [1] = 0xB0;
	header->OUI [2] = 0x52;
	return (sizeof (* header));
}


#endif

