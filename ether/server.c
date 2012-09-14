/*====================================================================*
 *
 *   server.c -
 *
 *
 *--------------------------------------------------------------------*/

/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/getoptv.h"
#include "../tools/putoptv.h"
#include "../tools/version.h"
#include "../tools/error.h"
#include "../tools/flags.h"

/*====================================================================*
 *   custom source files;
 *--------------------------------------------------------------------*/

#ifndef MAKEFILE
#include "../tools/getoptv.c"
#include "../tools/putoptv.c"
#include "../tools/version.c"
#include "../tools/error.c"
#endif

/*====================================================================*
 *   program variables;
 *--------------------------------------------------------------------*/

#define SOCKET_NAME "/tmp/server"

#define SERVER_VERBOSE (1 << 0)
#define SERVER_SILENCE (1 << 1)

/*====================================================================*
 *
 *   void * server (char const * pathname);
 *
 *--------------------------------------------------------------------*/

static void * server (char const * pathname) 

{
	struct sockaddr_un sockaddr_un;
	char buffer [1024];
	signed length = sizeof (buffer);
	signed status;
	signed sd1 = socket (AF_UNIX, SOCK_STREAM, 0);
	signed sd2 = -1;
	memset (&sockaddr_un, 0, sizeof (sockaddr_un));
	sockaddr_un.sun_family = AF_UNIX;
	strcpy (sockaddr_un.sun_path, pathname);
	if (sd1 == -1) 
	{
		error (1, errno, "socket () failed");
	}
	if (bind (sd1, (struct sockaddr *) (&sockaddr_un), SUN_LEN (&sockaddr_un)) == -1) 
	{
		error (1, errno, "bind () failed");
	}
	if (listen (sd1, 10) == -1) 
	{
		error (1, errno, "listen () failed");
	}
	error (0, 0, "listening ...");
	while (1) 
	{
		sd2 = accept (sd1, NULL, NULL);
		if (sd2 < 0) 
		{
			error (0, errno, "accept () failed");
			break;
		}
		if (setsockopt (sd2, SOL_SOCKET, SO_RCVLOWAT, (char *) (&length), sizeof (length)) == -1) 
		{
			error (0, errno, "setsockopt () failed");
			break;
		}
		status = recv (sd2, buffer, sizeof (buffer), 0);
		if (status < 0) 
		{
			error (0, errno, "recv () failed");
			break;
		}
		if (status < (signed)(sizeof (buffer))) 
		{
			error (0, ECANCELED, "client closed the connection before all data was sent");
			break;
		}
		status = send (sd2, buffer, sizeof (buffer), 0);
		if (status < 0) 
		{
			error (0, errno, "send () failed");
			break;
		}
		break;
	}
	if (sd1 != -1) 
	{
		close (sd1);
	}
	if (sd2 != -1) 
	{
		close (sd2);
	}
	unlink (pathname);
	return (0);
}


/*====================================================================*
 *   
 *   int main (int argc, char const * argv []) 
 *
 *--------------------------------------------------------------------*/

int main (int argc, char const * argv []) 

{
	static char const * optv [] = 
	{
		"qv",
		"",
		"basic server",
		"q\tquiet",
		"v\tverbose",
		(char const *) (0)
	};
	flag_t flags =(flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch ((char) (c)) 
		{
		case 'q':
			_setbits (flags, SERVER_SILENCE);
			break;
		case 'v':
			_setbits (flags, SERVER_VERBOSE);
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc) 
	{
		error (1, ECANCELED, ERROR_TOOMANY);
	}
	server (SOCKET_NAME);
	return (0);
}

