/*====================================================================*
 *   
 *   Copyright (c) 2011 Qualcomm Atheros Inc.
 *   
 *   Permission to use, copy, modify, and/or distribute this software 
 *   for any purpose with or without fee is hereby granted, provided 
 *   that the above copyright notice and this permission notice appear 
 *   in all copies.
 *   
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
 *   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL  
 *   THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
 *   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 *   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 *   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 *   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *   
 *--------------------------------------------------------------------*/

/*====================================================================*
 * 
 *   debug.h - Debugging Definitions and Declarations;
 *
 *   this file is a subset of the original that includes only those
 *   definitions and declaration needed for toolkit programs;
 *
 *.  Qualcomm Atheros HomePlug AV Powerline Toolkit
 *:  Published 2009-2011 by Qualcomm Atheros. ALL RIGHTS RESERVED
 *;  For demonstration and evaluation only. Not for production use
 *
 *--------------------------------------------------------------------*/

#ifndef DEBUG_HEADER
#define DEBUG_HEADER

/*====================================================================*
 *   constants;
 *--------------------------------------------------------------------*/

#define QUALCOMM_DEBUG 1
#define QUALCOMM_TRACE 1

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#if QUALCOMM_TRACE
#define EVENT(s) printk(KERN_DEBUG "---[ %s ]---\n", (s))
#define ENTER printk(KERN_DEBUG "-->[ %s:%d ]\n", __func__,__LINE__)
#define ABORT printk(KERN_DEBUG "<--( %s:%d )\n", __func__,__LINE__)
#define LEAVE printk(KERN_DEBUG "<--[ %s:%d ]\n", __func__,__LINE__)
#define CRUMB printk(KERN_DEBUG "... %s:%d\n",__func__,__LINE__) 
#else
#define EVENT {}
#define ENTER {}
#define ABORT {}
#define LEAVE {}
#define CRUMB {}
#endif

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#if QUALCOMM_DEBUG
#define address(e) if (!(e)) { printk ("%s(%d): address %s is null\n",__func__,__LINE__,#e); }
#define confirm(e) if (!(e)) { printk ("%s(%d): assertion %s is wrong\n",__func__,__LINE__, #e); }
#else
#define address(e) {}
#define confirm(e) {}
#endif

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif

