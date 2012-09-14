/*====================================================================*
 *
 *   signed setchanneltimeout (struct channel * channel, unsigned timeout);
 *
 *   channel.h
 *   
 *   set the current channel timeout;   
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
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SETCHANNELTIMEOUT_SOURCE
#define SETCHANNELTIMEOUT_SOURCE

#include <unistd.h>

#if defined (__APPLE__) || defined (__OpenBSD__)
#	include <sys/ioctl.h>
#endif

#include "../ether/channel.h"
#include "../tools/error.h"

#ifdef WIN32
#	if !defined(HAVE_VSNPRINTF)
#		undef vsnprintf
#	endif
#	if !defined(HAVE_SNPRINTF)
#		undef snprintf
#	endif
#	include <pcap-int.h>
#	include <Packet32.h>
#endif

signed setchanneltimeout (struct channel * channel, unsigned timeout) 

{
	channel->timeout = timeout;

#if defined (__linux__)

#elif defined (__APPLE__) || defined (__OpenBSD__)

	struct timeval timeval = 
	{
		0,
		0
	};
	if (channel->timeout > 1000) 
	{
		timeval.tv_sec = channel->timeout / 1000;
		timeval.tv_usec = 0;
	}
	else 
	{

#if defined (__MAC_10_6) 

/*
 *	accommodate known bug in BPF on MAC OS X 10.6; shorter times cause socket read
 *	operations to block indefinitely if no frames are waiting because tv_usec gets
 *      clobbered;
 */

		timeval.tv_sec = 1;
		timeval.tv_usec = 0;

#else

		timeval.tv_sec = 0;
		timeval.tv_usec = channel->timeout * 1000;

#endif

	}
	if (ioctl (channel->fd, BIOCSRTIMEOUT, &timeval) == -1) 
	{
		error (1, errno, "%s", channel->ifname);
	}

#elif defined (WINPCAP) || defined (LIBPCAP)

	PacketSetReadTimeout (channel->socket->adapter, channel->timeout);

#else
#error "Unknown Environment"
#endif

	return (0);
}


#endif

