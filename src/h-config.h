/* File: h-config.h */

#ifndef INCLUDED_H_CONFIG_H
#define INCLUDED_H_CONFIG_H

/*
 * Choose the hardware, operating system, and compiler.
 * Also, choose various "system level" compilation options.
 * A lot of these definitions take effect in "h-system.h"
 */


/*
 * no system definitions are needed for 4.3BSD, SUN OS, DG/UX
 */

/*
 * OPTION: Compile on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
 * Do not define it if you are on any kind of SUN OS.
 */
/* #define ultrix */

/*
 * OPTION: Compile on a Macintosh with MPW C 3.0
 */
/* #define MAC */

/*
 * OPTION: Compile on a SYS V version of UNIX
 */
/* #define SYS_V */

/*
 * OPTION: Compile on a SYS III version of UNIX
 */
/* #define SYS_III */

/*
 * OPTION: Compile on an ATARI ST with Mark Williams C
 */
/* #define ATARIST_MWC */

/*
 * OPTION: Compiling on a HPUX version of UNIX
 */
/* #define HPUX */


/*
 * OPTION: Compile on Solaris, treat it as System V
 */
#if defined(SOLARIS)
# define SYS_V
#include <netdb.h>
#endif


/*
 * Note that most compilers will do better WITHOUT "register" suggestions.
 * If your compiler is ancient or horrible, comment out the "define" below.
 */
#define register /* nothing */


/*
 * Note that old compilers do not understand "const".
 * If you have an good compiler, comment out the "define" below.
 */
#if !defined(__GNUC__) && !defined(__STDC__)
# define const /* nothing */
#endif





/*
 *  Prevent "lint" messages
 *
 * to prevent <string.h> from including <NLchar.h>, this prevents a bunch of lint errors.
 */
if defined(lint)
# if defined(SYS_V)
#  define RTPC_NO_NLS
# endif
#endif





/*
 * Every system seems to use its own symbol as a path separator.
 * Default to the standard Unix slash, but attempt to change this
 * for various other systems.
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


/*
 * Note that you'll be happier if you have a case-insensitive string
 * comparision routine on your system.  If your system lacks this,
 * you're still in luck, as we now provide one.  -CWS
 */
#if defined (NeXT) || defined(HPUX) || defined(ultrix) || \
    defined(NCR3K) || defined(linux) || defined(ibm032) || \
    defined(__386BSD__) || defined(SOLARIS) || defined (__osf__)
# define stricmp strcasecmp
#else
/* Let's make this work on systems lacking a such a routine. */
#define stricmp my_stricmp
#define NEEDS_STRICMP
#endif



#endif


