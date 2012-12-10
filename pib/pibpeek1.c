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
 *   signed pibpeek1 (void const * memory);
 *
 *   pib.h
 *
 *   print PIB identity information on stdout; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit.
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED.
 *;  For demonstration and evaluation only. Not for production use.
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PIBPEEK1_SOURCE
#define PIBPEEK1_SOURCE

#include <stdio.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../key/HPAVKey.h"
#include "../key/keys.h"
#include "../pib/pib.h"

static char const * CCoMode [] = 

{
	"Auto",
	"Never",
	"Always",
	"User"
};

static char const * MDURole [] = 

{
	"Slave",
	"Master"
};

signed pibpeek1 (void const * memory) 

{
	extern const struct key keys [KEYS];
	extern char const * CCoMode [];
	extern char const * MDURole [];
	struct PIB1 * PIB = (struct PIB1 *)(memory);
	char buffer [HPAVKEY_SHA_LEN * 3];
	size_t key;
	if (PIB->FWVersion == 1) 
	{
		if (PIB->PIBVersion == 2) 
		{
			struct PIB1_2 * PIB = (struct PIB1_2 *)(memory);
			printf ("\tPIB %d-%d\n", PIB->FWVersion, PIB->PIBVersion);
			printf ("\tMAC %s\n", hexstring (buffer, sizeof (buffer), PIB->MAC, sizeof (PIB->MAC)));
			printf ("\tDAK %s", hexstring (buffer, sizeof (buffer), PIB->DAK, sizeof (PIB->DAK)));
			for (key = 0; key < KEYS; key++) 
			{
				if (!memcmp (keys [key].DAK, PIB->DAK, HPAVKEY_DAK_LEN)) 
				{
					printf (" (%s)", keys [key].phrase);
					break;
				}
			}
			printf ("\n");
			printf ("\tNMK %s", hexstring (buffer, sizeof (buffer), PIB->NMK, sizeof (PIB->NMK)));
			for (key = 0; key < KEYS; key++) 
			{
				if (!memcmp (keys [key].NMK, PIB->NMK, HPAVKEY_NMK_LEN)) 
				{
					printf (" (%s)", keys [key].phrase);
					break;
				}
			}
			printf ("\n");
			printf ("\tFLG %s\n", hexstring (buffer, sizeof (buffer), (uint8_t *)(&PIB->FLG), sizeof (PIB->FLG)));
			return (0);
		}
		else if (PIB->PIBVersion == 3) 
		{
			struct PIB1_3 * PIB = (struct PIB1_3 *)(memory);
			printf ("\tPIB %d-%d\n", PIB->FWVersion, PIB->PIBVersion);
			printf ("\tMAC %s\n", hexstring (buffer, sizeof (buffer), PIB->MAC, sizeof (PIB->MAC)));
			printf ("\tDAK %s", hexstring (buffer, sizeof (buffer), PIB->DAK, sizeof (PIB->DAK)));
			for (key = 0; key < KEYS; key++) 
			{
				if (!memcmp (keys [key].DAK, PIB->DAK, HPAVKEY_DAK_LEN)) 
				{
					printf (" (%s)", keys [key].phrase);
					break;
				}
			}
			printf ("\n");
			printf ("\tNMK %s", hexstring (buffer, sizeof (buffer), PIB->NMK, sizeof (PIB->NMK)));
			for (key = 0; key < KEYS; key++) 
			{
				if (!memcmp (keys [key].NMK, PIB->NMK, HPAVKEY_NMK_LEN)) 
				{
					printf (" (%s)", keys [key].phrase);
					break;
				}
			}
			printf ("\n");
			printf ("\tNET \"%s\"\n", PIB->NET);
			printf ("\tMFG \"%s\"\n", PIB->MFG);
			printf ("\tUSR \"%s\"\n", PIB->USR);
			return (0);
		}
		else if (PIB->PIBVersion >= 4) 
		{
			struct PIB1_4 * PIB = (struct PIB1_4 *)(memory);
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
			printf ("\tNET %s\n", PIB->LocalDeviceConfig.NET);
			printf ("\tMFG %s\n", PIB->LocalDeviceConfig.MFG);
			printf ("\tUSR %s\n", PIB->LocalDeviceConfig.USR);
			return (0);
		}
	}
	else if (PIB->FWVersion == 2) 
	{
		struct PIB2_0 * PIB = (struct PIB2_0 *)(memory);
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
		return (0);
	}
	else if (PIB->FWVersion < 8) 
	{
		struct PIB3_0 * PIB = (struct PIB3_0 *)(memory);
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
		printf ("\tCCo %s\n", CCoMode [PIB->LocalDeviceConfig.CCoSelection]);
		printf ("\tMDU %s\n", PIB->LocalDeviceConfig.MDUConfiguration? MDURole [PIB->LocalDeviceConfig.MDURole & 1]: "N/A");
		return (0);
	}
	printf ("\tPIB %d-%d %d (unsupported)\n", PIB->FWVersion, PIB->PIBVersion, LE16TOH (PIB->PIBLength));
	return (1);
}


#endif

