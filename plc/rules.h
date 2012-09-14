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
 *   rules.h - Classification Rules Definitions and Declarations;
 *
 *   This software and documentation is the property of Intellon 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Intellon assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Intellon customers may modify and distribute the software 
 *   without obligation to Intellon. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Intellon HomePlug AV Application Programming Interface;
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *      Charles Maier <cmaier@qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef RULES_HEADER
#define RULES_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdint.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/symbol.h"

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define CONTROLS 3
#define VOLATILITIES 2
#define ACTIONS 12
#define OPERANDS 3
#define OPERATORS 2
#define STATES 8
#define FIELDS 22

#define CONTROL "control"
#define VOLATILITY "volatility"
#define ACTION "action"
#define OPERAND "operand"
#define OPERATOR "operator"
#define STATE "state"
#define FIELD "field"

#define CONTROL_ADD 0x00
#define CONTROL_REM 0x01
#define CONTROL_REMOVE CONTROL_REM

#define VOLATILITY_TEMP 0x00
#define VOLATILITY_PERM 0x01

#define ACTION_CAP0 0x00
#define ACTION_CAP1 0x01
#define ACTION_CAP2 0x02
#define ACTION_CAP3 0x03
#define ACTION_DROP 0x04
#define ACTION_DROPTX ACTION_DROP
#define ACTION_BOOST 0x05
#define ACTION_DROPRX 0x06
#define ACTION_STRIPTX 0x20
#define ACTION_STRIPRX 0x21
#define ACTION_TAGTX 0x22
#define ACTION_TAGRX 0x23

#define OPERAND_ALL 0x00
#define OPERAND_ANY 0x01
#define OPERAND_ALWAYS 0x02

#define OPERATOR_IS 0x00
#define OPERATOR_NOT 0x01

#define FIELD_ETH_DA 0x00
#define FIELD_ETH_SA 0x01
#define FIELD_VLAN_UP 0x02
#define FIELD_VLAN_ID 0x03
#define FIELD_IPV4_TOS 0x04
#define FIELD_IPV4_PROT 0x05
#define FIELD_IPV4_SA 0x06
#define FIELD_IPV4_DA 0x07
#define FIELD_IPV6_TC 0x08
#define FIELD_IPV6_FL 0x09
#define FIELD_IPV6_SA 0x0A
#define FIELD_IPV6_DA 0x0B
#define FIELD_TCP_SP 0x0C
#define FIELD_TCP_DP 0x0D
#define FIELD_UDP_SP 0x0E
#define FIELD_UDP_DP 0x0F
#define FIELD_IP_SP 0xE0
#define FIELD_IP_DP 0xE1
#define FIELD_HPAV_MME 0xE2
#define FIELD_ETH_TYPE 0xE3
#define FIELD_TCP_ACK 0xE4
#define FIELD_VLAN_TAG 0xE5

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

extern struct _code_ const controls [CONTROLS];
extern struct _code_ const volatilities [VOLATILITIES];
extern struct _code_ const actions [ACTIONS];
extern struct _code_ const operands [OPERANDS];
extern struct _code_ const fields [FIELDS];
extern struct _code_ const operators [OPERATORS];
extern struct _code_ const states [STATES];

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed classifier 

{
	uint8_t CR_PID;
	uint8_t CR_OPERAND;
	uint8_t CR_VALUE [16];
}

classifier;
typedef struct __packed cspec 

{
	uint16_t CSPEC_VERSION;
	uint32_t VLAN_TAG;
	uint32_t RSVD;
}

cspec;
typedef struct __packed rule 

{
	uint8_t MCONTROL;
	uint8_t MVOLATILITY;
	uint8_t MACTION;
	uint8_t MOPERAND;
	uint8_t NUM_CLASSIFIERS;
	struct classifier CLASSIFIER [3];
	struct cspec cspec;
}

rule;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

signed ParseRule (int * argcp, char const ** argvp [], struct rule * rule, struct cspec * cspec);

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#endif

