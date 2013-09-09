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
 *   signed InitDevice (struct plc * plc);
 *
 *   plc.h
 *
 *   call appropriate function based on image file format;
 *
 *   initialize powerline device memory before writing and starting
 *   runtime parameters and firmare; each generation of device has
 *   a different memory type; newer devices store memory parameters
 *   and configuration programs in an image file;
 *
 *   this implementation handles both old and new image file formats;
 *   call InitDevice1 or InitDevice2 directly if you know which file
 *   format will be used;
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef INITMEMORY_SOURCE
#define INITMEMORY_SOURCE

#include "../tools/files.h"
#include "../tools/error.h"
#include "../plc/plc.h"

signed InitDevice (struct plc * plc)

{
	return (NVMSelect (plc, InitDevice1, InitDevice2));
}


#endif

