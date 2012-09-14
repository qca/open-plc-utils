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
 *   signed TopologyPrint (struct plcnetworks * plctopology);
 *   
 *   plc.h
 * 
 *   print plctopology structure in human readable format on stdout;
 *
 *.  Atheros Powerline Toolkit for HomePlug AV;
 *:  Published 2010 by Atheros Communications; ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef TOPOLOGYPRINT_SOURCE
#define TOPOLOGYPRINT_SOURCE

#include <stdio.h>

#include "../plc/plc.h"
#include "../tools/memory.h"

signed PLCTopologyPrint (struct plctopology * plctopology) 

{
	signed plcnetworks = plctopology->plcnetworks;
	struct plcnetwork * plcnetwork = (struct plcnetwork *)(&plctopology->plcnetwork);
	while (plcnetworks--) 
	{
		signed plcstations = plcnetwork->plcstations;
		struct plcstation * plcstation = (struct plcstation *)(&plcnetwork->plcstation);
		while (plcstations--) 
		{
			char address [ETHER_ADDR_LEN * 3];
			printf ("%s ", plcstation->LOC? "LOC": "REM");
			printf ("%s ", plcstation->CCO? "CCO": "STA");
			printf ("%03d ", plcstation->TEI);
			printf ("%s ", hexstring (address, sizeof (address), plcstation->MAC, sizeof (plcstation->MAC)));
			printf ("%s ", hexstring (address, sizeof (address), plcstation->BDA, sizeof (plcstation->BDA)));
			printf ("%03d ", plcstation->RX);
			printf ("%03d ", plcstation->TX);
			printf ("%s ", plcstation->hardware);
			printf ("%s ", plcstation->firmware);
			printf ("\n");
			plcstation++;
		}
		plcnetwork = (struct plcnetwork *)(plcstation);
	}

#if 0

	printf ("\n");

#endif

	return (0);
}


#endif

