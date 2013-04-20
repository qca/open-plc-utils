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
 *   uint16_t psread (uint16_t values [], uint16_t limit, FILE * fp);
 *
 *   pib.h
 *
 *   read a prescaler file and populate a scaler array;
 *
 *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "../tools/chars.h"
#include "../tools/error.h"
#include "../tools/number.h"
#include "../pib/pib.h"

uint16_t psread (uint16_t values [], uint16_t limit, FILE * fp) 

{
	uint16_t index = 0;
	uint16_t count = 0;
	uint32_t value = 0;
	signed c;
	while ((c = getc (fp)) != EOF) 
	{
		if (isspace (c)) 
		{
			continue;
		}
		if ((c == '#') || (c == ';')) 
		{
			do 
			{
				c = getc (fp);
			}
			while ((c != '\n') && (c != EOF));
			continue;
		}
		index = 0;
		while (isdigit (c)) 
		{
			index *= 10;
			index += c - '0';
			c = getc (fp);
		}
		if (index != count) 
		{
			error (1, ECANCELED, "Carrier %d out of order", index);
		}
		if (index >= limit) 
		{
			error (1, EOVERFLOW, "Too many prescalers");
		}
		while (isblank (c)) 
		{
			c = getc (fp);
		}
		value = 0;
		while (isxdigit (c)) 
		{
			value *= 16;
			value += todigit (c);
			c = getc (fp);
		}
		values [index] = value;
		while ((c != '\n') && (c != EOF)) 
		{
			c = getc (fp);
		};
		count++;
	}
	return (count);
}

