/*====================================================================*
 *
 *   Copyright (c) 2015 Qualcomm Atheros, Inc.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or 
 *   without modification, are permitted (subject to the limitations 
 *   in the disclaimer below) provided that the following conditions 
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials 
 *     provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros nor the names of 
 *     its contributors may be used to endorse or promote products 
 *     derived from this software without specific prior written 
 *     permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE 
 *   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE 
 *   COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 *   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   void putpwd (char const charset [], unsigned limit, unsigned alpha, unsigned group, char space);
 *
 *   keys.h
 *
 *   print new password on stdout;
 *
 *   charset is an array containing an alphabet of printable ASCII
 *   characters; limit is the charset size in bytes; alpha is the
 *   number of characters to be selected from the alphabet; group
 *   the bunching factor; space is the character used to separate
 *   character groups; 
 *
 *   characters are not grouped when group is zero or greater than
 *   or equal to alpha;
 *
 *   Contributors:
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *      Pouyan Sepehrdad <pouyans@qti.qualcomm.com>
 *      Ning Shang <nshang@qti.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PUTPWD_SOURCE
#define PUTPWD_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "../tools/types.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../tools/flags.h"
#include "../key/keys.h"

void putpwd (char const charset [], unsigned limit, unsigned alpha, unsigned group, char space)

{
	signed fd;
	if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
	{
		error (1, errno, "can't open /dev/urandom");
	}
	while (alpha--)
	{
		unsigned index;
		if (read (fd, &index, sizeof (index)) != sizeof (index))
		{
			error (1, errno, "can't read /dev/urandom");
		}	
		index &= 0x1F;
		putc ( charset [index % limit], stdout);
		if ((alpha) && (group) && !(alpha % group))
		{
			putc (space, stdout);
		}
	}
	close (fd);
	return;
}

#endif

