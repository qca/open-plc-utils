/*====================================================================*
 *
 *   socket.h - substitute sys/socket.h for systems without one;
 *
 *   include Microsoft equivalent definitions and declarations;
 * 
 *.  Intellon INT6000 Linux Toolkit for HomePlug AV;
 *:  Published 2006-2009 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration; Not for production use;
 *
 *--------------------------------------------------------------------*/

#ifndef SOCKET_HEADER
#define SOCKET_HEADER

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#if defined (WIN32)
#include <winsock2.h>
#endif

/*====================================================================*
 *   end;
 *--------------------------------------------------------------------*/

#endif

