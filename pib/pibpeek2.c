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
 *   signed pibpeek2 (void const * memory);
 *
 *   pib.h
 *
 *   print Panther/Lynx PIB identity information on stdout;
 *
 *
 *   Contributor(s):
 *		Charles Maier
 *
 *--------------------------------------------------------------------*/

#ifndef PIBPEEK2_SOURCE
#define PIBPEEK2_SOURCE

#include <stdio.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../key/HPAVKey.h"
#include "../key/keys.h"
#include "../pib/pib.h"

static char const * CCoMode2 [] =

{
	"Auto",
	"Never",
	"Always",
	"User",
	"Covert",
	"Unknown"
};

static char const * MDURole2 [] =

{
	"Slave",
	"Master"
};

signed pibpeek2 (void const * memory)

{
	extern const struct key keys [KEYS];
	struct PIB3_0 * PIB = (struct PIB3_0 *)(memory);
	char buffer [HPAVKEY_SHA_LEN * 3];
	size_t key;
	printf ("\tPIB %d-%d %d bytes\n", PIB->VersionHeader.FWVersion, PIB->VersionHeader.PIBVersion, LE16TOH (PIB->VersionHeader.PIBLength));
	printf ("\tMAC %s\n", hexstring (buffer, sizeof (buffer), PIB->LocalDeviceConfig.MAC, sizeof (PIB->LocalDeviceConfig.MAC)));
	printf ("\tDAK %s", hexstring (buffer, sizeof (buffer), PIB->LocalDeviceConfig.DAK, sizeof (PIB->LocalDeviceConfig.DAK)));
	for (key = 0; key < KEYS; key++)
	{
		if (!memcmp (keys [key].DAK, PIB->LocalDeviceConfig.DAK, HPAVKEY_DAK_LEN))
		{
			printf (" (%s)", keys [key].phrase);
			break;
		}
	}
	printf ("\n");
	printf ("\tNMK %s", hexstring (buffer, sizeof (buffer), PIB->LocalDeviceConfig.NMK, sizeof (PIB->LocalDeviceConfig.NMK)));
	for (key = 0; key < KEYS; key++)
	{
		if (!memcmp (keys [key].NMK, PIB->LocalDeviceConfig.NMK, HPAVKEY_NMK_LEN))
		{
			printf (" (%s)", keys [key].phrase);
			break;
		}
	}
	printf ("\n");
	printf ("\tNID %s\n", hexstring (buffer, sizeof (buffer), PIB->LocalDeviceConfig.PreferredNID, sizeof (PIB->LocalDeviceConfig.PreferredNID)));
	printf ("\tNET %s\n", PIB->LocalDeviceConfig.NET);
	printf ("\tMFG %s\n", PIB->LocalDeviceConfig.MFG);
	printf ("\tUSR %s\n", PIB->LocalDeviceConfig.USR);
	printf ("\tCCo %s\n", CCoMode2 [PIB->LocalDeviceConfig.CCoSelection > SIZEOF (CCoMode2)-1?SIZEOF (CCoMode2)-1:PIB->LocalDeviceConfig.CCoSelection]);
	printf ("\tMDU %s\n", PIB->LocalDeviceConfig.MDUConfiguration? MDURole2 [PIB->LocalDeviceConfig.MDURole & 1]: "N/A");
	return (0);
}


#endif

