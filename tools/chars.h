/*====================================================================*:wq
 *
 *   chars.h - character selection and matcing macros; 
 *
 *.  published 2003 by charles maier associates limited for internal use;
 *:  modified sun 07 sep 03 by user root on host zeus;
 *;  compiled for debian gnu/linux using gcc compiler;
 *
 *--------------------------------------------------------------------*/

#ifndef CHARS_HEADER
#define CHARS_HEADER

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#ifndef isblank
#ifndef __CYGWIN__
#define isblank(c) ((char)(c) == ' ') || ((char)(c) == '\t')
#endif
#endif

#ifndef nobreak
#define nobreak(c) ((char)(c) != '\n') && ((int)(c) != EOF)
#endif

#ifndef isquote
#define isquote(c) ((char)(c) == '\'') || ((char)(c) == '\"')
#endif

#ifndef isslash
#define isslash(c) ((char)(c) == '/') || ((char)(c) == '\\')
#endif

#ifndef isident
#define isident(c) (isalnum (c) || (c == '_') || (c == '-') || (c == '.') || (c == ':'))
#endif

#ifndef isoctal
#define isoctal(c) ((char)(c) >= '0') && ((char)(c) <= '7')
#endif

#ifndef nomatch
#define nomatch(c,o) ((char)(c) != (char)(o)) && ((int)(c) != EOF)
#endif

#ifndef iskey
#define iskey(c) ((int)(c) < 0x20) || ((int)(c) > 0x7E)
#endif

/*====================================================================*
 *   end definitions;
 *--------------------------------------------------------------------*/

#endif

