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
 *   signed FlashDevice2 (struct plc * plc);
 *
 *   plc.h
 *
 *   flash a panther/lynx device; the flash sequence is determined
 *   by plc file descriptors that are valid;
 *
 *   the contidional statements uses here restricts flash options
 *   as follows:
 *
 *   1. Softloader only
 *   2. Firmware and Parameters together;
 *   3. Parameters only.
 *
 *   Notice that we do not allow a firmware only flash;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef FLASHDEVICE2_SOURCE
#define FLASHDEVICE2_SOURCE

#include "../tools/flags.h"
#include "../plc/plc.h"

signed FlashDevice2 (struct plc * plc)

{
	uint32_t options = (PLC_COMMIT_FORCE | PLC_COMMIT_NORESET | PLC_COMMIT_FACTPIB);
	if (plc->CFG.file != -1)
	{
		if (FlashSoftloader (plc, options))
		{
			return (-1);
		}
	}
	if ((plc->NVM.file != -1) && (plc->PIB.file != -1))
	{
		if (FlashFirmware (plc, options))
		{
			return (-1);
		}
		return (0);
	}
	if (plc->PIB.file != -1)
	{
		if (FlashParameters (plc, options))
		{
			return (-1);
		}
		return (0);
	}
	return (0);
}


#endif

