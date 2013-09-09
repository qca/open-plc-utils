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
 *   channel.h - raw packet channel definitions and declarations;
 *
 *   the channel structure contains information needed to perform
 *   ISO Layer 2, raw packet I/O in a variety of environments;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CHANNEL_HEADER
#define CHANNEL_HEADER

/*====================================================================*
 *  system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>

/*====================================================================*
 *  custom header files;
 *--------------------------------------------------------------------*/

#include "../ether/ether.h"
#include "../tools/types.h"

/*====================================================================*
 *   sort out the raw socket mess;
 *--------------------------------------------------------------------*/

#if defined (__linux__) || defined (__APPLE__) || defined (__OpenBSD__)
#       ifdef WINPCAP
#               error "Don't enable winpcap on Linux. It won't work."
#               endif
#       ifdef LIBPCAP
#               error "Don't enable libpcap on Linux. You don't need it."
#               endif
#elif defined (WIN32)
#       ifndef WINPCAP
#               error "Define preprocessor constant WINPCAP on Windows."
#               endif
#       ifdef LIBPCAP
#               error "Don't enable libpcap on Windows. It won't work."
#               endif
#elif defined (__CYGWIN__)
# 	error "cygwin is unsupported!"
#else
#       error "unknown environment"
#endif

/*====================================================================*
 *   channel flagword bitmasks;
 *--------------------------------------------------------------------*/

#define CHANNEL_VERBOSE (1 << 0)
#define CHANNEL_SILENCE (1 << 1)
#define CHANNEL_WARNING (1 << 2)
#define CHANNEL_SUCCESS (1 << 3)
#define CHANNEL_FAILURE (1 << 4)
#define CHANNEL_MONITOR (1 << 5)

#define CHANNEL_UPDATE_TARGET (1 << 5) /* used by efsu only */
#define CHANNEL_UPDATE_SOURCE (1 << 6) /* used by efsu only */
#define CHANNEL_LISTEN (1 << 7)        /* used by efsu only */

#define CHANNEL_ETHNUMBER 2
#if defined (__linux__)
#       define CHANNEL_ETHDEVICE "eth1"
#elif defined (__APPLE__)
#       define CHANNEL_ETHDEVICE "en0"
#       define CHANNEL_BPFDEVICE "/dev/bpf%d"
#elif defined (__OpenBSD__)
#       define CHANNEL_ETHDEVICE "bce0"
#       define CHANNEL_BPFDEVICE "/dev/bpf%d"
#else
#       define CHANNEL_ETHDEVICE "nic1"
#endif
#define CHANNEL_FOREVER (unsigned)(-1)
#define CHANNEL_BAILOUT 0
#define CHANNEL_CAPTURE 15
#define CHANNEL_TIMEOUT 50
#define CHANNEL_FLAGS 0

/*====================================================================*
 *   common channel error messages;
 *--------------------------------------------------------------------*/

#define CHANNEL_CANTSEND "%s: Send timeout or network error", __func__
#define CHANNEL_CANTREAD "%s: Read timeout or network error", __func__

/*====================================================================*
 *   channel ethertype definitions;
 *--------------------------------------------------------------------*/

#if !defined (__linux__)
#       define ETH_P_802_2 1
#endif

/*====================================================================*
 *   communication channel structure;
 *--------------------------------------------------------------------*/

typedef struct channel

{
	signed fd;
	signed ifstate;
	signed ifindex;
	char const * ifname;
	uint8_t peer [ETHER_ADDR_LEN];
	uint8_t host [ETHER_ADDR_LEN];
	uint16_t type;

#if defined (__linux__)

#elif defined (__APPLE__) || defined (__OpenBSD__)

	struct bpf
	{
		signed bpf_length;
		uint8_t * bpf_buffer;
		uint8_t * bpf_bp;
		signed bpf_bufused;
	}
	* bpf;

#elif defined (WINPCAP) || defined (LIBPCAP)

	pcap_t * socket;
	char errbuf [PCAP_ERRBUF_SIZE];

#else
#error "Unknown Environment"
#endif

	signed capture;
	signed timeout;
	flag_t flags;
}

CHANNEL;

/*====================================================================*
 *   channel functions;
 *--------------------------------------------------------------------*/

signed openchannel (struct channel *);
signed closechannel (struct channel const *);
ssize_t sendpacket (struct channel const *, void * memory, ssize_t extent);
ssize_t readpacket (struct channel const *, void * memory, ssize_t extent);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

