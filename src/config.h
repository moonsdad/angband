/* File: config.h */

/* Purpose: Angband specific configuration stuff */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * Look through the following lines, and where a comment includes the
 * tag "OPTION:", examine the associated "#define" statements, and decide
 * whether you wish to keep, comment, or uncomment them.  You should not
 * have to modify any lines not indicated by "OPTION".
 *
 * Note: Also examine the "system" configuration file "h-config.h"
 * and the variable initialization file "variables.c".  If you change
 * anything in "variables.c", you only need to recompile that file.
 *
 * And finally, remember that the "Makefile" will specify some rather
 * important compile time options, like what visual module to use.
 */



/*
 * OPTION: Person to bother if something goes wrong.
 */
#define WIZARD	"root"


/*
 * OPTION: for the AFS distributed file system, define this to ensure that
 * the program is secure with respect to the setuid code, this prohibits
 * inferior shells, also does not relinquish setuid priviledges at the start,
 * but instead calls the AFS library routines bePlayer(), beGames(),
 * and Authenticate().
 */
/* #define SECURE */





/*
 * OPTION: Allow the player to copy save files.  Turning this off
 * may or may not stop players from doing it, though.  In fact, this
 * option is not even referenced on SET_UID machines.
 */
#define ALLOW_FIDDLING

#define ALLOW_SCORE          /* Allow the user to check his score (v-key)    */

/*
 * OPTION: Allow checking of artifacts (in town)
 */
#define ALLOW_ARTIFACT_CHECK
 
/*
 * OPTION: Allow checking of dead uniques
 */
#define ALLOW_CHECK_UNIQUES

/*
 * OPTION: Allow characteres to be "auto-rolled"
 */
#define AUTOROLLER

/*
 * OPTION: Allow locations and monsters to be "targetted"
 */
#define TARGET



#define SATISFY_HUNGER		 /* Do 'satisfy hunger' rather then 'create food'
								to reduce the tedium for handling food. -CWS */



/*
 * OPTION: Set the "default" path to the angband "lib" directory.
 * Angband will use this value if it cannot getenv("ANGBAND_PATH").
 * The final slash is optional in either case.  Not used on Macintosh.
 * By default, the system expects the "angband" program to be located
 * in the same directory as the "lib" directory.  This can be changed.
 * Note that the "ANGBAND_PATH" environment variable over-rides this.
 * Note: this value is ignored by Macintosh, Windows, and Amiga, see
 * the file "arrays.c" for details.
 */
#define DEFAULT_PATH "./lib/"


/*
 * OPTION: On multiuser systems, be "nice" when autorolling.
 */
#undef NICE

/*
 * OPTION: On multiuser systems, add the "uid" to savefile names
 */
#define SET_UID

/*
 * OPTION: Check the "hours" file
 */
#undef CHECK_HOURS

/*
 * this sets the default user interface
 * to use the original key bindings (keypad for movement) set ROGUE_LIKE
 * to FALSE, to use the rogue-like key bindings (vi style movement)
 * set ROGUE_LIKE to TRUE
 * if you change this, you only need to recompile main.c
 */
#define ROGUE_LIKE TRUE






#ifdef MACINTOSH
/* Screen dimensions */
#define SCRN_ROWS	24
#define SCRN_COLS	80
#endif




/*
 * Prepare to use the "Secure" routines
 */
#ifdef SECURE
  extern int PlayerUID;
# define getuid() PlayerUID
# define geteuid() PlayerUID
#endif


/*
 * fix systems lacking usleep() -CWS 
 *
 * Note that Solaris 2.x users without the BSD compatibilty kit need to
 * define this as well.
 */
#if !defined(HPUX) && !defined(ultrix) && !defined(SOLARIS) && !defined(SGI)
# define HAS_USLEEP
#endif



