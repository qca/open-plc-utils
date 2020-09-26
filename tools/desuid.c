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

/*====================================================================*"
 *
 *   desuid.c - Permanently drop suid privileges
 *
 *   The desuid() function attempts to permanently drop the suid
 *   granted permissions.
 *
 *   Contributor(s):
 *      Nathaniel Houghton
 *
 *--------------------------------------------------------------------*/

#ifndef DESUID_SOURCE
#define DESUID_SOURCE

#if defined (__linux__)
#define _GNU_SOURCE
#include <unistd.h>
#elif defined (__APPLE__) || defined (__NetBSD__)
#include <unistd.h>
#elif defined (__OpenBSD__) || defined (__FreeBSD__)
#include <sys/types.h>
#include <unistd.h>
#elif defined (WIN32)
#else
#error "desuid() unimplemented"
#endif

#include "../tools/error.h"
#include "../tools/permissions.h"

#if defined (__linux__) || defined (__OpenBSD__) || defined (__FreeBSD__)

void
desuid(void)
{
	uid_t uid, euid, suid;

	if (getresuid(&uid, &euid, &suid) == -1)
	{
		error(1, errno, "could not determine privileges");
	}

	if (euid == uid && suid == uid)
	{
		return;
	}

	if (setresuid(uid, uid, uid) == -1)
	{
		error(1, errno, "could not revoke privileges");
	}
}

#elif defined (__APPLE__) || defined (__NetBSD__)

void
desuid(void)
{
	uid_t uid, euid;

	uid = getuid();
	euid = geteuid();

	if (uid == euid)
	{
		return;
	}

	if (setuid(uid) == -1)
	{
		error(1, errno, "could not revoke privileges");
	}

	/*
	 * If the saved uid is set to the effective uid, this will
	 * succeed, which means privileges could not be permanently
	 * revoked
	 */
	if (setuid(euid) != -1)
	{
		error(1, 0, "could not revoke privileges");
	}
}

#endif

#endif
