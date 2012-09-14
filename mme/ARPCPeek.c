/*====================================================================*
 *
 *   unsigned ARPCPeek (void const * memory, size_t extent, FILE * fp);
 *
 *   mme.h
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>

#include "../tools/types.h"
#include "../tools/endian.h"

void ARPCPeek (void const * memory, size_t extent, FILE * fp) 

{

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_arpc_data 
	{
		uint32_t BYPASS;
		uint16_t ARPCID;
		uint16_t DATALENGTH;
		uint8_t DATAOFFSET;
		uint8_t RESERVED [3];
		uint16_t ARGOFFSET;
		uint16_t STROFFSET;
		uint16_t ARGLENGTH;
		uint16_t STRLENGTH;
		uint8_t LIST [1];
	}
	* data = (struct vs_arpc_data *)(memory);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	fprintf (fp, "BYPASS=%d ", data->BYPASS);
	fprintf (fp, "ARPCID=%d ", data->ARPCID);
	fprintf (fp, "DATALENGTH=%d ", LE16TOH (data->DATALENGTH));
	fprintf (fp, "DATAOFFSET=%d ", data->DATAOFFSET);
	fprintf (fp, "ARGOFFSET=%d ", LE16TOH (data->ARGOFFSET));
	fprintf (fp, "STROFFSET=%d ", LE16TOH (data->STROFFSET));
	fprintf (fp, "ARGLENGTH=%d ", LE16TOH (data->ARGLENGTH));
	fprintf (fp, "STRLENGTH=%d ", LE16TOH (data->STRLENGTH));
	if (data->ARPCID == 1) 
	{
		fprintf (fp, "FORMAT=[%s]\n", &data->LIST [data->STROFFSET]);
	}
	return;
}

