/*====================================================================*
 *
 *   error.h - error function definitions and declarations;
 *
 *   this file is an alterantive to GNU header file of the same
 *   name; in addition to standard GNU error function declarations,
 *   some additional functions are declared; 
 *
 *.  Motley Tools by Charles Maier <cmaier@cmassoc.net>;
 *:  Published 2005 by Charles Maier Associates Limited;
 *;  Licensed under GNU General Public Licence Version 2 only;
 *
 *--------------------------------------------------------------------*/

#ifndef ERROR_HEADER
#define ERROR_HEADER

/*====================================================================*
 *   system header files; 
 *--------------------------------------------------------------------*/

#include <errno.h>

/*====================================================================*
 *   custom header files; 
 *--------------------------------------------------------------------*/

#include "../tools/types.h"

/*====================================================================*
 *    
 *--------------------------------------------------------------------*/

#if defined (WIN32)
#define __func__ __FUNCTION__
#endif

/*====================================================================*
 *   define error codes for systems that do not support POSIX codes;
 *--------------------------------------------------------------------*/

#ifndef ECANCELED
#define ECANCELED 0
#endif
#ifndef ENOTSUP
#define ENOTSUP EPERM
#endif
#ifndef EBADMSG
#define EBADMSG 0
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 124
#endif
#ifndef ENODATA
#define ENODATA 0
#endif
#ifndef EOVERFLOW
#define EOVERFLOW 0
#endif

/*====================================================================*
 *   define common error message strings; 
 *--------------------------------------------------------------------*/

#define ERROR_NOTROOT "This program needs root privileges"
#define ERROR_TOOMANY "Too many command line arguments"

#define CANT_START_TIMER "function %s can't start timer", __func__
#define CANT_RESET_TIMER "function %s can't reset timer", __func__

/*====================================================================*
 *    
 *--------------------------------------------------------------------*/

#define TRACE error (0, 0, "%s (%d)", __FILE__, __LINE__);

/*====================================================================*
 *   declare GNU error() and error_at_line() functions;  
 *--------------------------------------------------------------------*/

#ifdef __GNUC__

__attribute__ ((format (printf, 3, 4))) 

#endif

void error (signed status, errno_t number, char const * format, ...);

#ifdef __GNUC__

__attribute__ ((format (printf, 5, 6))) 

#endif

void error_at_line (signed status, errno_t number, char const * filename, unsigned lineno, char const * format, ...);

/*====================================================================*
 *   declare GNU error() function alternatives;  
 *--------------------------------------------------------------------*/

void extra (signed status, errno_t number, int argc, char const * argv []);

#ifdef __GNUC__

__attribute__ ((format (printf, 3, 4))) 

#endif

void event (signed status, signo_t number, char const * format, ...);

#ifdef __GNUC__

__attribute__ ((format (printf, 1, 2))) 

#endif

void alert (char const * format, ...);

/*====================================================================*
 *   end definitions and declarations;
 *--------------------------------------------------------------------*/

#endif

