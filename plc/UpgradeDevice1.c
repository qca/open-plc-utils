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
 *   int UpgradeDevice1 (struct int6k * int6k);
 *
 *   int6k.h
 *
 *   This plugin upgrades a device having NVRAM; runtime firmware must
 *   be running for this to work; NVM and PIB files in struct int6k
 *   must be opened before calling this plugin; this plugin is used by
 *   several program like int6k, int6kf and int6khost;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
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

