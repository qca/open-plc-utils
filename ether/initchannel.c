/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted (subject to the limitations
 *   in the disclaimer below) provided that the following conditions
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 *   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE
 *   COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 *   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   signed initchannel (struct channel * channel);
 *
 *   channel.h
 *
 *   open the low level raw packet interface
 *   (without binding it to a specific interface);
 *
 *
 *   Contributor(s):
 *	Nathaniel Houghton <nathan@brainwerk.org>
 *
 *--------------------------------------------------------------------*/

#ifndef INITCHANNEL_SOURCE
#define INITCHANNEL_SOURCE

#include <errno.h>

#if defined (__linux__)
#	include <sys/socket.h>
#elif defined (__APPLE__)
#	include <stdio.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#elif defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__)
#	include <stdio.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#elif defined (WINPCAP)
#else
#error "Unknown environment"
#endif

#include "../ether/channel.h"
#include "../tools/error.h"

signed initchannel (struct channel * channel)

{

#if defined (__linux__)

	if ((channel->fd = socket (PF_PACKET, SOCK_RAW, 0)) == -1)
	{
		error (1, errno, "%s", channel->ifname);
	}

#elif defined (__APPLE__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__)

	char filename [sizeof (CHANNEL_BPFDEVICE) + 1];
	unsigned count;
	int stat_errno = 0;
	int open_errno = 0;
	for (count = 0; count < 100; count++)
	{
		struct stat st;
		snprintf (filename, sizeof (filename), CHANNEL_BPFDEVICE, count);
		if (stat(filename, &st) == -1)
		{
			stat_errno = errno;
			continue;
		}
		if ((channel->fd = open (filename, O_RDWR)) != -1)
		{
			break;
		}
		else
		{
			open_errno = errno;
		}
	}
	if (channel->fd == -1)
	{
		if (open_errno)
		{
			error (1, open_errno, "Could not open bpf device");
		}
		else
		{
			error (1, stat_errno, "No bpf device found");
		}
	}

#elif defined (WINPCAP) || defined (LIBPCAP)

	/* Nothing to do for WIN32 */

#else
#error "Unknown Environment"
#endif

	return (0);
}


#endif

