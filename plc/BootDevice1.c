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
 *   signed BootDevice1 (struct plc * plc);
 *
 *   plc.h
 *  
 *   boot a powerline device by writing parameters and firmware into 
 *   volatile memory and starting execution; volatile memory must be 
 *   initialized and configured before calling this function;
 *
 *   this function is identical to BootDevice2 but calls functions 
 *   that expect the older image file format;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef BOOTDEVICE1_SOURCE
#define BOOTDEVICE1_SOURCE

#include <memory.h>

#include "../plc/plc.h"

signed BootDevice1 (struct plc * plc) 

{
	char string [PLC_VERSION_STRING];
	if (BootParameters1 (plc)) 
	{
		return (-1);
	}
	if (BootFirmware1 (plc)) 
	{
		return (-1);
	}
	if (WaitForStart (plc, string, sizeof (string))) 
	{
		return (-1);
	}
	Confirm (plc, "%s is running", string);
	return (0);
}


#endif

