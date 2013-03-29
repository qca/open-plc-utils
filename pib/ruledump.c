/*====================================================================*
 *   
 *   Copyright (c) 2012 Qualcomm Atheros Inc.
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
 *   void ruledump (struct rule * rule, FILE * fp);
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2012 by Qualcomm Atheros. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *--------------------------------------------------------------------*/

#ifndef RULEDUMP_SOURCE
#define RULEDUMP_SOURCE

#include <stdio.h>

#include "../tools/memory.h"    
#include "../plc/rules.h"    

/*====================================================================*
 *
 *   cspec_dump (struct cspec * cspec);
 *
 *
 *--------------------------------------------------------------------*/

void cspec_dump (struct cspec * cspec) 

{
	printf ("cspec->CSPEC_VERSION=%d\n", LE16TOH (cspec->CSPEC_VERSION));
	printf ("cspec->VLAN_TAG=%d\n", LE16TOH (cspec->VLAN_TAG));
	return;
}


/*====================================================================*
 *
 *   MMEClassifierDump (struct MMEClassifier * classifier);
 *
 *
 *--------------------------------------------------------------------*/

void MMEClassifierDump (struct MMEClassifier * classifier) 

{
	char string [48];
	printf ("classifier->CR_PID=0x%02X\n", (unsigned)(classifier->CR_PID));
	printf ("classifier->CR_OPERAND=0x%02X\n", classifier->CR_OPERAND);
	hexdecode (classifier->CR_VALUE, sizeof (classifier->CR_VALUE), string, sizeof (string));
	printf ("classifier->CR_VALUE=%s\n", string);
	return;
}


/*====================================================================*
 *
 *   PIBClassifierDump (struct PIBClassifier * classifier);
 *
 *
 *--------------------------------------------------------------------*/

void PIBClassifierDump (struct PIBClassifier * classifier) 

{
	char string [48];
	printf ("classifier->CR_PID=0x%08X\n", LE32TOH (classifier->CR_PID));
	printf ("classifier->CR_OPERAND=0x%08X\n", LE32TOH (classifier->CR_OPERAND));
	hexdecode (classifier->CR_VALUE, sizeof (classifier->CR_VALUE), string, sizeof (string));
	printf ("classifier->CR_VALUE=%s\n", string);
	return;
}


/*====================================================================*
 *
 *   MMERuleDump (struct MMERule * rule);
 *
 *
 *--------------------------------------------------------------------*/

void MMERuleDump (struct MMERule * rule) 

{
	unsigned count;
	printf ("rule->MCONTROL=%d\n", rule->MCONTROL);
	printf ("rule->MVOLATILITY=%d\n", rule->MVOLATILITY);
	printf ("rule->MACTION=%d\n", rule->MACTION);
	printf ("rule->MOPERAND=%d\n", rule->MOPERAND);
	printf ("rule->NUM_CLASSIFIERS=%d\n", rule->NUM_CLASSIFIERS);
	for (count = 0; count < rule->NUM_CLASSIFIERS; count++) 
	{
		MMEClassifierDump (&rule->CLASSIFIER [count]);
	}
	cspec_dump (&rule->cspec);
	printf ("\n");
	return;
}


/*====================================================================*
 *
 *   PIBRuleDump (struct PIBRule * rule);
 *
 *
 *--------------------------------------------------------------------*/

void PIBRuleDump (struct PIBRule * rule) 

{
	unsigned count;
	printf ("rule->MCONTROL=%d\n", rule->MCONTROL);
	printf ("rule->MVOLATILITY=%d\n", rule->MVOLATILITY);
	printf ("rule->MACTION=%d\n", rule->MACTION);
	printf ("rule->MOPERAND=%d\n", rule->MOPERAND);
	printf ("rule->NUM_CLASSIFIERS=%d\n", LE32TOH (rule->NUM_CLASSIFIERS));
	for (count = 0; count < LE32TOH (rule->NUM_CLASSIFIERS); count++) 
	{
		PIBClassifierDump (&rule->CLASSIFIER [count]);
	}
	cspec_dump (&rule->cspec);
	printf ("\n");
	return;
}


/*====================================================================*
 *
 *   classifier_priority_map_dump (struct classifier_priority_map * classifier_priority_map);
 *
 *
 *--------------------------------------------------------------------*/

void classifier_priority_map_dump (struct classifier_priority_map * classifier_priority_map) 

{
	printf ("classifier_priority_map->Priority=%d\n", LE32TOH (classifier_priority_map->Priority));
	PIBClassifierDump (&classifier_priority_map->CLASSIFIER);
	return;
}


/*====================================================================*
 *
 *   auto_connection_dump (struct auto_connection * auto_connection);
 *
 *
 *--------------------------------------------------------------------*/

void auto_connection_dump (struct auto_connection * auto_connection) 

{
	unsigned count;
	printf ("auto_connection->MACTION=%d\n", auto_connection->MACTION);
	printf ("auto_connection->MOPERAND=%d\n", auto_connection->MOPERAND);
	printf ("auto_connection->NUM_CLASSIFIERS=%d\n", LE16TOH (auto_connection->NUM_CLASSIFIERS));
	for (count = 0; count < LE16TOH (auto_connection->NUM_CLASSIFIERS); count++) 
	{
		PIBClassifierDump (&auto_connection->CLASSIFIER [count]);
	}
	cspec_dump (&auto_connection->cspec);
	return;
}


/*====================================================================*
 *
 *   PIBClassifiersDump (struct PIBClassifiers * classifiers);
 *
 *
 *--------------------------------------------------------------------*/

void PIBClassifiersDump (struct PIBClassifiers * classifiers) 

{
	unsigned count;
	printf ("classifiers->priority_count=%d\n", LE32TOH (classifiers->priority_count));
	printf ("classifiers->autoconn_count=%d\n", LE32TOH (classifiers->autoconn_count));
	printf ("\n");
	printf ("-------- PRIORITY MAPS --------\n\n");
	for (count = 0; count < LE32TOH (classifiers->priority_count); count++) 
	{
		classifier_priority_map_dump (&classifiers->classifier_priority_map [count]);
	}
	printf ("\n");
	printf ("-------- AUTO CONNECTIONS --------\n\n");
	for (count = 0; count < LE32TOH (classifiers->autoconn_count); count++) 
	{
		auto_connection_dump (&classifiers->auto_connection [count]);
	}
	printf ("\n");
	return;
}


/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

