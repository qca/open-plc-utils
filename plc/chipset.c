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

#ifndef CHIPSET_SOURCE
#define CHIPSET_SOURCE

#include "../plc/plc.h"
#include "../tools/types.h"
#include "../tools/symbol.h"

/*====================================================================*
 *
 *   char const * chipsetname (uint8_t MDEVICE)
 *
 *   plc.h
 *
 *   return the ASCII name string associated with the MDEVICE_CLASS
 *   field in the VS_SW_VER.CNF message; this field represents the
 *   chipset family or class of device;
 *
 *   the MDEVICE_CLASS field was named MDEVICEID at one time;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

char const * chipsetname (uint8_t MDEVICE_CLASS)

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
	return (typename (chipsets, SIZEOF (chipsets), MDEVICE_CLASS, chipsets [0].name));
}

/*====================================================================*
 *
 *   void chipset (void const * memory);
 *
 *   Chipset.h
 *
 *   replace VS_SW_VER message MDEVICE_CLASS field with correct value;
 *   the MDEVICE_CLASS field was named MDEVICEID at one time;
 *
 *   Atheros chipsets are identified by code in the VS_SW_VER vendor
 *   specific management message; the chipset [] vector translates a
 *   chipset code to a chipset name;
 *
 *   the basic assumption is that the firmware always tells the truth
 *   but the bootrom does not; because of engineering changes, the
 *   firmware uses a different device identification scheme than that
 *   used by the bootrom and that information appears in different
 *   locations depending on the source of the VS_SW_VER confirmation;
 *   see the Programmer's Guide for more information.
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
 *   QCA7000   0x06 / 0x22  0x001B58DC
 *   QCA7005   0x06 / 0x22  0x001B587C
 *
 *   some chipsets have have multiple IDENT field values; this is
 *   not an error; there may be multiple versions of a chipset;
 *
 *--------------------------------------------------------------------*/

void chipset (void const * memory)

{

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_sw_ver_confirm
	{
		struct ethernet_hdr ethernet;
		struct qualcomm_hdr qualcomm;
		uint8_t MSTATUS;
		uint8_t MDEVICE_CLASS;
		uint8_t MVERLENGTH;
		char MVERSION [254];
	}
	* confirm = (struct vs_sw_ver_confirm *) (memory);
	typedef struct __packed
	{
		uint32_t IDENT;
		uint8_t DEVICE_CLASS;
		uint8_t DEVICE;
	}
	tDevMap;
	struct __packed tDevClass
	{
		uint8_t RESERVED;
		uint32_t IDENT;
		uint32_t STEP_NUMBER;
	}
	* pDevClass = (struct tDevClass *) (& confirm->MVERSION [64]);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	tDevMap bootrom [] =
	{
		{
			0x00000042,
			0x01,
			CHIPSET_INT6000
		},
		{
			0x00006300,
			0x01,
			CHIPSET_INT6300
		},
		{
			0x00006400,
			0x03,
			CHIPSET_INT6400
		},
		{
			0x00007400,
			0x03,
			CHIPSET_AR7400
		},
		{
			0x0F001D1A,
			0x03,
			CHIPSET_QCA7450
		},
		{
			0x0E001D1A,
			0x03,
			CHIPSET_QCA7451
		},
		{
			0x001CFC00,
			0x05,
			CHIPSET_QCA7420
		},
		{
			0x001CFCFC,
			0x05,
			CHIPSET_QCA7420
		},
		{
			0x001CFCFC,
			0x06,
			CHIPSET_QCA7420
		},
		{
			0x001B58EC,
			0x06,
			CHIPSET_QCA6410
		},
		{
			0x001B58BC,
			0x06,
			CHIPSET_QCA6411
		},
		{
			0x001B58DC,
			0x06,
			CHIPSET_QCA7000
		},
		{
			0x001B587C,
			0x06,
			CHIPSET_QCA7000
		}
	};
	tDevMap firmware [] =
	{
		{
			0x00000000,
			0x01,
			CHIPSET_INT6000
		},
		{
			0x00000000,
			0x02,
			CHIPSET_INT6300
		},
		{
			0x00000000,
			0x03,
			CHIPSET_INT6400
		},
		{
			0x00000000,
			0x05,
			CHIPSET_AR6405
		},
		{
			0x00000000,
			0x04,
			CHIPSET_AR7400
		},
		{
			0x0F001D1A,
			0x20,
			CHIPSET_QCA7450
		},
		{
			0x0E001D1A,
			0x20,
			CHIPSET_QCA7451
		},
		{
			0x001CFCFC,
			0x20,
			CHIPSET_QCA7420
		},
		{
			0x001B58EC,
			0x21,
			CHIPSET_QCA6410
		},
		{
			0x001B58BC,
			0x21,
			CHIPSET_QCA6411
		},
		{
			0x001B58DC,
			0x22,
			CHIPSET_QCA7000
		}
	};
	unsigned chip;
	if (!strcmp (confirm->MVERSION, "BootLoader"))
	{
		for (chip = 0; chip < SIZEOF (bootrom); chip++)
		{
			if (bootrom [chip].DEVICE_CLASS != confirm->MDEVICE_CLASS)
			{
				continue;
			}
			if (bootrom [chip].IDENT != LE32TOH (pDevClass->IDENT))
			{
				continue;
			}
			confirm->MDEVICE_CLASS = bootrom [chip].DEVICE;
			return;
		}
	}
	else
	{
		for (chip = 0; chip < SIZEOF (firmware); chip++)
		{
			if (firmware [chip].DEVICE_CLASS != confirm->MDEVICE_CLASS)
			{
				continue;
			}
			if (firmware [chip].IDENT <= 5)
			{
				confirm->MDEVICE_CLASS = firmware [chip].DEVICE;
				return;
			}
			pDevClass = (struct tDevClass *) (& confirm->MVERSION [65]);
			if (firmware [chip].IDENT == LE32TOH (pDevClass->IDENT))
			{
				confirm->MDEVICE_CLASS = firmware [chip].DEVICE;
				return;
			}
			pDevClass = (struct tDevClass *) (& confirm->MVERSION [129]);
			if (firmware [chip].IDENT == LE32TOH (pDevClass->IDENT))
			{
				confirm->MDEVICE_CLASS = firmware [chip].DEVICE;
				return;
			}
			pDevClass = (struct tDevClass *) (& confirm->MVERSION [254]);
			if (firmware [chip].IDENT == LE32TOH (pDevClass->IDENT))
			{
				confirm->MDEVICE_CLASS = firmware [chip].DEVICE;
				return;
			}
		}
	}
	return;
}

#endif



