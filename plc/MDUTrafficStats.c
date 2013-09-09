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
 *   signed MDUTrafficStats (struct plc * plc);
 *
 *   plc.h
 *
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef MDUTRAFFICSTATS_SOURCE
#define MDUTRAFFICSTATS_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../plc/plc.h"
#include "../tools/flags.h"
#include "../tools/error.h"
#include "../tools/memory.h"

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define CC1_RDRP (1 <<  0)
#define CC1_RJBR (1 <<  1)
#define CC1_RFRG (1 <<  2)
#define CC1_ROVR (1 <<  3)
#define CC1_RUND (1 <<  4)
#define CC1_RCSE (1 <<  5)
#define CC1_RCDE (1 <<  6)
#define CC1_RFLR (1 <<  7)
#define CC1_RALN (1 <<  8)
#define CC1_RXUO (1 <<  9)
#define CC1_RXPF (1 << 10)
#define CC1_RXCF (1 << 11)
#define CC1_RBCA (1 << 12)
#define CC1_RMCA (1 << 13)
#define CC1_RFCS (1 << 14)
#define CC1_RPTK (1 << 15)
#define CC1_TRMGV (1 << 25)
#define CC1_TRMAX (1 << 26)
#define CC1_TR1K (1 << 27)
#define CC1_TR511 (1 << 28)
#define CC1_TR255 (1 << 29)
#define CC1_TR127 (1 << 30)
#define CC1_TR64 (1 << 31)

#define CC2_TDRP (1 <<  0)
#define CC2_TPFH (1 <<  1)
#define CC2_TNCL (1 <<  2)
#define CC2_TXCL (1 <<  3)
#define CC2_TLCL (1 <<  4)
#define CC2_TMCL (1 <<  5)
#define CC2_TSCL (1 <<  6)
#define CC2_TEDF (1 <<  7)
#define CC2_TDFR (1 <<  8)
#define CC2_TXPF (1 <<  9)
#define CC2_TBCA (1 << 10)
#define CC2_TMCA (1 << 11)
#define CC2_TPKT (1 << 12)
#define CC2_TBYT (1 << 13)
#define CC2_TFRG (1 << 14)
#define CC2_TUND (1 << 15)
#define CC2_TOVR (1 << 16)
#define CC2_TXCF (1 << 17)
#define CC2_TFCS (1 << 18)
#define CC2_TJBR (1 << 19)

#define MASTER_TX_RX 	(1 << 0)
#define SLAVE_TX_RX 	(1 << 1)
#define MASTER_FETCH 	(1 << 2)
#define SLAVE_CLEAR 	(1 << 3)
#define ETHERNET_STATS 	(1 << 4)

/*====================================================================*
 *   variables;
 *--------------------------------------------------------------------*/

/*
 *  Station Statistics;
 */

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed station_stats

{
	uint8_t TEI;
	uint8_t RESULT_CODE;
	uint32_t UPSTREAM_MME_FRAMES;
	uint32_t UPSTREAM_ETH_FRAMES_UNI;
	uint32_t UPSTREAM_ETH_FRAMES_MULTI;
	uint32_t UPSTREAM_ETH_FRAMES_BROAD;
	uint32_t UPSTREAM_DROP_MME_FROM_HOST;
	uint32_t UPSTREAM_DROP_MME_FW_GEN;
	uint32_t UPSTREAM_DROP_ETH_FROM_HOST;
	uint32_t DOWNSTREAM_MME_FRAMES;
	uint32_t DOWNSTREAM_ETH_FRAMES_UNI;
	uint32_t DOWNSTREAM_ETH_FRAMES_MULTI;
	uint32_t DOWNSTREAM_ETH_FRAMES_BROAD;
	uint32_t DOWNSTREAM_DROP_MME_FROM_HOST;
	uint32_t DOWNSTREAM_DROP_MME_FW_GEN;
	uint32_t DOWNSTREAM_DROP_ETH_FROM_HOST;
}

statistics;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*
 *  Ethernet Statistics;
 */

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed ethernet_stats

{
	uint16_t HW_MODULE_TYPE;
	uint16_t HW_MODULE_STATS_LEN;
	uint32_t TR64;
	uint32_t TR127;
	uint32_t TR255;
	uint32_t TR511;
	uint32_t TR1K;
	uint32_t TRMAX;
	uint32_t TRMGV;
	uint32_t RBYT;
	uint32_t RBKT;
	uint32_t RFCS;
	uint32_t RFCA;
	uint32_t RBCA;
	uint32_t RXCF;
	uint32_t RXPF;
	uint32_t RXUO;
	uint32_t RALN;
	uint32_t RFLR;
	uint32_t RCDE;
	uint32_t RCSE;
	uint32_t RUND;
	uint32_t ROVR;
	uint32_t RFRG;
	uint32_t RJBR;
	uint32_t RDRP;
	uint32_t TBYT;
	uint32_t TPKT;
	uint32_t TMCA;
	uint32_t TBCA;
	uint32_t TXPF;
	uint32_t TDFR;
	uint32_t TEDF;
	uint32_t TSCL;
	uint32_t TMCL;
	uint32_t TLCL;
	uint32_t TXCL;
	uint32_t TNCL;
	uint32_t TPFH;
	uint32_t TDRP;
	uint32_t TJBR;
	uint32_t TFCS;
	uint32_t TXCF;
	uint32_t TOVR;
	uint32_t TUND;
	uint32_t TFRG;
	uint32_t CARRY1;
	uint32_t CARRY2;
}

ethernet_stats;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*
 *   Classification Counters
 */

#ifndef __GNUC__
#pragma pack (push,1)
#endif

typedef struct __packed class_counters

{
	uint16_t HW_MODULE_TYPE;
	uint16_t HW_MODULE_STATS_LEN;
	uint32_t IcvFailure;
	uint32_t InvalidMacFrame;
	uint32_t CorruptedMacFrameHeader;
	uint32_t MacDaCompareRule;
	uint32_t PriorityOrTHresholdLessWatermark;
	uint32_t OversizePacket;
	uint32_t Other;
	uint32_t SA_Learning;
}

class_counters;

#ifndef __GNUC__
#pragma pack (pop)
#endif

/*====================================================================*
 *
 *   void StationStats (struct plc * plc, struct station_stats * stats);
 *
 *   display station statistics in human readable format; there may
 *   be one or more such functions depending on the audience; this
 *   output was designed by the Product Verification Team in Ocala;
 *
 *
 *--------------------------------------------------------------------*/

static void StationStats (struct plc * plc, struct station_stats * stats)

{
	fprintf (stderr, "TEI %d\n", stats->TEI);
	fprintf (stderr, "ERR 0x%02X\n", stats->RESULT_CODE);
	fprintf (stderr, "UPSTREAM_MME 0x%08X\n", stats->UPSTREAM_MME_FRAMES);
	fprintf (stderr, "UPSTREAM_UNICAST 0x%08X\n", stats->UPSTREAM_ETH_FRAMES_UNI);
	fprintf (stderr, "UPSTREAM_MULTICAST 0x%08X\n", stats->UPSTREAM_ETH_FRAMES_MULTI);
	fprintf (stderr, "UPSTREAM_BROADCAST 0x%08X\n", stats->UPSTREAM_ETH_FRAMES_BROAD);
	fprintf (stderr, "UPSTREAM_DROP_HOST_MME 0x%08X\n", stats->UPSTREAM_DROP_MME_FROM_HOST);
	fprintf (stderr, "UPSTREAM_DROP_FW_MME 0x%08X\n", stats->UPSTREAM_DROP_MME_FW_GEN);
	fprintf (stderr, "UPSTREAM_DROP_HOST_ETH 0x%08X\n", stats->UPSTREAM_DROP_ETH_FROM_HOST);
	fprintf (stderr, "DOWNSTREAM_MME 0x%08X\n", stats->DOWNSTREAM_MME_FRAMES);
	fprintf (stderr, "DOWNSTREAM_UNICAST 0x%08X\n", stats->DOWNSTREAM_ETH_FRAMES_UNI);
	fprintf (stderr, "DOWNSTREAM_MULTICAST 0x%08X\n", stats->DOWNSTREAM_ETH_FRAMES_MULTI);
	fprintf (stderr, "DOWNSTREAM_BROADCAST 0x%08X\n", stats->DOWNSTREAM_ETH_FRAMES_BROAD);
	fprintf (stderr, "DOWNSTREAM_DROP_HOST_MME 0x%08X\n", stats->DOWNSTREAM_DROP_MME_FROM_HOST);
	fprintf (stderr, "DOWNSTREAM_DROP_FW_MME 0x%08X\n", stats->DOWNSTREAM_DROP_MME_FW_GEN);
	fprintf (stderr, "DOWNSTREAM_DROP_HOST_ETH 0x%08X\n", stats->DOWNSTREAM_DROP_ETH_FROM_HOST);
	fprintf (stderr, "\n");
	return;
}


/*====================================================================*
 *
 *   void EthernetStats (struct plc * plc, struct ethernet_stats * stats);
 *
 *
 *--------------------------------------------------------------------*/

static void EthernetStats (struct plc * plc, struct ethernet_stats * stats)

{
	return;
}


/*====================================================================*
 *
 *   void ClassificationCounters (struct plc * plc, struct class_counters * counters);
 *
 *
 *--------------------------------------------------------------------*/

#if 0

static void ClassificationCounters (struct plc * plc, struct class_counters * counters)

{
	return;
}


#endif

/*====================================================================*
 *
 *   signed MDUTrafficStats (struct plc * plc, uint8_t command, uint8_t session, uint8_t slave);
 *
 *   request MDU traffic statistics using one VS_MDU_TRAFFIC_STATS
 *   request type; different types of confirmations are returned
 *   depending on the type of request;
 *
 *
 *--------------------------------------------------------------------*/

signed MDUTrafficStats (struct plc * plc, uint8_t command, uint8_t session, uint8_t slave)

{
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_mdu_station_stats_request
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t COMMAND;
		uint8_t SESSION;
		uint32_t SLAVE_BITMAP [8];
	}
	* request = (struct vs_mdu_station_stats_request *) (message);
	struct __packed vs_mdu_traffic_master_confirm
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t COMMAND;
		uint8_t SESSION;
		uint16_t RESERVED;
		uint8_t NUM_SLAVES;
		uint8_t NUM_SLAVES_LEFT;
		uint16_t STATS_LEN;
		struct station_stats STATS [1];
	}
	* master_confirm = (struct vs_mdu_traffic_master_confirm *) (message);
	struct __packed vs_mdu_traffic_slave_confirm
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t COMMAND;
		uint8_t SESSION;
		uint16_t STATS_LEN;
		struct station_stats STATS [1];
	}
	* slave_confirm = (struct vs_mdu_traffic_slave_confirm *) (message);

#if 1

	struct __packed vs_eth_hardware_stats_confirm
	{
		struct ethernet_std ethernet;
		struct qualcomm_std qualcomm;
		uint8_t COMMAND;
		uint8_t SESSION;
		uint8_t CHIPTYPE;
		uint8_t STATUS;
		uint16_t STATS_LEN;
		struct ethernet_stats STATS [1];
	}
	* ether_confirm = (struct vs_eth_hardware_stats_confirm *) (message);

#endif

#ifndef __GNUC__
#pragma pack (pop)
#endif

	Request (plc, "Request MDU Traffic Statistics (1)");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader (&request->qualcomm, 0, (VS_MDU_TRAFFIC_STATS | MMTYPE_REQ));
	request->COMMAND = command;
	request->SESSION = session;
	set32bitmap (request->SLAVE_BITMAP, slave);
	plc->packetsize = sizeof (* request);
	if (SendMME (plc) <= 0)
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 0, (VS_MDU_TRAFFIC_STATS | MMTYPE_CNF)) > 0)
	{
		if ((request->COMMAND > 0x00) && (request->COMMAND < 0x0020))
		{
			struct station_stats * stats;
			unsigned count;
			if (_anyset (request->COMMAND, MASTER_TX_RX | SLAVE_TX_RX))
			{
				stats = master_confirm->STATS;
				count = LE16TOH (master_confirm->STATS_LEN);
			}
			else
			{
				stats = slave_confirm->STATS;
				count = LE16TOH (slave_confirm->STATS_LEN);
			}
			while (count >= sizeof (struct station_stats))
			{
				StationStats (plc, stats++);
				count -= sizeof (struct station_stats);
			}
			continue;
		}
		if ((request->COMMAND >= 0x20) && (request->COMMAND < 0x24))
		{
			EthernetStats (plc, ether_confirm->STATS);
			continue;
		}
		if ((request->COMMAND >= 0x24) && (request->COMMAND < 0x28))
		{
			EthernetStats (plc, ether_confirm->STATS);
			continue;
		}
		if ((request->COMMAND >= 0x28) && (request->COMMAND < 0x32))
		{
			EthernetStats (plc, ether_confirm->STATS);
			continue;
		}
	}
	return (0);
}


#endif

