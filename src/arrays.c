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
cptr ANGBAND_W_HELP = NULL;		/* Wizard command help was LIBDIR(files/rwizcmds.hlp) */
cptr ANGBAND_OWIZ_HELP = NULL;		/* was LIBDIR(files/owizcmds.hlp) */

cptr ANGBAND_R_LIST = NULL;		/* Ascii monster race file */
cptr ANGBAND_K_LIST = NULL;		/* Ascii item kind file */


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
    strcpy(tail, "load.txt");
    ANGBAND_LOAD = string_make(path);
    strcpy(tail, "ANGBAND.log" );
    ANGBAND_LOG =  string_make(path);

    /* Parsable Item/Monster template files */
    strcpy(tail, "r_list.txt");
    ANGBAND_R_LIST = string_make(path);
    strcpy(tail, "k_list.txt");
    ANGBAND_K_LIST = string_make(path);
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
 * Grab one flag in a monster_race from a textual string
 *
 * The following substitutions were used to extract the
 * function below from the "dump" function (elsewhere).
 *
 *   s:^.*(\(flags.\) \& \(.*\)).* " \| \(.*\)");.*$:\1 \2 \3:g
 *   s:\(.*\) \(.*\) \(.*\):    else if (streq(what,"\3")) \1 |= \2;:g
 */
static int grab_one_flag(monster_race *r_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L;

    if (!what) what = what;

    else if (streq(what,"WINNER"))	flags1 |= MF1_WINNER;
    else if (streq(what,"QUESTOR"))	flags2 |= MF2_QUESTOR;
    else if (streq(what,"UNIQUE"))	flags2 |= MF2_UNIQUE;
    else if (streq(what,"MAX_HP"))	flags2 |= MF2_MAX_HP;
    else if (streq(what,"INTELLIGENT"))	flags2 |= MF2_INTELLIGENT;

    else if (streq(what,"SPECIAL"))	flags2 |= MF2_SPECIAL;
    else if (streq(what,"GOOD"))	flags2 |= MF2_GOOD;
    else if (streq(what,"CARRY_GOLD"))	flags1 |= MF1_CARRY_GOLD;
    else if (streq(what,"CARRY_OBJ"))	flags1 |= MF1_CARRY_OBJ;
    else if (streq(what,"PICK_UP"))	flags1 |= MF1_PICK_UP;

    else if (streq(what,"MV_ONLY_ATT"))	flags1 |= MF1_MV_ONLY_ATT;
    else if (streq(what,"MV_ATT_NORM"))	flags1 |= MF1_MV_ATT_NORM;
    else if (streq(what,"MV_20"))	flags1 |= MF1_MV_20;
    else if (streq(what,"MV_40"))	flags1 |= MF1_MV_40;
    else if (streq(what,"MV_75"))	flags1 |= MF1_MV_75;
    else if (streq(what,"MV_INVIS"))	flags1 |= MF1_MV_INVIS;
    else if (streq(what,"NO_INFRA"))	flags2 |= MF2_NO_INFRA;
    else if (streq(what,"MINDLESS"))	flags2 |= MF2_MINDLESS;
    else if (streq(what,"THRO_DR"))	flags1 |= MF1_THRO_DR;
    else if (streq(what,"THRO_WALL"))	flags1 |= MF1_THRO_WALL;
    else if (streq(what,"THRO_CREAT"))	flags1 |= MF1_THRO_CREAT;
    else if (streq(what,"MULTIPLY"))	flags1 |= MF1_MULTIPLY;
    else if (streq(what,"GROUP"))	flags2 |= MF2_GROUP;
    else if (streq(what,"HAS_60"))	flags1 |= MF1_HAS_60;
    else if (streq(what,"HAS_90"))	flags1 |= MF1_HAS_90;
    else if (streq(what,"HAS_1D2"))	flags1 |= MF1_HAS_1D2;
    else if (streq(what,"HAS_2D2"))	flags1 |= MF1_HAS_2D2;
    else if (streq(what,"HAS_4D2"))	flags1 |= MF1_HAS_4D2;

    else if (streq(what,"ANIMAL"))	flags2 |= MF2_ANIMAL;
    else if (streq(what,"EVIL"))	flags2 |= MF2_EVIL;
    else if (streq(what,"ORC"))	flags2 |= MF2_ORC;
    else if (streq(what,"TROLL"))	flags2 |= MF2_TROLL;
    else if (streq(what,"GIANT"))	flags2 |= MF2_GIANT;
    else if (streq(what,"DRAGON"))	flags2 |= MF2_DRAGON;
    else if (streq(what,"DEMON"))	flags2 |= MF2_DEMON;
    else if (streq(what,"UNDEAD"))	flags2 |= MF2_UNDEAD;

    else if (streq(what,"IM_ACID"))	flags2 |= MF2_IM_ACID;
    else if (streq(what,"IM_FIRE"))	flags2 |= MF2_IM_FIRE;
    else if (streq(what,"IM_COLD"))	flags2 |= MF2_IM_COLD;
    else if (streq(what,"IM_ELEC"))	flags2 |= MF2_IM_ELEC;
    else if (streq(what,"IM_POIS"))	flags2 |= MF2_IM_POIS;

    else if (streq(what,"HURT_LITE"))	flags2 |= MF2_HURT_LITE;
    else if (streq(what,"HURT_ROCK"))	flags2 |= MF2_HURT_ROCK;
    else if (streq(what,"CHARM_SLEEP"))	flags2 |= MF2_CHARM_SLEEP;
    else if (streq(what,"BREAK_WALL"))	flags2 |= MF2_BREAK_WALL;
    else if (streq(what,"DESTRUCT"))	flags2 |= MF2_DESTRUCT;

    else if (streq(what,"MALE"))	flags1 |= MF1_MALE;
    else if (streq(what,"FEMALE"))	flags1 |= MF1_FEMALE;
    else if (streq(what,"PLURAL"))	flags1 |= MF1_PLURAL;

    else if (streq(what,"CHAR_CLEAR"))	flags1 |= MF1_CHAR_CLEAR;
    else if (streq(what,"CHAR_MULTI"))	flags1 |= MF1_CHAR_MULTI;
    else if (streq(what,"ATTR_CLEAR"))	flags1 |= MF1_ATTR_CLEAR;
    else if (streq(what,"ATTR_MULTI"))	flags1 |= MF1_ATTR_MULTI;

    if (!flags1 && !flags2) return (0);

    if (flags1) r_ptr->cflags1 |= flags1;
    if (flags2) r_ptr->cflags2 |= flags2;

    return (1);
}


/*
 * Grab one spell in a monster_race from a textual string
 */
static int grab_one_spell(monster_race *r_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L, flags3 = 0L;

    int chance;

    if (!what) what = what;

    /* Hack -- store the "frequency" in the spell flags */
    else if (1 == sscanf(what, "1_IN_%d", &chance)) {

	/* Hack -- frequency stored as "flags" */
	flags1 |= ((u32b)chance & CS1_FREQ);
    }

    else if (streq(what,"HEAL"))		flags2 |= MS2_HEAL;
    else if (streq(what,"HASTE"))		flags2 |= MS2_HASTE;
    else if (streq(what,"BLINK"))		flags1 |= MS1_BLINK;
    else if (streq(what,"TELEPORT"))		flags1 |= MS1_TELEPORT;

    else if (streq(what,"TELE_TO"))		flags1 |= MS1_TELE_TO;
    else if (streq(what,"TELE_AWAY"))		flags2 |= MS2_TELE_AWAY;
    else if (streq(what,"TELE_LEVEL"))	flags2 |= MS2_TELE_LEVEL;
    else if (streq(what,"BLIND"))		flags1 |= MS1_BLIND;
    else if (streq(what,"HOLD"))		flags1 |= MS1_HOLD;
    else if (streq(what,"SLOW"))		flags1 |= MS1_SLOW;
    else if (streq(what,"CONF"))		flags1 |= MS1_CONF;
    else if (streq(what,"FEAR"))		flags1 |= MS1_FEAR;

    else if (streq(what,"CAUSE_1"))		flags1 |= MS1_CAUSE_1;
    else if (streq(what,"CAUSE_2"))		flags1 |= MS1_CAUSE_2;
    else if (streq(what,"CAUSE_3"))		flags1 |= MS1_CAUSE_3;
    else if (streq(what,"ARROW_1"))		flags1 |= MS1_ARROW_1;
    else if (streq(what,"ARROW_2"))		flags2 |= MS2_ARROW_2;
    else if (streq(what,"ARROW_3"))		flags3 |= MS3_ARROW_3;
    else if (streq(what,"RAZOR"))		flags2 |= MS2_RAZOR;

    else if (streq(what,"MANA_DRAIN"))	flags1 |= MS1_MANA_DRAIN;
    else if (streq(what,"MIND_BLAST"))	flags2 |= MS2_MIND_BLAST;
    else if (streq(what,"BRAIN_SMASH"))	flags2 |= MS2_BRAIN_SMASH;
    else if (streq(what,"FORGET"))		flags2 |= MS2_FORGET;
    else if (streq(what,"TRAP_CREATE"))	flags2 |= MS2_TRAP_CREATE;
    else if (streq(what,"DARKNESS"))		flags2 |= MS2_DARKNESS;
    else if (streq(what,"DARK_STORM"))	flags3 |= MS3_DARK_STORM;
    else if (streq(what,"MANA_STORM"))	flags3 |= MS3_MANA_STORM;

    else if (streq(what,"BO_ACID"))		flags1 |= MS1_BO_ACID;
    else if (streq(what,"BO_FIRE"))		flags1 |= MS1_BO_FIRE;
    else if (streq(what,"BO_COLD"))		flags1 |= MS1_BO_COLD;
    else if (streq(what,"BO_ELEC"))		flags2 |= MS2_BO_ELEC;
    else if (streq(what,"BO_ICEE"))		flags2 |= MS2_BO_ICEE;
    else if (streq(what,"BO_WATE"))		flags2 |= MS2_BO_WATE;
    else if (streq(what,"BO_MANA"))		flags1 |= MS1_BO_MANA;
    else if (streq(what,"BO_PLAS"))		flags2 |= MS2_BO_PLAS;
    else if (streq(what,"BO_NETH"))		flags2 |= MS2_BO_NETH;

    else if (streq(what,"BA_ACID"))		flags2 |= MS2_BA_ACID;
    else if (streq(what,"BA_FIRE"))		flags1 |= MS1_BA_FIRE;
    else if (streq(what,"BA_COLD"))		flags1 |= MS1_BA_COLD;
    else if (streq(what,"BA_ELEC"))		flags2 |= MS2_BA_ELEC;
    else if (streq(what,"BA_WATE"))		flags2 |= MS2_BA_WATE;
    else if (streq(what,"BA_POIS"))		flags2 |= MS2_BA_POIS;
    else if (streq(what,"BA_NETH"))		flags2 |= MS2_BA_NETH;

    else if (streq(what,"BR_ACID"))		flags1 |= MS1_BR_ACID;
    else if (streq(what,"BR_FIRE"))		flags1 |= MS1_BR_FIRE;
    else if (streq(what,"BR_COLD"))		flags1 |= MS1_BR_COLD;
    else if (streq(what,"BR_ELEC"))		flags1 |= MS1_BR_ELEC;
    else if (streq(what,"BR_POIS"))		flags1 |= MS1_BR_POIS;
    else if (streq(what,"BR_LITE"))		flags3 |= MS3_BR_LITE;
    else if (streq(what,"BR_DARK"))		flags3 |= MS3_BR_DARK;
    else if (streq(what,"BR_SOUN"))		flags2 |= MS2_BR_SOUN;
    else if (streq(what,"BR_CONF"))		flags2 |= MS2_BR_CONF;
    else if (streq(what,"BR_CHAO"))		flags2 |= MS2_BR_CHAO;
    else if (streq(what,"BR_SHAR"))		flags2 |= MS2_BR_SHAR;
    else if (streq(what,"BR_LIFE"))		flags2 |= MS2_BR_LIFE;
    else if (streq(what,"BR_DISE"))		flags2 |= MS2_BR_DISE;
    else if (streq(what,"BR_WALL"))		flags3 |= MS3_BR_WALL;
    else if (streq(what,"BR_SLOW"))		flags3 |= MS3_BR_SLOW;
    else if (streq(what,"BR_TIME"))		flags3 |= MS3_BR_TIME;
    else if (streq(what,"BR_GRAV"))		flags3 |= MS3_BR_GRAV;
    else if (streq(what,"BR_PLAS"))		flags3 |= MS3_BR_PLAS;
    else if (streq(what,"BR_NETH"))		flags2 |= MS2_BR_NETH;

    else if (streq(what,"S_MONSTER"))		flags1 |= MS1_S_MONSTER;
    else if (streq(what,"S_SUMMON"))		flags2 |= MS2_S_SUMMON;
    else if (streq(what,"S_UNDEAD"))		flags1 |= MS1_S_UNDEAD;
    else if (streq(what,"S_DEMON"))		flags1 |= MS1_S_DEMON;
    else if (streq(what,"S_DRAGON"))		flags1 |= MS1_S_DRAGON;
    else if (streq(what,"S_ANGEL"))		flags2 |= MS2_S_ANGEL;
    else if (streq(what,"S_REPTILE"))		flags3 |= MS3_S_REPTILE;
    else if (streq(what,"S_SPIDER"))		flags2 |= MS2_S_SPIDER;
    else if (streq(what,"S_ANT"))		flags3 |= MS3_S_ANT;
    else if (streq(what,"S_HOUND"))		flags2 |= MS2_S_HOUND;
    else if (streq(what,"S_UNIQUE"))		flags3 |= MS3_S_UNIQUE;
    else if (streq(what,"S_WRAITH"))		flags3 |= MS3_S_WRAITH;
    else if (streq(what,"S_GUNDEAD"))		flags3 |= MS3_S_GUNDEAD;
    else if (streq(what,"S_ANCIENTD"))	flags3 |= MS3_S_ANCIENTD;

    if (!flags1 && !flags2 && !flags3) return (0);

    if (flags1) r_ptr->spells1 |= flags1;
    if (flags2) r_ptr->spells2 |= flags2;
    if (flags3) r_ptr->spells3 |= flags3;

    return (1);
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



/*
 * Hack -- indicate errors during parsing of "r_list.txt"
 */
static int error_r_idx = -1;

/*
 * Initialize the "r_list" array by parsing an ascii file
 */
static errr init_r_list_txt(void)
{
    register char *s, *t;

    /* No monster yet */
    int m = -1;

    /* No r_ptr yet */
    monster_race *r_ptr = NULL;

    /* The "monsters" file */
    FILE *fp;

    /* No line should be more than 80 chars */
    char buf[160];

    /* Current race description */
    char desc[24*80];

    /* Open the monster file */
    fp = fopen(ANGBAND_R_LIST, "r");

    /* Failure */
    if (!fp) return (-1);

    
    /* Load the monster descriptions from the file */
    while (1) {

	/* Read a line from the file, stop when done */
	if (!fgets(buf, 160, fp)) break;

	/* Skip comments */
	if (buf[0] == '#') continue;

	/* Strip the final newline */
	for (s = buf; isprint(*s); ++s); *s = '\0';

	/* Blank lines terminate monsters */
	if (!buf[0]) {

	    /* No current r_ptr */
	    if (!r_ptr) continue;

	    /* Save the decription */
	    if (desc[0]) r_ptr->desc = string_make(desc);

	    /* Now there is no current r_ptr */
	    r_ptr = NULL;

	    /* Don't hog the processor */
	    gasp();

	    /* Next... */
	    continue;
	}

	/* The line better have a colon and such */
	if (buf[1] != ':') return (1);

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N') {

	    /* Not done the previous one */
	    if (r_ptr) return (11);

	    /* Find, verify, and nuke the colon before the name */
	    if (!(s = strchr(buf+2, ':'))) return (2);

	    /* Nuke the colon, advance to the name */
	    *s++ = '\0';

	    /* Require non-empty names */
	    if (!*s) return (3);

	    /* Get the index */
	    m = atoi(buf+2);

	    /* Save the index */
	    error_r_idx = m;

	    /* Verify */
	    if ((m < 0) || (m >= MAX_R_IDX)) return (7);

	    /* Start a new r_ptr */
	    r_ptr = &r_list[m];

	    /* Make sure we have not done him yet */
	    if (r_ptr->name) return (8);

	    /* Save the name */
	    r_ptr->name = string_make(s);

	    /* No desc yet */
	    desc[0] = '\0';

	    /* Next... */
	    continue;
	}

	/* There better be a current r_ptr */
	if (!r_ptr) return (10);

	/* Process 'I' for "Info" (one line only) */
	if (buf[0] == 'I') {

	    char chr, att;
	    int tmp, spd, hp1, hp2, aaf, ac, slp, rar, lev;
	    long exp;

	    /* Scan for the values */
	    if (11 != sscanf(buf, "I:%c:%c:%d:%dd%d:%d:%d:%d:%d:%d:%ld",
		&chr, &att, &spd, &hp1, &hp2,
		&aaf, &ac, &slp, &rar, &lev, &exp)) return (11);

	    /* Extract the color */
	    tmp = color_char_to_attr(att);
	    if (tmp < 0) return (12);
	    r_ptr->r_attr = tmp;

	    /* Save the values (XXX note that "speed" is encoded) */
	    r_ptr->r_char = chr;
	    r_ptr->speed = 10 * spd + 100;
	    r_ptr->hd[0] = hp1;
	    r_ptr->hd[1] = hp2;
	    r_ptr->aaf = aaf;
	    r_ptr->ac = ac;
	    r_ptr->sleep = slp;
	    r_ptr->rarity = rar;
	    r_ptr->level = lev;
	    r_ptr->mexp = exp;

	    /* Next... */
	    continue;
	}

	/* Process 'A' for "Attacks" (one line only) */
	if (buf[0] == 'A') {

	    int i;

	    /* Simply read each number following a colon */
	    for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i) {

		/* Store the attack damage index */
		r_ptr->damage[i] = atoi(s+1);

		/* Find the next colon */
		s = strchr(s+1, ':');
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'F' for "Flags" (multiple lines) */
	if (buf[0] == 'F') {

	    /* Parse every entry */
	    for (s = buf + 2; *s; ) {

		/* Find the end of this entry */
		for (t = s; *t && *t != ' ' && *t != '|'; ++t);

		/* Nuke and skip any dividers */
		if (*t) {
		    *t++ = '\0';
		    while (*t == ' ' || *t == '|') t++;
		}

		/* Parse this entry */
		if (!grab_one_flag(r_ptr, s)) return (18);

		/* Start the next entry */
		s = t;
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'S' for "Spells" (multiple lines) */
	if (buf[0] == 'S') {

	    /* Parse every entry */
	    for (s = buf + 2; *s; ) {

		/* Find the end of this entry */
		for (t = s; *t && *t != ' ' && *t != '|'; ++t);

		/* Nuke and skip any dividers */
		if (*t) {
		    *t++ = '\0';
		    while (*t == ' ' || *t == '|') t++;
		}

		/* Parse this entry */
		if (!grab_one_spell(r_ptr, s)) return (19);

		/* Start the next entry */
		s = t;
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'D' for "Description" */
	if (buf[0] == 'D') {

	    /* Collect the description, allocated later */
	    strcat(desc,buf+2);

	    /* Next... */
	    continue;
	}
    }

    /* Close the file */
    fclose(fp);


    /* Success */
    return (0);
}




#ifdef BINARY_ARRAY_IMAGES

/*
 * Attempt to "dump" a "quick-load binary image" for "r_list"
 *
 * But we cannot actually "read()" the whole chunk at once because
 * the piece of shit Macintosh "read()" function expects to read()
 * only 64K at a time.  Brain damaged personal computers piss me off.
 * We might even be able to squeeze by if we forbid expansion of the
 * records, but that is too heavy a price to pay...
 *
 * Thus: three separate files:
 *   r_list.raw = (Intro) + (N records) 
 *   r_name.raw = (N name sizes) + (N names)
 *   r_desc.raw = (N desc sizes) + (N descs)
 *
 * The "r_name" and "r_desc" files start with an array of "lengths"
 * (including terminators), where "zero" means "no string" (NULL).
 * Then the files contain each non-NULL string in order.
 */
static errr dump_r_list_raw()
{
    int i, fd;

    int mode = 0666;

    char tmp[1024];


    /* Hack -- sizes */
    uint len[MAX_R_IDX-1];
    
    /* Size of the "lengths" array */
    uint slen = sizeof(len);

    /* Size of one normal record */
    uint size = sizeof(monster_race);


#ifdef SET_UID
    mode = 0644;
#endif

#ifdef MACINTOSH
    _ftype = 'DATA';
#endif


    /* File "r_list.raw" -- raw records */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_list.raw");
    fd = my_topen(tmp, O_RDWR | O_CREAT | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Dump the version info */
    tmp[0] = CUR_VERSION_MAJ;
    tmp[1] = CUR_VERSION_MIN; 
    tmp[2] = CUR_PATCH_LEVEL;
    tmp[3] = size;

    /* Dump it */
    write(fd, tmp, 4);

    /* Attempt to dump the (normal) monster races */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	if (size != write(fd, (char*)(&r_list[i]), size)) break;
    }

    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);

    
    /* File "r_name.raw" -- race names */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_name.raw");
    fd = my_topen(tmp, O_RDWR | O_CREAT | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Prepare the "lengths" array */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	len[i] = string_size(r_list[i].name);
    }
    
    /* Dump the array */
    write(fd, (char*)(len), slen);

    /* Dump the names */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	char *str = (char*)(r_list[i].name);
	if (len[i] && (len[i] != write(fd, str, len[i]))) break;
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);

    
    /* File "r_desc.raw" -- race names */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_desc.raw");
    fd = my_topen(tmp, O_RDWR | O_CREAT | O_BINARY, mode);
    if (fd < 0) return (-1);
    
    /* Prepare the "lengths" array */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	len[i] = string_size(r_list[i].desc);
    }
    
    /* Dump the array */
    write(fd, (char*)(len), slen);

    /* Dump the names */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	char *str = (char*)(r_list[i].desc);
	if (len[i] && (len[i] != write(fd, str, len[i]))) break;
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);


    /* Success */
    return (0);
}



/*
 * Attempt to "quick-load" a binary image for "r_list"
 */
static errr init_r_list_raw()
{
    int i, fd;

    int mode = 0666;

    char tmp[1024];


    /* Hack -- sizes */
    uint len[MAX_R_IDX-1];
    
    /* Size of the "lengths" array */
    uint slen = sizeof(len);

    /* Size of one normal record */
    uint size = sizeof(monster_race);


#ifdef SET_UID
    mode = 0644;
#endif

#ifdef MACINTOSH
    _ftype = 'DATA';
#endif


    /* File "r_list.raw" -- raw records */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_list.raw");
    fd = my_topen(tmp, O_RDONLY | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Read and Verify the version info */    
    if ((4 != read(fd, tmp, 4)) ||
	(tmp[0] != CUR_VERSION_MAJ) ||
	(tmp[1] != CUR_VERSION_MIN) ||
	(tmp[2] != CUR_PATCH_LEVEL) ||
	(tmp[3] != size)) {

	/* Hack -- message */
	msg_print("Ignoring old 'm_list.raw'...");
	msg_print(NULL);
	
	/* Close */
	close(fd);

	/* Fail */
	return (-1);
    }

    /* Attempt to read the (normal) monster races */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	if (size != read(fd, (char*)(&r_list[i]), size)) break;
	r_list[i].name = NULL;
	r_list[i].desc = NULL;
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);


    /* File "r_name.raw" -- race names */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_name.raw");
    fd = my_topen(tmp, O_RDONLY | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Read the size array */
    if (slen != read(fd, (char*)(len), slen)) return (1);

    /* Attempt to read the (normal) monster race names */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	if (len[i] != read(fd, tmp, len[i])) break;
	if (len[i]) r_list[i].name = string_make(tmp);
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);


    /* File "r_desc.raw" -- descriptions */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "r_desc.raw");
    fd = my_topen(tmp, O_RDONLY | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Read the size array */
    if (slen != read(fd, (char*)(len), slen)) return (1);

    /* Attempt to read the (normal) monster race descs */
    for (i = 0; i < MAX_R_IDX-1; i++) {
	if (len[i] != read(fd, tmp, len[i])) break;
	if (len[i]) r_list[i].desc = string_make(tmp);
    }

    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_R_IDX-1) return (1);


    /* Success */
    return (0);
}

#endif


/*
 * Initialize the "r_list" array by parsing various files.
 */
static void init_r_list()
{
    errr err;


    /* XXX Hack -- prepare "ghost" race */
    monster_race *r_ptr = &r_list[MAX_R_IDX-1];


    /* Hack -- Give the ghost monster a "fake" name */
    r_ptr->name = ghost_name;

    /* Hack -- set the char/attr info */
    r_ptr->r_attr = TERM_WHITE;
    r_ptr->r_char = 'G';
    
    /* Hack -- Prepare a fake ghost name */
    strcpy(ghost_name, "Someone's Ghost");

    /* Hack -- Try to prevent a few "potential" bugs */
    r_ptr->cflags2 |= MF2_UNIQUE;


#ifdef BINARY_ARRAY_IMAGES

    /* Try to load a "binary image" */
    err = init_r_list_raw();

    /* Otherwise try the text version */
    if (!err) return;
    
#endif

    /* Try the text version */
    err = init_r_list_txt();

    /* Still no luck? Fail! */
    if (err) {
    
	/* Warning */
	msg_print(format("Fatal error #%d parsing 'r_list.txt', record %d",
			 err, error_r_idx));
	msg_print(NULL);

	/* Quit */
	quit("cannot load 'r_list.txt'");
    }

#ifdef BINARY_ARRAY_IMAGES

    /* Attempt to dump a "quick-load" version */
    err = dump_r_list_raw();

    /* Warn on errors */
    if (err) {
	msg_print("Warning: unable to create binary monster race images!");
	msg_print(NULL);
    }

#endif

}






/*
 * Grab one flag in a inven_kind from a textual string
 */
static bool grab_one_kind_flag(inven_kind *k_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L, flags3 = 0L;

    if (!what) what = what;

    else if (streq(what, "STR"))		flags1 |= TR1_STR;
    else if (streq(what, "INT"))		flags1 |= TR1_INT;
    else if (streq(what, "WIS"))		flags1 |= TR1_WIS;
    else if (streq(what, "DEX"))		flags1 |= TR1_DEX;
    else if (streq(what, "CON"))		flags1 |= TR1_CON;
    else if (streq(what, "CHR"))		flags1 |= TR1_CHR;

    else if (streq(what, "SEARCH"))		flags1 |= TR1_SEARCH;
    else if (streq(what, "SPEED"))		flags1 |= TR1_SPEED;
    else if (streq(what, "STEALTH"))		flags1 |= TR1_STEALTH;
    else if (streq(what, "TUNNEL"))		flags1 |= TR1_TUNNEL;
    else if (streq(what, "INFRA"))		flags1 |= TR1_INFRA;
    else if (streq(what, "ATTACK_SPD"))		flags1 |= TR1_ATTACK_SPD;

    else if (streq(what, "KILL_DRAGON"))	flags1 |= TR1_KILL_DRAGON;
    else if (streq(what, "SLAY_DRAGON"))	flags1 |= TR1_SLAY_DRAGON;
    else if (streq(what, "SLAY_ANIMAL"))	flags1 |= TR1_SLAY_ANIMAL;
    else if (streq(what, "SLAY_EVIL"))		flags1 |= TR1_SLAY_EVIL;

    else if (streq(what, "IMPACT"))		flags1 |= TR1_IMPACT;
    else if (streq(what, "BRAND_COLD"))		flags1 |= TR1_BRAND_COLD;
    else if (streq(what, "BRAND_FIRE"))		flags1 |= TR1_BRAND_FIRE;
    else if (streq(what, "BRAND_ELEC"))		flags1 |= TR1_BRAND_ELEC;

    else if (streq(what, "SLAY_UNDEAD"))	flags1 |= TR1_SLAY_UNDEAD;
    else if (streq(what, "SLAY_DEMON"))		flags1 |= TR1_SLAY_DEMON;

    else if (streq(what, "SLAY_TROLL"))		flags1 |= TR1_SLAY_TROLL;
    else if (streq(what, "SLAY_GIANT"))		flags1 |= TR1_SLAY_GIANT;
    else if (streq(what, "SLAY_ORC"))		flags1 |= TR1_SLAY_ORC;

    else if (streq(what, "FREE_ACT"))		flags2 |= TR2_FREE_ACT;
    else if (streq(what, "HOLD_LIFE"))		flags2 |= TR2_HOLD_LIFE;


    else if (streq(what, "IM_FIRE"))		flags2 |= TR2_IM_FIRE;
    else if (streq(what, "IM_COLD"))		flags2 |= TR2_IM_COLD;
    else if (streq(what, "IM_ACID"))		flags2 |= TR2_IM_ACID;
    else if (streq(what, "IM_ELEC"))		flags2 |= TR2_IM_ELEC;
    else if (streq(what, "IM_POIS"))		flags2 |= TR2_IM_POIS;


    else if (streq(what, "RES_ACID"))		flags2 |= TR2_RES_ACID;
    else if (streq(what, "RES_ELEC"))		flags2 |= TR2_RES_ELEC;
    else if (streq(what, "RES_FIRE"))		flags2 |= TR2_RES_FIRE;
    else if (streq(what, "RES_COLD"))		flags2 |= TR2_RES_COLD;
    else if (streq(what, "RES_POIS"))		flags2 |= TR2_RES_POIS;
    else if (streq(what, "RES_LITE"))		flags2 |= TR2_RES_LITE;
    else if (streq(what, "RES_DARK"))		flags2 |= TR2_RES_DARK;

    else if (streq(what, "RES_BLIND"))		flags2 |= TR2_RES_BLIND;
    else if (streq(what, "RES_CONF"))		flags2 |= TR2_RES_CONF;
    else if (streq(what, "RES_SOUND"))		flags2 |= TR2_RES_SOUND;
    else if (streq(what, "RES_SHARDS"))		flags2 |= TR2_RES_SHARDS;

    else if (streq(what, "RES_NETHER"))		flags2 |= TR2_RES_NETHER;
    else if (streq(what, "RES_NEXUS"))		flags2 |= TR2_RES_NEXUS;
    else if (streq(what, "RES_CHAOS"))		flags2 |= TR2_RES_CHAOS;
    else if (streq(what, "RES_DISEN"))		flags2 |= TR2_RES_DISEN;

    else if (streq(what, "SUST_STR"))		flags2 |= TR2_SUST_STR;
    else if (streq(what, "SUST_INT"))		flags2 |= TR2_SUST_INT;
    else if (streq(what, "SUST_WIS"))		flags2 |= TR2_SUST_WIS;
    else if (streq(what, "SUST_DEX"))		flags2 |= TR2_SUST_DEX;
    else if (streq(what, "SUST_CON"))		flags2 |= TR2_SUST_CON;
    else if (streq(what, "SUST_CHR"))		flags2 |= TR2_SUST_CHR;

    else if (streq(what, "EASY_KNOW"))		flags3 |= TR3_EASY_KNOW;
    else if (streq(what, "HIDE_TYPE"))		flags3 |= TR3_HIDE_TYPE;
    else if (streq(what, "SHOW_MODS"))		flags3 |= TR3_SHOW_MODS;
    else if (streq(what, "INSTA_ART"))		flags3 |= TR3_INSTA_ART;

    else if (streq(what, "FEATHER"))		flags3 |= TR3_FEATHER;
    else if (streq(what, "LITE"))		flags3 |= TR3_LITE;
    else if (streq(what, "SEE_INVIS"))		flags3 |= TR3_SEE_INVIS;
    else if (streq(what, "TELEPATHY"))		flags3 |= TR3_TELEPATHY;

    else if (streq(what, "SLOW_DIGEST"))	flags3 |= TR3_SLOW_DIGEST;
    else if (streq(what, "REGEN"))		flags3 |= TR3_REGEN;

    else if (streq(what, "XTRA_MIGHT"))		flags3 |= TR3_XTRA_MIGHT;
    else if (streq(what, "XTRA_SHOTS"))		flags3 |= TR3_XTRA_SHOTS;

    else if (streq(what, "IGNORE_FIRE"))	flags3 |= TR3_IGNORE_FIRE;
    else if (streq(what, "IGNORE_COLD"))	flags3 |= TR3_IGNORE_COLD;
    else if (streq(what, "IGNORE_ELEC"))	flags3 |= TR3_IGNORE_ELEC;
    else if (streq(what, "IGNORE_ACID"))	flags3 |= TR3_IGNORE_ACID;

    else if (streq(what, "ACTIVATE"))		flags3 |= TR3_ACTIVATE;
    else if (streq(what, "DRAIN_EXP"))		flags3 |= TR3_DRAIN_EXP;
    else if (streq(what, "TELEPORT"))		flags3 |= TR3_TELEPORT;
    else if (streq(what, "AGGRAVATE"))		flags3 |= TR3_AGGRAVATE;

    else if (streq(what, "BLESSED"))		flags3 |= TR3_BLESSED;
    else if (streq(what, "CURSED"))		flags3 |= TR3_CURSED;
    else if (streq(what, "HEAVY_CURSE"))	flags3 |= TR3_HEAVY_CURSE;
    else if (streq(what, "PERMA_CURSE"))	flags3 |= TR3_PERMA_CURSE;

    if (!flags1 && !flags2 && !flags3) return (FALSE);

    if (flags1) k_ptr->flags1 |= flags1;
    if (flags2) k_ptr->flags2 |= flags2;
    if (flags3) k_ptr->flags3 |= flags3;

    return (TRUE);
}


/*
 * Hack -- location saver for error messages
 */
static int error_k_idx = -1;


/*
 * Initialize the "k_list" array by parsing a file
 * Note that "k_list" starts out totally cleared
 */
static errr init_k_list_txt()
{
    register char *s, *t;

    /* No item kind yet */
    int m = -1;

    /* No k_ptr yet */
    inven_kind *k_ptr = NULL;

    /* The "objects" file */
    FILE *fp;

    /* No line should be more than 80 chars */
    char buf[160];

    /* Open the file */
    fp = fopen(ANGBAND_K_LIST, "r");

    /* Failure */
    if (!fp) return (-1);

    /* Parse the file to initialize "k_list" */
    while (1) {

	/* Read a line from the file, stop when done */
	if (!fgets(buf, 160, fp)) break;

	/* Skip comments */
	if (buf[0] == '#') continue;

	/* Strip the final newline */
	for (s = buf; isprint(*s); ++s); *s = '\0';

	/* Blank lines terminate monsters */
	if (!buf[0]) {

	    /* No current k_ptr */
	    if (!k_ptr) continue;

	    /* Now there is no current k_ptr */
	    k_ptr = NULL;

	    /* Don't hog the processor */
	    gasp();

	    /* Next... */
	    continue;
	}

	/* The line better have a colon and such */
	if (buf[1] != ':') return (1);

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N') {

	    /* Not done the previous one */
	    if (k_ptr) return (2);

	    /* Find, verify, and nuke the colon before the name */
	    if (!(s = strchr(buf+2, ':'))) return (3);

	    /* Nuke the colon, advance to the name */
	    *s++ = '\0';

	    /* Do not allow empty names */
	    if (!*s) return (4);

	    /* Get the index */
	    m = atoi(buf+2);

	    /* For errors */
	    error_k_idx = m;

	    /* Verify */
	    if ((m < 0) || (m >= MAX_K_IDX)) return (5);

	    /* Start a new k_ptr */
	    k_ptr = &k_list[m];

	    /* Make sure we have not done him yet */
	    if (k_ptr->name) return (6);

	    /* Save the name */
	    k_ptr->name = string_make(s);

	    /* Next... */
	    continue;
	}

	/* There better be a current k_ptr */
	if (!k_ptr) return (10);

	/* Process 'I' for "Info" (one line only) */
	if (buf[0] == 'I') {

	    char sym, col;
	    int tmp, tval, sval, pval, num, wgt, lev;
	    long cost;

	    /* Scan for the values */
	    if (9 != sscanf(buf+2, "%c:%c:%d:%d:%d:%d:%d:%d:%ld",
		&sym, &col, &tval, &sval, &pval,
		&num, &wgt, &lev, &cost)) return (11);

	    /* Extract the color */
	    tmp = color_char_to_attr(col);
	    if (tmp < 0) return (12);

	    /* Save the values */
	    k_ptr->k_char = sym;
	    k_ptr->k_attr = tmp;
	    k_ptr->tval = tval;
	    k_ptr->sval = sval;
	    k_ptr->pval = pval;
	    k_ptr->number = num;
	    k_ptr->weight = wgt;
	    k_ptr->level = lev;
	    k_ptr->cost = cost;


	    /* Next... */
	    continue;
	}

	/* Process 'A' for "Allocation" (one line only) */
	if (buf[0] == 'A') {

	    int i;

	    /* Simply read each number following a colon */
	    for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i) {

		/* Default chance */
		k_ptr->chance[i] = 1;

		/* Store the attack damage index */
		k_ptr->locale[i] = atoi(s+1);

		/* Find the slash */
		t = strchr(s+1, '/');

		/* Find the next colon */
		s = strchr(s+1, ':');

		/* If the slash is "nearby", use it */
		if (t && (!s || t < s)) {
		    int chance = atoi(t+1);
		    if (chance > 0) k_ptr->chance[i] = chance;
		}
	    }

	    /* Next... */
	    continue;
	}

	/* Hack -- Process 'P' for "power" and such */
	if (buf[0] == 'P') {

	    int ac, hd1, hd2, th, td, ta;

	    /* Scan for the values */
	    if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
		&ac, &hd1, &hd2, &th, &td, &ta)) return (15);

	    k_ptr->ac = ac;
	    k_ptr->dd = hd1;
	    k_ptr->ds = hd2;
	    k_ptr->tohit = th;
	    k_ptr->todam = td;
	    k_ptr->toac =  ta;

	    /* Next... */
	    continue;
	}

	/* Hack -- Process 'F' for flags */
	if (buf[0] == 'F') {

	    huge flags1, flags2, flags3;

	    /* XXX XXX Hack -- Scan for "pure" values */
	    /* Note that "huge" may not equal "u32b" */
	    if (3 == sscanf(buf+2, "0x%lx:0x%lx:0x%lx",
		&flags1, &flags2, &flags3)) {

		k_ptr->flags1 = flags1;
		k_ptr->flags2 = flags2;
		k_ptr->flags3 = flags3;

		continue;
	    }

	    /* Parse every entry textually */
	    for (s = buf + 2; *s; ) {

		/* Find the end of this entry */
		for (t = s; *t && *t != ' ' && *t != '|'; ++t);

		/* Nuke and skip any dividers */
		if (*t) {
		    *t++ = '\0';
		    while (*t == ' ' || *t == '|') t++;
		}

		/* Parse this entry */
		if (!grab_one_kind_flag(k_ptr, s)) return (18);

		/* Start the next entry */
		s = t;
	    }

	    /* Next... */
	    continue;
	}
    }

    /* Close the file */
    fclose(fp);

    /* Success */
    return (0);
}






#ifdef BINARY_ARRAY_IMAGES


/*
 * Attempt to "dump" a "quick-load binary image" for "k_list"
 */
static errr dump_k_list_raw()
{
    int i, fd;

    int mode = 0666;

    char tmp[1024];


    /* Hack -- sizes */
    uint len[MAX_K_IDX];
    
    /* Size of the "lengths" array */
    uint slen = sizeof(len);

    /* Size of one normal record */
    uint size = sizeof(inven_kind);


#ifdef SET_UID
    mode = 0644;
#endif

#ifdef MACINTOSH
    _ftype = 'DATA';
#endif


    /* File "k_list.raw" -- raw records */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "k_list.raw");
    fd = my_topen(tmp, O_RDWR | O_CREAT | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Dump the version info */
    tmp[0] = CUR_VERSION_MAJ;
    tmp[1] = CUR_VERSION_MIN; 
    tmp[2] = CUR_PATCH_LEVEL;
    tmp[3] = size;

    /* Dump it */
    write(fd, tmp, 4);

    /* Attempt to dump the (normal) monster races */
    for (i = 0; i < MAX_K_IDX; i++) {
	if (size != write(fd, (char*)(&k_list[i]), size)) break;
    }

    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_K_IDX) return (1);

    
    /* File "k_name.raw" -- item kind names */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "k_name.raw");
    fd = my_topen(tmp, O_RDWR | O_CREAT | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Prepare the "lengths" array */
    for (i = 0; i < MAX_K_IDX; i++) {
	len[i] = string_size(k_list[i].name);
    }
    
    /* Dump the array */
    write(fd, (char*)(len), slen);

    /* Dump the names */
    for (i = 0; i < MAX_K_IDX; i++) {
	char *str = (char*)(k_list[i].name);
	if (len[i] && (len[i] != write(fd, str, len[i]))) break;
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_K_IDX) return (1);


    /* Success */
    return (0);
}




/*
 * Attempt to "quick-load" a binary image for "k_list"
 */
static errr init_k_list_raw()
{
    int i, fd;

    int mode = 0666;

    char tmp[1024];


    /* Hack -- sizes */
    uint len[MAX_K_IDX];
    
    /* Size of the "lengths" array */
    uint slen = sizeof(len);

    /* Size of one normal record */
    uint size = sizeof(inven_kind);


#ifdef SET_UID
    mode = 0644;
#endif

#ifdef MACINTOSH
    _ftype = 'DATA';
#endif


    /* File "k_list.raw" -- raw records */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "k_list.raw");
    fd = my_topen(tmp, O_RDONLY | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Read and Verify the version info */    
    if ((4 != read(fd, tmp, 4)) ||
	(tmp[0] != CUR_VERSION_MAJ) ||
	(tmp[1] != CUR_VERSION_MIN) ||
	(tmp[2] != CUR_PATCH_LEVEL) ||
	(tmp[3] != size)) {

	/* Hack -- message */
	msg_print("Ignoring old 'k_list.raw'...");
	msg_print(NULL);

	/* Close */
	close(fd);

	/* Fail */
	return (-1);
    }

    /* Attempt to read the (normal) object kinds */
    for (i = 0; i < MAX_K_IDX; i++) {
	if (size != read(fd, (char*)(&k_list[i]), size)) break;
	k_list[i].name = NULL;
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_K_IDX) return (1);


    /* File "k_name.raw" -- kind names */
    sprintf(tmp, "%s%s%s", ANGBAND_DIR_DATA, PATH_SEP, "k_name.raw");
    fd = my_topen(tmp, O_RDONLY | O_BINARY, mode);
    if (fd < 0) return (-1);

    /* Read the size array */
    if (slen != read(fd, (char*)(len), slen)) return (1);

    /* Attempt to read the (normal) item kind names */
    for (i = 0; i < MAX_K_IDX; i++) {
	if (len[i] != read(fd, tmp, len[i])) break;
	if (len[i]) k_list[i].name = string_make(tmp);
    }
    
    /* Close and verify */
    gasp();
    close(fd);
    if (i < MAX_K_IDX) return (1);


    /* Success */
    return (0);
}

#endif



/*
 * Note that "k_list" starts out totally cleared
 */
static void init_k_list()
{
    errr err;


#ifdef BINARY_ARRAY_IMAGES

    /* Try to load a "binary image" */
    err = init_k_list_raw();

    /* Otherwise try the text version */
    if (!err) return;

#endif
    
    /* Try a text version */
    err = init_k_list_txt();

    /* Still no luck? Fail! */
    if (err) {
    
	/* Warning */
	msg_print(format("Fatal error #%d parsing 'k_list.txt', record %d",
			 err, error_k_idx));
	msg_print(NULL);

	/* Quit */
	quit("cannot load 'k_list.txt'");
    }

#ifdef BINARY_ARRAY_IMAGES

    /* Attempt to dump a "quick-load" version */
    err = dump_k_list_raw();
    
    /* Warn on errors */
    if (err) {
	msg_print("Warning: unable to create 'k_list.raw'");
	msg_print(NULL);
    }

#endif

}






/*
 * Grab one flag in a inven_very from a textual string
 */
static bool grab_one_very_flag(inven_very *v_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L, flags3 = 0L;

    if (!what) what = what;

    else if (streq(what, "STR"))		flags1 |= TR1_STR;
    else if (streq(what, "INT"))		flags1 |= TR1_INT;
    else if (streq(what, "WIS"))		flags1 |= TR1_WIS;
    else if (streq(what, "DEX"))		flags1 |= TR1_DEX;
    else if (streq(what, "CON"))		flags1 |= TR1_CON;
    else if (streq(what, "CHR"))		flags1 |= TR1_CHR;

    else if (streq(what, "SEARCH"))		flags1 |= TR1_SEARCH;
    else if (streq(what, "SPEED"))		flags1 |= TR1_SPEED;
    else if (streq(what, "STEALTH"))		flags1 |= TR1_STEALTH;
    else if (streq(what, "TUNNEL"))		flags1 |= TR1_TUNNEL;
    else if (streq(what, "INFRA"))		flags1 |= TR1_INFRA;
    else if (streq(what, "ATTACK_SPD"))		flags1 |= TR1_ATTACK_SPD;

    else if (streq(what, "KILL_DRAGON"))	flags1 |= TR1_KILL_DRAGON;
    else if (streq(what, "SLAY_DRAGON"))	flags1 |= TR1_SLAY_DRAGON;
    else if (streq(what, "SLAY_ANIMAL"))	flags1 |= TR1_SLAY_ANIMAL;
    else if (streq(what, "SLAY_EVIL"))		flags1 |= TR1_SLAY_EVIL;

    else if (streq(what, "IMPACT"))		flags1 |= TR1_IMPACT;
    else if (streq(what, "BRAND_COLD"))		flags1 |= TR1_BRAND_COLD;
    else if (streq(what, "BRAND_FIRE"))		flags1 |= TR1_BRAND_FIRE;
    else if (streq(what, "BRAND_ELEC"))		flags1 |= TR1_BRAND_ELEC;

    else if (streq(what, "SLAY_UNDEAD"))	flags1 |= TR1_SLAY_UNDEAD;
    else if (streq(what, "SLAY_DEMON"))		flags1 |= TR1_SLAY_DEMON;

    else if (streq(what, "SLAY_TROLL"))		flags1 |= TR1_SLAY_TROLL;
    else if (streq(what, "SLAY_GIANT"))		flags1 |= TR1_SLAY_GIANT;
    else if (streq(what, "SLAY_ORC"))		flags1 |= TR1_SLAY_ORC;

    else if (streq(what, "FREE_ACT"))		flags2 |= TR2_FREE_ACT;
    else if (streq(what, "HOLD_LIFE"))		flags2 |= TR2_HOLD_LIFE;


    else if (streq(what, "IM_FIRE"))		flags2 |= TR2_IM_FIRE;
    else if (streq(what, "IM_COLD"))		flags2 |= TR2_IM_COLD;
    else if (streq(what, "IM_ACID"))		flags2 |= TR2_IM_ACID;
    else if (streq(what, "IM_ELEC"))		flags2 |= TR2_IM_ELEC;
    else if (streq(what, "IM_POIS"))		flags2 |= TR2_IM_POIS;


    else if (streq(what, "RES_ACID"))		flags2 |= TR2_RES_ACID;
    else if (streq(what, "RES_ELEC"))		flags2 |= TR2_RES_ELEC;
    else if (streq(what, "RES_FIRE"))		flags2 |= TR2_RES_FIRE;
    else if (streq(what, "RES_COLD"))		flags2 |= TR2_RES_COLD;
    else if (streq(what, "RES_POIS"))		flags2 |= TR2_RES_POIS;
    else if (streq(what, "RES_LITE"))		flags2 |= TR2_RES_LITE;
    else if (streq(what, "RES_DARK"))		flags2 |= TR2_RES_DARK;

    else if (streq(what, "RES_BLIND"))		flags2 |= TR2_RES_BLIND;
    else if (streq(what, "RES_CONF"))		flags2 |= TR2_RES_CONF;
    else if (streq(what, "RES_SOUND"))		flags2 |= TR2_RES_SOUND;
    else if (streq(what, "RES_SHARDS"))		flags2 |= TR2_RES_SHARDS;

    else if (streq(what, "RES_NETHER"))		flags2 |= TR2_RES_NETHER;
    else if (streq(what, "RES_NEXUS"))		flags2 |= TR2_RES_NEXUS;
    else if (streq(what, "RES_CHAOS"))		flags2 |= TR2_RES_CHAOS;
    else if (streq(what, "RES_DISEN"))		flags2 |= TR2_RES_DISEN;

    else if (streq(what, "SUST_STR"))		flags2 |= TR2_SUST_STR;
    else if (streq(what, "SUST_INT"))		flags2 |= TR2_SUST_INT;
    else if (streq(what, "SUST_WIS"))		flags2 |= TR2_SUST_WIS;
    else if (streq(what, "SUST_DEX"))		flags2 |= TR2_SUST_DEX;
    else if (streq(what, "SUST_CON"))		flags2 |= TR2_SUST_CON;
    else if (streq(what, "SUST_CHR"))		flags2 |= TR2_SUST_CHR;

    else if (streq(what, "EASY_KNOW"))		flags3 |= TR3_EASY_KNOW;
    else if (streq(what, "HIDE_TYPE"))		flags3 |= TR3_HIDE_TYPE;
    else if (streq(what, "SHOW_MODS"))		flags3 |= TR3_SHOW_MODS;
    else if (streq(what, "INSTA_ART"))		flags3 |= TR3_INSTA_ART;

    else if (streq(what, "FEATHER"))		flags3 |= TR3_FEATHER;
    else if (streq(what, "LITE"))		flags3 |= TR3_LITE;
    else if (streq(what, "SEE_INVIS"))		flags3 |= TR3_SEE_INVIS;
    else if (streq(what, "TELEPATHY"))		flags3 |= TR3_TELEPATHY;

    else if (streq(what, "SLOW_DIGEST"))	flags3 |= TR3_SLOW_DIGEST;
    else if (streq(what, "REGEN"))		flags3 |= TR3_REGEN;

    else if (streq(what, "XTRA_MIGHT"))		flags3 |= TR3_XTRA_MIGHT;
    else if (streq(what, "XTRA_SHOTS"))		flags3 |= TR3_XTRA_SHOTS;

#if 0
    else if (streq(what, "IGNORE_FIRE"))	flags3 |= TR3_IGNORE_FIRE;
    else if (streq(what, "IGNORE_COLD"))	flags3 |= TR3_IGNORE_COLD;
    else if (streq(what, "IGNORE_ELEC"))	flags3 |= TR3_IGNORE_ELEC;
    else if (streq(what, "IGNORE_ACID"))	flags3 |= TR3_IGNORE_ACID;
#endif

    else if (streq(what, "ACTIVATE"))		flags3 |= TR3_ACTIVATE;
    else if (streq(what, "DRAIN_EXP"))		flags3 |= TR3_DRAIN_EXP;
    else if (streq(what, "TELEPORT"))		flags3 |= TR3_TELEPORT;
    else if (streq(what, "AGGRAVATE"))		flags3 |= TR3_AGGRAVATE;

    else if (streq(what, "BLESSED"))		flags3 |= TR3_BLESSED;
    else if (streq(what, "CURSED"))		flags3 |= TR3_CURSED;
    else if (streq(what, "HEAVY_CURSE"))	flags3 |= TR3_HEAVY_CURSE;
    else if (streq(what, "PERMA_CURSE"))	flags3 |= TR3_PERMA_CURSE;

    if (!flags1 && !flags2 && !flags3) return (FALSE);

    if (flags1) v_ptr->flags1 |= flags1;
    if (flags2) v_ptr->flags2 |= flags2;
    if (flags3) v_ptr->flags3 |= flags3;

    return (TRUE);
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
      case '.': 
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
      case 'M':
      case 'R':
      case 'T': break;
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

    /* wizard mode commands follow */ // XXX
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

