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
 *   signed ChangeIdent (struct plc * plc);
 *
 *   plc.h
 * 
 *   read PIB file header; replace MAC, DAK and NMK; update preferred
 *   NID; re-write PIB file header and PIB file checksum; use the MAC,
 *   DAK and NMK values stored in struct plc;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef CHANGEIDENT_SOURCE
#define CHANGEIDENT_SOURCE

#include <unistd.h>
#include <stdint.h>

#include "../plc/plc.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../key/HPAVKey.h"
#include "../pib/pib.h"

signed ChangeIdent (struct plc * plc) 

{
	struct simple_pib simple_pib;
	memset (&simple_pib, 0, sizeof (simple_pib));
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (read (plc->PIB.file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib)) 
	{
		error (1, errno, FILE_CANTREAD, plc->PIB.name);
	}
	memcpy (simple_pib.MAC, plc->MAC, sizeof (simple_pib.MAC));
	memcpy (simple_pib.DAK, plc->DAK, sizeof (simple_pib.DAK));
	memcpy (simple_pib.NMK, plc->NMK, sizeof (simple_pib.NMK));
	{
		uint8_t level = simple_pib.PreferredNID [HPAVKEY_NID_LEN-1] >> 4;
		HPAVKeyNID (simple_pib.PreferredNID, simple_pib.NMK, level & 1);
	}
	simple_pib.CHECKSUM = 0;
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (write (plc->PIB.file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib)) 
	{
		error (1, errno, FILE_CANTSAVE, plc->PIB.name);
	}
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	simple_pib.CHECKSUM = fdchecksum32 (plc->PIB.file, LE16TOH (simple_pib.PIBLENGTH), 0);
	if (lseek (plc->PIB.file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, plc->PIB.name);
	}
	if (write (plc->PIB.file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib)) 
	{
		error (1, errno, FILE_CANTSAVE, plc->PIB.name);
	}
	return (0);
}


#endif

