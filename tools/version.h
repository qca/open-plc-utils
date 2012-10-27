/*====================================================================*
 *
 *   version.h - package version definitions and declarations;
 *
 *   Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *   Copyright (c) 2001-2006 by Charles Maier Associates;
 *   Licensed under the Internet Software Consortium License;
 *
 *--------------------------------------------------------------------*/

#ifndef VERSION_HEADER
#define VERSION_HEADER

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define COMPANY "Qualcomm Atheros, Ocala FL USA"
#define PACKAGE "Qualcomm Atheros Powerline Toolkit"
#define PROJECT "open-plc"
#define VERSION "0.0.1-dev"
#define RELEASE "OSR-6010"
#define COMPILE __TIME__ " " __DATE__ 
#define CONTACT "cmaier@qca.qualcomm.com"
#define LICENSE "Internet Software Concortium"

/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/

void version (void);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

