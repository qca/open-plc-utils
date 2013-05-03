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
 *   signed closechannel (struct channel const * channel);
 *
 *   channel.h
 *
 *   close ethernet raw packet channel;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *	Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CLOSECHANNEL_SOURCE
#define CLOSECHANNEL_SOURCE

#include <unistd.h>
#include <stdlib.h>

#include "../ether/channel.h"

signed closechannel (struct channel const * channel) 

{

#if defined (__linux__) 

	return (close (channel->fd));

#elif defined (__APPLE__) || (__OpenBSD__)

	free (channel->bpf->bpf_buffer);
	free (channel->bpf);
	return (close (channel->fd));

#elif defined (WINPCAP) || defined (LIBPCAP)

	pcap_close (channel->socket);
	return (0);

#else
#error "Unknown Environment"
#endif

}


#endif

