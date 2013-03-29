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
 *   int gethwaddr (void volatile * memory, char const * device);
 *   
 *   ether.h
 *
 *   encode memory with the hardware address of a named host Ethernet
 *   interface;
 *
 *   there are two ways to obtain the hardware address on Linux; we
 *   use the first because some systems do not support getifaddrs() 
 *   and some implementations are inconsistent; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *      Charles Maier <cmaier@qualcomm.com>
 *      
 *--------------------------------------------------------------------*/

#ifndef GETHWADDR_SOURCE
#define GETHWADDR_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include "../tools/error.h"
#include "../ether/ether.h"

#ifndef OID_802_3_CURRENT_ADDRESS
#define OID_802_3_CURRENT_ADDRESS 0x01010102 
#endif

int gethwaddr (void * memory, char const * device) 

{

#if defined (__linux__)

#	include <ifaddrs.h>
#	include <netpacket/packet.h>
#	include <sys/ioctl.h>

	struct ifreq ifreq;
	int fd;
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		error (1, errno, "%s: %s", __func__, device);
	}
	memcpy (ifreq.ifr_name, device, sizeof (ifreq.ifr_name));
	if (ioctl (fd, SIOCGIFHWADDR, &ifreq) == -1) 
	{
		close (fd);
		return (-1);
	}
	memcpy (memory, ifreq.ifr_ifru.ifru_hwaddr.sa_data, ETHER_ADDR_LEN);
	close (fd);

#elif defined (__linux__) 

#include <ifaddrs.h>
#include <net/if_types.h>

	struct ifaddrs *ifaddrs;
	struct ifaddrs *ifaddr;
	if (getifaddrs (&ifaddrs) == -1) 
	{
		error (1, errno, "No interfaces available");
	}
	for (ifaddr = ifaddrs; ifaddr; ifaddr = ifaddr->ifa_next) 
	{
		if (strcmp (device, ifaddr->ifa_name)) 
		{
			continue;
		}
		if (!ifaddr->ifa_addr) 
		{
			continue;
		}
		if (ifaddr->ifa_addr->sa_family == AF_PACKET) 
		{
			struct sockaddr_ll * sockaddr = (struct sockaddr_ll *) (ifaddr->ifa_addr);
			memcpy (memory, sockaddr->sll_addr, ETHER_ADDR_LEN);
			break;
		}
	}
	freeifaddrs (ifaddrs);

#elif defined (__APPLE__) 

#include <ifaddrs.h>
#include <net/if_dl.h>

	struct ifaddrs *ifaddrs;
	struct ifaddrs *ifaddr;
	if (getifaddrs (&ifaddrs) == -1) 
	{
		error (1, errno, "No interfaces available");
	}
	for (ifaddr = ifaddrs; ifaddr; ifaddr = ifaddr->ifa_next) 
	{
		if (strcmp (device, ifaddr->ifa_name)) 
		{
			continue;
		}
		if (!ifaddr->ifa_addr) 
		{
			continue;
		}
		if (ifaddr->ifa_addr->sa_family == AF_LINK) 
		{
			struct sockaddr_dl * sockaddr = (struct sockaddr_dl *) (ifaddr->ifa_addr);
			memcpy (memory, LLADDR (sockaddr), ETHER_ADDR_LEN);
			break;
		}
	}
	freeifaddrs (ifaddrs);

#elif defined (__OpenBSD__)

#include <ifaddrs.h>
#include <net/if_dl.h>

	struct ifaddrs *ifaddrs;
	struct ifaddrs *ifaddr;
	if (getifaddrs (&ifaddrs) == -1) 
	{
		error (1, errno, "No interfaces available");
	}
	for (ifaddr = ifaddrs; ifaddr; ifaddr = ifaddr->ifa_next) 
	{
		if (strcmp (device, ifaddr->ifa_name)) 
		{
			continue;
		}
		if (!ifaddr->ifa_addr) 
		{
			continue;
		}
		if (ifaddr->ifa_addr->sa_family == AF_LINK) 
		{
			struct sockaddr_dl * sockaddr = (struct sockaddr_dl *) (ifaddr->ifa_addr);
			memcpy (memory, LLADDR (sockaddr), ETHER_ADDR_LEN);
			break;
		}
	}
	freeifaddrs (ifaddrs);

#elif defined (WINPCAP) || defined (LIBPCAP)

	LPADAPTER adapter = PacketOpenAdapter ((PCHAR)(device));
	PPACKET_OID_DATA data = (PPACKET_OID_DATA)(malloc (ETHER_ADDR_LEN + sizeof (PACKET_OID_DATA)));
	if (!data) 
	{
		error (1, errno, "Can't allocate packet: %s", device);
	}
	data->Oid = OID_802_3_CURRENT_ADDRESS;
	data->Length = ETHER_ADDR_LEN;
	if ((adapter == 0) || (adapter->hFile == INVALID_HANDLE_VALUE)) 
	{
		error (1, errno, "Can't access interface: %s", device);
	}
	if (!PacketRequest (adapter, FALSE, data)) 
	{
		memset (memory, 0, ETHER_ADDR_LEN);
		PacketCloseAdapter (adapter);
		free (data);
		return (-1);
	}
	memcpy (memory, data->Data, data->Length);
	PacketCloseAdapter (adapter);
	free (data);

#else
#error "Unknown environment"
#endif

	return (0);
}


#endif

