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
 *   char * getifname (signed number);
 *
 *   ether.h
 *
 *   return the PCAP interface name for a given interface number; this
 *   function is only needed when using LIBPCAP or WINPCAP libraries;
 *   
 *   This software and documentation is the property of Atheros 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Atheros assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Atheros customers may modify and distribute the software 
 *   without obligation to Atheros. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *      Charles Maier <cmaier@qualcomm.com>
 *      
 *--------------------------------------------------------------------*/

#ifndef GETIFNAME_SOURCE
#define GETIFNAME_SOURCE

#include <string.h>

#if defined (WINPCAP) || defined (LIBPCAP)
#include <pcap.h>
#endif

#include "../ether/ether.h"
#include "../tools/error.h"

char * getifname (signed index) 

{
	char * name = (char *)(0);

#if defined (__linux__)

#elif defined (__APPLE__) || defined (__OpenBSD__)

#elif defined (WINPCAP) || defined (LIBPCAP)

	char buffer [PCAP_ERRBUF_SIZE];
	pcap_if_t *devices = (pcap_if_t *)(0);
	pcap_if_t *device;
	signed count;
	if (pcap_findalldevs (&devices, buffer) == -1) 
	{
		error (1, errno, "can't enumerate pcap devices");
	}
	for (device = devices, count = 1; device; device = device->next, count++) 
	{
		if (count == index) 
		{
			name = strdup (device->name);
			break;
		}
	}
	if (!device) 
	{
		error (1, EINVAL, "invalid interface: %d", index);
	}
	pcap_freealldevs (devices);

#else
#error "Unknown environment"
#endif

	return (name);
}


#endif

