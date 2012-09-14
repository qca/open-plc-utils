/*====================================================================*
 *
 *   in.h - Substitute Linux header for systems without one;
 *
 *   Emulate the POSIX header netinet/in.h; 
 *
 *   specifically, obtain declare endian conversion functions htons(), 
 *   ntohs(), htonl() and ntohl();
 *
 *.  Intellon Linux Toolkit; 
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *   Contributor(s):
 *	Charles Maier <charles.maier@atheros.com>
 *
 *--------------------------------------------------------------------*/

#ifndef IN_HEADER
#define IN_HEADER
 
/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#if defined (WIN32)
#include <winsock2.h>
#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif
 

