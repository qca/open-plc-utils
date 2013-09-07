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
 *   mme.h -
 *
 *   message header definitions and function declarations;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Matthieu Poullet <m.poullet@avm.de>
 *
 *--------------------------------------------------------------------*/

#ifndef MME_HEADER
#define MME_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../ether/ether.h"
#include "../ether/channel.h"
#include "../mme/homeplug.h"
#include "../mme/qualcomm.h"

/*====================================================================*
 *   manage cross-platform structure packing;
 *--------------------------------------------------------------------*/

#ifndef __packed
#ifdef __GNUC__
#define __packed __attribute__ ((packed))
#else
#define __packed
#endif
#endif

/*====================================================================*
 *   Ethernet, HomePlug and Qualcomm Frame headers;
 *--------------------------------------------------------------------*/

#ifndef __GNUC__
#pragma pack (push, 1)
#endif

typedef struct __packed ethernet_std

{
	uint8_t ODA [ETHER_ADDR_LEN];
	uint8_t OSA [ETHER_ADDR_LEN];
	uint16_t MTYPE;
}

ethernet_std;
typedef struct __packed homeplug_std

{
	uint8_t MMV;
	uint16_t MMTYPE;
}

homeplug_std;
typedef struct __packed homeplug_fmi

{
	uint8_t MMV;
	uint16_t MMTYPE;
	uint8_t FMSN;
	uint8_t FMID;
}

homeplug_fmi;
typedef struct __packed qualcomm_std

{
	uint8_t MMV;
	uint16_t MMTYPE;
	uint8_t OUI [ETHER_ADDR_LEN >> 1];
}

qualcomm_std;
typedef struct __packed qualcomm_fmi

{
	uint8_t MMV;
	uint16_t MMTYPE;
	uint8_t FMSN;
	uint8_t FMID;
	uint8_t OUI [ETHER_ADDR_LEN >> 1];
}

qualcomm_fmi;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *   Composite message formats;
 *--------------------------------------------------------------------*/

#ifndef __GNUC__
#pragma pack (push, 1)
#endif

typedef struct __packed message

{
	struct ethernet_std ethernet;
	uint8_t content [ETHERMTU];
}

MESSAGE;
typedef struct __packed homeplug

{
	struct ethernet_std ethernet;
	struct homeplug_fmi homeplug;
	uint8_t content [ETHERMTU - sizeof (struct homeplug_fmi)];
}

HOMEPLUG;
typedef struct __packed qualcomm

{
	struct ethernet_std ethernet;
	struct qualcomm_fmi qualcomm;
	uint8_t content [ETHERMTU - sizeof (struct qualcomm_fmi)];
}

QUALCOMM;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

void MMEPeek (void const * memory, size_t extent, FILE * fp);
void ARPCPeek (void const * memory, size_t extent, FILE * fp);
void ARPCWrite (signed fd, void const * memory, size_t extent);
void ARPCPrint (FILE * fp, void const * memory, size_t extent);

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

char const * MMEName (uint16_t MMTYPE);
char const * MMEMode (uint16_t MMTYPE);
char const * MMECode (uint16_t MMTYPE, uint8_t MSTATUS);

/*====================================================================*
 *   header encode functions;
 *--------------------------------------------------------------------*/

signed EthernetHeader (void * memory, const uint8_t peer [], const uint8_t host [], uint16_t protocol);
signed HomePlugHeader (struct homeplug_std *, uint8_t MMV, uint16_t MMTYPE);
signed QualcommHeader (struct qualcomm_std *, uint8_t MMV, uint16_t MMTYPE);
signed HomePlugHeader1 (struct homeplug_fmi *, uint8_t MMV, uint16_t MMTYPE);
signed QualcommHeader1 (struct qualcomm_fmi *, uint8_t MMV, uint16_t MMTYPE);

/*====================================================================*
 *   header decode functions;
 *--------------------------------------------------------------------*/

signed UnwantedMessage (void const * memory, size_t extent, uint8_t MMV, uint16_t MMTYPE);
signed FirmwareMessage (void const * memory);

/*====================================================================*
 *   intermmediate level Ethernet send/receive functions;
 *--------------------------------------------------------------------*/

ssize_t sendmessage (struct channel const *, struct message *, ssize_t length);
ssize_t readmessage (struct channel const *, struct message *, uint8_t MMV, uint16_t MMTYPE);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

