/* File: arrays.c */ 

/* Purpose: initialize various arrays */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include <stdio.h>
#include <errno.h>

#ifndef MSDOS
#include <sys/param.h>
#endif

#include "angband.h"

#ifdef ibm032
#include <ctype.h>
#endif

#ifdef MSDOS
extern int          errno;

#endif

#if defined(GEMDOS) && (__STDC__ == 0)
#include <access.h>
char               *strcat();

#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#include <fcntl.h>
#ifdef __MINT__
#include <ctype.h>
#endif
#endif
#else
#include <strings.h>
#ifndef VMS
#include <sys/file.h>
#else
#include <file.h>
#endif
#endif

#ifndef VMS
#if defined(ultrix) || defined(USG)
void exit();

#endif
#endif


/* This is done first in main.c, so I'll rudely drop it into the front
 * of files.c...  If NEW_FILEPATHS is defined, we'll look for the ANGBAND_PATH
 * environment variable, and then look for the files in there.  This is
 * much nicer than having explicit paths, and should help us make things
 * more portable.  Now, if only someone would graft on the NetHack display
 * code...
 *
 * This is rather inelegant code; no checks are made for malloc() or
 * realloc() failures.  If you malloc/realloc are broken, you're in
 * trouble, and your compiler vendor needs a good swift kick in the
 * forehead. [cjh]
 */
#ifdef NEW_FILEPATHS
char *ANGBAND_TST;		/* was LIBDIR(test)			*/
char *ANGBAND_HOU;		/* was LIBDIR(files/hours)		*/
char *ANGBAND_MOR;		/* was LIBDIR(files/news)		*/
char *ANGBAND_TOP;		/* was LIBDIR(files/newscores)		*/
char *ANGBAND_BONES;		/* was LIBDIR(bones)			*/
char *ANGBAND_HELP;		/* was LIBDIR(files/roglcmds.hlp)	*/
char *ANGBAND_ORIG_HELP;	/* was LIBDIR(files/origcmds.hlp)	*/
char *ANGBAND_WIZ_HELP;		/* was LIBDIR(files/rwizcmds.hlp)	*/
char *ANGBAND_OWIZ_HELP;	/* was LIBDIR(files/owizcmds.hlp)	*/
char *ANGBAND_WELCOME;		/* was LIBDIR(files/welcome.hlp)	*/
char *ANGBAND_LOG;		/* was LIBDIR(files/ANGBAND.log)	*/
char *ANGBAND_VER;		/* was LIBDIR(files/version.hlp)	*/
char *ANGBAND_LOAD;		/* was LIBDIR(files/loadcheck)		*/
char *ANGBAND_WIZ;		/* was LIBDIR(files/wizards)		*/
char *ANGBAND_SAV;		/* was LIBDIR(save)			*/

/*
 * Find the path to all of our important files and directories...
 * Use the ANGBAND_PATH environment var if possible, else use DEFAULT_PATH,
 *
 *  If your system can't do "getenv()", you'll have to kludge this.  [cjh]
 */
void get_file_paths()
{
#ifndef MAXPATHLEN
#define BOZO_MAXPATHLEN
#define MAXPATHLEN 512
#endif

    /* Grab the base "path" */
    char *angband_path = NULL;


    angband_path = getenv( "ANGBAND_PATH" );
    if (angband_path == NULL) {

	angband_path = (char *)malloc( strlen( DEFAULT_PATH ) + 1 );
	strcpy( angband_path, DEFAULT_PATH );
    }

    ANGBAND_TST = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_TST, angband_path );
    strcat( ANGBAND_TST, PATH_SEP );
    strcat( ANGBAND_TST, "test" );
    ANGBAND_TST = (char *)realloc( ANGBAND_TST, strlen( ANGBAND_TST ) + 1 );

    ANGBAND_HOU = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_HOU, angband_path );
    strcat( ANGBAND_HOU, PATH_SEP );
    strcat( ANGBAND_HOU, "files" );
    strcat( ANGBAND_HOU, PATH_SEP );
    strcat( ANGBAND_HOU, "hours" );
    ANGBAND_HOU = (char *)realloc( ANGBAND_HOU, strlen( ANGBAND_HOU ) + 1 );

    ANGBAND_MOR = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_MOR, angband_path );
    strcat( ANGBAND_MOR, PATH_SEP );
    strcat( ANGBAND_MOR, "files" );
    strcat( ANGBAND_MOR, PATH_SEP );
    strcat( ANGBAND_MOR, "news" );
    ANGBAND_MOR = (char *)realloc( ANGBAND_MOR, strlen( ANGBAND_MOR ) + 1 );

    ANGBAND_TOP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_TOP, angband_path );
    strcat( ANGBAND_TOP, PATH_SEP );
    strcat( ANGBAND_TOP, "files" );
    strcat( ANGBAND_TOP, PATH_SEP );
    strcat( ANGBAND_TOP, "newscores" );
    ANGBAND_TOP = (char *)realloc( ANGBAND_TOP, strlen( ANGBAND_TOP ) + 1 );

    ANGBAND_BONES = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_BONES, angband_path );
    strcat( ANGBAND_BONES, PATH_SEP );
    strcat( ANGBAND_BONES, "bones" );
    ANGBAND_BONES = (char *)realloc( ANGBAND_BONES, strlen( ANGBAND_BONES ) + 1 );

    ANGBAND_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_HELP, angband_path );
    strcat( ANGBAND_HELP, PATH_SEP );
    strcat( ANGBAND_HELP, "files" );
    strcat( ANGBAND_HELP, PATH_SEP );
    strcat( ANGBAND_HELP, "roglcmds.hlp" );
    ANGBAND_HELP = (char *)realloc( ANGBAND_HELP, strlen( ANGBAND_HELP ) + 1 );

    ANGBAND_ORIG_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_ORIG_HELP, angband_path );
    strcat( ANGBAND_ORIG_HELP, PATH_SEP );
    strcat( ANGBAND_ORIG_HELP, "files" );
    strcat( ANGBAND_ORIG_HELP, PATH_SEP );
    strcat( ANGBAND_ORIG_HELP, "origcmds.hlp" );
    ANGBAND_ORIG_HELP = (char *)realloc(ANGBAND_ORIG_HELP,
					strlen( ANGBAND_ORIG_HELP ) + 1 );

    ANGBAND_WIZ_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_WIZ_HELP, angband_path );
    strcat( ANGBAND_WIZ_HELP, PATH_SEP );
    strcat( ANGBAND_WIZ_HELP, "files" );
    strcat( ANGBAND_WIZ_HELP, PATH_SEP );
    strcat( ANGBAND_WIZ_HELP, "rwizcmds.hlp" );
    ANGBAND_WIZ_HELP = (char *)realloc(ANGBAND_WIZ_HELP,
				       strlen( ANGBAND_WIZ_HELP ) + 1 );

    ANGBAND_OWIZ_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_OWIZ_HELP, angband_path );
    strcat( ANGBAND_OWIZ_HELP, PATH_SEP );
    strcat( ANGBAND_OWIZ_HELP, "files" );
    strcat( ANGBAND_OWIZ_HELP, PATH_SEP );
    strcat( ANGBAND_OWIZ_HELP, "owizcmds.hlp" );
    ANGBAND_OWIZ_HELP = (char *)realloc(ANGBAND_OWIZ_HELP,
					strlen( ANGBAND_OWIZ_HELP ) + 1 );

    ANGBAND_WELCOME = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_WELCOME, angband_path );
    strcat( ANGBAND_WELCOME, PATH_SEP );
    strcat( ANGBAND_WELCOME, "files" );
    strcat( ANGBAND_WELCOME, PATH_SEP );
    strcat( ANGBAND_WELCOME, "welcome.hlp" );
    ANGBAND_WELCOME = (char *)realloc( ANGBAND_WELCOME, strlen( ANGBAND_WELCOME ) + 1 );

    ANGBAND_LOG = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_LOG, angband_path );
    strcat( ANGBAND_LOG, PATH_SEP );
    strcat( ANGBAND_LOG, "files" );
    strcat( ANGBAND_LOG, PATH_SEP );
    strcat( ANGBAND_LOG, "ANGBAND.log" );
    ANGBAND_LOG = (char *)realloc( ANGBAND_LOG, strlen( ANGBAND_LOG ) + 1 );

    ANGBAND_VER = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_VER, angband_path );
    strcat( ANGBAND_VER, PATH_SEP );
    strcat( ANGBAND_VER, "files" );
    strcat( ANGBAND_VER, PATH_SEP );
    strcat( ANGBAND_VER, "version.hlp" );
    ANGBAND_VER = (char *)realloc( ANGBAND_VER, strlen( ANGBAND_VER ) + 1 );

    ANGBAND_LOAD = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_LOAD, angband_path );
    strcat( ANGBAND_LOAD, PATH_SEP );
    strcat( ANGBAND_LOAD, "files" );
    strcat( ANGBAND_LOAD, PATH_SEP );
    strcat( ANGBAND_LOAD, "loadcheck" );
    ANGBAND_LOAD = (char *)realloc( ANGBAND_LOAD, strlen( ANGBAND_LOAD ) + 1 );

    ANGBAND_WIZ = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_WIZ, angband_path );
    strcat( ANGBAND_WIZ, PATH_SEP );
    strcat( ANGBAND_WIZ, "files" );
    strcat( ANGBAND_WIZ, PATH_SEP );
    strcat( ANGBAND_WIZ, "wizards" );
    ANGBAND_WIZ = (char *)realloc( ANGBAND_WIZ, strlen( ANGBAND_WIZ ) + 1 );

    ANGBAND_SAV = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_SAV, angband_path );
    strcat( ANGBAND_SAV, PATH_SEP );
    strcat( ANGBAND_SAV, "save" );
    ANGBAND_SAV = (char *)realloc( ANGBAND_SAV, strlen( ANGBAND_SAV ) + 1 );

    return;
}

#ifdef BOZO_MAXPATHLEN
#undef BOZO_MAXPATHLEN
#undef MAXPATHLEN
#endif /* BOZO_MAXPATHLEN */

#endif /* NEW_FILEPATHS */
