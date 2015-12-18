/* File: h-config.h */

#ifndef INCLUDED_H_CONFIG_H
#define INCLUDED_H_CONFIG_H





/* this allows intelligent compilers to do better, as they know more
 * about how certain functions behave -CWS */

#if !(defined(__GNUC__) || defined(__STDC__))
#define const
#endif


/* no system definitions are needed for 4.3BSD, SUN OS, DG/UX */

/* if you are compiling on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
   define this, not needed for SUNs */
/* #ifndef ultrix
#define ultrix
#endif */

#if defined(SOLARIS)
#define SYS_V
#include <netdb.h>
#endif

/* if you are compiling on a SYS V version of UNIX, define this */
/* #define SYS_V */

/* if you are compiling on a SYS III version of UNIX, define this */
/* #define SYS_III */

/* if you are compiling on an ATARI ST with Mark Williams C, define this */
/* #define ATARIST_MWC */

/* if you are compiling on a Macintosh with MPW C 3.0, define this */
/* #define MAC */

/* if you are compiling on a HPUX version of UNIX, define this */
/* #define HPUX */


/*
 *  Prevent "lint" messages
 * to prevent <string.h> from including <NLchar.h>, this prevents a bunch of lint errors.
 */
#if defined(SYS_V) && defined(lint)
#define RTPC_NO_NLS
#endif

/****************************************************************************
 * System dependent defines follow, you should not need to change anything  *
 * below (if you have a supported system).  If you run into problems during *
 * compilation, you might want to check the defines below.                  *
 ****************************************************************************/

/* For the NEW_FILEPATHS option, we'll use PATH_SEP as the path separator;
 * this will help make at least one section of Angband more portable.  If
 * you don't seem something sensible here, either add a section for your
 * filesystem, or just define PATH_SEP to something useful.          [cjh]
 */

/*
 * NOTE: This is 100% untested on all but Atari, UNIX, and OS/2...  I'm
 *       guessing at the Mac and VMS PATH_SEP values!                [cjh]
 */

#if defined(ultrix) || defined(SYS_V) || defined(SYS_III) \
 || defined(__MINT__) || defined(HPUX) || defined(unix) \
 || defined(BSD)
#  define PATH_SEP "/"
#else
#  if defined(__EMX__) || defined(MSDOS) || defined(OS2) || defined(WINNT) \
   || defined(ATARIST_MWC) || defined(ATARI) || defined(ATARIST)
#    define PATH_SEP "\\"
#  else
#    ifdef MAC
#      define PATH_SEP ":" /* or is it "::"? */
#    else
#      ifdef VMS
#        define PATH_SEP "."
#      endif /* VMS */
#    endif /* Mac */
#  endif /* DOS filesystems */
#endif /* UNIX filesystems */


/* Note that you'll be happier if you have a case-insensitive string
 * comparision routine on your system.  If your system lacks this,
 * you're still in luck, as we now provide one.  -CWS
 */

#if defined (NeXT) || defined(HPUX) || defined(ultrix) \
|| defined(NCR3K) || defined(linux) || defined(ibm032) \
|| defined(__386BSD__) || defined(SOLARIS) || defined (__osf__)
#define stricmp strcasecmp
#else
/* Let's make this work on systems lacking a such a routine. */
#define stricmp my_stricmp
#define NEEDS_STRICMP
#endif



#endif


