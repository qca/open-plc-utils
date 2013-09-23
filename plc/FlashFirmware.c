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
 *   signed FlashFirmware (struct plc * plc, uint32_t options);
 *
 *   plc.h
 *
 *   write powerline parameters and firmware into flash memory using
 *   VS_MODULE_OPERATION messages; force flash and do not reset;
 *
 *   struct vs_module_spec is defined in plc.h;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FLASHFIRMWARE_SOURCE
#define FLASHFIRMWARE_SOURCE

#include "../plc/plc.h"

signed FlashFirmware (struct plc * plc, uint32_t options)

{
	struct vs_module_spec vs_module_spec [] =
	{
		{
			PLC_MODULEID_PARAMETERS,
			0,
			0,
			0
		},
		{
			PLC_MODULEID_FIRMWARE,
			0,
			0,
			0
		}
	};
	ModuleSpec (&plc->PIB, &vs_module_spec [0]);
	ModuleSpec (&plc->NVM, &vs_module_spec [1]);
	if (ModuleSession (plc, (sizeof (vs_module_spec) / sizeof (struct vs_module_spec)), vs_module_spec))
	{
		return (-1);
	}
	if (ModuleWrite (plc, &plc->PIB, 0, &vs_module_spec [0]))
	{
		return (-1);
	}
	if (ModuleWrite (plc, &plc->NVM, 1, &vs_module_spec [1]))
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

