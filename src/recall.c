/* File: recall.c */

/* Purpose: maintain and display monster memory */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



static const char  *desc_atype[] = {
    "do something undefined",
    "attack",
    "weaken",
    "confuse",
    "terrify",
    "burn",
    "shoot acid",
    "freeze",
    "electrify",
    "corrode",
    "blind",
    "paralyse",
    "steal money",
    "steal things",
    "poison",
    "reduce dexterity",
    "reduce constitution",
    "drain intelligence",
    "drain wisdom",
    "lower experience",
    "call for help",
    "disenchant",
    "eat your food",
    "absorb light",
    "absorb charges",
    "reduce all stats"
};

static const char  *desc_amethod[] = {
    "make an undefined advance",
    "hit",
    "bite",
    "claw",
    "sting",
    "touch",
    "kick",
    "gaze",
    "breathe",
    "spit",
    "wail",
    "embrace",
    "crawl on you",
    "release spores",
    "beg",
    "slime you",
    "crush",
    "trample",
    "drool",
    "insult",
    "butt",
    "charge",
    "engulf",
    "moan"
};

static const char  *desc_howmuch[] = {
    " not at all",
    " a bit",
    " somewhat",
    " quite",
    " very",
    " most",
    " highly",
    " extremely"
};

static const char  *desc_move[] = {
   "move invisibly",
   "open doors",
   "pass through walls",
   "kill weaker creatures",
   "pick up objects",
   "breed explosively"
};

static const char  *desc_spell[] = {
    "1",
    "2",
    "4",
    "8",
    "teleport short distances",
    "teleport long distances",
    "teleport %s prey",
    "cause light wounds",
    "cause serious wounds",
    "paralyse %s prey",
    "induce blindness",
    "confuse",
    "terrify",
    "summon a monster",
    "summon the undead",
    "slow %s prey",
    "drain mana",
    "summon demonkind",
    "summon dragonkind",
    "lightning",	/* breaths 1 */
    "poison gases",
    "acid",
    "frost",
    "fire",	/* end */
    "cast fire bolts",
    "cast frost bolts",
    "cast acid bolts",
    "cast magic missiles",
    "cause critical wounds",
    "cast fire balls",
    "cast frost balls",
    "cast mana bolts",
    "chaos",	/* breaths2 */
    "shards",
    "sound",
    "confusion",
    "disenchantment",
    "nether",	/* end of part1 */
    "cast lightning bolts",
    "cast lightning balls",
    "cast acid balls",
    "create traps",
    "wound %s prey",
    "cast mind blasting",
    "teleport away %s prey",
    "heal",
    "haste",
    "fire missiles",
    "cast plasma bolts",
    "summon many creatures",
    "cast nether bolts",
    "cast ice bolts",
    "cast darkness",
    "cause amnesia",
    "sear your mind",
    "cast stinking clouds",
    "teleport %s preys level",
    "cast water bolts",
    "cast whirlpools",
    "cast nether balls",
    "summon an angel",
    "summon spiders",
    "summon hounds",
    "nexus",	/* part 2... */
    "elemental force",	/* breaths 3 */
    "inertia",
    "light",
    "time",
    "gravity",
    "darkness",
    "plasma",	/* end */
    "fire arrows",
    "summon Ringwraiths",
    "cast darkness storms",
    "cast mana storms",
    "summon reptiles",
    "summon ants",
    "summon unique creatures",
    "summon greater undead",
    "summon ancient dragons"
};

static const char  *desc_weakness[] = {
    "bright light",
    "rock remover"
};

static const char  *desc_immune[] = {
    "frost",
    "fire",
    "lightning",
    "poison",
    "acid"
};


static cptr wd_They[4] = { "They", "He", "She", "It" };
static cptr wd_they[4] = { "they", "he", "she", "it" };
static cptr wd_poss[4] = { "their", "his", "her", "its" };
static cptr wd_these[4] = { "these", "this", "this", "this" };
static cptr wd_creat[4] = { "creatures", "creature", "creature", "creature" };
static cptr wd_do[4] =   { "do", "does", "does", "does" };
static cptr wd_are[4] =  { "are", "is", "is", "is" };
static cptr wd_have[4] = { "have", "has", "has", "has" };
static cptr wd_live[4] = { "live", "lives", "lives", "lives" };
static cptr wd_move[4] = { "move", "moves", "moves", "moves" };
static cptr wd_appear[4] = { "appear", "appears", "appears", "appears" };
static cptr wd_resist[4] = { "resist", "resists", "resists", "resists" };



/* Pluralizer: count, singular, plural) */
#define plural(c, ss, sp)	((c) == 1 ? (ss) : (sp))

/* Number of kills needed for information. */
/* the higher the level of the monster, the fewer the kills you need */
#define knowarmor(l,d)		((d) > 304 / (4 + (l)))

/* the higher the level of the monster, the fewer the attacks you need, */
/* the more damage an attack does, the more attacks you need */
#define knowdamage(l,a,d)	((4 + (l))*(a) > 80 * (d))

/* use slightly different tests for unique monsters to hasten learning -CFT */
#define knowuniqarmor(l,d)    ((d) > 304 / (38 + (5*(l))/4))
#define knowuniqdamage(l,a,d) ((4 +(long)(l))*(2*(long)(a)) > 80 * (d))













/*
 * Keep track of which lines have been cleared
 */
static int max_clear = 0;


/*
 * A simple "recall fresh" function
 */
static void recall_fresh(void)
{
    /* Hack -- use a special window */
    if (use_recall_win && term_recall) {

	Term_activate(term_recall);
	Term_fresh();
	Term_activate(term_screen);
    }

    /* On top of the normal window */
    else {

	Term_fresh();
    }
}


/*
 * A simple "recall clear" function
 */
static void recall_clear(void)
{
    /* Hack -- use a special window */
    if (use_recall_win && term_recall) {

	Term_activate(term_recall);
	Term_clear();
	Term_activate(term_screen);
    }

    /* On top of the normal window */
    else {
        
	msg_print(NULL);
	max_clear = 0;
    }
}


/*
 * A simple "recall print" function, with color.
 */
static void recall_putstr(int x, int y, int n, byte a, cptr s)
{
    /* Hack -- use a special window */
    if (use_recall_win && term_recall) {

	Term_activate(term_recall);
	Term_putstr(x, y, n, a, s);
	Term_activate(term_screen);
    }

    /* On top of the normal window */
    else {
        
	/* Be sure we have cleared up to the line below us */
	while (max_clear < y+2) {
	    Term_erase(0, max_clear, 80-1, max_clear);
	    max_clear++;
	}

	/* Send it to the Term */
	Term_putstr(x, y, n, a, s);
    }
}



/*
 * Buffer text, dumping full lines via "recall_print()".
 * Very similar to the "message()" function in many ways.
 * Automatically wraps to the next line when necessary.
 * Also wraps to the next line on any "newline" in "str".
 * If "str" is NULL, call "recall_clear()" and restart.
 * There are never more chars buffered than can be printed.
 */
static void c_roff(byte a, cptr str)
{
    register cptr p, r;

    /* Line buffer */
    static char roff_buf[256];

    /* Current Pointer into roff_buf */
    static char *roff_p = roff_buf;

    /* Last space saved into roff_buf */
    static char *roff_s = NULL;

    /* Place to print current line */
    static int roff_row = 0;

#ifdef USE_COLOR
    if (!use_color) a = TERM_WHITE;
#else
    a = TERM_WHITE;
#endif

    /* Special handling for "new sequence" */
    if (!str) {

	/* Reset the row */
	roff_row = 1;

	/* Reset the pointer */
	roff_p = roff_buf;

	/* No spaces yet */
	roff_s = NULL;

	/* Simple string (could just return) */
	str = "";
    }

    /* Scan the given string, character at a time */
    for (p = str; *p; p++) {

	int wrap = (*p == '\n');
	char ch = *p;

	/* Clean up the char */
	if (!isprint(ch)) ch = ' ';

	/* We may be "forced" to wrap */
	if (roff_p >= roff_buf + 80) wrap = 1;

	/* Try to avoid "hanging" single letter words */
	if ((ch == ' ') && (roff_p + 2 >= roff_buf + 80)) wrap = 1;

	/* Handle line-wrap */
	if (wrap) {

	    /* We must not dump past here */
	    *roff_p = '\0';

	    /* Assume nothing will be left over */
	    r = roff_p;

	    /* If we are in the middle of a word, try breaking on a space */
	    if (roff_s && (ch != ' ')) {

		/* Nuke the space */
		*roff_s = '\0';

		/* Remember the char after the space */
		r = roff_s + 1;
	    }

	    /* Dump the line, advance the row */	    
	    recall_putstr(0, roff_row++, -1, TERM_WHITE, roff_buf);

	    /* No spaces yet */
	    roff_s = NULL;

	    /* Restart the buffer scanner */
	    roff_p = roff_buf;

	    /* Restore any "wrapped" chars */
	    while (*r) *roff_p++ = *r++;
	}

	/* Save the char.  Hack -- skip leading spaces (and newlines) */
	if ((roff_p > roff_buf) || (ch != ' ')) {
	    if (ch == ' ') roff_s = roff_p;
	    *roff_p++ = ch;
	}
    }
}

static void roff(cptr str)
{
    c_roff(TERM_WHITE, str);
}



/*
 * Do we know anything special about this monster? 
 */
int bool_roff_recall(int r_idx)
{
    register monster_lore *l_ptr;
    register int          i;

    if (wizard) return TRUE;

    l_ptr = &l_list[r_idx];

    if (l_ptr->r_cflags1 || l_ptr->r_cflags2 ||
	l_ptr->r_spells1 || l_ptr->r_spells2 || l_ptr->r_spells3 ||
	l_ptr->r_kills || l_ptr->r_deaths) {
	return TRUE;
    }

    for (i = 0; i < 4; i++) {
	if (l_ptr->r_attacks[i]) {
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Print out what we have discovered about this monster.
 * i, j, k changed from int, to avoid PC's 16bit ints -CFT
 */
int roff_recall(int r_idx)
{
    register monster_lore  *l_ptr;
    register monster_race  *r_ptr;
    register u32b         i, j, k;

    const char             *p, *q;
    u16b                  *pu;

    int			n_len; /* XXX Somehow got in here earier -RMHV */
    int                 mspeed;
    u32b		flags;
    u32b              rcflags1, rcflags2;
    u32b              rspells1, rspells2, rspells3;
    int                 breath = FALSE, magic = FALSE;
    int			msex;

    monster_lore        save_mem;

    vtype               temp;

    /* Access the race and lore */
    r_ptr = &r_list[r_idx];
    l_ptr = &l_list[r_idx];

    /* XXX Note that "it" and "they" are "reversed" */
    if (r_ptr->gender == 'p' ) msex = 0;
    if (r_ptr->gender == 'f' ) msex = 2;
    if (r_ptr->gender == 'm' ) msex = 1;
    else msex = 3;


    /* Hack -- Wizards know everything */
    if (wizard) {

	/* Save the "old" memory */
	save_mem = *l_ptr;

	/* Make assumptions about kills, etc */
	l_ptr->r_kills = MAX_SHORT;
	l_ptr->r_wake = l_ptr->r_ignore = MAX_UCHAR;

	/* Count the total possible treasures */
	j = (((r_ptr->cflags1 & MF1_HAS_4D2) ? 8 : 0) +
			 ((r_ptr->cflags1 & MF1_HAS_2D2) ? 4 : 0) +
			 ((r_ptr->cflags1 & MF1_HAS_1D2) ? 2 : 0) +
			 ((r_ptr->cflags1 & MF1_HAS_90)  ? 1 : 0) +
			 ((r_ptr->cflags1 & MF1_HAS_60)  ? 1 : 0));

	/* Full "flag" knowledge */
	l_ptr->r_cflags1 = (r_ptr->cflags1 & ~CM1_TREASURE) | (j << CM1_TR_SHIFT);
	l_ptr->r_cflags2 = r_ptr->cflags2;
	l_ptr->r_spells1 = r_ptr->spells1 | CS1_FREQ;
	l_ptr->r_spells2 = r_ptr->spells2;
	l_ptr->r_spells3 = r_ptr->spells3;

	j = 0;
	pu = r_ptr->damage;
	while (*pu != 0 && j < 4) {
	    l_ptr->r_attacks[j] = MAX_UCHAR;
	    j++;
	    pu++;
	}
    }


    /* Load the various flags */
    rspells1 = l_ptr->r_spells1 & r_ptr->spells1 & ~CS1_FREQ;
    rspells2 = l_ptr->r_spells2 & r_ptr->spells2;
    rspells3 = l_ptr->r_spells3 & r_ptr->spells3;

    /* the MF1_WINNER property is always known, set it if a win monster */
    rcflags1 = l_ptr->r_cflags1 | (MF1_WINNER & r_ptr->cflags1);
    rcflags2 = l_ptr->r_cflags2 & r_ptr->cflags2;
    if ((r_ptr->cflags2 & MF2_UNIQUE) || (msex == 0))
	(void)sprintf(temp, "%s:\n", r_ptr->name);
    else
	(void)sprintf(temp, "The %s:\n", r_ptr->name);

    /* Begin a new "recall" */
    roff(temp);
	/* Conflict history. */
	/* changed to act better for unique monsters -CFT */

    /* Treat uniques differently... -CFT */
    if (r_ptr->cflags2 & MF2_UNIQUE) {

	/* We've been killed... */
	if (l_ptr->r_deaths) {
	    (void)sprintf(temp, "%s %s slain %d of your ancestors",
			  wd_They[msex], wd_have[msex], l_ptr->r_deaths);
	    roff(temp);

	    /* but we've also killed it */
	    if (u_list[r_idx].dead) {
		sprintf(temp, ", but you have avenged %s!  ",
			plural(l_ptr->r_deaths, "him", "them"));
		roff(temp);
	    }
	    else {
		sprintf(temp, ", who %s unavenged.  ",
			plural(l_ptr->r_deaths, "remains", "remain"));
		roff(temp);
	    }
	}

	/* Dead unique who never hurt us */
	else if (u_list[r_idx].dead) {	/* we killed it w/o dying... yet! */
	    roff("You have slain this foe.  ");
	}
    }

    /* Not unique, but killed us */
    else if (l_ptr->r_deaths) {

	(void)sprintf(temp,
		      "%d of your ancestors %s been killed by %s %s, and ",
		      l_ptr->r_deaths, plural(l_ptr->r_deaths, "has", "have"),
		      wd_these[msex], wd_creat[msex]);
	roff(temp);

	if (l_ptr->r_kills == 0) {
	    sprintf(temp, "%s %s not ever known to have been defeated.  ",
		     wd_they[msex], wd_are[msex]);
	    roff(temp);
	}
	else {
	    (void)sprintf(temp,
			"at least %d of the beasts %s been exterminated.  ",
			  l_ptr->r_kills, plural(l_ptr->r_kills, "has", "have"));
	    roff(temp);
	}
    }

    /* Not unique, and never killed us */
    else if (l_ptr->r_kills) {
	(void)sprintf(temp, "At least %d of these creatures %s",
		      l_ptr->r_kills, plural(l_ptr->r_kills, "has", "have"));
	roff(temp);
	roff(" been killed by you and your ancestors.  ");
    }

    /* Never killed (and not unique) */
    else {
	roff("No battles to the death are recalled.  ");
    }


    /* Hack -- Describe the ghost (first time is weird) */
    if (r_idx == MAX_R_IDX-1) {
	roff("You feel you know it, and it knows you.  ");
	roff("This can only mean trouble.  ");
    }

    /* Describe a "normal" monster */
    else {
	roff(desc_list[r_idx].desc);
	roff("  ");
    }


    /* Describe location */

    k = FALSE;
    if (r_ptr->level == 0) {
	roff(format("%s %s in the town", wd_They[msex], wd_live[msex]));
	k = TRUE;
    }
    else if (l_ptr->r_kills) {
	if (depth_in_feet) {
	    roff(format("%s %s normally found at depths of %d feet",
			wd_They[msex], wd_are[msex], r_ptr->level * 50));
	}
	else {
	    roff(format("%s %s normally found on dungeon level %d",
			wd_They[msex], wd_are[msex], r_ptr->level));
	}
	k = TRUE;
    }


    /* Extract the "speed" */
    /* the r_list speed value is 10 greater, so that it can be a byte */
    mspeed = r_ptr->speed - 10;

    /* Describe movement, if any observed */
    if ((rcflags1 & CM1_ALL_MV_FLAGS) || (rcflags1 & CM1_RANDOM_MOVE)) {
	if (k) {
	    roff(", and ");
	}
	else {
	    roff(wd_They[msex]);
	    roff(" ");
	    k = TRUE;
	}
	roff(wd_move[msex]);
	if (rcflags1 & CM1_RANDOM_MOVE) {
	    roff(desc_howmuch[(rcflags1 & CM1_RANDOM_MOVE) >> 3]);
	    roff(" erratically");
	}
	if (mspeed == 1) {
	    roff(" at normal speed");
	}
	else {
	    if (rcflags1 & CM1_RANDOM_MOVE) {
		roff(", and");
	    }
	    if (mspeed <= 0) {
		if (mspeed == -1) roff(" very");
		else if (mspeed < -1) roff(" incredibly");
		roff(" slowly");
	    }
	    else {
		if (mspeed == 3) roff(" very");
		else if (mspeed > 3) roff(" unbelievably");
		roff(" quickly");
	    }
	}
    }

    /* The code above includes "attack speed" */
    if (rcflags1 & MF1_MV_ONLY_ATT) {
	if (k) {
	    roff(", but ");
	}
	else {
	    roff(wd_They[msex]);
	    roff(" ");
	    k = TRUE;
	}
	roff(wd_do[msex]);
	roff(" not deign to chase intruders");
    }

    /* End this sentence */
    if (k) {
	roff(".  ");
    }


    /* Kill it once to know experience, and quality */
    /* (evil, undead, monsterous).  The quality of being a dragon is obvious. */
    if (l_ptr->r_kills) {

	if (r_ptr->cflags2 & MF2_UNIQUE) {
	    roff("Killing this");
	}
	else {
	    roff("A kill of ");
	    roff(wd_these[msex]);
	}

	/* Describe the "quality" */
	if (r_ptr->cflags2 & MF2_ANIMAL) roff(" natural");
	if (r_ptr->cflags2 & MF2_EVIL) roff(" evil");
	if (r_ptr->cflags2 & MF2_UNDEAD) roff(" undead");

	/* XXX Plural "specials" will yield stupid message */

	roff(" ");
	if (r_ptr->cflags2 & MF2_GIANT) roff("giant");
	else if (r_ptr->cflags2 & MF2_ORC) roff("orc");
	else if (r_ptr->cflags2 & MF2_DRAGON) roff("dragon");
	else if (r_ptr->cflags2 & MF2_DEMON) roff("demon");
	else if (r_ptr->cflags2 & MF2_TROLL) roff("troll");
	else roff(wd_creat[msex]);

	/* calculate the integer exp part */
	i = (long)r_ptr->mexp * r_ptr->level / p_ptr->lev;

	/* calculate the fractional exp part scaled by 100, */
	/* must use long arithmetic to avoid overflow  */
	j = (((long)r_ptr->mexp * r_ptr->level % p_ptr->lev) * (long)1000 /
	     p_ptr->lev + 5) / 10;

	/* Mention the experience */
	(void)sprintf(temp, " is worth %lu.%02lu point%s",
		      i,(unsigned long)j,
		      (unsigned long)(i == 1 && j == 0 ? "" : "s"));
	roff(temp);

	/* Take account of annoying English */
	p = "th";
	i = p_ptr->lev % 10;
	if ((p_ptr->lev / 10) == 1);
	else if (i == 1) p = "st";
	else if (i == 2) p = "nd";
	else if (i == 3) p = "rd";

	/* Take account of "leading vowels" in numbers */
	q = "";
	i = p_ptr->lev;
	if ((i == 8) || (i == 11) || (i == 18)) q = "n";

	/* Mention the dependance on the player's level */
	(void)sprintf(temp, " for a%s %lu%s level character.  ",
		      q, (long)i, p);
	roff(temp);
    }


    /* Note that "group" flag should be obvious */
    if (r_ptr->cflags2 & MF2_GROUP) {
	sprintf(temp, "%s usually %s in groups.  ",
		wd_They[msex], wd_appear[msex]);
	roff(temp);
    }


    /* Spells known, if have been used against us. */
    /* Count something */
    i = 0;

    /* Describe the Breath, if any */
    k = TRUE;

    /* First, handle (and forget!) the "breath" */
    if ((rspells1 & CS1_BREATHE) ||
	(rspells2 & CS2_BREATHE) ||
	(rspells3 & CS3_BREATHE)) {

	/* Note that breathing has occurred */
	breath = TRUE;

	/* Process the "breath" and remove it */
	j = rspells1 & CS1_BREATHE;
	rspells1 &= ~CS1_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		roff(wd_They[msex]);
		roff(" can breathe ");
		k = FALSE;
	    }
	    else if (j || (rspells2 & CS2_BREATHE) || (rspells3 & CS3_BREATHE)) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i], wd_poss[msex]);
	    roff(temp);
	}

	/* Process the "breath" and remove it */
	j = rspells2 & CS2_BREATHE;
	rspells2 &= ~CS2_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		roff(wd_They[msex]);
		roff(" can breathe ");
		k = FALSE;
	    }
	    else if (j || (rspells3 & CS3_BREATHE)) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i + 32], wd_poss[msex]);
	    roff(temp);
	}

	/* Process the "breath" and remove it */
	j = rspells3 & CS3_BREATHE;
	rspells1 &= ~CS3_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		roff(wd_They[msex]);
		roff(" can breathe ");
		k = FALSE;
	    }
	    else if (j) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i + 64], wd_poss[msex]);
	    roff(temp);
	}
    }

    /* Now dump the normal spells */
    k = TRUE;

    /* Dump the normal spells */
    if (rspells1 || rspells2 || rspells3) {

	/* Note that spells exist */
	magic = TRUE;

	/* Describe the spells */
	j = rspells1 & ~CS1_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    sprintf(temp, ", and %s also", wd_are[msex]);
		    roff(temp);
		}
		else {
		    sprintf(temp, "%s %s", wd_They[msex], wd_are[msex]);
		    roff(temp);
		}
		if (l_ptr->r_cflags2 & MF2_INTELLIGENT) {
		    roff(" magical, casting spells intelligently which ");
		}
		else {
		    roff(" magical, casting spells which ");
		}
		k = FALSE;
	    }
	    else if ((j & CS_SPELLS) || rspells2 || rspells3) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i], wd_poss[msex]);
	    roff(temp);
	}

	j = rspells2 & ~CS2_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    sprintf(temp, ", and %s also", wd_are[msex]);
		    roff(temp);
		}
		else {
		    sprintf(temp, "%s %s", wd_They[msex], wd_are[msex]);
		    roff(temp);
		}
		roff(" magical, casting spells which ");
		k = FALSE;
	    }
	    else if (j || rspells3) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i + 32], wd_poss[msex]);
	    roff(temp);
	}

	j = rspells3 & ~CS3_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    sprintf(temp, ", and %s also", wd_are[msex]);
		    roff(temp);
		}
		else {
		    sprintf(temp, "%s %s", wd_They[msex], wd_are[msex]);
		    roff(temp);
		}
		roff(" magical, casting spells which ");
		k = FALSE;
	    }
	    else if (j) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i + 64], wd_poss[msex]);
	    roff(temp);
	}
    }


    /* The monster has SOME form of magic */
    if (breath || magic) {

	/* XXX Could offset by level (?) */

	/* Describe the spell frequency */
	if ((l_ptr->r_spells1 & CS1_FREQ) > 5) {	/* Could offset by level */
	    (void)sprintf(temp, "; 1 time in %lu", r_ptr->spells1 & CS1_FREQ);
	    roff(temp);
	}

	/* End this sentence */
	roff(".  ");
    }


    /* Do we know how hard they are to kill? Armor class, hit die. */
    /* hasten learning of uniques -CFT */
    if (knowarmor(r_ptr->level, l_ptr->r_kills) ||
	((r_ptr->cflags2 & MF2_UNIQUE) &&
	 knowuniqarmor(r_ptr->level, l_ptr->r_kills))) {

	(void)sprintf(temp, "%s %s an armor rating of %d",
			wd_They[msex], wd_have[msex], r_ptr->ac);
	roff(temp);
	(void)sprintf(temp, " and a%s life rating of %dd%d.  ",
		      ((r_ptr->cflags2 & MF2_MAX_HP) ? " maximized" : ""),
		      r_ptr->hd[0], r_ptr->hd[1]);
	roff(temp);
    }

    /* I wonder why this wasn't here before? -CFT */
    if (rcflags2 & MF2_BREAK_WALL) {
	roff(wd_They[msex]);
	roff(" can bore through rock.  ");
	k = FALSE;
    }

    /* Do we know how clever they are? Special abilities. */
    k = TRUE;
    j = rcflags1;

    for (i = 0; j & CM1_SPECIAL; i++) {
	if (j & (MF1_MV_INVIS << i)) {
	    j &= ~(MF1_MV_INVIS << i);
	    if (k) {
		roff(wd_They[msex]);
		roff(" can ");
		k = FALSE;
	    }
	    else if (j & CM1_SPECIAL) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    roff(desc_move[i]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    /* Do we know its special weaknesses? Most cflags2 flags. */
    k = TRUE;
    j = rcflags2;

    if (j & MF2_HURT_LITE) {
	if (k) {
	    roff(wd_They[msex]);
	    roff(" ");
	    roff(wd_are[msex]);
	    roff(" susceptible to ");
	    roff(desc_weakness[0]);
	    k = FALSE;
	}
    }

    if (j & MF2_HURT_ROCK) {
	if (k) {
	    roff(wd_They[msex]);
	    roff(" ");
	    roff(wd_are[msex]);
	    roff(" susceptible to ");
	    roff(desc_weakness[1]);
	    k = FALSE;
	}
	else {
	    roff(" and ");
	    roff(desc_weakness[1]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    /* Do we know its special weaknesses? Most cflags2 flags. */
    k = TRUE;
    flags = MF2_IM_COLD | MF2_IM_FIRE | MF2_IM_ACID;
    flags |= MF2_IM_POIS | MF2_IM_ELEC;

    /* Mega-Hack */
    for (i = 0; j & flags; i++) {
	if (j & (MF2_IM_COLD << i)) {
	    j &= ~(MF2_IM_COLD << i);
	    if (k) {
		roff(wd_They[msex]);
		roff(" ");
		roff(wd_resist[msex]);
		roff(" ");
		k = FALSE;
	    }
	    else if (j & flags) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    roff(desc_immune[i]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    if (rcflags2 & MF2_NO_INFRA) {
	roff(wd_They[msex]);
	roff(" ");
	roff(wd_are[msex]);
	roff(" cold blooded");
    }
    if (rcflags2 & MF2_CHARM_SLEEP) {
	if (rcflags2 & MF2_NO_INFRA) {
	    roff(", and");
	}
	else {
	    roff(wd_They[msex]);
	}
	roff(" cannot be charmed or slept");
    }
    if (rcflags2 & (MF2_CHARM_SLEEP | MF2_NO_INFRA)) {
	roff(".  ");
    }

    /* Do we know how aware it is? */
    if ((((int)l_ptr->r_wake * (int)l_ptr->r_wake) > r_ptr->sleep) ||
	(l_ptr->r_ignore == MAX_UCHAR) ||
	(r_ptr->sleep == 0 && l_ptr->r_kills >= 10)) {

	/* XXX Plural verbs */
	roff(wd_They[msex]);
	roff(" ");
	if (r_ptr->sleep > 200) {
	    roff("prefers to ignore");
	}
	else if (r_ptr->sleep > 95) {
	    roff("pays very little attention to");
	}
	else if (r_ptr->sleep > 75) {
	    roff("pays little attention to");
	}
	else if (r_ptr->sleep > 45) {
	    roff("tends to overlook");
	}
	else if (r_ptr->sleep > 25) {
	    roff("takes quite a while to see");
	}
	else if (r_ptr->sleep > 10) {
	    roff("takes a while to see");
	}
	else if (r_ptr->sleep > 5) {
	    roff(wd_are[msex]);
	    roff(" fairly observant of");
	}
	else if (r_ptr->sleep > 3) {
	    roff(wd_are[msex]);
	    roff(" observant of");
	}
	else if (r_ptr->sleep > 1) {
	    roff(wd_are[msex]);
	    roff(" very observant of");
	}
	else if (r_ptr->sleep != 0) {
	    roff(wd_are[msex]);
	    roff(" vigilant for");
	}
	else {
	    roff(wd_are[msex]);
	    roff(" ever vigilant for");
	}
	(void)sprintf(temp, " intruders, which %s may notice from %d feet.  ",
		      wd_they[msex], 10 * r_ptr->aaf);
	roff(temp);
    }


    /* Do we know what it might carry? */
    if (rcflags1 & (MF1_CARRY_OBJ | MF1_CARRY_GOLD)) {

	int num = (rcflags1 & CM1_TREASURE) >> CM1_TR_SHIFT;

	roff(wd_They[msex]);
	roff(" may");


	/* Only one treasure observed */
	if (num == 1) {
	    if ((r_ptr->cflags1 & CM1_TREASURE) == MF1_HAS_60) {
		roff(" sometimes");
	    }
	    else {
		roff(" often");
	    }
	}

	/* Only two treasures observed */
	else if ((num == 2) &&
		 ((r_ptr->cflags1 & CM1_TREASURE) ==
		  (MF1_HAS_60 | MF1_HAS_90))) {
	    roff(" often");
	}

	/* They have to carry it before they drop it */
	roff(" carry");


	if (r_ptr->cflags2 & MF2_SPECIAL) /* it'll tell you who has better treasure -CFT */
	    p = (num==1?"n exceptional object":" exceptional objects");
	else if (r_ptr->cflags2 & MF2_GOOD)
	    p = (num==1?" good object":" good objects");
	else
	    p = (num==1?"n object":" objects");

	/* One or more drops */
	if (num == 1) {
	    p = " an object";
	}
	else if (num == 2) {
	    roff(" one or two");
	}
	else {
	    (void)sprintf(temp, " up to %lu", (long)j);
	    roff(temp);
	}

	if (rcflags1 & MF1_CARRY_OBJ) {
	    roff(p);
	    if (rcflags1 & MF1_CARRY_GOLD) {
		roff(" or treasure");
		if (num > 1) roff("s");
	    }

	/* End this sentence */
	    roff(".  ");
	}
	else if (num != 1) roff(" treasures.  ");
	else roff(" treasure.  ");
    }


    /* We know about attacks it has used on us, and maybe the damage they do. */

    /* Count the number of "known" attacks, used for punctuation */
    for (k = j = 0; j < 4; j++) {
	if (l_ptr->r_attacks[j]) k++;
    }

    pu = r_ptr->damage;

    /* Count the number of attacks printed so far, used for punctuation */
    j = 0;

    /* Examine the actual attacks */    
    for (i = 0; *pu != 0 && i < 4; pu++, i++) {

	int att_type, att_how, d1, d2;

	/* Skip unknown attacks */
	if (!l_ptr->r_attacks[i]) continue;

	/* Extract the attack info */
	att_type = a_list[*pu].attack_type;
	att_how = a_list[*pu].attack_desc;
	d1 = a_list[*pu].attack_dice;
	d2 = a_list[*pu].attack_sides;

	/* Count the attacks as printed */
	j++;

	/* Introduce the attack description */
	if (j == 1) {
	    roff(wd_They[msex]);
	    roff(" can ");
	}
	else if (j == k) {
	    roff(", and ");
	}
	else {
	    roff(", ");
	}

	/* Hack -- only describe attacks this routine "knows" about */
	if (att_how > 23) att_how = 0;
	roff(desc_amethod[att_how]);

	/* Non-standard attacks, or attacks doing damage */
	if (att_type != 1 || (d1 && d2)) {
	    roff(" to ");

	    /* Hack -- only describe certain attacks */
	    if (att_type > 25) att_type = 0;
	    roff(desc_atype[att_type]);

	    /* Does it do damage? */
	    if (d1 && d2) {

		/* Hack -- do we KNOW the damage? */
		if (knowdamage(r_ptr->level, l_ptr->r_attacks[i], (int)d1 * (int)d2) ||
		    ((r_ptr->cflags2 & MF2_UNIQUE) &&	/* learn uniques faster -CFT */
		     knowuniqdamage(r_ptr->level, l_ptr->r_attacks[i], (int)d1 * (int)d2))) {

		    /* Hack -- Loss of experience */
		    if (att_type == 19) {
			roff(" by");
		    }

		    /* Normal attacks */
		    else {
			roff(" with damage");
		    }

		    /* Display the damage */
		    (void)sprintf(temp, " %dd%d", d1, d2);
		    roff(temp);
		}
	    }
	}
    }

    /* Finish sentence above */
    if (j) {
	roff(".");
    }

    /* Hack -- Or describe the lack of attacks */
    else if (k > 0 && l_ptr->r_attacks[0] >= 10) {
	sprintf(temp, " %s %s no physical attacks.",
		wd_They[msex], wd_have[msex]);
	roff(temp);
    }

    /* Or describe the lack of knowledge */
    else {
	sprintf(temp, "Nothing is known about %s attack.",
		wd_poss[msex]);
	roff(temp);
    }



    /* XXX Hack -- Always know the win creature. */
    if (r_ptr->cflags1 & MF1_WINNER) {
	roff("  Killing him wins the game!");
    }

    /* Finish the roff-ing */
    roff("\n");

    /* Refresh the roff-ing */
    recall_fresh();
    

    /* Hack -- Undo the "wizard memory" */
    if (wizard) *l_ptr = save_mem;


    /* Not needed for graphic recall */
    if (use_recall_win && term_recall) return (ESCAPE);

    /* Prompt for pause */
    prt("--pause--", 0, n_len + 3);

    /* Get a keypress */
    k = inkey();

    /* Return it */
    return (k);
}


/*
 * Identify a character, allow recall of monnsters
 * XXX Original command somehow lost RMHV
 */
void do_cmd_query_symbol(void)
{

    /* The turn is free */
    free_turn_flag = TRUE;
    /* Get a character, or abort */

    /* Find that character info, and describe it */

    /* Allow access to monster memory. */

}

