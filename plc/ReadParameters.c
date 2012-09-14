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
 *   signed ReadParameters (struct plc * plc); 
 *
 *   plc.h
 *
 *   Read the PIB Image from an INT6000 using as many VS_RD_MOD
 *   messages as needed; Write image blocks to file as they are 
 *   read;
 *
 *   the objective here is to read the PIB module in 1024 byte blocks 
 *   until the module offset equals or exceeds the PIB length; we do
 *   not know the PIB length until we have read the header contained
 *   in the first block at OFFSET 0;
 *
 *   MOFFSET and MLENGTH fields occupy different offsets in REQ and CNF 
 *   messages; we exploit that by initializing them using CNF message
 *   offsets then copying values into REQ message offsets before each
 *   read; this works because confirmation messages always return all 
 *   the data we request or nothing at all;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READPARAMETERS_SOURCE
#define READPARAMETERS_SOURCE

#include "../plc/plc.h" 

signed ReadParameters (struct plc * plc) 

{
	return (PLCSelect (plc, ReadParameters1, ReadParameters2));
}


#endif

