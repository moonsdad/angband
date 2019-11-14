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
#endif

#if defined(Pyramid) || defined(NeXT) || defined(sun) || \
    defined(NCR3K) || defined(linux) || defined(ibm032) || \
    defined (__osf__)
# include <sys/time.h>
#endif

#include <time.h>

#if !defined(NeXT) && !defined(ATARIST_MWC)
# include <fcntl.h>
#endif

#ifdef MAC
#else
# ifndef __TURBOC__
#  include <unistd.h>
# endif
# ifndef GEMDOS
#  ifdef VMS
#   include <types.h>
#  else
#   include <sys/types.h>
#  endif
# endif
#endif



#if defined(__MINT__)
# include <support.h>
#endif

#if !defined(ATARIST_MWC)
# ifdef __TURBOC__
#  include <mem.h>
# else
#  include <memory.h>
# endif
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
# endif
#endif

#if defined(GEMDOS) && (__STDC__ == 0)
# include <access.h>
  char *strcat();
#endif



#if !defined(linux)
  extern long atol();
#endif

#ifdef SYS_III
  extern char *index();
#endif




#endif
