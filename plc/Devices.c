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
 *   Devices.c - device address synonym tables
 *
 *   plc.h
 *
 *   some device addresses are so common or significant that we give
 *   them names; users can enter the name instead of the address; all
 *   toolkit programs reference this table via function synonym();
 *
 *   function synonym() uses a binary search to lookup symbols and so
 *   they must be entered in alphabetical order or lookup errors will 
 *   occur;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef DEVICES_SOURCE
#define DEVICES_SOURCE

/*====================================================================*
 *   custom include files;
 *--------------------------------------------------------------------*/

#include "../plc/plc.h"
#include "../mme/mme.h"
#include "../tools/types.h"

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

struct _term_ const devices [PLCDEVICES] = 

{
	{
		"all",
		BROADCAST
	},
	{
		"broadcast",
		BROADCAST
	},
	{
		"local",
		LOCALCAST
	}
};

uint8_t const broadcast [ETHER_ADDR_LEN] = 

{
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF
};

uint8_t const localcast [ETHER_ADDR_LEN] = 

{
	0x00,
	0xB0,
	0x52,
	0x00,
	0x00,
	0x01
};


/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#endif

