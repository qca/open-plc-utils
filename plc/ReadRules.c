/*====================================================================*
 *
 *   signed ReadRules (struct plc * plc);
 *
 *   plc.h
 *
 *   Read and Display the classifier rules read from a PLC device.
 *
 *   Contributor(s):
 *      Nathan Houghton <nhoughto@qca.qualcomm.com>
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef READRULES_SOURCE
#define READRULES_SOURCE

#include <limits.h>
#include <stdio.h>

#include "../plc/plc.h"
#include "../plc/rules.h"
#include "../tools/error.h"

signed ReadRules (struct plc * plc)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_classification_request
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MCONTROL;
		uint8_t RSVD;
		uint8_t OFFSET;
		uint8_t COUNT;
	}
	* request = (struct vs_classification_request *)(message);
	struct __packed vs_classification_confirm
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t MSTATUS;
		uint8_t TOTAL_CLASSIFIERS;
		uint8_t OFFSET;
		uint8_t COUNT;
		struct __packed MMEReadRule
		{
			uint8_t MACTION;
			uint8_t MOPERAND;
			uint8_t NUM_CLASSIFIERS;
			struct MMEClassifier CLASSIFIER [3];
			struct cspec cspec;
		}
		RULESET [60];
	}
	* confirm = (struct vs_classification_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	unsigned index = 0;
	unsigned total = UINT_MAX;
	Request (plc, "Read Classifier Rules");
	while (index < total)
	{
		struct MMEReadRule * rule;
		memset (message, 0, sizeof (* message));
		EthernetHeader (& request->ethernet, channel->peer, channel->host, channel->type);
		QualcommHeader (& request->qualcomm, 0, (VS_CLASSIFICATION | MMTYPE_REQ));
		plc->packetsize = ETHER_MIN_LEN - ETHER_CRC_LEN;
		request->MCONTROL = 0x02;
		request->OFFSET = index;
		request->COUNT = SIZEOF (confirm->RULESET);
		if (SendMME (plc) <= 0)
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
			return (-1);
		}
		if (ReadMME (plc, 0, (VS_CLASSIFICATION | MMTYPE_CNF)) <= 0)
		{
			error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTREAD);
			return (-1);
		}
		if (confirm->MSTATUS)
		{
			Failure (plc, PLC_WONTDOIT);
			return (-1);
		}
		total = confirm->TOTAL_CLASSIFIERS;
		index += confirm->COUNT;
		rule = confirm->RULESET;
		while (confirm->COUNT--)
		{
			int i;
			int rule_len;
			const char * p1;
			const char * p2;
			struct cspec * cspec;
			if (rule->NUM_CLASSIFIERS > RULE_MAX_CLASSIFIERS)
			{
				error (1, 0, "too many classifiers in rule (%d, expecting <= %d)", rule->NUM_CLASSIFIERS, RULE_MAX_CLASSIFIERS);
			}
			rule_len = sizeof (* rule) - (RULE_MAX_CLASSIFIERS - rule->NUM_CLASSIFIERS) * sizeof (struct MMEReadRule) - sizeof (struct cspec);
			if (rule->MACTION == ACTION_AUTOCONNECT || rule->MACTION == ACTION_TAGTX || rule->MACTION == ACTION_TAGRX)
			{
				cspec = (struct cspec *)((uint8_t *) rule + rule_len);
				rule_len += sizeof (struct cspec);
			}
			if (rule->MACTION == ACTION_TAGTX)
			{
				printf ("-T 0x%08X -V %d ", ntohl (cspec->VLAN_TAG), cspec->CSPEC_VERSION);
			}
			p1 = reword (rule->MACTION, actions, CLASSIFIER_ACTIONS);
			if (p1 == NULL)
			{
				error (1, 0, "invalid classifier action");
			}
			p2 = reword (rule->MOPERAND, operands, CLASSIFIER_OPERANDS);
			if (p2 == NULL)
			{
				error (1, 0, "invalid classifier operand");
			}
			printf ("%s", p1);
			printf (" %s ", p2);

/* need to dump out the actual conditions here */

			for (i = 0; i < rule->NUM_CLASSIFIERS; ++i)
			{
				struct MMEClassifier * classifier = & rule->CLASSIFIER [i];
				PrintRule (classifier->CR_PID, classifier->CR_OPERAND, classifier->CR_VALUE);
				putchar (' ');
			}
			printf ("add temp\n");
			rule = (struct MMEReadRule *)((uint8_t *)(rule) + rule_len);
		}
	}
	return (0);
}

#endif

