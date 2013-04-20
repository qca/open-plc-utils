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
 *   signed manifest (void const * memory, size_t extent);
 *
 *   print manifest contents on stdout in human readable format;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MANIFEST_SOURCE
#define MANIFEST_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "../tools/format.h"
#include "../tools/memory.h"
#include "../tools/endian.h"
#include "../tools/error.h"
#include "../tools/files.h"
#include "../nvm/nvm.h"

/*
 *   strings must appear in order of hardware types;
 */

static char const * compatibility [] = 

{
	"QCA7420",
	(char const *)(0)
};


/*
 *   strings must appear in order of field type; the last string is the error string;
 */

static char const * nvm_fields [] = 

{
	"Signature",
	"Hardware Compatibility",
	"Chain Major Version",
	"Chain Minor Version",
	"Chain Type",
	"Build Major Version",
	"Build Minor Version",
	"Build Type",
	"Manifest Version",
	"Build Number",
	"Build Date",
	"Build Time",
	"Device Type",
	"Build Hostname",
	"Build Username",
	"Build Description",
	"Build Version String",
	"Build Sustaining Release",
	"Build Major Subversion",
	"Generic Identifier 0",
	"Generic Identifier 1",
	"Softloader Major Version",
	"Softloader Minor Version",
	"Manifest Free Space",
	"Unknown Type"
};


/*
 *   strings must appear in order of chain type;
 */

static char const * nvm_chains [] = 

{
	"Softloader",
	"Firmware",
	"Parameter Block",
	"CustomModule"
};


/*====================================================================*
 *
 *   char * myitoa (unsigned number, char buffer [], size_t length);
 *
 *   non-standard version of itoa () where buffer is packed right-
 *   to-left to avoid reversing digits; ensure buffer is longer
 *   than needed;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

static char * myitoa (unsigned number, char buffer [], size_t length) 

{
	buffer [--length] = (char)(0);
	do 
	{
		buffer [--length] = '0' + (number % 10);
		number /= 10;
	}
	while (number);
	return (buffer);
}


/*====================================================================*
 *
 *   signed manifest (void const * memory, size_t extent);
 *
 *   nvm.h
 *   
 *   walk through a manifest and print known values, when present; 
 *   known values are those defined in nvm.h and named here; 
 *
 *   the TLVNode data structure used here is not the same a that in
 *   the firmware headers; our structure assumes values are 32-bit 
 *   integers and treats strings are exceptions to that rule;
 *
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

signed manifest (void const * memory, size_t extent) 

{
	uint8_t * offset = (uint8_t *)(memory);
	uint32_t length = (uint32_t)(extent);
	char string [1024];
	while (length > 0) 
	{
		struct TLVNode * node = (struct TLVNode *)(offset);
		uint32_t type = LE32TOH (node->type);
		uint32_t size = LE32TOH (node->size);
		uint32_t data = LE32TOH (node->data);

#if 0

		if (type == NVM_FIELD_SIGNATURE) 
		{
			printf ("\t%s: %08X\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_HARDWARE_COMPAT) 
		{
			strfbits (string, sizeof (string), compatibility, "|", data);
			printf ("\t%s: %s\n", nvm_fields [type], string);
		}
		else if (type == NVM_FIELD_CHAIN_MAJOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_CHAIN_MINOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_CHAIN_TYPE) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], data < SIZEOF (nvm_chains)? nvm_chains [data]: myitoa (data, string, sizeof (string)));
		}
		else if (type == NVM_FIELD_BUILD_MAJOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_MINOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_TYPE) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
		}
		else if (type == NVM_FIELD_MANIFEST_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_NUMBER) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_DATE) 
		{
			printf ("\t%s: %08X\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_TIME) 
		{
			printf ("\t%s: %06X\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_DEVICE_TYPE) 
		{
			printf ("\t%s: %04X\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_HOSTNAME) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
		}
		else if (type == NVM_FIELD_BUILD_USERNAME) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
		}
		else if (type == NVM_FIELD_BUILD_DESCRIPTION) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
		}
		else if (type == NVM_FIELD_BUILD_VERSION_STRING) 
		{
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
		}
		else if (type == NVM_FIELD_BUILD_SUSTAINING_RELEASE) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_BUILD_MAJOR_SUBVERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_GENERIC_ID0) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_GENERIC_ID1) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_SL_MAJOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_SL_MINOR_VERSION) 
		{
			printf ("\t%s: %d\n", nvm_fields [type], data);
		}
		else if (type == NVM_FIELD_FREE_SPACE) 
		{
			printf ("\t%s: %d/%d bytes\n", nvm_fields [type], size, extent);
		}
		else 
		{
			error (0, 0, "%s: %d", nvm_fields [sizeof (nvm_fields) / sizeof (const char *) - 1], type);
		}

#else

		switch (type) 
		{
		case NVM_FIELD_SIGNATURE:
		case NVM_FIELD_BUILD_DATE:
			printf ("\t%s: %08X\n", nvm_fields [type], data);
			break;
		case NVM_FIELD_BUILD_TIME:
			printf ("\t%s: %06X\n", nvm_fields [type], data);
			break;
		case NVM_FIELD_DEVICE_TYPE:
			printf ("\t%s: %04X\n", nvm_fields [type], data);
			break;
		case NVM_FIELD_CHAIN_TYPE:
			printf ("\t%s: %s\n", nvm_fields [type], data < SIZEOF (nvm_chains)? nvm_chains [data]: myitoa (data, string, sizeof (string)));
			break;
		case NVM_FIELD_BUILD_HOSTNAME:
		case NVM_FIELD_BUILD_USERNAME:
		case NVM_FIELD_BUILD_DESCRIPTION:
		case NVM_FIELD_BUILD_VERSION_STRING:
		case NVM_FIELD_BUILD_TYPE:
			printf ("\t%s: %s\n", nvm_fields [type], (char const *)(&node->data));
			break;
		case NVM_FIELD_HARDWARE_COMPAT:
			strfbits (string, sizeof (string), compatibility, "|", data);
			printf ("\t%s: %s\n", nvm_fields [type], string);
			break;
		case NVM_FIELD_MANIFEST_VERSION:
		case NVM_FIELD_CHAIN_MAJOR_VERSION:
		case NVM_FIELD_CHAIN_MINOR_VERSION:
		case NVM_FIELD_BUILD_NUMBER:
		case NVM_FIELD_BUILD_MAJOR_VERSION:
		case NVM_FIELD_BUILD_MINOR_VERSION:
		case NVM_FIELD_BUILD_MAJOR_SUBVERSION:
		case NVM_FIELD_BUILD_SUSTAINING_RELEASE:
		case NVM_FIELD_GENERIC_ID0:
		case NVM_FIELD_GENERIC_ID1:
		case NVM_FIELD_SL_MAJOR_VERSION:
		case NVM_FIELD_SL_MINOR_VERSION:
			printf ("\t%s: %d\n", nvm_fields [type], data);
			break;
		case NVM_FIELD_FREE_SPACE:
			printf ("\t%s: %d/%d bytes\n", nvm_fields [type], size, extent);
			break;
		default:
			error (0, 0, "%s: %d", nvm_fields [sizeof (nvm_fields) / sizeof (const char *) - 1], type);
			break;
		}

#endif

		length -= sizeof (* node) - sizeof (node->size) + LE32TOH (node->size);
		offset += sizeof (* node) - sizeof (node->size) + LE32TOH (node->size);
	}
	return (0);
}

#endif

