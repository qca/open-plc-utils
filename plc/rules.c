/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros Inc.
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
 *   rules.c - Classification Rules Lookup Tables;
 *
 *   rules.h
 *
 *   QoS related symbol tables used by function ParseRule;
 *
 *   Contributor(s):
 *	Charles Maier <cmaier@qca.qualcomm.com>
 *	Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef RULES_SOURCE
#define RULES_SOURCE

#include "../plc/rules.h"

struct _code_ const controls [] =

{
	{
		CONTROL_ADD,
		"Add"
	},
	{
		CONTROL_REM,
		"Rem"
	},
	{
		CONTROL_REMOVE,
		"Remove"
	}
};

struct _code_ const volatilities [] =

{
	{
		VOLATILITY_TEMP,
		"Temp"
	},
	{
		VOLATILITY_PERM,
		"Perm"
	}
};

struct _code_ const actions [] =

{
	{
		ACTION_CAP0,
		"CAP0"
	},
	{
		ACTION_CAP1,
		"CAP1"
	},
	{
		ACTION_CAP2,
		"CAP2"
	},
	{
		ACTION_CAP3,
		"CAP3"
	},
	{
		ACTION_BOOST,
		"Boost"
	},
	{
		ACTION_DROP,
		"Drop"
	},
	{
		ACTION_DROPTX,
		"DropTX"
	},
	{
		ACTION_DROPRX,
		"DropRX"
	},
	{
		ACTION_AUTOCONNECT,
		"AutoConnect"
	},
	{
		ACTION_STRIPTX,
		"StripTX"
	},
	{
		ACTION_STRIPRX,
		"StripRX"
	},
	{
		ACTION_TAGTX,
		"TagTX"
	},
	{
		ACTION_TAGRX,
		"TagRX"
	}
};

struct _code_ const operands [] =

{
	{
		OPERAND_ALL,
		"All"
	},
	{
		OPERAND_ANY,
		"Any"
	},
	{
		OPERAND_ALWAYS,
		"Always"
	}
};

struct _code_ const fields [] =

{
	{
		FIELD_ETH_DA,
		"EthDA"
	},
	{
		FIELD_ETH_SA,
		"EthSA"
	},
	{
		FIELD_VLAN_UP,
		"VLANUP"
	},
	{
		FIELD_VLAN_ID,
		"VLANID"
	},
	{
		FIELD_IPV4_TOS,
		"IPv4TOS"
	},
	{
		FIELD_IPV4_PROT,
		"IPv4PROT"
	},
	{
		FIELD_IPV4_SA,
		"IPv4SA"
	},
	{
		FIELD_IPV4_DA,
		"IPv4DA"
	},
	{
		FIELD_IPV6_TC,
		"IPv6TC"
	},
	{
		FIELD_IPV6_FL,
		"IPv6FL"
	},
	{
		FIELD_IPV6_SA,
		"IPv6SA"
	},
	{
		FIELD_IPV6_DA,
		"IPv6DA"
	},
	{
		FIELD_TCP_SP,
		"TCPSP"
	},
	{
		FIELD_TCP_DP,
		"TCPDP"
	},
	{
		FIELD_UDP_SP,
		"UDPSP"
	},
	{
		FIELD_UDP_DP,
		"UDPDP"
	},
	{
		FIELD_IP_SP,
		"IPSP"
	},
	{
		FIELD_IP_DP,
		"IPDP"
	},
	{
		FIELD_HPAV_MME,
		"MME"
	},
	{
		FIELD_ETH_TYPE,
		"ET"
	},
	{
		FIELD_TCP_ACK,
		"TCPAck"
	},
	{
		FIELD_VLAN_TAG,
		"VLANTag"
	}
};

struct _code_ const operators [] =

{
	{
		OPERATOR_IS,
		"Is"
	},
	{
		OPERATOR_NOT,
		"Not"
	}
};

struct _code_ const states [] =

{
	{
		OPERATOR_IS,
		"True"
	},
	{
		OPERATOR_NOT,
		"False"
	},
	{
		OPERATOR_IS,
		"On"
	},
	{
		OPERATOR_NOT,
		"Off"
	},
	{
		OPERATOR_IS,
		"Yes"
	},
	{
		OPERATOR_NOT,
		"No"
	},
	{
		OPERATOR_IS,
		"Present"
	},
	{
		OPERATOR_NOT,
		"Missing"
	}
};

#endif

