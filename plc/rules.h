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
 *   rules.h - Classification Rules Definitions and Declarations;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit;
 *:  Copyright (c) 2006-2010 by Intellon Corporation; ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nathaniel.houghton@qca.qualcomm.com>
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

#include "../plc/plc.h"
#include "../tools/symbol.h"

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define CLASSIFIER_STRING 64
#define CLASSIFIER_CONTROLS 3
#define CLASSIFIER_VOLATILITIES 2
#define CLASSIFIER_ACTIONS 13
#define CLASSIFIER_OPERANDS 3
#define CLASSIFIER_OPERATORS 2
#define CLASSIFIER_STATES 8
#define CLASSIFIER_FIELDS 22

#define CLASSIFIER_CONTROL_NAME "control"
#define CLASSIFIER_VOLATILITY_NAME "volatility"
#define CLASSIFIER_ACTION_NAME "action"
#define CLASSIFIER_OPERAND_NAME "operand"
#define CLASSIFIER_OPERATOR_NAME "operator"
#define CLASSIFIER_STATE_NAME "state"
#define CLASSIFIER_FIELD_NAME "field"

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
#define ACTION_AUTOCONNECT 0x06
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

#define RULE_MAX_AUTOCONN 16
#define RULE_MAX_PRIORITY_MAPS 8
#define RULE_MAX_CLASSIFIERS 3

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

extern struct _code_ const controls [CLASSIFIER_CONTROLS];
extern struct _code_ const volatilities [CLASSIFIER_VOLATILITIES];
extern struct _code_ const actions [CLASSIFIER_ACTIONS];
extern struct _code_ const operands [CLASSIFIER_OPERANDS];
extern struct _code_ const fields [CLASSIFIER_FIELDS];
extern struct _code_ const operators [CLASSIFIER_OPERATORS];
extern struct _code_ const states [CLASSIFIER_STATES];

#ifndef __GNUC__
#pragma pack (push,1)
#endif

/*
 *	MME and PIB Classifier Rules are similar but differ in size
 *      and so we declare structures for each use; observe that
 *	function ParseRule uses the compact MMERule structure;
 */

typedef struct __packed cspec

{
	uint16_t CSPEC_VERSION;
	uint32_t VLAN_TAG;
	uint32_t RSVD;
}

cspec;
typedef struct __packed MMEClassifier

{
	uint8_t CR_PID;
	uint8_t CR_OPERAND;
	uint8_t CR_VALUE [16];
}

MMEClassifier;
typedef struct __packed PIBClassifier

{
	uint32_t CR_PID;
	uint32_t CR_OPERAND;
	uint8_t CR_VALUE [16];
}

PIBClassifier;
typedef struct __packed MMERule

{
	uint8_t MCONTROL;
	uint8_t MVOLATILITY;
	uint8_t MACTION;
	uint8_t MOPERAND;
	uint8_t NUM_CLASSIFIERS;
	struct MMEClassifier CLASSIFIER [RULE_MAX_CLASSIFIERS];
	struct cspec cspec;
}

MMERule;
typedef struct __packed PIBRule

{
	uint8_t MCONTROL;
	uint8_t MVOLATILITY;
	uint8_t MACTION;
	uint8_t MOPERAND;
	uint32_t NUM_CLASSIFIERS;
	struct PIBClassifier CLASSIFIER [RULE_MAX_CLASSIFIERS];
	struct cspec cspec;
}

PIBRule;
typedef struct __packed classifier_priority_map

{
	uint32_t Priority;
	struct PIBClassifier CLASSIFIER;
}

classifier_priority_map;
typedef struct __packed auto_connection

{
	uint8_t MACTION;
	uint8_t MOPERAND;
	uint16_t NUM_CLASSIFIERS;
	struct PIBClassifier CLASSIFIER [RULE_MAX_CLASSIFIERS];
	struct cspec cspec;
	uint8_t RSVD [14];
}

auto_connection;
typedef struct __packed PIBClassifiers

{
	uint32_t priority_count;
	uint32_t autoconn_count;
	struct classifier_priority_map classifier_priority_map [RULE_MAX_PRIORITY_MAPS];
	struct auto_connection auto_connection [RULE_MAX_AUTOCONN];
}

PIBClassifiers;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

signed MakeRule (struct plc * plc, struct MMERule * rule);
signed ParseRule (int * argcp, char const ** argvp [], struct MMERule *, struct cspec *);
signed PrintRule (uint32_t CR_PID, uint32_t CR_OPERAND, uint8_t CR_VALUE []);
signed ReadRules (struct plc * plc);
void cspec_dump (struct cspec *);
void classifier_priority_map_dump (struct classifier_priority_map *);
void auto_connection_dump (struct auto_connection *);
void PIBClassifiersDump (struct PIBClassifiers *);
void PIBClassifierDump (struct PIBClassifier *);
void MMEClassifierDump (struct MMEClassifier *);
void PIBRuleDump (struct PIBRule *);
void MMERuleDump (struct MMERule *);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

