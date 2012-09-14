/*====================================================================*
 *
 *   signed closechannel (struct channel const * channel);
 *
 *   channel.h
 *
 *   close ethernet raw packet channel;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
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

