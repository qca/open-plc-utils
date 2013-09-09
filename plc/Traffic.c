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
 *   signed  Traffic (struct plc * plc);
 *
 *   plc.h
 *
 *   generate bi-directional traffic between a local device and all
 *   remote devices on a powerline network; remote devices need not
 *   be connected; no traffic is generated between remote devices;
 *
 *   this function is similar to NetworkTraffic but takes less time
 *   since not all powerline paths are exercised and those that are
 *   can be exercised in parallel;
 *
 *
 *--------------------------------------------------------------------*/

#ifndef TRAFFIC_SOURCE
#define TRAFFIC_SOURCE

#include "../plc/plc.h"

signed Traffic (struct plc * plc)

{
	return (PLCSelect (plc, Traffic1, Traffic2));
}


#endif

