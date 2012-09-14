/*====================================================================*
 *   
 *   Copyright (c) 2011 by Qualcomm Atheros.
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
 *   channel.c - global channel structure;
 *
 *   channel.h
 *
 *   define and initialize a global channel structure; this structure
 *   is initialized for communication with Atheros devices and it is
 *   referenced by Atheros Linux Toolkit programs that do not need a 
 *   full int6k data structure;
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
 *      Charles Maier <cmaier@qualcomm.com>
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CHANNEL_SOURCE
#define CHANNEL_SOURCE

#include "../ether/channel.h"

struct channel channel = 

{
	(file_t)(-1),
	0,
	CHANNEL_ETHNUMBER,
	CHANNEL_ETHDEVICE,
	{
		0x00,
		0xB0,
		0x52,
		0x00,
		0x00,
		0x01
	},
	{
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	},
	0x88E1,

#if defined (__linux__)

#elif defined (__APPLE__) || defined (__OpenBSD__)

	(struct bpf *) (0),

#elif defined (WINPCAP) || defined (LIBPCAP)

	(pcap_t *)(0),
	{
		0
	},

#else
#error "Unknown Environment"
#endif

	CHANNEL_TIMEOUT,
	CHANNEL_OPTIONS
};


#endif

