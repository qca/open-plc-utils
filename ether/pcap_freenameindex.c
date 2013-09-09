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
 *   void pcap_freenameindex (struct if_nameindex *)
 *
 *   ether.h
 *
 *   a WinPcap version of POSIX if_freenameindex function;
 *
 *   see The Open Group Base Specifications Issue 6 IEEE Std 1003.1,
 *   2004 Edition for a description of this function;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef PCAP_FREENAMEINDEX_SOURCE
#define PCAP_FREENAMEINDEX_SOURCE

#include <pcap.h>
#include <stdlib.h>

#include "../ether/ether.h"

void pcap_freenameindex (struct if_nameindex * if_nameindex)

{

#if defined (WINPCAP) || defined (LIBPCAP)

	struct if_nameindex * ifp = if_nameindex;
	while (ifp->if_index)
	{
		free (ifp->if_name);
	}
	free (if_nameindex);

#endif

	return;
}


#endif

