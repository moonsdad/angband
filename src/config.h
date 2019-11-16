/* config.h: configuration definitions
 *
 * Copyright (c) 1989 James E. Wilson
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* Person to bother if something goes wrong.
 * Recompile files.c and misc2.c if these change.
 */

#define WIZARD	"root"



/* Other miscellaneous defines that can be configured as the local maintainer
 * wishes.
 */

#define SET_UID		         /* define on multi-user systems                 */
#undef CHECK_HOURS            /* define if checking the 'hours' file          */
#define ALLOW_FIDDLING       /* Allow the players to copy save files         */
#define ALLOW_SCORE          /* Allow the user to check his score (v-key)    */
#define ALLOW_ARTIFACT_CHECK /* Allow the user to check artifacts            */
#define ALLOW_CHECK_UNIQUES  /* Allow player to check (dead) unique monsters */
#define TARGET               /* Enable targeting mode                        */
#define AUTOROLLER           /* Allow autorolling of characters              */
#undef NICE		             /* Be nice to other users during autorolling    */
#define SATISFY_HUNGER		 /* Do 'satisfy hunger' rather then 'create food'
								to reduce the tedium for handling food. -CWS */


/*****************************************************
 * files used by moria, set these to valid pathnames *
 *****************************************************/

/* Define OLD_FILEPATHS to use the old, nasty way of putting complete
 * paths directly into Angband.  Define NEW_FILEPATHS to use the new,
 * studly method (which uses the ANGBAND_PATH environment variable to
 * look for the files).                                         [cjh]
 */

#undef  OLD_FILEPATHS
#define NEW_FILEPATHS


/* Change this to something sensible, if necessary.  Angband will look
 * in this directory if the ANGBAND_PATH environment variable isn't   
 * set, or if the files aren't found in ANGBAND_PATH.            [cjh]
 */

#ifdef NEW_FILEPATHS
#define DEFAULT_PATH "/User/games/lib/angband"
#endif

#ifdef OLD_FILEPATHS

/* Try to fix filename inclusion in a portable fashion.
 * John Whitly@cs.Buffalo.edu says this works under gcc 2.5.5, but my
 * older version chokes.  I dunno. -CWS
 */

#ifdef __STDC__
#define LIBDIR(FILE) "/User/games/lib/angband/" #FILE
#else
#define LIBDIR(FILE) "/User/games/lib/angband/FILE"
#endif

/* probably unix */
#define ANGBAND_TST       LIBDIR(test)
#define ANGBAND_HOURS       LIBDIR(files/hours)
#define ANGBAND_NEWS       LIBDIR(files/news.hlp)
#define ANGBAND_TOP       LIBDIR(files/newscores)
#define ANGBAND_DIR_BONES     LIBDIR(bones)
#define ANGBAND_R_HELP      LIBDIR(files/cmds_r.hlp)
#define ANGBAND_O_HELP LIBDIR(files/cmds_o.hlp)
#define ANGBAND_W_HELP  LIBDIR(files/cmds_w.hlp)
#define ANGBAND_OWIZ_HELP LIBDIR(files/owizcmds.hlp)
#define ANGBAND_WELCOME   LIBDIR(files/welcome.hlp)
#define ANGBAND_LOG       LIBDIR(files/ANGBAND.log)
#define ANGBAND_VERSION       LIBDIR(files/version.hlp)
#define ANGBAND_LOAD      LIBDIR(files/loadcheck)
#define ANGBAND_WIZ       LIBDIR(files/wizards.txt)
#define ANGBAND_DIR_SAVE       LIBDIR(save)
#endif /* OLD_FILEPATHS */


/* this sets the default user interface
 * to use the original key bindings (keypad for movement) set ROGUE_LIKE
 * to FALSE, to use the rogue-like key bindings (vi style movement)
 * set ROGUE_LIKE to TRUE
 * if you change this, you only need to recompile main.c */

#define ROGUE_LIKE TRUE


/* for the AFS distributed file system, define this to ensure that
   the program is secure with respect to the setuid code, this prohibits
   inferior shells, also does not relinquish setuid priviledges at the start,
   but instead calls the AFS library routines bePlayer(), beGames(),
   and Authenticate() */

/* #define SECURE */





/****************************************************************************
 * System dependent defines follow, you should not need to change anything  *
 * below (if you have a supported system).  If you run into problems during *
 * compilation, you might want to check the defines below.                  *
 ****************************************************************************/







/* fix systems lacking usleep() -CWS 
 *
 * Note that Solaris 2.x users without the BSD compatibilty kit need to
 * define this as well.
 */

#if defined(HPUX) || defined(ultrix)
#define NEEDS_USLEEP
#endif

#ifdef NEEDS_USLEEP
#define usleep microsleep

int microsleep(unsigned long);

#endif





#ifdef MAC
#ifdef RSRC
#define MACRSRC		/* i.e., we're building the resources */
#else
#define MACGAME		/* i.e., we're building the game */
#endif
#endif

#ifdef MAC
/* Screen dimensions */
#define SCRN_ROWS	24
#define SCRN_COLS	80
#endif


#ifdef SECURE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif


