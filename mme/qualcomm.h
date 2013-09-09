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
 *   qualcomm.h - Qualcomm Definitions and Declarations;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef QUALCOMM_HEADER
#define QUALCOMM_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdint.h>

/*====================================================================*
 *   header constants;
 *--------------------------------------------------------------------*/

#define BROADCAST "FF:FF:FF:FF:FF:FF"
#define LOCALCAST "00:B0:52:00:00:01"

/*====================================================================*
 * Qualcomm Manufacturer Specific Management Message Types;
 *--------------------------------------------------------------------*/

#define MS_PB_ENC 0x8000
#define MS_ADC_CAP 0x8004
#define MS_DISCOVER 0x8008

/*====================================================================*
 *  Qualcomm Vendor Specific Management Message Types;
 *--------------------------------------------------------------------*/

#define VS_SW_VER 0xA000
#define VS_WR_MEM 0xA004
#define VS_RD_MEM 0xA008
#define VS_ST_MAC 0xA00C
#define VS_GET_NVM 0xA010
#define VS_RSVD_1 0xA014
#define VS_RSVD_2 0xA018
#define VS_RS_DEV 0xA01C
#define VS_WR_MOD 0xA020
#define VS_RD_MOD 0xA024
#define VS_MOD_NVM 0xA028
#define VS_WD_RPT 0xA02C
#define VS_LNK_STATS 0xA030
#define VS_SNIFFER 0xA034
#define VS_NW_INFO 0xA038
#define VS_RSVD_3 0xA03C
#define VS_CP_RPT 0xA040
#define VS_ARPC 0xA044
#define VS_SET_KEY 0xA050
#define VS_MFG_STRING 0xA054
#define VS_RD_CBLOCK 0xA058
#define VS_SET_SDRAM 0xA05C
#define VS_HOST_ACTION 0xA060
#define VS_OP_ATTRIBUTES 0xA068
#define VS_ENET_SETTINGS 0xA06C
#define VS_TONE_MAP_CHAR 0xA070
#define VS_NW_INFO_STATS 0xA074
#define VS_SLAVE_MEM 0xA078
#define VS_FAC_DEFAULTS 0xA07C
#define VS_MULTICAST_INFO 0xA084
#define VS_CLASSIFICATION 0xA088
#define VS_RX_TONE_MAP_CHAR 0xA090
#define VS_SET_LED_BEHAVIOR 0xA094
#define VS_WRITE_AND_EXECUTE_APPLET 0xA098
#define VS_MDIO_COMMAND 0xA09C
#define VS_SLAVE_REG 0xA0A0
#define VS_BANDWIDTH_LIMITING 0xA0A4
#define VS_SNID_OPERATION 0xA0A8
#define VS_NN_MITIGATE 0xA0AC
#define VS_MODULE_OPERATION 0xA0B0
#define VS_DIAG_NETWORK_PROBE 0xA0B4
#define VS_PL_LINK_STATUS 0xA0B8
#define VS_GPIO_STATE_CHANGE 0xA0BC
#define VS_CONN_ADD 0xA0C0
#define VS_CONN_MOD 0xA0C4
#define VS_CONN_REL 0xA0C8
#define VS_CONN_INFO 0xA0CC
#define VS_MULTIPORT_LNK_STA 0xA0D0
#define VS_EM_ID_TABLE 0xA0DC
#define VS_STANDBY 0xA0E0
#define VS_SLEEPSCHEDULE 0xA0E4
#define VS_SLEEPSCHEDULE_NOTIFICATION 0xA0E8
#define VS_MICROCONTROLLER_DIAG 0xA0F0
#define VS_GET_PROPERTY 0xA0F8
#define VS_SET_PROPERTY 0xA100
#define VS_PHYSWITCH_MDIO 0xA104
#define VS_SELFTEST_ONETIME_CONFIG 0xA10C
#define VS_SELFTEST_RESULTS 0xA110
#define VS_MDU_TRAFFIC_STATS 0xA114
#define VS_FORWARD_CONFIG 0xA118
#define VS_HYBRID_INFO 0xA200

/*====================================================================*
 *   module codes for VS_RD_MOD, VS_WR_MOD and VS_MOD_NVM;
 *--------------------------------------------------------------------*/

#define VS_MODULE_MAC   (1 << 0)
#define VS_MODULE_PIB   (1 << 1)
#define VS_MODULE_FORCE (1 << 4)

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

