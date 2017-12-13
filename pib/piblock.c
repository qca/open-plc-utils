/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros, Inc.
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


#ifndef PIBLOCK_SOURCE
#define PIBLOCK_SOURCE

#include <unistd.h>
#include <errno.h>

#include "../tools/error.h"
#include "../tools/files.h"
#include "../pib/pib.h"
#include "../plc/plc.h"

/*====================================================================*
 *
 *   signed pibchain2 (struct _file_ * file);
 *
 *   recalculates the checksum for a panther / lynx PIB file
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *  Christian Aurich <christian.aurich@i2se.com>
 *
 *--------------------------------------------------------------------*/

static signed pibchain2 (struct _file_ const * file)

{
	unsigned module = 0;
	struct nvm_header2 nvm_header;
	uint32_t prev = ~0;
	uint32_t next = 0;
	if (lseek (file->file, 0, SEEK_SET))
	{
		error (1, errno, NVM_IMG_CHECKSUM, file->name, module);
	}
	do
	{
		if (read (file->file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header))
		{
			error (1, errno, NVM_HDR_CANTREAD, file->name, module);
		}
		if (LE16TOH (nvm_header.MajorVersion) != 1)
		{
			error (1, errno, NVM_HDR_VERSION, file->name, module);
		}
		if (LE16TOH (nvm_header.MinorVersion) != 1)
		{
			error (1, errno, NVM_HDR_VERSION, file->name, module);
		}
		if (checksum32 (&nvm_header, sizeof (nvm_header), 0))
		{
			error (1, errno, NVM_HDR_CHECKSUM, file->name, module);
		}
		if (LE32TOH (nvm_header.PrevHeader) != prev)
		{
			error (1, errno, NVM_HDR_LINK, file->name, module);
		}
		if (LE32TOH (nvm_header.ImageType) == NVM_IMAGE_PIB)
		{
			nvm_header.ImageChecksum = fdchecksum32 (file->file, LE32TOH (nvm_header.ImageLength), 0);
			if (lseek (file->file, (off_t)(0) - LE32TOH (nvm_header.ImageLength), SEEK_CUR) == -1)
			{
				error (1, errno, FILE_CANTHOME, file->name);
			}
			nvm_header.HeaderChecksum = checksum32 (&nvm_header, sizeof (nvm_header), nvm_header.HeaderChecksum);
			if (lseek (file->file, (off_t)(0) - sizeof (nvm_header), SEEK_CUR) == -1)
			{
				error (1, errno, FILE_CANTHOME, file->name);
			}
			if (write (file->file, &nvm_header, sizeof (nvm_header)) != sizeof (nvm_header))
			{
				error (1, errno, FILE_CANTSAVE, file->name);
			}
			if (lseek (file->file, (off_t)(0) - sizeof (nvm_header), SEEK_CUR) == -1)
			{
				error (1, errno, FILE_CANTHOME, file->name);
			}
			break;
		}
		if (fdchecksum32 (file->file, LE32TOH (nvm_header.ImageLength), nvm_header.ImageChecksum))
		{
			error (1, errno, NVM_IMG_CHECKSUM, file->name, module);
		}
		prev = next;
		next = LE32TOH (nvm_header.NextHeader);
		module++;
	}
	while (~nvm_header.NextHeader);
	return (0);
}

/*====================================================================*
 *
 *   signed piblock (struct _file_ const * pib);
 *
 *   recompute and update the checksum on a thunderbolt or lightning
 *   PIB file where the checksum is located inside the PIB;
 *
 *
 *   extended to be able to update the checksum for panther/lynx PIB
 *   files;
 *
 *
 *--------------------------------------------------------------------*/


signed piblock (struct _file_ const * file)

{
	struct simple_pib simple_pib;
	if (lseek (file->file, 0, SEEK_SET))
	{
		error (1, errno, "1 " FILE_CANTHOME, file->name);
	}
	if (read (file->file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib))
	{
		error (1, errno, FILE_CANTREAD, file->name);
	}
	if (lseek (file->file, 0, SEEK_SET))
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}


  if (simple_pib.PIBVERSION == 0x0001 && simple_pib.RESERVED1 == 0x0001)
  {
    pibchain2(file);
  }
  else
  {
	  simple_pib.CHECKSUM = fdchecksum32 (file->file, LE16TOH (simple_pib.PIBLENGTH), simple_pib.CHECKSUM);
	  if (lseek (file->file, 0, SEEK_SET))
	  {
		  error (1, errno, "2 " FILE_CANTHOME, file->name);
	  }
	  if (write (file->file, &simple_pib, sizeof (simple_pib)) != sizeof (simple_pib))
	  {
		  error (1, errno, FILE_CANTSAVE, file->name);
	  }
	  if (lseek (file->file, 0, SEEK_SET))
	  {
		  error (1, errno, FILE_CANTHOME, file->name);
	  }
  }
	return (0);
}


#endif

