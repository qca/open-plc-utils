/*====================================================================*
 *
 *   version.h - package version definitions and declarations;
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2001-2006 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef VERSION_HEADER
#define VERSION_HEADER

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define COMPANY "Qualcomm Atheros, Ocala FL USA"
#define PACKAGE "Qualcomm Atheros Powerline Toolkit"
#define PROJECT "plc-utils"
#define VERSION "2.1"
#define RELEASE "2-dev"
#define COMPILE __TIME__ " " __DATE__ 
#define CONTACT "cmaier@qualcomm.com"
#define LICENSE "Restricted License"

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

void version (void);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

