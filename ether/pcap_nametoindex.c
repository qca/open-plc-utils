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
 *   unsigned pcap_nametoindex (char const * name);
 *
 *   ether.h
 *
 *   a WinPcap version of POSIX if_nametoindex function; return error
 *   in non-pcap environments;
 *
 *   see The Open Group Base Specifications Issue 6 IEEE Std 1003.1, 
 *   2004 Edition for a description of this function;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

#ifndef PCAP_NAMETOINDEX_SOURCE
#define PCAP_NAMETOINDEX_SOURCE

#include <pcap.h>
#include <string.h>
#include <errno.h>

#include "../ether/ether.h"

unsigned pcap_nametoindex (char const * name) 

{

#if defined (WINPCAP) || defined (LIBPCAP)

	char buffer [PCAP_ERRBUF_SIZE];
	pcap_if_t * devices = (pcap_if_t *)(0);
	pcap_if_t * device;
	if (pcap_findalldevs (&devices, buffer) != -1) 
	{
		unsigned index = 1;
		for (device = devices; device; device = device->next) 
		{
			if (!strcmp (name, device->name)) 
			{
				index++;
				continue;
			}
			pcap_freealldevs (devices);
			return (index);
		}
		pcap_freealldevs (devices);
	}

#endif
#if defined (__APPLE__)

	errno = ENXIO;

#endif

	return (0);
}


#endif

