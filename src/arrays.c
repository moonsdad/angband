/* File: arrays.c */ 

/* Purpose: initialize various arrays */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/*
 * Hack -- make sure we have a good "ANSI" definition for "CTRL()"
 */
#undef CTRL
#define CTRL(C) ((C)&037)




/*
 * Prepare the ANGBAND_xxx filepath "constants".
 *
 * This is done first in main.c, so I'll rudely drop it into the front
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

cptr ANGBAND_BONES;		/* was LIBDIR(bones)			*/
cptr ANGBAND_SAV;		/* was LIBDIR(save)			*/
cptr ANGBAND_TST;		/* was LIBDIR(test)			*/

cptr ANGBAND_MOR;		/* was LIBDIR(files/news)		*/
cptr ANGBAND_WELCOME;		/* was LIBDIR(files/welcome.hlp)	*/
cptr ANGBAND_VER;		/* was LIBDIR(files/version.hlp)	*/
cptr ANGBAND_TOP;		/* was LIBDIR(files/newscores)		*/

cptr ANGBAND_WIZ;		/* was LIBDIR(files/wizards)		*/
cptr ANGBAND_HOURS;		/* was LIBDIR(files/hours)		*/
cptr ANGBAND_LOAD;		/* was LIBDIR(files/loadcheck)		*/
cptr ANGBAND_LOG;		/* was LIBDIR(files/ANGBAND.log)	*/

cptr ANGBAND_HELP;		/* was LIBDIR(files/roglcmds.hlp)	*/
cptr ANGBAND_ORIG_HELP;	/* was LIBDIR(files/origcmds.hlp)	*/
cptr ANGBAND_WIZ_HELP;		/* was LIBDIR(files/rwizcmds.hlp)	*/
cptr ANGBAND_OWIZ_HELP;	/* was LIBDIR(files/owizcmds.hlp)	*/


/*
 * Find the paths to all of our important files and directories...
 * Use the ANGBAND_PATH environment var if possible, else use DEFAULT_PATH,
 *
 * Note that this function is called BEFORE Term_init().
 *
 * If your system can't do "getenv()", you'll have to kludge this.  [cjh]
 */
void get_file_paths()
{
#ifndef MAXPATHLEN
#define BOZO_MAXPATHLEN
#define MAXPATHLEN 512
#endif

    /* Grab the base "path" */
    char *angband_path = NULL;


    /* Get the environment variable */
    angband_path = getenv("ANGBAND_PATH");
    if (angband_path == NULL) {
	angband_path = (char *)malloc( strlen( DEFAULT_PATH ) + 1 );
	strcpy( angband_path, DEFAULT_PATH );
    }

    ANGBAND_TST = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_TST, angband_path );
    strcat( ANGBAND_TST, PATH_SEP );
    strcat( ANGBAND_TST, "test" );
    ANGBAND_TST = (char *)realloc( ANGBAND_TST, strlen( ANGBAND_TST ) + 1 );

    ANGBAND_HOURS = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_HOURS, angband_path );
    strcat( ANGBAND_HOURS, PATH_SEP );
    strcat( ANGBAND_HOURS, "files" );
    strcat( ANGBAND_HOURS, PATH_SEP );
    strcat( ANGBAND_HOURS, "hours" );
    ANGBAND_HOURS = (char *)realloc( ANGBAND_HOU, strlen( ANGBAND_HOU ) + 1 );

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
#ifdef BOZO_MAXPATHLEN
#undef BOZO_MAXPATHLEN
#undef MAXPATHLEN
#endif /* BOZO_MAXPATHLEN */

#endif /* NEW_FILEPATHS */
}



/*
 * Hack -- let the various systems "breathe"
 */
static void gasp()
{

#if defined(MACINTOSH) || defined(_Windows)

    /* Don't hog the processor */
    Term_xtra(TERM_XTRA_CHECK, -999);

#endif

}



/*
 * Convert a "color letter" into an actual color
 * The colors are: dwsorgbuDWvyRGBU, see below
 * No longer includes MULTI or CLEAR
 */
static int color_char_to_attr(char c)
{
    switch (c) {

	case 'd': return (TERM_BLACK);
	case 'w': return (TERM_WHITE);
	case 's': return (TERM_GRAY);
	case 'o': return (TERM_ORANGE);
	case 'r': return (TERM_RED);
	case 'g': return (TERM_GREEN);
	case 'b': return (TERM_BLUE);
	case 'u': return (TERM_UMBER);

	case 'D': return (TERM_D_GRAY);
	case 'W': return (TERM_L_GRAY);
	case 'v': return (TERM_VIOLET);
	case 'y': return (TERM_YELLOW);
	case 'R': return (TERM_L_RED);
	case 'G': return (TERM_L_GREEN);
	case 'B': return (TERM_L_BLUE);
	case 'U': return (TERM_L_UMBER);
    }

    return (-1);
}



static char original_commands(char command)
{
    int dir_val;
    
    /* Process the command */
    switch (command) {

	/* White space */
	case CTRL('J'):		/* not used */
	case CTRL('M'):		/* not used */
	    command = ' ';
	case ' ':
	    break;

	/* Suicide */
	case CTRL('K'):		/* ^K = exit    */
	    command = 'Q';
	    break;

	/* Locate */
	case 'L':
	    command = 'W';
	    break;

	/* Search mode */
	case 'S':
	    command = '#';
	    break;

	/* Browse */
	case 'b':
	    command = 'P';
	    break;

	/* Help */
	case 'h':
	    command = '?';
	    break;

	/* Spike */
	case 'j':
	    command = 'S';
	    break;

      case CTRL('F'):		/* ^F = repeat feeling */
      case CTRL('R'):		/* ^R = redraw screen  */
      case CTRL('P'):		/* ^P = repeat  */
      case CTRL('W'):		/* ^W = enter wizard mode */
      case CTRL('X'):		/* ^X = save    */
      case '.': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT */
	  int temp = target_mode;
	  target_mode = FALSE;
#endif
	  if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	      case 1:
		command = 'B';
		break;
	      case 2:
		command = 'J';
		break;
	      case 3:
		command = 'N';
		break;
	      case 4:
		command = 'H';
		break;
	      case 6:
		command = 'L';
		break;
	      case 7:
		command = 'Y';
		break;
	      case 8:
		command = 'K';
		break;
	      case 9:
		command = 'U';
		break;
	      default:
		command = ' ';
		break;
	    }
	else
	    command = ' ';
#ifdef TARGET
	  target_mode = temp; /* restore old target code ... -CFT */
#endif
        }
	break;
      case '/':
      case '<':
      case '>':
      case '-':
      case '=':
      case '{':
      case '?':
      case 'A':
	break;
      case '1':
	command = 'b';
	break;
      case '2':
	command = 'j';
	break;
      case '3':
	command = 'n';
	break;
      case '4':
	command = 'h';
	break;
      case '5':			/* Rest one turn */
	command = '.';
	break;
      case '6':
	command = 'l';
	break;
      case '7':
	command = 'y';
	break;
      case '8':
	command = 'k';
	break;
      case '9':
	command = 'u';
	break;
      case 'B':
	command = 'f';
	break;
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'g':
	break;
      case 'M':
      case 'R':
	break;
      case 'T': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	int temp = target_mode;
	target_mode = FALSE;
#endif
	if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	      case 1:
		command = CTRL('B');
		break;
	      case 2:
		command = CTRL('J');
		break;
	      case 3:
		command = CTRL('N');
		break;
	      case 4:
		command = CTRL('H');
		break;
	      case 6:
		command = CTRL('L');
		break;
	      case 7:
		command = CTRL('Y');
		break;
	      case 8:
		command = CTRL('K');
		break;
	      case 9:
		command = CTRL('U');
		break;
	      default:
		command = ' ';
		break;
	    }
	else
	    command = ' ';
#ifdef TARGET
	  target_mode = temp;
#endif
        }
	break;
      case 'a':
	command = 'z';
	break;
      case 'c':
      case 'd':
      case 'e':
	break;
      case 'f':
	command = 't';
	break;
      case 'i':
	break;
      case 'l':
	command = 'x';
	break;
      case 'm':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
	break;
      case 't':
	command = 'T';
	break;
      case 'u':
	command = 'Z';
	break;
      case 'z':
	command = 'a';
	break;
      case 'v':
      case 'V':
      case 'w':
	break;
      case 'x':
	command = 'X';
	break;

    /* wizard mode commands follow */
      case '\\':		/* \ = wizard help */
      case CTRL('A'):		/* ^A = cure all */
      case CTRL('D'):		/* ^D = up/down */
      case CTRL('E'):		/* ^E = wizchar */
      case CTRL('G'):		/* ^G = treasure */
      case CTRL('I'):		/* ^I = identify */
      case CTRL('O'):		/* ^O = generate objects */
      case CTRL('T'):		/* ^T = teleport */
      case CTRL('V'):		/* ^V = treasure */
      case CTRL('Z'):		/* ^Z = genocide */
      case ':':			/* map area */
      case '~':			/* artifact list to file */
      case '!':			/* rerate hitpoints */
      case '@':			/* create object */
      case '$':			/* wiz. light */
      case '%':			/* '%' == self knowledge */
      case '&':			/* & = summon  */
      case '*':			/* Indentify up to level */
      case '+':			/* add experience */
      case '|':			/* check uniques - cba */
	break;
      default:
	command = '(';		/* Anything illegal. */
	break;
    }
    return command;
}

