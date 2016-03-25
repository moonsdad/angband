/* File: main.c */ 

/* Purpose: initialization, main() function and main loop */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Local "game mode" vars
 */
static int new_game = FALSE;
static int fiddle = FALSE;
static int force_rogue_like = FALSE;
static int force_keys_to = FALSE;

int unfelt    = TRUE;
int be_nasty  = FALSE;
int player_uid;
int quests[QUEST_MAX];
monster_race ghost;

#if (COST_ADJ != 100)
static void price_adjust();
#endif


/* Unique artifact weapon flags, even though some are in the wrong place!*/
s32b GROND, RINGIL, AEGLOS, ARUNRUTH, MORMEGIL, ANGRIST, GURTHANG,
  CALRIS, ANDURIL, STING, ORCRIST, GLAMDRING, DURIN, AULE, THUNDERFIST,
  BLOODSPIKE, DOOMCALLER, NARTHANC, NIMTHANC, DETHANC, GILETTAR, RILIA,
  BELANGIL, BALLI, LOTHARANG, FIRESTAR, ERIRIL, CUBRAGOL, BARD, COLLUIN,
  HOLCOLLETH, TOTILA, PAIN, ELVAGIL, AGLARANG, EORLINGAS, BARUKKHELED,
  WRATH, HARADEKKET, MUNDWINE, GONDRICAM, ZARCUTHRA, CARETH, FORASGIL,
  CRISDURIAN, COLANNON, HITHLOMIR, THALKETTOTH, ARVEDUI, THRANDUIL, THENGEL,
  HAMMERHAND, CELEBORN, THROR, MAEDHROS, OLORIN, ANGUIREL, OROME,
  EONWE, THEODEN, ULMO, OSONDIR, TURMIL, TIL, DEATHWREAKER, AVAVIR, TARATOL;

/* Unique artifact armor flags */
s32b DOR_LOMIN, NENYA, NARYA, VILYA, BELEGENNON, FEANOR, ISILDUR, SOULKEEPER,
FINGOLFIN, ANARION, POWER, PHIAL, BELEG, DAL, PAURHACH, PAURNIMMEN, PAURAEGEN,
PAURNEN, CAMMITHRIM, CAMBELEG, INGWE, CARLAMMAS, HOLHENNETH, AEGLIN, CAMLOST,
NIMLOTH, NAR, BERUTHIEL, GORLIM, ELENDIL, THORIN, CELEGORM, THRAIN,
GONDOR, THINGOL, THORONGIL, LUTHIEN, TUOR, ROHAN, TULKAS, NECKLACE, BARAHIR,
CASPANION, RAZORBACK, BLADETURNER;


/*
 * Unix machines need to "check wizard permissions"
 */
static bool is_wizard(int uid)
{
    int		test;
    FILE	*fp;
    char	buf[100];

    bool allow = FALSE;


    /* Open the wizard file */
    fp = my_tfopen(ANGBAND_WIZ, "r");

    if (!fp) {
	fprintf(stderr, "Can't get wizard check...");
	exit_game();
    }
    do {
	(void)fgets(buf, sizeof buf, fp);
	if (sscanf(buf, "%d", &test)) {
	    if (test == uid && buf[0] != '#') {
		fclose(fp);
		return TRUE;
	    }
	}
    } while (!feof(fp));

    /* Close the file */
    fclose(fp);

    /* Result */
    return (allow);
}



/*
 * Initialize, restore, and get the ball rolling.	-RAK-	
 */
int main(int argc, char * argv[])
{
    int generate, i;
    int result = FALSE;

#ifndef __MINT__
    FILE *fp;
    char temphost[MAXHOSTNAMELEN+1];
    char thishost[MAXHOSTNAMELEN+1];
    char discard[120];
#endif
    char string[80];
    struct rlimit rlp;

    /* Save the "program name" */
    argv0 = argv[0];
    

#if !defined(MSDOS) && !defined(HPUX)
    /* Disable core dumps */
    getrlimit(RLIMIT_CORE,&rlp);
    rlp.rlim_cur=0;
    setrlimit(RLIMIT_CORE,&rlp);
#endif
    
    /* default command set defined in config.h file */
    rogue_like_commands = ROGUE_LIKE;
    
    strcpy(p_ptr->misc.name, "\0");
    


#ifndef SET_UID
# if !defined(MSDOS) && !defined(__EMX__)
    (void) umask(0);
# endif
#endif

#ifdef SECURE
    Authenticate();
#endif
    
#ifdef MSDOS			/* -CFT */
    msdos_init();		/* set up some screen stuff + get cnf file */
#endif
    

    /* Get the file paths */
    get_file_paths();

    /* call this routine to grab a file pointer to the highscore file
     * and prepare things to relinquish setuid privileges */
    init_scorefile();

    /* Call this routine to grab a file pointer to the log files and
     * start the backup process before relinquishing setuid privileges */
    init_files();

#ifndef MSDOS
    /* Get the user id (?) */
    player_uid = getuid();
    if (player_uid < 0) {
	perror("Can't set permissions correctly!  Getuid call failed.\n");
	exit(0);
    }
    user_name(p_ptr->misc.name, player_uid);
#else
    user_name(p_ptr->misc.name);
#endif

#if defined(SET_UID) && !defined(SECURE)
    /* Set the user id or quit */
    if (setuid(geteuid()) != 0) {
	quit("setuid(): cannot set permissions correctly!");
    }
#endif
    
#ifdef ANNOY
    if (player_uid == ANNOY) be_nasty=TRUE;
    else
#endif
    be_nasty=FALSE;

#if !defined(MSDOS) && !defined(__MINT__)
    (void)gethostname(thishost, (sizeof thishost) - 1);	/* get host */
    fp = my_tfopen(ANGBAND_LOAD, "r");
    if (!fp) quit("cannot get load-check!");

    /* Find ourself */
    while (1) {
	if (fscanf(fp, "%s%d", temphost, &LOAD) == EOF) {
	    LOAD=100;
	    break;
	}

	/* Hack -- Discard comments */
	if (temphost[0]=='#') {
	    (void)fgets(discard, (sizeof discard)-1, fp);
	    continue;
	}

    /* Until we've found ourselves */
	if (!strcmp(temphost,thishost) || !strcmp(temphost,"localhost")) break;
    }

    fclose(fp);
#endif

    /* use curses */
    init_curses();

    /* check for user interface option */
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv) {
	switch (argv[0][1]) {
	  case 'A':
	  case 'a':
	    if (!is_wizard(player_uid)) goto usage;
	    peek=TRUE;
	    break;
	  case 'N':
	  case 'n':
	    new_game = TRUE;
	    break;
	  case 'O':
	  case 'o':
	    /* rogue_like_commands may be set in get_char() */
	    /* so delay this until after read savefile if any */
	    force_rogue_like = TRUE;
	    force_keys_to = FALSE;
	    break;
	  case 'R':
	  case 'r':
	    force_rogue_like = TRUE;
	    force_keys_to = TRUE;
	    break;
	  case 'S':
	  case 's':
	    init_curses();
	    if (isdigit((int)argv[0][2])) display_scores(0, atoi(&argv[0][2]));
	    else display_scores(0, 10);
	    exit_game();
	  case 'D':
	  case 'd':
	    if (!is_wizard(player_uid)) goto usage;
	    if (isdigit((int)argv[0][2])) delete_entry(atoi(&argv[0][2]));
	    else display_scores(0, 10);
	    exit_game();
	  case 'F':
	  case 'f':
	    if (is_wizard(player_uid) || to_be_wizard) FIDDLE = TRUE;
	    else goto usage;
	    break;
	  case 'W':
	  case 'w':
	    if (is_wizard(player_uid)) to_be_wizard = TRUE;
	    else goto usage;
#ifndef MSDOS
	    if (isdigit((int)argv[0][2])) player_uid = atoi(&argv[0][2]);
#endif
	    break;
	  case 'u':
	  case 'U':
	    if (!argv[0][2]) goto usage;
	    strcpy(p_ptr->misc.name, &argv[0][2]);
		d_check(p_ptr->misc.name);
		NO_SAVE=TRUE;
	    break;

	  default:
	  usage:

	    if (is_wizard(player_uid)) {
#ifdef MSDOS
		puts("Usage: angband [-afnorw] [-s<num>] [-d<num>] <file>");
#else
		puts("Usage: angband [-afnor] [-s<num>] [-u<name>] [-w<uid>] [-d<num>]");
#endif
		puts("  a       Activate \"peek\" mode");
		puts("  d<num>  Delete high score number <num>");
		puts("  f       Enter \"fiddle\" mode");
		puts("  n       Start a new character");
		puts("  o       Use original command set");
		puts("  r       Use the \"rogue-like\" command set");
		puts("  s<num>  Show high scores.  Show <num> scores, or first 10");
#ifdef MSDOS
		puts("  w       Start in wizard mode");
		puts(" <file>   Play with savefile named <file>");
#else
		puts("  w<num>  Start in wizard mode, as uid number <num>");
		puts("  u<name> Play with character named <name>");
#endif
		puts("Each option must be listed separately (ie '-r -n', not '-rn')");
	    }
	    else {
#ifdef MSDOS
		puts("Usage: angband [-nor] [-s<num>] <file>");
#else
		puts("Usage: angband [-nor] [-s<num>] [-u<name>]");
#endif
		puts("  n       Start a new character");
		puts("  o       Use original command set");
		puts("  r       Use the \"rogue-like\" command set");
		puts("  s<num>  Show high scores.  Show <num> scores, or first 10");
#ifdef MSDOS
		puts(" <file>   Play with savefile named <file>");
#else
		puts("  u<name> Play with character named <name>");
#endif
		puts("Each option must be listed separately (ie '-r -n', not '-rn')");
	    }
	    /* Actually abort the process */
	    exit(1);
	}
    }

    /* catch those nasty signals */
    /* must come after init_curses as some of the signal handlers use curses */
    signals_init();

    /* Check operating hours			*/
    /* If not wizard  No_Control_Y	        */
    read_times();

    /* Some necessary initializations		*/
    /* all made into constants or initialized in variables.c */

#if (COST_ADJ != 100)
    price_adjust();
#endif

    /* Grab a random seed from the clock          */
    init_seeds();

    /* Init monster and treasure levels for allocate */
    init_m_level();
    init_t_level();

    /* Init the store inventories			*/
    store_init();

#ifndef MACINTOSH
    /* On Mac, if -n is passed, no savefile is used */
    /* If -n is not passed, the calling routine will know savefile name,
       hence, this code is not necessary */
#endif

    (void) sprintf(savefile, "%s/%d%s", ANGBAND_SAV, player_uid, p_ptr->misc.name);

 /* This restoration of a saved character may get ONLY the monster memory. In
    this case, get_char returns false. It may also resurrect a dead character
    (if you are the wizard). In this case, it returns true, but also sets the
    parameter "generate" to true, as it does not recover any cave details. */

    if (FIDDLE) {
	if (get_char(&generate))
	    save_player();
	exit_game();
    }

    result = get_char(&generate);
    /* enter wizard mode before showing the character display, but must wait
       until after get_char in case it was just a resurrection */
    if (to_be_wizard)
	if (!enter_wiz_mode())
	    exit_game();

    if ((new_game == FALSE) && result) {
	change_name();

	/* could be restoring a dead character after a signal or HANGUP */
	if (p_ptr->misc.chp < 0)
	    death = TRUE;
    } else {			/* Create character */
	/* Unique Weapons, Armour and Rings */
	GROND=0;
	RINGIL=0;
	AEGLOS=0;
	ARUNRUTH=0;
	MORMEGIL=0;
	ANGRIST=0;
	GURTHANG=0;
	CALRIS=0;
	ANDURIL=0;
	STING=0;
	ORCRIST=0;
	GLAMDRING=0;
	DURIN=0;
	AULE=0;
	THUNDERFIST=0;
	BLOODSPIKE=0;
	DOOMCALLER=0;
	NARTHANC=0;
	NIMTHANC=0;
	DETHANC=0;
	GILETTAR=0;
	RILIA=0;
	BELANGIL=0;
	BALLI=0;
	LOTHARANG=0;
	FIRESTAR=0;
	ERIRIL=0;
	CUBRAGOL=0;
	BARD=0;
	COLLUIN=0;
	HOLCOLLETH=0;
	TOTILA=0;
	PAIN=0;
	ELVAGIL=0;
	AGLARANG=0;
	EORLINGAS=0;
	BARUKKHELED=0;
	WRATH=0;
	HARADEKKET=0;
	MUNDWINE=0;
	GONDRICAM=0;
	ZARCUTHRA=0;
	CARETH=0;
	FORASGIL=0;
	CRISDURIAN=0;
	COLANNON=0;
	HITHLOMIR=0;
	THALKETTOTH=0;
	ARVEDUI=0;
	THRANDUIL=0;
	THENGEL=0;
	HAMMERHAND=0;
	CELEGORM=0;
	THROR=0;
	MAEDHROS=0;
	OLORIN=0;
	ANGUIREL=0;
	OROME=0;
	EONWE=0;
	THEODEN=0;
	ULMO=0;
	OSONDIR=0;
	TURMIL=0;
	TIL=0;
	DEATHWREAKER=0;
	AVAVIR=0;
	TARATOL=0;
	DOR_LOMIN=0;
	BELEGENNON=0;
	FEANOR=0;
	ISILDUR=0;
	SOULKEEPER=0;
	FINGOLFIN=0;
	ANARION=0;
	BELEG=0;
	DAL=0;
	PAURHACH=0;
	PAURNIMMEN=0;
	PAURAEGEN=0;
	PAURNEN=0;
	CAMMITHRIM=0;
	CAMBELEG=0;
	HOLHENNETH=0;
	AEGLIN=0;
	CAMLOST=0;
	NIMLOTH=0;
	NAR=0;
	BERUTHIEL=0;
	GORLIM=0;
	THORIN=0;
	CELEBORN=0;
	GONDOR=0;
	THINGOL=0;
	THORONGIL=0;
	LUTHIEN=0;
	TUOR=0;
	ROHAN=0;
	CASPANION=0;
	RAZORBACK=0;
	BLADETURNER=0;
	NARYA=0;
	NENYA=0;
	VILYA=0;
	POWER=0;
	PHIAL=0;
	INGWE=0;
	CARLAMMAS=0;
	TULKAS=0;
	NECKLACE=0;
	BARAHIR=0;
	ELENDIL=0;
	THRAIN=0;

	for (i=0; i<QUEST_MAX; i++) quests[i]=0;

	quests[SAURON_QUEST]=99;

	/* Unique Monster Flags */
	for (i=0; i<MAX_R_IDX; i++)
	    u_list[i].exist=0, u_list[i].dead=0;
	player_birth();

	/* if we're creating a new character, change the savefile name */
    (void) sprintf(savefile, "%s/%d%s", ANGBAND_SAV, player_uid, p_ptr->misc.name);
	char_inven_init();
	p_ptr->flags.food = 7500;
	p_ptr->flags.food_digested = 2;
        if (class[p_ptr->misc.pclass].spell == MAGE)
	{			/* Magic realm   */
	    clear_screen();	/* makes spell list easier to read */
	    calc_spells(A_INT);
	    calc_mana(A_INT);
	}
	else if (class[p_ptr->misc.pclass].spell == PRIEST)
	{			/* Clerical realm*/
	    calc_spells(A_WIS);
	    clear_screen();	/* force out the 'learn prayer' message */
	    calc_mana(A_WIS);
	}
        if (!_new_log())
	{
	    (void) sprintf(string, "Can't get at log file \"%s\".", ANGBAND_LOG);
	    msg_print(string);
	    exit_game();
	}
	/* prevent ^c quit from entering score into scoreboard,
	   and prevent signal from creating panic save until this point,
	   all info needed for save file is now valid */
	character_generated = 1;
	generate = TRUE;
    }

    /* Reset "rogue_like_commands" */
    if (force_rogue_like) {
	rogue_like_commands = force_keys_to;
    }

    flavor_init();

    /* Begin the game */
    clear_screen();
    prt_stat_block();

    /* Make a level */
    if (generate) generate_cave();

    /* Loop till dead, or exit */
    while(!death) {

	/* Dungeon logic */
	dungeon();

#ifndef MACINTOSH
	/* check for eof here, see inkey() in io.c */
	/* eof can occur if the process gets a HANGUP signal */
	if (eof_flag)
	{
	    (void) strcpy(died_from, "(end of input: saved)");
	    if (!save_player())
	    {
		(void) strcpy(died_from, "unexpected eof");
	    }
	    /* should not reach here, by if we do, this guarantees exit */
	    death = TRUE;
	}
#endif
	good_item_flag = FALSE;

	/* Make the New level */
	if (!death) generate_cave();
    }

	/* Character gets buried. */
    exit_game();
    /* should never reach here, but just in case */
    return (0);
}





/*
 * Init players with some belongings			-RAK-
 */
static void char_inven_init()
{
    register int i, j;
    inven_type inven_init;
    
    /* this is needed for bash to work right, it can't hurt anyway */
    for (i = 0; i < INVEN_TOTAL; i++)
	invcopy(&inventory[i], OBJ_NOTHING);

    /* Give the player useful objects */
    for (i = 0; i < 5; i++) {
	j = player_init[p_ptr->misc.pclass][i];
	invcopy(&inven_init, j);
	store_bought(&inven_init);
	if (inven_init.tval == TV_SWORD || inven_init.tval == TV_HAFTED
	    || inven_init.tval == TV_BOW)
	    inven_init.ident |= ID_SHOW_HITDAM;
	(void) inven_carry(&inven_init);
    }

    /* weird place for it, but why not? */
    for (i = 0; i < 64; i++)
	spell_order[i] = 99;
}


/*
 * Initializes M_LEVEL array for use with PLACE_MONSTER	-RAK-	
 */
static void init_m_level()
{
    register int i, k;

    for (i = 0; i <= MAX_R_LEV; i++)
	m_level[i] = 0;

    k = MAX_R_IDX - WIN_MON_TOT;
    for (i = 0; i < k; i++)
	m_level[r_list[i].level]++;

    for (i = 1; i <= MAX_R_LEV; i++)
	m_level[i] += m_level[i-1];
}


/*
 * Initializes T_LEVEL array for use with PLACE_OBJECT	-RAK-	
 */
static void init_t_level()
{
    register int i, l;
    int tmp[MAX_OBJ_LEVEL+1];

    for (i = 0; i <= MAX_OBJ_LEVEL; i++)
	t_level[i] = 0;
    for (i = 0; i < MAX_DUNGEON_OBJ; i++)
	t_level[object_list[i].level]++;
    for (i = 1; i <= MAX_OBJ_LEVEL; i++)
	t_level[i] += t_level[i-1];

    /* now produce an array with object indexes sorted by level, by using
       the info in t_level, this is an O(n) sort! */
    /* this is not a stable sort, but that does not matter */
    for (i = 0; i <= MAX_OBJ_LEVEL; i++)
	tmp[i] = 1;
    for (i = 0; i < MAX_DUNGEON_OBJ; i++)
    {
	l = object_list[i].level;
	sorted_objects[t_level[l] - tmp[l]] = i;
	tmp[l]++;
    }
}


#if (COST_ADJ != 100)
/*
 * Adjust prices of objects				-RAK-	
 */
static void price_adjust()
{
    register int i;

    /* round half-way cases up */
    for (i = 0; i < MAX_OBJECTS; i++)
	object_list[i].cost = ((object_list[i].cost * COST_ADJ) + 50) / 100;
}
#endif

static int d_check(char *a)
{
    while (*a)
	if (iscntrl(*a)) {
	    msg_print("Yuch! No control characters, Thankyou!");
	    exit_game();
	} else a++;
    return (0);
}


