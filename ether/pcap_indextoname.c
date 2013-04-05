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
 *   char * pcap_indextoname (unsigned ifindex, char * ifname);
 *
 *   ether.h
 *
 *   a WinPcap version of POSIX if_indextoname function; return error 
 *   in non-pcap environments; set errno to ENXIO on error on MacOSX; 
 *
 *   see The Open Group Base Specifications Issue 6 IEEE Std 1003.1, 
 *   2004 Edition for a description of this function;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef PCAP_INDEXTONAME_SOURCE
#define PCAP_INDEXTONAME_SOURCE

#include <pcap.h>
#include <string.h>
#include <errno.h>

#include "../ether/ether.h"

char * pcap_indextoname (unsigned ifindex, char * ifname) 

{

#if defined (WINPCAP) || defined (LIBPCAP)

	char buffer [PCAP_ERRBUF_SIZE];
	pcap_if_t * devices = (pcap_if_t *)(0);
	pcap_if_t * device;
	if ((index--) && (pcap_findalldevs (&devices, buffer) != -1)) 
	{
		for (device = devices; device; device = device->next) 
		{
			if (!index--) 
			{
				memcpy (ifname, device->name, strlen (device->name));
				pcap_freealldevs (devices);
				return (ifname);
			}
		}
		pcap_freealldevs (devices);
	}

#endif
#if defined (__APPLE__)

	errno = ENXIO;

#endif

	return ((char *)(0));
}


#endif

