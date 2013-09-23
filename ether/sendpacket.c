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
 *   ssize_t sendpacket (struct channel const * channel, void * memory, ssize_t extent);
 *
 *   channel.h
 *
 *   send one packet over a raw packet channel; return the number of
 *   bytes sent or -1 on error; dump outgoing packets on stdout when
 *   the VERBOSE flag is set;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *	Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SENDPACKET_SOURCE
#define SENDPACKET_SOURCE

#include <unistd.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/flags.h"

ssize_t sendpacket (struct channel const * channel, void * memory, ssize_t extent)

{
	if (_anyset (channel->flags, CHANNEL_VERBOSE))
	{
		hexdump (memory, 0, extent, stdout);
	}

#if defined (__linux__)

	extent = sendto (channel->fd, memory, extent, 0, (struct sockaddr *) (0), (socklen_t) (0));

#elif defined (__APPLE__) || defined (__OpenBSD__)

	extent = write (channel->fd, memory, extent);

#elif defined (WINPCAP) || defined (LIBPCAP)

	if (pcap_sendpacket (channel->socket, (byte *)(memory), extent))
	{
		extent = -1;
	}

#else
#error "Unknown Environment"
#endif

	return (extent);
}


#endif

