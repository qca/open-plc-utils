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
 *   bool UnwantedMessage (void const * memory, size_t extent, uint8_t MMV, uint16_t MMTYPE);
 *
 *   mme.h
 *
 *   return true if memory does not contains a Qualcomm Atheros message
 *   of the specified version and type; the message version determines
 *   the location of the OUI field; messages with MMV = 1 have an FMI
 *   used to track multi-part confirmation counts; out of order counts
 *   are treated as an error;
 *
 *   constant __WHYNOT__ displays reason for message rejection;
 *
 *   batch is the fragment group identifier taken from the fragment
 *   frame;
 *
 *   total is the number of fragements in the group taken from the
 *   fragment frame;
 *
 *   index is the fragment identifier from the fragment frame;
 *
 *   count is the number of fragments received so far;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef UNWANTEDMESSAGE_SOURCE
#define UNWANTEDMESSAGE_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/endian.h"
#include "../tools/error.h"
#include "../mme/mme.h"

signed UnwantedMessage (void const * memory, size_t extent, uint8_t MMV, uint16_t MMTYPE)

{
	extern const byte localcast [ETHER_ADDR_LEN];
//	struct message * message = (struct message *)(memory);
	struct homeplug * homeplug = (struct homeplug *)(memory);
	if (!extent)
	{
		return (-1);
	}
	if (extent < (ETHER_MIN_LEN - ETHER_CRC_LEN))
	{

#if defined (__WHYNOT__)

		error (0, 0, "Wrong Ethernet Frame Size: Received " SIZE_T_SPEC " bytes but need at least %d", extent, ETHER_MIN_LEN - ETHER_CRC_LEN);

#endif

		return (-1);
	}
	if (extent > (ETHER_MAX_LEN))
	{

#if defined (__WHYNOT__)

		error (0, 0, "Wrong Ethernet Frame Size: Received " SIZE_T_SPEC " bytes but need at most %d", extent, ETHER_MAX_LEN);

#endif

		return (-1);
	}
	if (ntohs (homeplug->ethernet.MTYPE) != ETH_P_HPAV)
	{

#if defined (__WHYNOT__)

		error (0, 0, "Wrong Ethernet Frame Type: Received %04X while waiting for %04X", ntohs (homeplug->ethernet.MTYPE), ETH_P_HPAV);

#endif

		return (-1);
	}
	if (homeplug->homeplug.MMV != MMV)
	{

#if defined (__WHYNOT__)

		error (0, 0, "Wrong Message Version: Received %02X but expected %02X", homeplug->homeplug.MMV, MMV);

#endif

		return (-1);
	}
	if (homeplug->homeplug.MMV == 0)
	{
		struct qualcomm_hdr * qualcomm = (struct qualcomm_hdr *)(&homeplug->homeplug);
		if (LE16TOH (qualcomm->MMTYPE) != MMTYPE)
		{

#if defined (__WHYNOT__)

			error (0, 0, "Wrong Message Type: Received %04X while waiting for %04X", LE16TOH (qualcomm->MMTYPE), MMTYPE);

#endif

			return (-1);
		}
		if ((MMTYPE < VS_MMTYPE_MIN) || (MMTYPE > VS_MMTYPE_MAX))
		{
		}
		else if (memcmp (localcast, qualcomm->OUI, sizeof (qualcomm->OUI)))
		{

#if defined (__WHYNOT__)

			error (0, 0, "Wrong Message Vendor");

#endif

			return (-1);
		}
	}
	if (homeplug->homeplug.MMV == 1)
	{
		struct qualcomm_fmi * qualcomm = (struct qualcomm_fmi *)(&homeplug->homeplug);

#if FMI

		static unsigned total = 0;
		static unsigned index = 0;
		static unsigned count = 0;

#endif

		if (LE16TOH (qualcomm->MMTYPE) != MMTYPE)
		{

#if defined (__WHYNOT__)

			error (0, 0, "Wrong Message Type: Received %04X while waiting for %04X", LE16TOH (qualcomm->MMTYPE), MMTYPE);

#endif

			return (-1);
		}

#if FMI

		index = qualcomm->FMID >> 0 & 0x0F;
		if (!index)
		{
			total = qualcomm->FMID >> 4 & 0x0F;
			count = qualcomm->FMID >> 0 & 0x0F;
			if (memcmp (localcast, qualcomm->OUI, sizeof (qualcomm->OUI)))
			{

#if defined (__WHYNOT__)

				error (0, 0, "Wrong Message Vendor");

#endif

				return (-1);
			}
		}
		if (index != count)
		{

#if defined (__WHYNOT__)

			error (0, 0, "Message Fragment Out-of-order");

#endif

			return (-1);
		}
		if (count > total)
		{

#if defined (__WHYNOT__)

			error (0, 0, "Fragment Count Exceeds Total");

#endif

			return (-1);
		}
		count++;

#endif

	}
	return (0);
}


#endif

