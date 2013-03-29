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

/*====================================================================*"
 *
 *   chipset.c -
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2010-2012 by Qualcomm Atheros. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CHIPSET_SOURCE
#define CHIPSET_SOURCE

#include <string.h>
#include <errno.h>

#include "../tools/types.h"
#include "../tools/error.h"
#include "../tools/symbol.h"
#include "../plc/plc.h"

/*====================================================================*
 *
 *   char const * chipsetname (uint8_t MDEVICE) 
 *
 *   plc.h
 *
 *   return the ASCII name string associated with the DEVICEID value
 *   from VS_SW_VER.CNF messages; chipsetnames represent a family or
 *   class of device; 
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

char const * chipsetname (uint8_t MDEVICEID) 

{
	static const struct _type_ chipsets [] = 
	{
		{
			CHIPSET_UNKNOWN,
			"UNKNOWN"
		},
		{
			CHIPSET_INT6000,
			"INT6000"
		},
		{
			CHIPSET_INT6300,
			"INT6300"
		},
		{
			CHIPSET_INT6400,
			"INT6400"
		},
		{
			CHIPSET_AR7400,
			" AR7400"
		},
		{
			CHIPSET_AR6405,
			" AR6405"
		},
		{
			CHIPSET_PANTHER_LYNX,
			"PANTHER/LYNX"
		},
		{
			CHIPSET_QCA7450,
			"QCA7450"
		},
		{
			CHIPSET_QCA7451,
			"QCA7451"
		},
		{
			CHIPSET_QCA7420,
			"QCA7420"
		},
		{
			CHIPSET_QCA6410,
			"QCA6410"
		},
		{
			CHIPSET_QCA7000,
			"QCA7000"
		}
	};
	return (typename (chipsets, SIZEOF (chipsets), MDEVICEID, chipsets [0].name));
}


/*====================================================================*
 *
 *   void chipset (void const * memory);
 *
 *   plc.h
 *
 *   replace VS_SW_VER message DEVICEID field with correct device ID
 *   to compensate for bootloader chipset identification errors and
 *   the new chipset number scheme implemented with the panther/lynx
 *   firmware; do not ask for an explanation; just do it;
 *
 *   the basic assumption is that the firmware always tells the truth  
 *   but the bootloader does not; because of engineering changes, the
 *   firmware uses a different device identification scheme than that
 *   used by the bootloader and that information appears in different
 *   places depending on the source of the VS_SW_VER confirmation;
 *
 *   INT6000   0x01 / 0x01  0x00000042 / NA
 *   INT6300   0x01 / 0x02  0x00006300 / NA
 *   INT6400   0x03 / 0x03  0x00006400 / NA
 *    AR7400   0x03 / 0x04  0x00007400 / NA
 *    AR6405   0x03 / 0x05  0x00006400 / NA
 *   QCA7450   0x03 / 0x20  0x0F001D1A / NA
 *   QCA7420   0x06 / 0x20  0x001CFCFC 
 *   QCA6410   0x06 / 0x21  0x001B58EC
 *   QCA6411   0x06 / 0x21  0x001B58BC
 *   QCA7000   0x06 / 0x22  0x001B589C
 *
 *   some chipsets have have multiple IDENT numbers below; this is
 *   not an error;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

void chipset (void const * memory) 

{

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICEID;
		uint8_t MVERLENGTH;
		uint8_t MVERSION [254];
	}
	* confirm = (struct vs_sw_ver_confirm *) (memory);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	uint32_t identity;
	if (!strcmp ((char *)(confirm->MVERSION), "BootLoader")) 
	{
		identity = LE32TOH (* (uint32_t *)(&confirm->MVERSION [65]));
		if (confirm->MDEVICEID == CHIPSET_INT6000) 
		{
			if (identity == 0x00006300) 
			{
				confirm->MDEVICEID = CHIPSET_INT6300;
			}
		}
		else if (confirm->MDEVICEID == CHIPSET_INT6400) 
		{
			if (identity == 0x00006400) 
			{
				confirm->MDEVICEID = CHIPSET_INT6400;
			}
			else if (identity == 0x00007400) 
			{
				confirm->MDEVICEID = CHIPSET_AR7400;
			}
			else if (identity == 0x00001D1A) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7450;
			}
			else if (identity == 0x0F001D1A) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7450;
			}
			else if (identity == 0x0E001D1A) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7451;
			}
			else 
			{
				confirm->MDEVICEID = CHIPSET_UNKNOWN;
			}
		}
		else if (confirm->MDEVICEID == CHIPSET_AR6405) 
		{
			if (identity == 0x001CFC00) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7420;
			}
			else if (identity == 0x001B5800) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else 
			{
				confirm->MDEVICEID = CHIPSET_UNKNOWN;
			}
		}
		else if (confirm->MDEVICEID == CHIPSET_PANTHER_LYNX) 
		{
			if (identity == 0x001CFC00) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7420;
			}
			else if (identity == 0x001CFCFC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7420;
			}
			else if (identity == 0x001B5800) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else if (identity == 0x001B58EC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA6410;
			}
			else if (identity == 0x001B58BC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA6411;
			}
			else if (identity == 0x001B58DC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else if (identity == 0x001B589C) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else 
			{
				confirm->MDEVICEID = CHIPSET_UNKNOWN;
			}
		}
		else 
		{
			confirm->MDEVICEID = CHIPSET_UNKNOWN;
		}
	}
	else if (confirm->MDEVICEID == CHIPSET_INT6000) 
	{
	}
	else if (confirm->MDEVICEID == CHIPSET_INT6300) 
	{
	}
	else if (confirm->MDEVICEID == CHIPSET_INT6400) 
	{
	}
	else if (confirm->MDEVICEID == CHIPSET_AR7400) 
	{
	}
	else if (confirm->MDEVICEID == CHIPSET_AR6405) 
	{
	}
	else if (confirm->MDEVICEID == CHIPSET_QCA7420) 
	{
		identity = LE32TOH (* (uint32_t *)(&confirm->MVERSION [129]));
		if (identity == 0x00001D1A) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7450;
		}
		else if (identity == 0x0F001D1A) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7450;
		}
		else if (identity == 0x0E001D1A) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7451;
		}
		else 
		{
			identity = LE32TOH (* (uint32_t *)(&confirm->MVERSION [254]));
			if (identity == 0x001CFC00) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7420;
			}
			else	if (identity == 0x001CFCFC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7420;
			}
			else if (identity == 0x001B5800) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else if (identity == 0x001B589C) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else if (identity == 0x001B58DC) 
			{
				confirm->MDEVICEID = CHIPSET_QCA7000;
			}
			else 
			{
				confirm->MDEVICEID = CHIPSET_UNKNOWN;
			}
		}
	}
	else if (confirm->MDEVICEID == CHIPSET_QCA6410) 
	{
		identity = LE32TOH (* (uint32_t *)(&confirm->MVERSION [254]));
		if (identity == 0x001B58EC) 
		{
			confirm->MDEVICEID = CHIPSET_QCA6410;
		}
		else if (identity == 0x001B58BC) 
		{
			confirm->MDEVICEID = CHIPSET_QCA6411;
		}
		else 
		{
			confirm->MDEVICEID = CHIPSET_UNKNOWN;
		}
	}
	else if (confirm->MDEVICEID == CHIPSET_QCA7000) 
	{
		identity = LE32TOH (* (uint32_t *)(&confirm->MVERSION [254]));
		if (identity == 0x001B5800) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7000;
		}
		else if (identity == 0x001B58DC) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7000;
		}
		else if (identity == 0x001B589C) 
		{
			confirm->MDEVICEID = CHIPSET_QCA7000I;
		}
		else 
		{
			confirm->MDEVICEID = CHIPSET_UNKNOWN;
		}
	}
	else 
	{
		confirm->MDEVICEID = CHIPSET_UNKNOWN;
	}
	return;
}


#endif

