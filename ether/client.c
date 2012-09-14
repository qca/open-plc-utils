/*====================================================================*
 *
 *   client.c -
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
 *
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
 *   program constants;
 *--------------------------------------------------------------------*/

#define SOCKET_NAME     "/tmp/server"

#define CLIENT_VERBOSE (1 << 0)
#define CLIENT_SILENCE (1 << 1)

/*====================================================================*
 *
 *   void client (char const * pathname);
 *   
 *--------------------------------------------------------------------*/

static void client (char const * pathname) 

{
	struct sockaddr_un sockaddr_un;
	char buffer [1024];
	signed length = 0;
	unsigned offset = 0;
	signed status;
	signed fd = -1;
	memset (&sockaddr_un, 0, sizeof (sockaddr_un));
	sockaddr_un.sun_family = AF_UNIX;
	strcpy (sockaddr_un.sun_path, pathname);
	while (1) 
	{
		fd = socket (AF_UNIX, SOCK_STREAM, 0);
		if (fd < 0) 
		{
			error (0, errno, "socket() failed");
			break;
		}
		status = connect (fd, (struct sockaddr *) (&sockaddr_un), SUN_LEN (&sockaddr_un));
		if (status < 0) 
		{
			error (0, errno, "connect () failed");
			break;
		}
		memset (buffer, 'a', sizeof (buffer));
		status = send (fd, buffer, sizeof (buffer), 0);
		if (status < 0) 
		{
			error (0, errno, "send () failed");
			break;
		}
		while (offset < sizeof (buffer)) 
		{
			length = recv (fd, buffer + offset, sizeof (buffer) - offset, 0);
			if (length < 0) 
			{
				error (1, errno, "recv () failed");
			}
			if (length > 0) 
			{
				offset += length;
			}
			write (STDOUT_FILENO, buffer, offset);
			write (STDOUT_FILENO, "\n", 1);
			break;
		}
		break;
	}
	if (fd != -1) 
	{
		close (fd);
	}
	return;
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
		"basic client",
		"q\tquiet",
		"v\tverbose",
		(char const *) (0)
	};
	flag_t flags = (flag_t)(0);
	signed c;
	optind = 1;
	while ((c = getoptv (argc, argv, optv)) != -1) 
	{
		switch (c) 
		{
		case 'q':
			_setbits (flags, CLIENT_SILENCE);
			break;
		case 'v':
			_setbits (flags, CLIENT_VERBOSE);
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
	client (SOCKET_NAME);
	return (0);
}

