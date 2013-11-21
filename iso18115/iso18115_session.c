/*====================================================================*
 *
 *   Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or 
 *   without modification, are permitted (subject to the limitations 
 *   in the disclaimer below) provided that the following conditions 
 *   are met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials 
 *     provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros nor the names of 
 *     its contributors may be used to endorse or promote products 
 *     derived from this software without specific prior written 
 *     permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE 
 *   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE 
 *   COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 *   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   void iso18115_session (struct session * session);
 *
 *   iso18115.h
 *
 *   print the SLAC session variable if the SLAC_SESSION bit is set 
 *   in the sesssion variable flagword;
 *
 *   macro HEXSTRING is defined in memory.h and is used to reduce the
 *   number of arguments one must type to invoke function hexstring;
 *
 *--------------------------------------------------------------------*/

#ifndef SLAC_SESSION_SOURCE
#define SLAC_SESSION_SOURCE

#include <stdio.h>

#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../iso18115/iso18115.h"

void iso18115_session (struct session * session) 

{ 
	if (_anyset (session->flags, SLAC_SESSION)) 
	{ 
		char string [256]; 
		debug (0, __func__, "session.RunID %s", HEXSTRING (string, session->RunID)); 
		debug (0, __func__, "session.APPLICATION_TYPE %d", session->APPLICATION_TYPE); 
		debug (0, __func__, "session.SECURITY_TYPE %d", session->SECURITY_TYPE); 
		debug (0, __func__, "session.RESP_TYPE %d", session->RESP_TYPE); 
		debug (0, __func__, "session.NUM_SOUNDS %d", session->NUM_SOUNDS); 
		debug (0, __func__, "session.TIME_OUT %d", session->TIME_OUT); 
		debug (0, __func__, "session.NumGroups %d", session->NumGroups); 
		debug (0, __func__, "session.AAG %s", hexstring (string, sizeof (string), session->AAG, sizeof (session->AAG))); 
		debug (0, __func__, "session.MSOUND_TARGET %s", HEXSTRING (string, session->MSOUND_TARGET)); 
		debug (0, __func__, "session.FORWARDING_STA %s", HEXSTRING (string, session->FORWARDING_STA)); 
		debug (0, __func__, "session.PEV_ID %s", HEXSTRING (string, session->PEV_ID)); 
		debug (0, __func__, "session.PEV_MAC %s", HEXSTRING (string, session->PEV_MAC)); 
		debug (0, __func__, "session.EVSE_ID %s", HEXSTRING (string, session->EVSE_ID)); 
		debug (0, __func__, "session.EVSE_MAC %s", HEXSTRING (string, session->EVSE_MAC)); 
		debug (0, __func__, "session.RND %s", HEXSTRING (string, session->RND)); 
		debug (0, __func__, "session.NMK %s", HEXSTRING (string, session->NMK)); 
		debug (0, __func__, "session.NID %s", HEXSTRING (string, session->NID)); 
		debug (0, __func__, "session.original_nmk %s", HEXSTRING (string, session->original_nmk)); 
		debug (0, __func__, "session.original_nid %s", HEXSTRING (string, session->original_nid)); 
		debug (0, __func__, "session.state %d", session->state); 
		debug (0, __func__, "session.sounds %d", session->sounds); 
		debug (0, __func__, "session.limit %d", session->limit); 
		debug (0, __func__, "session.pause %d", session->pause); 
		debug (0, __func__, "session.chargetime %d", session->chargetime); 
		debug (0, __func__, "session.settletime %d", session->settletime); 
		debug (0, __func__, "session.counter %d", session->counter); 
		debug (0, __func__, "session.flags 0x%04X", session->flags); 
	} 
	return; 
} 

#endif



