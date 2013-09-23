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
 *   signed FlashSoftloader (struct plc * plc, uint32_t options);
 *
 *   plc.h
 *
 *   This plugin upgrades a device having NVRAM; runtime firmware must
 *   be running for this to work; NVM and PIB files in struct plc must
 *   be opened before calling this plugin; this plugin is used by many
 *   programs;
 *
 *   This function is for panther/lynx devices although is should also
 *   work for AR7400 devices, too.
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FLASHSOFTLOADER_SOURCE
#define FLASHSOFTLOADER_SOURCE

#include "../plc/plc.h"

signed FlashSoftloader (struct plc * plc, uint32_t options)

{
	struct vs_module_spec vs_module_spec =
	{
		PLC_MODULEID_SOFTLOADER,
		0,
		0,
		0
	};
	ModuleSpec (&plc->CFG, &vs_module_spec);
	if (ModuleSession (plc, 1, &vs_module_spec))
	{
		return (-1);
	}
	if (ModuleWrite (plc, &plc->CFG, 0, &vs_module_spec))
	{
		return (-1);
	}
	if (ModuleCommit (plc, options))
	{
		return (-1);
	}
	return (0);
}


#endif

