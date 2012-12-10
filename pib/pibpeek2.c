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
 *   signed pibpeek2 (void const * memory);
 *
 *   pib.h
 *
 *   print Panther/Lynx PIB identity information on stdout; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *   Contributor(s):
 *		Charles Maier <cmaier@qualcomm.com>
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
	"User"
};

static char const * MDURole2 [] = 

{
	"Slave",
	"Master"
};

signed pibpeek2 (void const * memory) 

{
	extern const struct key keys [KEYS];
	extern char const * CCoMode2 [];
	extern char const * MDURole2 [];
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
	printf ("\tCCo %s\n", CCoMode2 [PIB->LocalDeviceConfig.CCoSelection]);
	printf ("\tMDU %s\n", PIB->LocalDeviceConfig.MDUConfiguration? MDURole2 [PIB->LocalDeviceConfig.MDURole & 1]: "N/A");
	return (0);
}


#endif

