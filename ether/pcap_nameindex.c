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
 *   struct if_nameindex * pcap_nameindex (void)
 *
 *   ether.h
 *
 *   a WinPcap version of POSIX if_nameindex function; return error
 *   in non-pcap environments;
 *
 *   see The Open Group Base Specifications Issue 6 IEEE Std 1003.1,
 *   2004 Edition for a description of this function;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef PCAP_NAMEINDEX_SOURCE
#define PCAP_NAMEINDEX_SOURCE

#include <pcap.h>
#include <string.h>
#include <stdlib.h>

#include "../ether/ether.h"

struct if_nameindex * pcap_nameindex (void)

{

#if defined (WINPCAP) || defined (LIBPCAP)

	char buffer [PCAP_ERRBUF_SIZE];
	pcap_if_t * devices = (pcap_if_t *)(0);
	pcap_if_t * device;
	if (pcap_findalldevs (&devices, buffer) != -1)
	{
		struct if_nameindex * ifs;
		struct if_nameindex * ifp;
		unsigned count = 1;
		for (device = devices; device; device = device->next)
		{
			count++;
		}
		ifp = ifs = (struct if_nameindex *)(malloc (count * sizeof (struct if_nameindex)));
		if (ifs) for (device = devices; device; device = device->next)
		{
			ifp->if_index = device->index;
			ifp->if_name = strdup (device->name);
			ifp++;
		}
		memset (ifp, 0, sizeof (* ifp));
		pcap_freealldevs (devices);
		return (ifs);
	}

#endif

	return ((struct if_nameindex *)(0));
}


#endif

