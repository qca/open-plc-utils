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
 *   signed EthernetHeader (void * memory, const uint8_t peer [], const uint8_t host [], uint16_t protocol);
 *
 *   mme.h
 *
 *   encode a memory region with a standard Ethernet frame header in
 *   network byte order;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s): 
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef ETHERNETHEADER_SOURCE
#define ETHERNETHEADER_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../mme/mme.h"

signed EthernetHeader (void * memory, const uint8_t peer [], const uint8_t host [], uint16_t protocol) 

{
	struct ether_header * header = (struct ether_header *)(memory);
	memcpy (header->ether_dhost, peer, sizeof (header->ether_dhost));
	memcpy (header->ether_shost, host, sizeof (header->ether_shost));
	header->ether_type = htons (protocol);
	return (sizeof (* header));
}


#endif

