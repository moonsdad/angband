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
 * First, we'll look for the ANGBAND_PATH environment variable,
 * and then look for the files in there.  If that doesn't work,
 * we'll try the DEFAULT_PATH constant.  So be sure that one of
 * these two things works...
 *
 * The code is now a lot cleaner, with no realloc calls. -BEN-
 * And it is no longer optional, everyone must use it.  If you
 * wish to reinstate "constant paths", please do so by giving
 * the variables below constant "initial values".
 */

cptr ANGBAND_DIR_FILES = NULL;		/* Dir: ascii files  */
cptr ANGBAND_DIR_BONES = NULL;		/* Dir: ascii bones files was LIBDIR(bones) */
cptr ANGBAND_DIR_SAVE = NULL;		/* Dir: binary save files was LIBDIR(save) */
cptr ANGBAND_TST = NULL;		/* was LIBDIR(test) */
cptr ANGBAND_DIR_DATA = NULL;		/* Dir: system dependant files */

cptr ANGBAND_NEWS = NULL;		/* News file was LIBDIR(files/news) */
cptr ANGBAND_WELCOME = NULL;		/* Player generation help  was LIBDIR(files/welcome.hlp) */
cptr ANGBAND_VERSION = NULL;		/* Version information was LIBDIR(files/version.hlp) */
cptr ANGBAND_TOP = NULL;		/* was LIBDIR(files/newscores) */

cptr ANGBAND_WIZ = NULL;		/* Acceptable wizard uid's was LIBDIR(files/wizards) */
cptr ANGBAND_HOURS = NULL;		/* Hours of operation was LIBDIR(files/hours) */
cptr ANGBAND_LOAD = NULL;		/* Load information was LIBDIR(files/loadcheck) */
cptr ANGBAND_LOG = NULL;		/* Log file of some form was LIBDIR(files/ANGBAND.log) */

cptr ANGBAND_R_HELP = NULL;		/* Roguelike command help was LIBDIR(files/roglcmds.hlp) */
cptr ANGBAND_O_HELP = NULL;		/* Original command help was LIBDIR(files/origcmds.hlp) */
cptr ANGBAND_W_HELP = NULL;		/* Wizard command help *was LIBDIR(files/rwizcmds.hlp) */
cptr ANGBAND_OWIZ_HELP = NULL;		/* was LIBDIR(files/owizcmds.hlp) */



/*
 * Find the paths to all of our important files and directories...
 * Use the ANGBAND_PATH environment var if possible, else use DEFAULT_PATH,
 * and then branch off appropriately from there (see below).
 *
 * Note that this function is called BEFORE Term_init().
 *
 * If your system can't do "getenv()", you'll have to kludge this.  [cjh]
 * For example, on "MACINTOSH", we set a global "folder pointer" based on
 * where the "lib" folder was last time, and if we cannot use that one,
 * we ask the user to interactively "reset" it.  See "main-mac.c".
 */
void get_file_paths()
{
    /* The current path (and "workspace") */
    char path[1024];

    /* Pointer to the "end" of the workspace */
    char *tail;

    /* Grab the base "path" */
    char *angband_path = NULL;


#ifdef MACINTOSH

    /* Hack -- The Macintosh uses a "system" path-prefix */
    strcpy(path, PATH_SEP);

#else

# ifdef _Windows

    /* Hack -- The Windows system also uses a special prefix */
    strcpy(path, get_lib_path());

# else

#  ifdef AMIGA

    /* The Amiga uses a special method */
    strcpy(path, "Angband:");

#  else

    /* Get the environment variable */
    angband_path = getenv("ANGBAND_PATH");

    /* Use the angband_path, or a default */
    strcpy(path, angband_path ? angband_path : DEFAULT_PATH);

    /* Be sure not to duplicate any "Path separator" */
    if (!suffix(path,PATH_SEP)) strcat(path, PATH_SEP);

#  endif

# endif

#endif


    /* Prepare to append to the Base Path */
    tail = path + strlen(path);

    /* Find some directory names */
    strcpy(tail, "data");
    ANGBAND_DIR_DATA = string_make(path);
    strcpy(tail, "save");
    ANGBAND_DIR_SAVE = string_make(path);
    strcpy(tail, "bones");
    ANGBAND_DIR_BONES = string_make(path);
    strcpy(tail, "files");
    ANGBAND_DIR_FILES = string_make(path);

    /* Add a path separator */
    strcat(tail, PATH_SEP);

    /* Use the "files" directory (from above) */
    tail = tail + strlen(tail);

    /* The basic info files */
    strcpy(tail, "news.hlp");
    ANGBAND_NEWS = string_make(path);
    strcpy(tail, "welcome.hlp");
    ANGBAND_WELCOME = string_make(path);
    strcpy(tail, "version.hlp");
    ANGBAND_VERSION = string_make(path);
    strcpy(tail, "newscores" );
    ANGBAND_TOP = string_make(path);

    /* The command help files */
    strcpy(tail, "cmds_r.hlp");
    ANGBAND_R_HELP = string_make(path);
    strcpy(tail, "cmds_o.hlp");
    ANGBAND_O_HELP = string_make(path);
    strcpy(tail, "cmds_w.hlp");
    ANGBAND_W_HELP = string_make(path);
    strcpy(tail,"owizcmds.hlp";
    ANGBAND_OWIZ_HELP = string_make(path);

    /* Some parsable text files */
    strcpy(tail, "wizards.txt");
    ANGBAND_WIZ = string_make(path);
    strcpy(tail, "hours.txt");
    ANGBAND_HOURS = string_make(path);
    strcpy(tail, "loadcheck.txt");
    ANGBAND_LOAD = string_make(path);
    strcpy(tail, "ANGBAND.log" );
    ANGBAND_LOG =  string_make(path);
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
 * Hack -- return the "buffer length" of a string
 * This is "zero" for NULL and "strlen(str)+1" otherwise.
 */
static uint string_size(cptr str)
{
    if (!str) return (0);
    return (strlen(str) + 1);
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
	case ' ':
	    return (' ');

	/* Suicide */
	case CTRL('K'):		/* ^K = exit    */
	    return ('Q');

	/* Locate */
	case 'L':
	    return ('W');

	/* Search mode */
	case 'S':
	    return ('#');

	/* Browse */
	case 'b':
	    return ('P');

	/* Help */
	case 'h':
	    return ('?');

	/* Spike */
	case 'j':
	    return ('S');

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

