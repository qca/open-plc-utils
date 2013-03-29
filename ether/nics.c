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
 *   nics.c - network interface enumerator;
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
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <memory.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/error.h"
#include "../tools/memory.h"
#include "../ether/ether.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/error.c"
#include "../tools/hexdecode.c"
#include "../tools/decdecode.c"
#include "../tools/hexstring.c"
#include "../tools/decstring.c"
#include "../tools/error.c"
#endif

#ifndef MAKEFILE
#include "../ether/hostnics.c"
#endif

/*====================================================================*
 *
 *   void enumerate ();
 *
 *   enumerate host interface details on stdout;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static void enumerate () 

{
	struct nic nics [16];
	struct nic * nic;
	unsigned count = hostnics (nics, sizeof (nics) / sizeof (struct nic));
	for (nic = nics; count--; nic++) 
	{
		byte memory [ETHER_ADDR_LEN];
		char string [ETHER_ADDR_LEN * 3];
		memset (memory, 0x00, sizeof (memory));
		if (!memcmp (memory, nic->ethernet, sizeof (memory))) 
		{
			continue;
		}
		memset (memory, 0xFF, sizeof (memory));
		if (!memcmp (memory, nic->ethernet, sizeof (memory))) 
		{
			continue;
		}
		printf (" %d", nic->ifindex);
		printf (" %s", hexstring (string, sizeof (string), nic->ethernet, sizeof (nic->ethernet)));
		printf (" %s", decstring (string, sizeof (string), nic->internet, sizeof (nic->internet)));
		printf (" %s", nic->ifname);
		printf (" %s", nic->ifdesc);
		printf ("\n");
	}
	return;
}


/*====================================================================*
 *
 *   int main (int argc, char const * argv []);
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"",
		PUTOPTV_S_DIVINE,
		"Atheros Ethernet Interface Enumerator",
		(char const *) (0)
	};
	signed c;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc) 
	{
		error (1, ECANCELED, ERROR_TOOMANY);
	}
	enumerate ();
	return (0);
}

