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
 *   signed ModuleSpec (struct _file_ * module, struct vs_module_spec * vs_module_spec);
 *
 *   plc.h
 *
 *   compute file length and checksum for module operation functions;
 *   struct vs_module_spec is defined in plc.h;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

#ifndef MODULESPEC_SOURCE
#define MODULESPEC_SOURCE

#include "../tools/error.h"
#include "../tools/files.h"
#include "../plc/plc.h"

signed ModuleSpec (struct _file_ * file, struct vs_module_spec * spec) 

{
	off_t length;
	if ((length = lseek (file->file, 0, SEEK_END)) == -1) 
	{
		error (1, errno, FILE_CANTSIZE, file->name);
	}

#if 1

	if (length % sizeof (uint32_t)) 
	{
		error (1, ENOTSUP, "%s not multiple of " SIZE_T_SPEC " bytes", file->name, sizeof (uint32_t));
	}

#endif

	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	spec->MODULE_LENGTH = length;
	spec->MODULE_CHKSUM = fdchecksum32 (file->file, length, 0);
	if (lseek (file->file, 0, SEEK_SET)) 
	{
		error (1, errno, FILE_CANTHOME, file->name);
	}
	return (0);
}


#endif

