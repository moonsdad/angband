/* File: h-system.h */

#ifndef INCLUDED_H_SYSTEM_H
#define INCLUDED_H_SYSTEM_H

/*
 * Include the basic "system" files.
 * Make sure all "system" constants/macros are defined.
 * Make sure all "system" functions have "extern" declarations.
 */


/* Include some very basic system header files */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#if defined(NeXT)
# include <libc.h>
#else
# include <stdlib.h>
#include <unistd.h>
#endif

#if defined(Pyramid) || defined(NeXT) || defined(sun) || \
    defined(NCR3K) || defined(linux) || defined(ibm032) || \
    defined (__osf__)
# include <sys/time.h>
#endif


#include <time.h>

#if !defined(NeXT)
# include <fcntl.h>
#endif

/* This does not seem to work... */
/* #include <ansi.h> */


#if defined(__MINT__)
# include <support.h>
#include <string.h>
#endif


#ifdef USG
# ifdef ATARIST_MWC
   extern char *strcat();
   extern char *strcpy();
   extern int   strlen();
# else
#  include <string.h>
# endif
#else
# ifdef VMS
#  include <file.h>
# else
#  include <strings.h>
#  include <sys/file.h>
   extern char *strchr();
   extern char *strstr();
# endif
#endif

#if !defined(linux)
  extern long atol();
#endif


#endif

