/* File: h-system.h */

#ifndef INCLUDED_H_SYSTEM_H
#define INCLUDED_H_SYSTEM_H

/* Include some very basic system header files */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#if defined(Pyramid) || defined(NeXT) || defined(sun) || \
    defined(NCR3K) || defined(linux) || defined(ibm032) || \
    defined (__osf__)
# include <sys/time.h>
#endif


/* this takes care of almost all "implicit declaration" warnings -CWS */

#if defined(NeXT)
#include <libc.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#endif


#include <time.h>


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


#ifndef linux
long atol();
#endif



#endif

