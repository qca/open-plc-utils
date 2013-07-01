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
 *   homeplug.h - HomePlug Definitions and Declarations;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Copyright (c) 2009-2013 by Qualcomm Atheros Inc. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use.
 *
 *   Contributor(s): 
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *	Alex Vasquez <alex.vasquez@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef HOMEPLUG_HEADER
#define HOMEPLUG_HEADER

/*====================================================================*
 *   HomePlug AV Constants;
 *--------------------------------------------------------------------*/

#define HOMEPLUG_MMV 0x01  
#define HOMEPLUG_MMTYPE 0x0000

/*====================================================================*
 * HomePlug Management Message Ranges for Information Only;
 *--------------------------------------------------------------------*/

#define CC_MMTYPE_MIN 0x0000
#define CC_MMTYPE_MAX 0x1FFF
#define CP_MMTYPE_MIN 0x2000
#define CP_MMTYPE_MAX 0x3FFF
#define NN_MMTYPE_MIN 0x4000
#define NN_MMTYPE_MAX 0x5FFF
#define CM_MMTYPE_MIN 0x6000
#define CM_MMTYPE_MAX 0x7FFF
#define MS_MMTYPE_MIN 0x8000
#define MS_MMTYPE_MAX 0x9FFF
#define VS_MMTYPE_MIN 0xA000 
#define VS_MMTYPE_MAX 0xBFFF
#define HA_MMTYPE_MIN 0xC000 
#define HA_MMTYPE_MAX 0xFFFF

/*====================================================================*
 * HomePlug AV MMEs have 4 variants indicated by the 2 MMTYPE LSBs;
 *--------------------------------------------------------------------*/

#define MMTYPE_CC 0x0000
#define MMTYPE_CP 0x2000
#define MMTYPE_NN 0x4000
#define MMTYPE_CM 0x6000
#define MMTYPE_MS 0x8000
#define MMTYPE_VS 0xA000
#define MMTYPE_XX 0xC000

#ifndef IHPAPI_HEADER
#define MMTYPE_REQ 0x0000
#define MMTYPE_CNF 0x0001
#define MMTYPE_IND 0x0002
#define MMTYPE_RSP 0x0003
#define MMTYPE_MODE  (MMTYPE_REQ|MMTYPE_CNF|MMTYPE_IND|MMTYPE_RSP)
#define MMTYPE_MASK ~(MMTYPE_REQ|MMTYPE_CNF|MMTYPE_IND|MMTYPE_RSP)
#endif

/*====================================================================*
 * HomePlug AV Management Message Types;
 *--------------------------------------------------------------------*/

#define CC_CCO_APPOINT 0x0000
#define CC_BACKUP_APPOINT 0x0004
#define CC_LINK_INFO 0x0008
#define CC_HANDOVER 0x000C
#define CC_HANDOVER_INFO 0x0010
#define CC_DISCOVER_LIST 0x0014
#define CC_LINK_NEW 0x0018
#define CC_LINK_MOD 0x001C
#define CC_LINK_SQZ 0x0020
#define CC_LINK_REL 0x0024
#define CC_DETECT_REPORT 0x0028
#define CC_WHO_RU 0x002C
#define CC_ASSOC 0x0030
#define CC_LEAVE 0x0034
#define CC_SET_TEI_MAP 0x0038
#define CC_RELAY 0x003C
#define CC_BEACON_RELIABILITY 0x0040
#define CC_ALLOC_MOVE 0x0044
#define CC_ACCESS_NEW 0x0048
#define CC_ACCESS_REL 0x004C
#define CC_DCPPC 0x0050
#define CC_HP1_DET 0x0054
#define CC_BLE_UPDATE 0x0058
#define CP_PROXY_APPOINT 0x2000
#define PH_PROXY_APPOINT 0x2004
#define CP_PROXY_WAKE 0x2008
#define NN_INL 0x4000
#define NN_NEW_NET 0x4004
#define NN_ADD_ALLOC 0x4008
#define NN_REL_ALLOC 0x400C
#define NN_REL_NET 0x4010
#define CM_ASSOCIATED_STA 0x6000
#define CM_ENCRYPTED_PAYLOAD 0x6004
#define CM_SET_KEY 0x6008
#define CM_GET_KEY 0x600C
#define CM_SC_JOIN 0x6010
#define CM_CHAN_EST 0x6014
#define CM_TM_UPDATE 0x6018
#define CM_AMP_MAP 0x601C
#define CM_BRG_INFO 0x6020
#define CM_CONN_NEW 0x6024
#define CM_CONN_REL 0x6028
#define CM_CONN_MOD 0x602C
#define CM_CONN_INFO 0x6030
#define CM_STA_CAP 0x6034
#define CM_NW_INFO 0x6038
#define CM_GET_BEACON 0x603C
#define CM_HFID 0x6040
#define CM_MME_ERROR 0x6044
#define CM_NW_STATS 0x6048
#define CM_SLAC_PARAM 0x6064
#define CM_START_ATTEN_CHAR 0x6068
#define CM_ATTEN_CHAR 0x606C
#define CM_PKCS_CERT 0x6070
#define CM_MNBC_SOUND 0x6074
#define CM_VALIDATE 0x6078
#define CM_SLAC_MATCH 0x607C
#define CM_SLAC_USER_DATA 0x6080
#define CM_ATTEN_PROFILE 0x6084

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

