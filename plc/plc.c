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
 *   plc.c - Qualcomm Atheros Powerline Data Structure;
 *
 *   this structure contains information needed to perform various
 *   operations on Qualcomm Atheros powerline devices; it represents
 *   one instance of a device and could easily be converted to an
 *   object in the future;
 *
 *   this structure points to a channel and a message structure;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Alex Vasquez <avasquez@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PLC_SOURCE
#define PLC_SOURCE

#include "../plc/plc.h"
#include "../ether/channel.h"

struct plc plc =

{
	(struct channel *) (&channel),
	(struct message *) (0),
	(void *) (0),
	0,

/*
 * Local Device Address (LDA) buffer needed by all operations;
 */

	{
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	},

/*
 * Remote Device Address (RDA) buffer needed by selected operations;
 */

	{
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	},

/*
 *   Network Membership Key (NMK) needed by VS_SET_KEY operations;
 *   The default is an encrypted version of password "HomePlugAV";
 */

	{
		0x50,
		0xD3,
		0xE4,
		0x93,
		0x3F,
		0x85,
		0x5B,
		0x70,
		0x40,
		0x78,
		0x4D,
		0xF8,
		0x15,
		0xAA,
		0x8D,
		0xB7
	},

/*
 *   Device Access Key (DAK) needed for VS_SET_KEY operations;
 *   The default is an encrypted version of password "HomePlugAV";
 */

	{
		0x68,
		0x9F,
		0x07,
		0x4B,
		0x8B,
		0x02,
		0x75,
		0xA2,
		0x71,
		0x0B,
		0x0B,
		0x57,
		0x79,
		0xAD,
		0x16,
		0x30
	},

/*
 * struct _file_ CFG; MAC software will be read from this file and
 * written to RAM for each device specified on the command line;
 * some tools use this file for the panther/lynx softloader;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ cfg; SDRAM configuration will be read from flash
 * on the specified device and written to this file;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ NVM; runtime firmware will be read from this file
 * and written to RAM for each device specified on the command line;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ nvm; MAC software will be read from SDRAM on the
 * specified device and written to this file; interlocks elsewhere
 * in the code should prevent this file from being overwritten
 * multiple times, by accident, and ensure that it is created before
 * and new MAC software is written to device RAM;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ PIB; PIB information will be read from this file
 * and written to RAM for each device specified on the command line;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ pib; PIB information will be read from SDRAM on
 * the specified device and written to this file; interlocks elsewhere
 * in the code should prevent this file from being overwritten
 * multiple times, by accident, and ensure that it is created before
 * any new PIB software is written to device RAM;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ XML; optional XML PIB edit instructions; this is an
 * advanced feature;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ rpt; Watchdog Report data will be read from the device
 * and written to this file;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 * struct _file_ socket;
 */

	{
		(file_t) (-1),
		(char const *)(0)
	},

/*
 *   miscellaneous small integers used as needed when arguments
 *   are required for an MME;
 */

	HARDWAREID,
	SOFTWAREID,
	PLCSESSION,
	HOSTACTION,
	SECTORCODE,
	MODULECODE,
	PUSHBUTTON,
	READACTION,
	PLCOUPLING,
	FLASH_SIZE,

/*
 *   various boolean flags; bailout causes exit on first failure;
 *   timeout indicates a real timeout; count is the loop counter;
 *   pause is a loop wait timer; values are declared as constants
 *   above;
 */

	PLC_STATE,
	PLC_TIMER,
	PLC_SLEEP,
	PLC_COUNT,
	PLC_INDEX,
	PLC_FLAGS,
};


#endif

