/* File: death.c */ 

/* Purpose: code executed when player dies */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/*
 * some incorrectly define NULL as integer constant, so load this before
 * local includes 
 */

#include <stdio.h>
#include <signal.h>
#include "constant.h"
#include "angband.h"

#ifdef Pyramid
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <ctype.h>

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#else
#ifdef __MINT__        
#include <sys/types.h>
#endif
#include <unistd.h>
#endif

#ifdef MSDOS
#include <io.h>
#else
#if !defined(ATARIST_MWC) && !defined(MAC)
#ifndef VMS
#include <pwd.h>
#else
#include <file.h>
#endif
#endif
#endif

#ifdef linux
#include <sys/file.h>
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#include <fcntl.h>
#endif
#else
#include <strings.h>
#endif

#if defined(USG) || defined(VMS)
#ifndef L_SET
#define L_SET 0
#endif
#endif

#ifndef VMS
#ifndef MAC
#if defined(ultrix) || defined(USG)
void                perror();
void                exit();

#endif
#endif
#endif

#ifndef MAC
#ifdef SYS_V
struct passwd      *getpwuid();

#endif
#endif

/* Lets do all prototypes correctly.... -CWS */
#ifndef NO_LINT_ARGS
#ifdef __STDC__
static void  date(char *);
static char *center_string(char *, cptr);
static void  print_tomb(void);
static void  kingly(void);

#else
static void  date();
static char *center_string();
static void  print_tomb();
static void  kingly();

#endif
#endif

#ifndef MAC
char        *getlogin();
#endif

#if !defined(time_t)
#define time_t long
#endif

static void date(char *day)
{
    register char *tmp;
    time_t         c;

    c = time((time_t *)0);
    tmp = ctime(&c);
    tmp[10] = '\0';
    (void)strcpy(day, tmp);
}

/* Centers a string within a 31 character string		-JWT-	 */
static char *center_string(char *centered_str, cptr in_str)
{
    register int i, j;

    i = strlen(in_str);
    j = 15 - i / 2;
    (void)sprintf(centered_str, "%*s%s%*s", j, "", in_str, 31 - i - j, "");
    return centered_str;
}


/* Not touched for Mac port */
void display_scores(int from, int to)
{
    register int i = 0, j, k, l;
    int          fd;
    high_scores  score;

/* MAX_SAVE_HISCORES scores, 2 lines per score */
    char         list[2 * MAX_SAVE_HISCORES][128];
    char         hugebuffer[10000];
    char         string[100];

    vtype        tmp_str;

    if (to < 0)
	to = 20;
    if (to > MAX_SAVE_HISCORES)
	to = MAX_SAVE_HISCORES;
#ifdef SET_UID
    if (1 > (fd = my_topen(ANGBAND_TOP, O_RDONLY | O_CREAT, 0644))) {
#else
    if (1 > (fd = my_topen(ANGBAND_TOP, O_RDONLY | O_CREAT, 0666))) {
#endif
	(void)sprintf(string, "Error opening score file \"%s\"\n", ANGBAND_TOP);
	prt(string, 0, 0);
	return;
    }
    while (0 < read(fd, (char *)&score, sizeof(high_scores))) {
	if (score.uid != -1 && getpwuid(score.uid) != NULL)
	    (void)sprintf(hugebuffer, "%3d) %-7ld %s the %s %s (Level %d), played by %s",
			  i / 2 + 1,
			  (long)score.points, score.name,
			  race[score.prace].trace, class[score.pclass].title,
			  (int)score.lev, getpwuid(score.uid)->pw_name);
	else
	    (void)sprintf(hugebuffer, "%3d) %-7ld %s the %s %s (Level %d)",
			  i / 2 + 1,
			  (long)score.points, score.name,
			  race[score.prace].trace, class[score.pclass].title,
			  (int)score.lev);
	strncpy(list[i], hugebuffer, 127);
	(void)sprintf(hugebuffer,
		      "             Killed by %s on Dungeon Level %d.",
		      score.died_from, score.dun_level);
	strncpy(list[i + 1], hugebuffer, 127);
	i += 2;
	if (i >= (MAX_SAVE_HISCORES * 2))
	    break;
    }

    signal(SIGTSTP, SIG_IGN);
    k = from * 2;
    do {
	if (k > 0) {
	    sprintf(tmp_str, "\t\tAngband Hall of Fame (from position %d)",
		    (k / 2) + 1);
	    put_buffer(tmp_str, 0, 0);
	} else {
	    put_buffer("\t\tAngband Hall of Fame                     ", 0, 0);
	}
	put_buffer("     Score", 1, 0);
	l = 0;
	for (j = k; j < i && j < (to * 2) && j < (k + 20); j++, l++)
	    put_buffer(list[j], l + 2, 0);
	k += 20;
	if (!look_line(23)) {
	/* What happens upon dying.				-RAK-	 */
	    msg_print(NULL);
	    clear_screen();
	    flush();		   /* flush all input */
	    nosignals();	   /* Can't interrupt or suspend. */
	    (void)save_char();	   /* Save the memory at least. */
	    restore_term();
	    exit(0);
	}
	clear_screen();
    } while (k < (to * 2) && k < i);
}

/* Pauses for user response before returning		-RAK-	 */
int look_line(int prt_line)
{
    prt("[Press ESC to quit, any other key to continue.]", prt_line, 17);
    if (inkey() == ESCAPE) {
	erase_line(prt_line, 0);
	return 0;
    } else {
	erase_line(prt_line, 0);
	return 1;
    }
}


/* Not touched for Mac port */
/*
 * init_scorefile Open the score file while we still have the setuid
 * privileges.  Later when the score is being written out, you must be sure
 * to flock the file so we don't have multiple people trying to write to it
 * at the same time. Craig Norborg (doc)		Mon Aug 10 16:41:59
 * EST 1987 
 */
void init_scorefile()
{
#ifdef SET_UID
    if (1 > (highscore_fd = my_topen(ANGBAND_TOP, O_RDWR | O_CREAT, 0644)))
#else
    if (1 > (highscore_fd = my_topen(ANGBAND_TOP, O_RDWR | O_CREAT, 0666)))
#endif
    {
	(void)fprintf(stderr, "Can't open score file \"%s\"\n", ANGBAND_TOP);
	exit(1);
    }
}


/* Attempt to open the intro file			-RAK-	 */
/*
 * Hack -- Check the hours file vs. what time it is	-Doc
 */
void read_times(void)
{
    register int i;
    vtype        in_line;
    FILE        *file1;

#ifdef CHECKHOURS
/* Attempt to read hours.dat.	 If it does not exist,	   */
/* inform the user so he can tell the wizard about it	 */

    file1 = my_tfopen(ANGBAND_HOU, "r");
    if (file1) {
	while (fgets(in_line, 80, file1)) {
	    if (strlen(in_line) > 3) {
		if (!strncmp(in_line, "SUN:", 4))
		    (void)strcpy(days[0], in_line);
		else if (!strncmp(in_line, "MON:", 4))
		    (void)strcpy(days[1], in_line);
		else if (!strncmp(in_line, "TUE:", 4))
		    (void)strcpy(days[2], in_line);
		else if (!strncmp(in_line, "WED:", 4))
		    (void)strcpy(days[3], in_line);
		else if (!strncmp(in_line, "THU:", 4))
		    (void)strcpy(days[4], in_line);
		else if (!strncmp(in_line, "FRI:", 4))
		    (void)strcpy(days[5], in_line);
		else if (!strncmp(in_line, "SAT:", 4))
		    (void)strcpy(days[6], in_line);
	    }
	}
	(void)fclose(file1);
    }

    else {
	restore_term();
	fprintf(stderr, "There is no hours file \"%s\".\nPlease inform the wizard, %s, so he can correct this!\n", ANGBAND_HOU, WIZARD);
	exit(1);
    }

/* Check the hours, if closed	then exit. */
    if (!check_time()) {
	file1 = my_tfopen(ANGBAND_HOU, "r");
	if (file1) {
	    clear_screen();
	    for (i = 0; fgets(in_line, 80, file1); i++) {
		put_buffer(in_line, i, 0);
	    }
	    (void)fclose(file1);
	    pause_line(23);
	}
	exit_game();
    }
#endif				   /* CHECK_HOURS */

/* Print the introduction message, news, etc.		 */
    if ((file1 = my_tfopen(ANGBAND_MOR, "r")) != NULL) {
	clear_screen();
	for (i = 0; fgets(in_line, 80, file1) != NULL; i++)
	    put_buffer(in_line, i, 0);
	pause_line(23);
	(void)fclose(file1);
    }
}


/*
 * File perusal.	    -CJS- primitive, but portable 
 */
void helpfile(cptr filename)
{
    bigvtype tmp_str;
    FILE    *file;
    char     input;
    int      i;

    file = my_tfopen(filename, "r");
    if (!file) {
	(void)sprintf(tmp_str, "Can not find help file \"%s\".\n", filename);
	prt(tmp_str, 0, 0);
	return;
    }

    save_screen();

    while (!feof(file))
    {
	clear_screen();
	for (i = 0; i < 23; i++) {
	    if (fgets(tmp_str, BIGVTYPESIZ - 1, file) != NULL) {
		put_buffer(tmp_str, i, 0);
	    }
	}

	prt("[Press any key to continue.]", 23, 23);
	input = inkey();
	if (input == ESCAPE) break;
    }

    (void)fclose(file);

    restore_screen();
}

/*
 * Prints a list of random objects to a file.  Note that -RAK-	 
 * the objects produced is a sampling of objects which		 
 * be expected to appear on that level.				
 */
void print_objects()
{
    register int         i;
    int                  nobj, j, level;
    vtype                filename1;
    bigvtype             tmp_str;
    register FILE       *file1;
    register inven_type *i_ptr;

    prt("Produce objects on what level?: ", 0, 0);
    level = 0;
    if (!get_string(tmp_str, 0, 32, 10))
	return;
    level = atoi(tmp_str);
    prt("Produce how many objects?: ", 0, 0);
    nobj = 0;
    if (!get_string(tmp_str, 0, 27, 10))
	return;
    nobj = atoi(tmp_str);
    if ((nobj > 0) && (level > -1) && (level < 1201)) {
	if (nobj > 10000)
	    nobj = 10000;
	prt("File name: ", 0, 0);
	if (get_string(filename1, 0, 11, 64)) {
	    if (strlen(filename1) == 0)
		return;
	    if ((file1 = my_tfopen(filename1, "w")) != NULL) {
		(void)sprintf(tmp_str, "%d", nobj);
		prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
		put_qio();
		(void)fprintf(file1, "*** Random Object Sampling:\n");
		(void)fprintf(file1, "*** %d objects\n", nobj);
		(void)fprintf(file1, "*** For Level %d\n", level);
		(void)fprintf(file1, "\n");
		(void)fprintf(file1, "\n");
		j = popt();
		for (i = 0; i < nobj; i++) {
		    if (randint(MAX_DUNGEON_OBJ) == 1) {
			int                 tmp;

		again:
			tmp = randint(MAX_OBJECTS - (SPECIAL_OBJ - 1)) +
			    (SPECIAL_OBJ - 1) - 1;
			switch (tmp) {
			  case (SPECIAL_OBJ - 1):
			    if (object_list[tmp].level <= level)
				msg_print("*Remove Curse*");
			    break;
			  case (SPECIAL_OBJ):
			    if (object_list[tmp].level <= level)
				msg_print("[Resistance of Scarabtarices]");
			    break;
			  case (SPECIAL_OBJ + 1):
			    if (object_list[tmp].level <= level)
				msg_print("[Mordenkainen's Escapes]");
			    break;
			  case (SPECIAL_OBJ + 2):
			    if (object_list[tmp].level <= level)
				msg_print("[Kelek's Grimoire of Power]");
			    break;
			  case (SPECIAL_OBJ + 3):
			    if (randint(2) == 1)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("[Tenser's Transformations]");
			    break;
			  case (SPECIAL_OBJ + 4):
			    if (randint(2) == 1)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("[Raal's Tome of Destruction]");
			    break;
			  case (SPECIAL_OBJ + 5):
			    if (object_list[tmp].level <= level)
				msg_print("[Ethereal Openings]");
			    break;
			  case (SPECIAL_OBJ + 6):
			    if (object_list[tmp].level <= level)
				msg_print("[Godly Insights]");
			    break;
			  case (SPECIAL_OBJ + 7):
			    if (object_list[tmp].level <= level)
				msg_print("[Purifications and Healing]");
			    break;
			  case (SPECIAL_OBJ + 8):
			    if (object_list[tmp].level <= level)
				msg_print("[Wrath of God]");
			    break;
			  case (SPECIAL_OBJ + 9):
			    if (randint(2) > 1)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("[Holy Infusions]");
			    break; /* End of Books */
			  case (SPECIAL_OBJ + 10):	/* Narya */
			    if (randint(4) > 1)
				goto again;
			    if (NARYA)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Narya");
			    NARYA = TRUE;
			    break;
			  case (SPECIAL_OBJ + 11):	/* Nenya */
			    if (randint(5) > 1)
				goto again;
			    if (NENYA)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Nenya");
			    NENYA = TRUE;
			    break;
			  case (SPECIAL_OBJ + 12):	/* Vilya */
			    if (randint(6) > 1)
				goto again;
			    if (VILYA)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Vilya");
			    VILYA = TRUE;
			    break;
			  case (SPECIAL_OBJ + 13):	/* The Ring of Power */
			    if (randint(10) > 1)
				goto again;
			    if (POWER)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Power (The One Ring)");
			    POWER = TRUE;
			    break;
			  case (SPECIAL_OBJ + 14):	/* Dragon Scale mails */
			  case (SPECIAL_OBJ + 15):
			  case (SPECIAL_OBJ + 16):
			  case (SPECIAL_OBJ + 17):
			  case (SPECIAL_OBJ + 18):
			  case (SPECIAL_OBJ + 19):
			  case (SPECIAL_OBJ + 20):
			    if (randint(2) > 1)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Dragon Scale Mail");
			    break;
			  case (SPECIAL_OBJ + 23):	/* Multi-Hued Dragon
							 * Scale Mail */
			    if (randint(3) > 1)
				goto again;
			    if (object_list[tmp].level <= level)
				msg_print("Multi-Hued Dragon Scale Mail");
			    break;
			}
			if (object_list[tmp].level > level)
			    continue;
			invcopy(&t_list[j], tmp);
			if (object_list[t_list[j].index].tval == TV_HARD_ARMOR)
			    t_list[j].toac += 10 + randint(10);
			i_ptr = &t_list[j];
			store_bought(i_ptr);
			objdes(tmp_str, i_ptr, TRUE);
		    } else {
			invcopy(&t_list[j],
				sorted_objects[get_obj_num(level, FALSE)]);
			magic_treasure(j, level, 0, FALSE);
			i_ptr = &t_list[j];
			store_bought(i_ptr);
			if (i_ptr->flags & TR_CURSED)
			    add_inscribe(i_ptr, ID_DAMD);
			objdes(tmp_str, i_ptr, TRUE);
		    }
		    (void)fprintf(file1, "%d %s.\n", i_ptr->level, tmp_str);
		}
		pusht((s16b) j);
		(void)fclose(file1);
		prt("Completed.", 0, 0);
	    } else
		prt("File could not be opened.", 0, 0);
	}
    } else
	prt("Parameters no good.", 0, 0);
}


/*
 * Print the character to a file or device.
 */
int file_character(cptr filename1)
{
    register int		i;
    int				j;
    int				fd;
    inven_type			*i_ptr;
    cptr			p;
    cptr			colon;
    cptr			blank;

    register FILE		*file1;

    int                 xbth, xbthb, xfos, xsrh;
    int			xstl, xdis, xsave, xdev;
    vtype                 xinfra;

    register struct misc *p_ptr;

    vtype			out_val, prt1;
    bigvtype			prt2;

    fd = my_topen(filename1, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0 && errno == EEXIST) {
	(void)sprintf(out_val, "Replace existing file %s?", filename1);
	if (get_Yn(out_val)) {
	    fd = my_topen(filename1, O_WRONLY, 0644);
	}
    }
    if (fd >= 0) {
	/* on some non-unix machines, fdopen() is not reliable, */
	/* hence must call close() and then fopen()  */
	(void)close(fd);
	file1 = my_tfopen(filename1, "w");
    }
    else {
	file1 = NULL;
    }

    /* Dump a character sheet */
    if (file1) {

	prt("Writing character sheet...", 0, 0);
	put_qio();

	colon = ":";
	blank = " ";

	(void)fprintf(file1, "%c\n\n", CTRL('L'));
	(void)fprintf(file1, " Name%9s %-23s", colon, py.misc.name);
	(void)fprintf(file1, "Age%11s %6d ", colon, (int)py.misc.age);
	cnv_stat(py.stats.use_stat[A_STR], prt1);
	(void)fprintf(file1, "   STR : %s\n", prt1);
	(void)fprintf(file1, " Race%9s %-23s", colon, race[py.misc.prace].trace);
	(void)fprintf(file1, "Height%8s %6d ", colon, (int)py.misc.ht);
	cnv_stat(py.stats.use_stat[A_INT], prt1);
	(void)fprintf(file1, "   INT : %s\n", prt1);
	(void)fprintf(file1, " Sex%10s %-23s", colon,
		      (py.misc.male ? "Male" : "Female"));
	(void)fprintf(file1, "Weight%8s %6d ", colon, (int)py.misc.wt);
	cnv_stat(py.stats.use_stat[A_WIS], prt1);
	(void)fprintf(file1, "   WIS : %s\n", prt1);
	(void)fprintf(file1, " Class%8s %-23s", colon,
		      class[py.misc.pclass].title);
	(void)fprintf(file1, "Social Class : %6d ", py.misc.sc);
	cnv_stat(py.stats.use_stat[A_DEX], prt1);
	(void)fprintf(file1, "   DEX : %s\n", prt1);
	(void)fprintf(file1, " Title%8s %-23s", colon, title_string());
	(void)fprintf(file1, "%22s", blank);
	cnv_stat(py.stats.use_stat[A_CON], prt1);
	(void)fprintf(file1, "   CON : %s\n", prt1);
	(void)fprintf(file1, "%34s", blank);
	(void)fprintf(file1, "%26s", blank);
	cnv_stat(py.stats.use_stat[A_CHR], prt1);
	(void)fprintf(file1, "   CHR : %s\n\n", prt1);

	(void)fprintf(file1, " + To Hit    : %6d", py.misc.dis_th);
	(void)fprintf(file1, "%7sLevel      :%9d", blank, (int)py.misc.lev);
	(void)fprintf(file1, "   Max Hit Points : %6d\n", py.misc.mhp);
	(void)fprintf(file1, " + To Damage : %6d", py.misc.dis_td);
	(void)fprintf(file1, "%7sExperience :%9ld", blank, (long)py.misc.exp);
	(void)fprintf(file1, "   Cur Hit Points : %6d\n", py.misc.chp);
	(void)fprintf(file1, " + To AC     : %6d", py.misc.dis_tac);
	(void)fprintf(file1, "%7sMax Exp    :%9ld", blank, (long)py.misc.max_exp);
	(void)fprintf(file1, "   Max Mana%8s %6d\n", colon, py.misc.mana);
	(void)fprintf(file1, "   Total AC  : %6d", py.misc.dis_ac);
	
	if (py.misc.lev >= MAX_PLAYER_LEVEL) {
	    (void)fprintf(file1, "%7sExp to Adv.:%9s", blank, "****");
	}
	else {
	    (void)fprintf(file1, "%7sExp to Adv.:%9ld", blank,
			  (long) (player_exp[py.misc.lev - 1] *
				   py.misc.expfact / 100));
	}

	(void)fprintf(file1, "   Cur Mana%8s %6d\n", colon, py.misc.cmana);
	(void)fprintf(file1, "%28sGold%8s%9ld\n", blank, colon, (long)py.misc.au);

	p_ptr = &py.misc;
	xbth = p_ptr->bth + p_ptr->ptohit * BTH_PLUS_ADJ
	    + (class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
	xbthb = p_ptr->bthb + p_ptr->ptohit * BTH_PLUS_ADJ
	    + (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);
    /* this results in a range from 0 to 29 */
	xfos = 40 - p_ptr->fos;
	if (xfos < 0)
	    xfos = 0;
	xsrh = p_ptr->srh;
    /* this results in a range from 0 to 9 */
	xstl = p_ptr->stl + 1;
	xdis = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
	    + (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
	xsave = p_ptr->save + stat_adj(A_WIS)
	    + (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
	xdev = p_ptr->save + stat_adj(A_INT)
	    + (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

	(void)sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

	(void)fprintf(file1, "(Miscellaneous Abilities)\n\n");
	(void)fprintf(file1, " Fighting    : %-10s", likert(xbth, 12));
	(void)fprintf(file1, "   Stealth     : %-10s", likert(xstl, 1));
	(void)fprintf(file1, "   Perception  : %s\n", likert(xfos, 3));
	(void)fprintf(file1, " Bows/Throw  : %-10s", likert(xbthb, 12));
	(void)fprintf(file1, "   Disarming   : %-10s", likert(xdis, 8));
	(void)fprintf(file1, "   Searching   : %s\n", likert(xsrh, 6));
	(void)fprintf(file1, " Saving Throw: %-10s", likert(xsave, 6));
	(void)fprintf(file1, "   Magic Device: %-10s", likert(xdev, 6));
	(void)fprintf(file1, "   Infra-Vision: %s\n\n", xinfra);

	/* Write out the character's history     */
	(void)fprintf(file1, "Character Background\n");
	for (i = 0; i < 4; i++) {
	    (void)fprintf(file1, " %s\n", py.misc.history[i]);
	}

	/* Write out the equipment list.	     */
	j = 0;
	(void)fprintf(file1, "\n  [Character's Equipment List]\n\n");
	if (!equip_ctr) {
	    (void)fprintf(file1, "  Character has no equipment in use.\n");
	}
	else {
	    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
		i_ptr = &inventory[i];
		if (i_ptr->tval != TV_NOTHING) {
		    switch (i) {
		      case INVEN_WIELD:
			p = "You are wielding";
			break;
		      case INVEN_HEAD:
			p = "Worn on head";
			break;
		      case INVEN_NECK:
			p = "Worn around neck";
			break;
		      case INVEN_BODY:
			p = "Worn on body";
			break;
		      case INVEN_ARM:
			p = "Worn on shield arm";
			break;
		      case INVEN_HANDS:
			p = "Worn on hands";
			break;
		      case INVEN_RIGHT:
			p = "Right ring finger";
			break;
		      case INVEN_LEFT:
			p = "Left  ring finger";
			break;
		      case INVEN_FEET:
			p = "Worn on feet";
			break;
		      case INVEN_OUTER:
			p = "Worn about body";
			break;
		      case INVEN_LIGHT:
			p = "Light source is";
			break;
		      case INVEN_AUX:
			p = "Secondary weapon";
			break;
		      default:
			p = "*Unknown value*";
			break;
		    }
		    objdes(prt2, &inventory[i], TRUE);
		    (void)fprintf(file1, "  %c) %-19s: %s\n", j + 'a', p, prt2);
		    j++;
		}
	    }
	}

	/* Write out the character's inventory.	     */
	(void)fprintf(file1, "%c\n\n", CTRL('L'));
	(void)fprintf(file1, "  [General Inventory List]\n\n");
	if (!inven_ctr) {
	    (void)fprintf(file1, "  Character has no objects in inventory.\n");
	}
	else {
	    for (i = 0; i < inven_ctr; i++) {
		objdes(prt2, &inventory[i], TRUE);
		(void)fprintf(file1, "%c) %s\n", i + 'a', prt2);
	    }
	}
	(void)fprintf(file1, "%c", CTRL('L'));

	fprintf(file1, "  [%s%s Home Inventory]\n\n", py.misc.name,
		(toupper(py.misc.name[strlen(py.misc.name)-1]) == 'S' ? "'" : "'s"));
	if (store[MAX_STORES-1].store_ctr == 0) {
	    (void) fprintf(file1, "  Character has no objects at home.\n");
	}
	else {
	    for (i = 0; i < store[MAX_STORES-1].store_ctr; i++) {
		if (i == 12) fprintf(file1, "\n");  
		objdes(prt2, &store[MAX_STORES-1].store_inven[i].sitem, TRUE);
		(void) fprintf(file1, "%c) %s\n", (i%12)+'a', prt2);
	    }
	}
	(void) fprintf(file1, "%c", CTRL('L'));

	(void)fclose(file1);
	prt("Completed.", 0, 0);
	return TRUE;
    }

    else {

	if (fd >= 0) (void)close(fd);
	(void)sprintf(out_val, "Can't open file %s:", filename1);
	msg_print(out_val);
	return FALSE;
    }
}

/*
 * Prints the gravestone of the character		-RAK-	 
 */
static void print_tomb()
{
    register int         i;
    char                 day[11];
    cptr		 p;
    vtype                str, tmp_str;
    FILE                *fp;

    if (stricmp(died_from, "Interrupting") && !wizard) {
	sprintf(str, "%s/%d", ANGBAND_BONES, dun_level);
	if ((fp = my_tfopen(str, "r")) == NULL && (dun_level > 1)) {
	    if ((fp = my_tfopen(str, "w")) != NULL) {
#ifndef __MINT__
#ifdef SET_UID
		(void)fchmod(fileno(fp), 0644);
#else
		(void)fchmod(fileno(fp), 0666);
#endif
#endif
		fprintf(fp, "%s\n%d\n%d\n%d",
		  py.misc.name, py.misc.mhp, py.misc.prace, py.misc.pclass);
		if (fp) fclose(fp);
	    }
	} else {
	    if (fp) fclose(fp);
	}
    }

    /* Draw a tombstone */
    clear_screen();
    put_buffer("_______________________", 1, 15);
    put_buffer("/", 2, 14);
    put_buffer("\\         ___", 2, 38);
    put_buffer("/", 3, 13);
    put_buffer("\\ ___   /   \\      ___", 3, 39);
    put_buffer("/            RIP            \\   \\  :   :     /   \\", 4, 12);
    put_buffer("/", 5, 11);
    put_buffer("\\  : _;,,,;_    :   :", 5, 41);
    (void)sprintf(str, "/%s\\,;_          _;,,,;_",
		  center_string(tmp_str, py.misc.name));
    put_buffer(str, 6, 10);
    put_buffer("|               the               |   ___", 7, 9);
    p = total_winner ? "Magnificent" : title_string();
    (void)sprintf(str, "| %s |  /   \\", center_string(tmp_str, p));
    put_buffer(str, 8, 9);
    put_buffer("|", 9, 9);
    put_buffer("|  :   :", 9, 43);
    if (!total_winner) {
	p = class[py.misc.pclass].title;
    }
    else if (py.misc.male) {
	p = "*King*";
    }
    else {
	p = "*Queen*";
    }

    (void)sprintf(str, "| %s | _;,,,;_   ____", center_string(tmp_str, p));
    put_buffer(str, 10, 9);
    (void)sprintf(str, "Level : %d", (int)py.misc.lev);
    (void)sprintf(str, "| %s |          /    \\", center_string(tmp_str, str));
    put_buffer(str, 11, 9);
    (void)sprintf(str, "%ld Exp", (long)py.misc.exp);
    (void)sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 12, 9);
    (void)sprintf(str, "%ld Au", (long)py.misc.au);
    (void)sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 13, 9);
    (void)sprintf(str, "Died on Level : %d", dun_level);
    (void)sprintf(str, "| %s |         _;,,,,;_", center_string(tmp_str, str));
    put_buffer(str, 14, 9);
    put_buffer("|            killed by            |", 15, 9);
    p = died_from;
    i = strlen(p);
    died_from[i] = '.';			   /* add a trailing period */
    died_from[i + 1] = '\0';
    (void)sprintf(str, "| %s |", center_string(tmp_str, p));
    put_buffer(str, 16, 9);
    died_from[i] = '\0';		   /* strip off the period */
    date(day);
    (void)sprintf(str, "| %s |", center_string(tmp_str, day));
    put_buffer(str, 17, 9);
    put_buffer("*|   *     *     *    *   *     *  | *", 18, 8);
    put_buffer("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______",
	       19, 0);

    flush();

    put_buffer("(ESC to abort, return to print on screen, or file name)", 23, 0);
    put_buffer("Character record?", 22, 0);

    if (get_string(str, 22, 18, 60)) {

	for (i = 0; i < INVEN_ARRAY_SIZE; i++) {
    inven_type *i_ptr = &inventory[i];
    if (i_ptr && i_ptr->tval != TV_NOTHING) {
	    known1(i_ptr);
	    known2(i_ptr);
	}
    }

    /* Show player */
    clear_screen();
    calc_bonuses();
    display_char();

    put_buffer("Type ESC to skip the inventory:", 23, 0);

    if (inkey() != ESCAPE) {
	    clear_screen();
	    msg_print("You are using:");
	    (void)show_equip(TRUE, 0);
	    msg_print(NULL);
	    if (inven_ctr) {
		msg_print("You are carrying:");
		clear_from(1);
		(void)show_inven(0, inven_ctr - 1, TRUE, 0, 0);
		msg_print(NULL);
	    }
          msg_print ("You have stored at your house:");
          clear_from (1);
	{ /* show home's inventory... */
            store_type *s_ptr = &store[7]; /* home */
            int ii = 0, j = 0;
            vtype t1, t2;
            
            while ( ii <s_ptr->store_ctr) {
              j = 0;
              sprintf(t2, "(page %d)", (ii==0?1:2));
              prt(t2, 1, 3);
              while ((ii<s_ptr->store_ctr) && (j<12)){
                known1(&s_ptr->store_inven[ii].sitem);
                known2(&s_ptr->store_inven[ii].sitem);
                objdes(t1, &s_ptr->store_inven[ii].sitem, TRUE);
                sprintf(t2, "%c) %s", 'a'+j, t1);
                prt(t2, j+2, 4); 
                j++;
                ii++;
	    } /* items 1-12, 13-24 loop */
              if (ii < s_ptr->store_ctr) { /* if we're done, skip this */
                msg_print(NULL);
                msg_print("Home inventory:");
                clear_from (1);
	    }
	  } /* outer while loop */
            msg_print(NULL);
	} /* scope block of display-home inventory code -CFT */

	}
    }
}


/* Calculates the total number of points earned		-JWT-	 */
long total_points()
{
    return (py.misc.max_exp + (100 * py.misc.max_dlv));
}



/*
 * Enters a players name on a hi-score table, if "legal".
 */
static errr top_twenty(void)
{
    int        i, k;
    int          j;

    high_scores   scores[MAX_SAVE_HISCORES], myscore;

    /* Wipe screen */
    clear_screen();

    /* Wizard-mode pre-empts scoring */
    if (wizard || to_be_wizard) {
	display_scores(0, 10);
	(void)save_char();
	restore_term();
	exit(0);
    }

    /* Interupted */    
    if (!total_winner && !stricmp(died_from, "Interrupting")) {
	msg_print("Score not registered due to interruption.");
	display_scores(0, 10);
	(void)save_char();
	restore_term();
	exit(0);
    }

    /* Quitter */
    if (!total_winner && !stricmp(died_from, "Quitting")) {
	msg_print("Score not registered due to quitting.");
	display_scores(0, 10);
	(void)save_char();
	restore_term();
	exit(0);
    }

    myscore.points = total_points();
    myscore.dun_level = dun_level;
    myscore.lev = py.misc.lev;
    myscore.max_lev = py.misc.max_dlv;
    myscore.mhp = py.misc.mhp;
    myscore.chp = py.misc.chp;
    myscore.uid = player_uid;
/* First character of sex, lower case */
    myscore.sex = py.misc.male;
    myscore.prace = py.misc.prace;
    myscore.pclass = py.misc.pclass;
    (void)strcpy(myscore.name, py.misc.name);
    (void)strncpy(myscore.died_from, died_from, strlen(died_from));
    myscore.died_from[strlen(died_from)] = '\0';
/* Get rid of '.' at end of death description */

/* First, get a lock on the high score file so no-one else tries */
/* to write to it while we are using it */
/* added sys_v call to lockf - cba */
#ifdef USG
    if (lockf(highscore_fd, F_LOCK, 0) != 0)
#else
    if (0 != flock(highscore_fd, LOCK_EX))
#endif
    {
	perror("Error gaining lock for score file");
	exit_game();
    }
/* Check to see if this score is a high one and where it goes */
    i = 0;
#ifndef BSD4_3
    (void)lseek(highscore_fd, (long)0, L_SET);
#else
    (void)lseek(highscore_fd, (off_t) 0, L_SET);
#endif
    while ((i < MAX_SAVE_HISCORES)
    && (0 != read(highscore_fd, (char *)&scores[i], sizeof(high_scores)))) {
	i++;
    }

    j = 0;
    while (j < i && (scores[j].points >= myscore.points)) {
	j++;
    }
/* i is now how many scores we have, and j is where we put this score */

/* If its the first score, or it gets appended to the file */
    if (!i || (i == j && j < MAX_SAVE_HISCORES)) {
	(void)lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
	(void)write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    } else if (j < i) {
    /* If it gets inserted in the middle */
    /* Bump all the scores up one place */
	for (k = MY_MIN(i, (MAX_SAVE_HISCORES - 1)); k > j; k--) {
	    (void)lseek(highscore_fd, (long)(k * sizeof(high_scores)), L_SET);
	    (void)write(highscore_fd, (char *)&scores[k - 1], sizeof(high_scores));
	}
    /* Write out your score */
	(void)lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
	(void)write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    }
/* added usg lockf call - cba */
#ifdef USG
    lockf(highscore_fd, F_ULOCK, 0);
#else
    (void)flock(highscore_fd, LOCK_UN);
#endif
    (void)close(highscore_fd);

    /* Hack -- Display the top fifteen scores */
    if (j < 10) {
	display_scores(0, 10);
    }
    
    /* Display the scores surrounding the player */
    else if (j > (i - 5)) {
	display_scores(i - 9, i + 1);
    }
    else {
	display_scores(j - 5, j + 5);
    }
    

    /* Success */
    return (0);
}

/*
 * Enters a players name on the hi-score table     SM	 
 */
void delete_entry(int which)
{
    register int i;
    high_scores  scores[MAX_SAVE_HISCORES];

/* added usg lockf call - cba */
#ifdef USG
    if (lockf(highscore_fd, F_LOCK, 0) != 0)
#else
    if (0 != flock(highscore_fd, LOCK_EX))
#endif
    {
	perror("Error gaining lock for score file");
	exit_game();
    }
/* Check to see if this score is a high one and where it goes */
    i = 0;
    (void)lseek(highscore_fd, (off_t) 0, L_SET);
    while ((i < MAX_SAVE_HISCORES) &&
	 (0 != read(highscore_fd, (char *)&scores[i], sizeof(high_scores))))
	i++;

    if (i >= which) {

#if defined(sun) || defined(ultrix) || defined(NeXT)
	ftruncate(highscore_fd, 0);
#endif

/* If its the first score, or it gets appended to the file */
	lseek(highscore_fd, 0, L_SET);
	if (which > 0)
	    write(highscore_fd, (char *)&scores[0],
		  (which - 1) * sizeof(high_scores));
	if (i > which)
	    write(highscore_fd, (char *)&scores[which],
		  (i - which) * sizeof(high_scores));
	} else {
	    puts(" The high score table does not have that many entries!");
	}

/* added usg lockf call - cba */
#ifdef USG
    lockf(highscore_fd, F_ULOCK, 0);
#else
    (void)flock(highscore_fd, LOCK_UN);
#endif
    (void)close(highscore_fd);
    if (which < 10) {
	display_scores(0, 10);
    } else if (which > (i - 10)) {
	display_scores(i - 10, i);
    } else
	display_scores(which - 5, which + 5);
}








/*
 * Change the player into a King!			-RAK-	 
 */
static void kingly()
{
    register struct misc *p_ptr;
    register cptr p;

    /* Change the character attributes.		 */
    dun_level = 0;

    /* Change the character attributes.		 */
    (void)strcpy(died_from, "Ripe Old Age");
    p_ptr = &py.misc;
    (void)restore_level();
    p_ptr->lev += MAX_PLAYER_LEVEL;
    p_ptr->au += 250000L;
    p_ptr->max_exp += 5000000L;
    p_ptr->exp = p_ptr->max_exp;

    /* Let the player know that he did good.	 */
    clear_screen();
    put_buffer("#", 1, 34);
    put_buffer("#####", 2, 32);
    put_buffer("#", 3, 34);
    put_buffer(",,,  $$$  ,,,", 4, 28);
    put_buffer(",,=$   \"$$$$$\"   $=,,", 5, 24);
    put_buffer(",$$        $$$        $$,", 6, 22);
    put_buffer("*>         <*>         <*", 7, 22);
    put_buffer("$$         $$$         $$", 8, 22);
    put_buffer("\"$$        $$$        $$\"", 9, 22);
    put_buffer("\"$$       $$$       $$\"", 10, 23);
    p = "*#########*#########*";
    put_buffer(p, 11, 24);
    put_buffer(p, 12, 24);
    put_buffer("Veni, Vidi, Vici!", 15, 26);
    put_buffer("I came, I saw, I conquered!", 16, 21);
    if (p_ptr->male) {
	put_buffer("All Hail the Mighty King!", 17, 22);
    }
    else {
	put_buffer("All Hail the Mighty Queen!", 17, 22);
    }
    flush();
    pause_line(23);
}


/*
 * Exit the game
 * Handles the gravestone end top-twenty routines	-RAK-	
 */
void exit_game(void)
{
    register int        i;

#ifdef MAC
/* Prevent strange things from happening */
    enablefilemenu(FALSE);
#endif

    /* What happens upon dying.				-RAK-	 */
    /* Flush the messages */
    msg_print(NULL);

    /* Flush the input */
    flush();

    /* Can't interrupt or suspend. */
    nosignals();

    if (turn >= 0) {

	/* Handle retirement */
	if (total_winner) kingly();

	/* You are dead */
	print_tomb();

	/* Handle score, show Top scores */
	if (!wizard && !to_be_wizard)
	top_twenty();
	else msg_print("Score not registered.");
    }

    i = log_index;
   /* Save the memory at least. */
    (void)save_char();
    if (i > 0) display_scores(0, 10);
    erase_line(23, 0);
    restore_term();
    exit(0);
}
