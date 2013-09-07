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
 *   void MMEPeek (void const * memory, size_t length, FILE *fp);
 *
 *   mme.h
 *
 *   print a HomePlug AV frame header on stdout in human readable
 *   format;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MMPEEK_SOURCE
#define MMPEEK_SOURCE

#include <stdio.h>
#include <stdint.h>

#include "../tools/memory.h"
#include "../mme/mme.h"

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6 /* normally defined in ethernet.h or if_ether.h */
#endif

void MMEPeek (void const * memory, size_t extent, FILE *fp)

{
	char address [ETHER_ADDR_LEN * 3];
	struct message * message = (struct message *)(memory);
	fprintf (fp, "ODA=%s ", hexstring (address, sizeof (address), message->ethernet.ODA, sizeof (message->ethernet.ODA)));
	fprintf (fp, "OSA=%s ", hexstring (address, sizeof (address), message->ethernet.OSA, sizeof (message->ethernet.OSA)));
	fprintf (fp, "MTYPE=%04X ", ntohs (message->ethernet.MTYPE));
	if (ntohs (message->ethernet.MTYPE) == ETH_P_HPAV)
	{
		struct homeplug_std * homeplug = (struct homeplug_std *)(&message->content);
		uint16_t mmtype = LE16TOH (homeplug->MMTYPE);
		if (mmtype < MMTYPE_VS)
		{
			if (homeplug->MMV == 0x00)
			{
				struct homeplug_std * homeplug = (struct homeplug_std *)(&message->content);
				mmtype = LE16TOH (homeplug->MMTYPE);
				fprintf (fp, "MMV=%02X ", homeplug->MMV);
				fprintf (fp, "MMTYPE=%04X ", mmtype);
				fprintf (fp, "%s.%s\n", MMEName (mmtype), MMEMode (mmtype));
				return;
			}
			if (homeplug->MMV == 0x01)
			{
				struct homeplug_fmi * homeplug = (struct homeplug_fmi *)(&message->content);
				mmtype = LE16TOH (homeplug->MMTYPE);
				fprintf (fp, "MMV=%02X ", homeplug->MMV);
				fprintf (fp, "MMTYPE=%04X ", mmtype);
				fprintf (fp, "FMID=%02X ", homeplug->FMID);
				fprintf (fp, "FMSN=%02X ", homeplug->FMSN);
				fprintf (fp, "%s.%s\n", MMEName (mmtype), MMEMode (mmtype));
				return;
			}
		}
		if (mmtype < MMTYPE_XX)
		{
			if (homeplug->MMV == 0x00)
			{
				struct qualcomm_std * qualcomm = (struct qualcomm_std *)(&message->content);
				mmtype = LE16TOH (qualcomm->MMTYPE);
				fprintf (fp, "MMV=%02X ", qualcomm->MMV);
				fprintf (fp, "MMTYPE=%04X ", mmtype);
				fprintf (fp, "OUI=%s ", hexstring (address, sizeof (address), qualcomm->OUI, sizeof (qualcomm->OUI)));
				fprintf (fp, "%s.%s\n", MMEName (mmtype), MMEMode (mmtype));
				return;
			}
			if (homeplug->MMV == 0x01)
			{
				struct qualcomm_fmi * qualcomm = (struct qualcomm_fmi *)(&message->content);
				mmtype = LE16TOH (qualcomm->MMTYPE);
				fprintf (fp, "MMV=%02X ", qualcomm->MMV);
				fprintf (fp, "MMTYPE=%04X ", mmtype);
				fprintf (fp, "FMID=%02X ", qualcomm->FMID);
				fprintf (fp, "FMSN=%02X ", qualcomm->FMSN);
				fprintf (fp, "OUI=%s ", hexstring (address, sizeof (address), qualcomm->OUI, sizeof (qualcomm->OUI)));
				fprintf (fp, "%s.%s\n", MMEName (mmtype), MMEMode (mmtype));
				return;
			}
		}
	}
	fprintf (fp, "UNKNOWN_MESSAGE_TYPE\n");
	return;
}


#endif

