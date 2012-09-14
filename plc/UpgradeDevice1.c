/*====================================================================*
 *  
 *   int UpgradeDevice1 (struct int6k * int6k);
 *
 *   int6k.h
 *  
 *   This plugin upgrades a device having NVRAM; runtime firmware must
 *   be running for this to work; NVM and PIB files in struct int6k
 *   must be opened before calling this plugin; this plugin is used by
 *   several program like int6k, int6kf and int6khost;
 *   
 *   See the Atheros HomePlug AV Firmware Technical Reference Manual 
 *   for more information;
 *
 *   This software and documentation is the property of Atheros 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Atheros assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Atheros customers may modify and distribute the software 
 *   without obligation to Atheros. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Atheros Powerline Toolkit for HomePlug AV; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <charles.maier@atheros.com>
 *
 *--------------------------------------------------------------------*/

#ifndef UPGRADEDEVICE1_SOURCE
#define UPGRADEDEVICE1_SOURCE

#include "../plc/plc.h"

int UpgradeDevice1 (struct plc * plc) 

{
	if (WriteNVM (plc)) 
	{
		return (-1);
	}
	if (WritePIB (plc)) 
	{
		return (-1);
	}
	if (FlashDevice1 (plc)) 
	{
		return (-1);
	}
	return (0);
}

#endif

