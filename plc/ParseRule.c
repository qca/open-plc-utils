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
 *   signed ParseRule (int * argcp, char const ** argvp [], struct rule * rule, struct cspec * cspec);
 *
 *   rules.h
 *
 *   This module takes an argument vector and an argument count
 *   and fills in a classification rule structure that is suitable for
 *   sending in a VS_CLASSIFICATION MME;
 *
 *   This module is currently used by plcrule and pibruin;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *   Contributor(s): 
 *	Charles Maier <cmaier@qualcomm.com>
 *	Nathaniel Houghton <nathaniel.houghton@qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#include <memory.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../plc/rules.h"

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6 
#endif

signed ParseRule (int * argcp, char const ** argvp [], struct MMERule * rule, struct cspec * cspec) 

{
	int argc = * argcp;
	char const ** argv = * argvp;
	union 
	{
		uint32_t wide;
		uint16_t word;
		uint8_t byte [4];
	}
	temp;
	signed code;
	struct MMEClassifier * classifier = (struct MMEClassifier *)(&rule->CLASSIFIER);
	if ((code = lookup (* argv++, actions, ACTIONS)) == -1) 
	{
		assist (*--argv, ACTION, actions, ACTIONS);
	}
	rule->MACTION = (uint8_t)(code);
	argc--;
	if ((code = lookup (* argv++, operands, OPERANDS)) == -1) 
	{
		assist (*--argv, OPERAND, operands, OPERANDS);
	}
	rule->MOPERAND = (uint8_t)(code);
	argc--;
	while ((* argv) && (lookup (* argv, controls, CONTROLS) == -1)) 
	{
		if ((code = lookup (* argv++, fields, FIELDS)) == -1) 
		{
			assist (*--argv, FIELD, fields, FIELDS);
		}
		classifier->CR_PID = (uint8_t)(code);
		argc--;
		if ((code = lookup (* argv++, operators, OPERATORS)) == -1) 
		{
			assist (*--argv, OPERATOR, operators, OPERATORS);
		}
		classifier->CR_OPERAND = (uint8_t)(code);
		argc--;
		if (!argc || !* argv) 
		{
			error (1, ENOTSUP, "I have %s '%s' but no value", OPERATOR, *--argv);
		}
		switch (classifier->CR_PID) 
		{
		case FIELD_ETH_SA:
		case FIELD_ETH_DA:
			bytespec (* argv++, classifier->CR_VALUE, ETHER_ADDR_LEN);
			break;
		case FIELD_IPV4_SA:
		case FIELD_IPV4_DA:
			ipv4spec (* argv++, classifier->CR_VALUE);
			break;
		case FIELD_IPV6_SA:
		case FIELD_IPV6_DA:
			ipv6spec (* argv++, classifier->CR_VALUE);
			break;
		case FIELD_VLAN_UP:
		case FIELD_IPV6_TC:
		case FIELD_IPV4_TOS:
		case FIELD_IPV4_PROT:
			classifier->CR_VALUE [0] = (uint8_t)(basespec (* argv++, 0, sizeof (classifier->CR_VALUE [0])));
			break;
		case FIELD_VLAN_ID:
		case FIELD_TCP_SP:
		case FIELD_TCP_DP:
		case FIELD_UDP_SP:
		case FIELD_UDP_DP:
		case FIELD_IP_SP:
		case FIELD_IP_DP:
			temp.word = (uint16_t)(basespec (* argv++, 0, sizeof (temp.word)));
			temp.word = htons (temp.word);
			memcpy (classifier->CR_VALUE, &temp, sizeof (temp.word));
			break;
		case FIELD_ETH_TYPE:
			temp.word = (uint16_t)(basespec (* argv++, 0, sizeof (temp.word)));
			temp.word = htons (temp.word);
			memcpy (classifier->CR_VALUE, &temp, sizeof (temp.word));
			break;
		case FIELD_IPV6_FL:
			temp.wide = (uint32_t)(basespec (* argv++, 0, sizeof (temp.wide))) & 0x000FFFFF;
			temp.wide = htonl (temp.wide);
			memcpy (classifier->CR_VALUE, &temp.byte [1], 3);
			break;
		case FIELD_HPAV_MME:
			bytespec (* argv++, classifier->CR_VALUE, sizeof (uint16_t) + sizeof (uint8_t));
			temp.byte [0] = classifier->CR_VALUE [1];
			classifier->CR_VALUE [1] = classifier->CR_VALUE [2];
			classifier->CR_VALUE [2] = temp.byte [0];
			break;
		case FIELD_TCP_ACK:
			if ((code = lookup (* argv++, states, STATES)) == -1) 
			{
				assist (*--argv, STATE, states, STATES);
			}
			memset (classifier->CR_VALUE, 0, sizeof (classifier->CR_VALUE));
			break;
		case FIELD_VLAN_TAG:
			if ((code = lookup (* argv++, states, STATES)) == -1) 
			{
				assist (*--argv, STATE, states, STATES);
			}
			memset (classifier->CR_VALUE, 0, sizeof (classifier->CR_VALUE));
			classifier->CR_OPERAND ^= code;
			break;
		default:
			error (1, ENOTSUP, "%s", argv [-2]);
			break;
		}
		rule->NUM_CLASSIFIERS++;
		classifier++;
		argc--;
	}
	memcpy (classifier, cspec, sizeof (* cspec));
	if ((code = lookup (* argv++, controls, CONTROLS)) == -1) 
	{
		assist (*--argv, CONTROL, controls, CONTROLS);
	}
	rule->MCONTROL = (uint8_t)(code);
	argc--;
	if ((code = lookup (* argv++, volatilities, VOLATILITIES)) == -1) 
	{
		assist (*--argv, VOLATILITY, volatilities, VOLATILITIES);
	}
	rule->MVOLATILITY = (uint8_t)(code);
	argc--;
	* argcp = argc;
	* argvp = argv;
	return (0);
}

