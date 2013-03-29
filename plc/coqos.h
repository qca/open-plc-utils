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
 *   coqos.h - Intellon Definitions and Declarations;
 *
 *--------------------------------------------------------------------*/

#ifndef COQOS_HEADER
#define COQOS_HEADER

/*====================================================================*
 *   custom heder files;
 *--------------------------------------------------------------------*/

#include "../plc/rules.h"

/*====================================================================*
 *   network classification rule structure;
 *--------------------------------------------------------------------*/

#ifndef __GNUC__
#pragma pack (push,1)
#endif

struct __packed connection 

{
	uint32_t REQ_ID;
	uint32_t RSVD;
	uint8_t APP_DA [6];
	uint8_t RSVD2;
	struct __packed 
	{
		uint8_t MOPERAND;
		uint8_t NUM_CLASSIFIERS;
		struct MMEClassifier CLASSIFIERS [RULE_MAX_CLASSIFIERS];
	}
	rule;
	struct __packed 
	{
		uint16_t CSPEC_VERSION;
		uint8_t CONN_CAP;
		uint8_t CONN_COQOS_PRIO;
		uint16_t CONN_RATE;
		uint32_t CONN_TTL;
	}
	cspec;
};

struct __packed conn_info 

{
	uint8_t SNID;
	uint16_t CID;
	uint8_t STEI;
	uint8_t DTEI;
	uint8_t LLID;
	uint8_t RSVD;
	uint16_t CSPECVERSION;
	uint8_t CONN_CAP;
	uint8_t CONN_COQOS_PRIO;
	uint16_t CONN_RATE;
	uint32_t CONN_TTL;
	uint16_t BW_USED;
};


#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

