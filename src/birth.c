/* File: birth.c */

/* Purpose: create a player character */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * Hold the data from the previous "roll"
 */
static struct {

    u16b age;
    u16b wt;
    u16b ht;
    u16b sc;
    s16b  disarm;

    u16b stat[6];


    char history[4][60];

    player_background bg;

} prev;






/*
 * Save the current data for later
 */
static void save_prev_data()
{
    register int i;

    /* Save the stats */
    for (i = 0; i < 6; i++) {
	prev.stat[i] = (u16b) p_ptr->stats.max_stat[i];
    }

    return;
}


/*
 * Load the previous data
 */
static int load_prev_data()
{
    register int        i;

    if (!prev.stat[0]) return 0;
    for (i = 0; i < 6; i++) {
	p_ptr->stats.cur_stat[i] = prev.stat[i];
	p_ptr->stats.max_stat[i] = prev.stat[i];
	p_ptr->stats.use_stat[i] = prev.stat[i];
    }

    p_ptr->misc.ptodam = todam_adj();
    p_ptr->misc.ptohit = tohit_adj();
    p_ptr->misc.pac = toac_adj();
    prev.stat[0] = 0;
    return 1;
}








/*
 * Choose the character's sex				-JWT-	 
 */
static void choose_sex(void)
{
    char        c;

    clear_from(20);
    put_str("Choose a sex (? for Help):", 20, 2);
    put_str("m) Male       f) Female", 21, 2);

    while (1) {
	move_cursor(20, 29);
	c = inkey();
	if (c == 'm' || c == 'M') {
	    p_ptr->misc.male = TRUE;
	    c_put_str(TERM_L_BLUE, "Male", 4, 15);
	    break;
	}
	else if (c == 'f' || c == 'F') {
	    p_ptr->misc.male = FALSE;
	    c_put_str(TERM_L_BLUE, "Female", 4, 15);
	    break;
	}
	else if (c == '?') {
	    helpfile(ANGBAND_WELCOME);
	}
	else {
	    bell();
	}
    }
}


/*
 * Allows player to select a race			-JWT-	 
 */
static void choose_race(void)
{
    int                  j, k, l, m;
    char                 s, tmp_str[80];

    k = 0;
    l = 2;
    m = 21;

    clear_from(20);
    put_str("Choose a race (? for Help):", 20, 2);

    for (j = 0; j < MAX_RACES; j++) {
	(void)sprintf(tmp_str, "%c) %s", j + 'a', race[j].trace);
	put_str(tmp_str, m, l);
	l += 15;
	if (l > 70) {
	    l = 2;
	    m++;
	}
    }

    while (1) {
	move_cursor(20, 30);
	s = inkey();
	j = s - 'a';
	if ((j < MAX_RACES) && (j >= 0)) {
	    p_ptr->misc.prace = j;
	    c_put_str(TERM_L_BLUE, race[j].trace, 3, 15);
	    break;
	}
	else if (s == '?') {
	    helpfile(ANGBAND_WELCOME);
	}
	else {
	    bell();
	}
    }
}



/*
 * Gets a character class				-JWT-	 
 */
static void choose_class()
{
    int         i, j, k, l, m;
    int          cl[MAX_CLASS];
    player_class   *c_ptr;
    char         tmp_str[80], s;
    u32b       mask;

    /* Clear the display */
    clear_from(20);

    /* Prepare to list */
    i = p_ptr->misc.prace;
    k = 0;
    l = 2;
    m = 21;
    mask = 0x1;

    /* No legal choices yet */
    for (j = 0; j < MAX_CLASS; j++) cl[j] = 0;

    put_str("Choose a class (? for Help):", 20, 2);
    /* Display the legal choices */
    for (j = 0; j < MAX_CLASS; j++) {
	if (race[i].rtclass & mask) {
	    sprintf(tmp_str, "%c) %s", k + 'a', class[j].title);
	    put_str(tmp_str, m, l);
	    cl[k++] = j;
	    l += 15;
	    if (l > 70) {
		l = 2;
		m++;
	    }
	}
	mask <<= 1;
    }

    /* Get a class */
    p_ptr->misc.pclass = 0;
    while (1) {
	move_cursor(20, 31);
	s = inkey();
	j = s - 'a';
	if ((j < k) && (j >= 0)) {
	    p_ptr->misc.pclass = cl[j];
	    c_ptr = &class[p_ptr->misc.pclass];
	    clear_from(20);
	    c_put_str(TERM_L_BLUE, c_ptr->title, 5, 15);
	    break;
	}
	else if (s == '?') {
	    helpfile(ANGBAND_WELCOME);
	}
	else {
	    bell();
	}
    }
}


static void get_class()
{
    int        i;
    int                 min_value, max_value;
    int                 percent;
    char                buf[50];
    register struct misc *m_ptr;
    player_class         *c_ptr;

    c_ptr = &class[p_ptr->misc.pclass];
    change_stat(A_STR, c_ptr->madj_str);
    change_stat(A_INT, c_ptr->madj_int);
    change_stat(A_WIS, c_ptr->madj_wis);
    change_stat(A_DEX, c_ptr->madj_dex);
    change_stat(A_CON, c_ptr->madj_con);
    change_stat(A_CHR, c_ptr->madj_chr);

    for (i = 0; i < 6; i++) {
	p_ptr->stats.cur_stat[i] = p_ptr->stats.max_stat[i];
	p_ptr->stats.use_stat[i] = p_ptr->stats.max_stat[i];
    }
    p_ptr->misc.ptodam = todam_adj();           /* Real values		 */
    p_ptr->misc.ptohit = tohit_adj();
    p_ptr->misc.ptoac = toac_adj();
    p_ptr->misc.pac = 0;
    p_ptr->misc.dis_td = p_ptr->misc.ptodam;	/* Displayed values	 */
    p_ptr->misc.dis_th = p_ptr->misc.ptohit;
    p_ptr->misc.dis_tac = p_ptr->misc.ptoac;
    p_ptr->misc.dis_ac = p_ptr->misc.pac + p_ptr->misc.dis_tac;

/*
 * now set misc stats, do this after setting stats because of con_adj() for
 * hitpoints 
 */
    m_ptr = &p_ptr->misc;
    m_ptr->hitdie += c_ptr->adj_hd;
    m_ptr->mhp = con_adj() + m_ptr->hitdie;
    m_ptr->chp = m_ptr->mhp;
    m_ptr->chp_frac = 0;

/*
 * initialize hit_points array: put bounds on total possible hp,
 * only succeed if it is within 1/8 of average value
 */
    min_value = (MAX_PLAYER_LEVEL * 3 * (m_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    max_value = (MAX_PLAYER_LEVEL * 5 * (m_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;

    player_hp[0] = m_ptr->hitdie;
    do {
	for (i = 1; i < MAX_PLAYER_LEVEL; i++) {
	    player_hp[i] = randint((int)m_ptr->hitdie);
	    player_hp[i] += player_hp[i - 1];
	}
    }
    while ((player_hp[MAX_PLAYER_LEVEL - 1] < min_value) ||
	   (player_hp[MAX_PLAYER_LEVEL - 1] > max_value));

    if (peek) {
	percent = (int)(((long)player_hp[MAX_PLAYER_LEVEL - 1] * 200L) /
		(m_ptr->hitdie + ((MAX_PLAYER_LEVEL - 1) * m_ptr->hitdie)));
	sprintf(buf, "%d%% Life Rating", percent);
	msg_print(buf);
    }
    m_ptr->bth += c_ptr->mbth;
    m_ptr->bthb += c_ptr->mbthb;   /* RAK */
    m_ptr->srh += c_ptr->msrh;
    m_ptr->disarm = c_ptr->mdis + todis_adj();
    m_ptr->fos += c_ptr->mfos;
    m_ptr->stl += c_ptr->mstl;
    m_ptr->save += c_ptr->msav;
    m_ptr->expfact += c_ptr->m_exp;
}





/*
 * Returns adjusted stat -JK-
 * Algorithm by -JWT-
 *
 * Used by change_stats and auto_roller
 *
 * auto_roll is boolean and states maximum changes should be used rather
 * than random ones to allow specification of higher values to wait for
 */
static int adjust_stat(int stat_value, s16b amount, int auto_roll)
{
    register int i;

    /* Negative amounts (unused) */
    if (amount < 0) {
	for (i = 0; i > amount; i--) {

/* OK so auto_roll conditions not needed for negative amounts since stat_value
 * is always 15 at least currently!  -JK
 */
	    if (stat_value > 108) {
		stat_value--;
	    }
	    else if (stat_value > 88) {
		stat_value -= ((auto_roll ? 6 : randint(6)) + 2);
	    }
	    else if (stat_value > 18) {
		stat_value -= ((auto_roll ? 15 : randint(15)) + 5);
		if (stat_value < 18) stat_value = 18;
	    }
	    else if (stat_value > 3) {
		stat_value--;
	    }
	}
    }

    /* Positive amounts */
    else {
	for (i = 0; i < amount; i++) {
	    if (stat_value < 18) {
		stat_value++;
	    }
	    else if (stat_value < 88) {
		stat_value += ((auto_roll ? 15 : randint(15)) + 5);
	    }
	    else if (stat_value < 108) {
		stat_value += ((auto_roll ? 6 : randint(6)) + 2);
	    }
	    else if (stat_value < 118) {
		stat_value++;
	    }
	}
    }

    /* Return the result */
    return stat_value;
}


/*
 * Changes stats by given amount -JWT-
 */
static void change_stat(int stat, int amount)
{
    int max = p_ptr->stats.max_stat[stat];
    int tmp = adjust_stat(max, (s16b)amount, FALSE);
    p_ptr->stats.max_stat[stat] = tmp;
}



/*
 * Generates character's stats -JWT-
 */
static void get_stats(void)
{
    register int        i, j;
    int dice[18];

    /* Roll and verify some stats */
    while (TRUE) {

	/* Roll some dice */
	for (j = i = 0; i < 18; i++) {
	    dice[i] = randint(3 + i % 3); /* Roll 3,4,5 sided dice once each */
	    j += dice[i];
	}

	/* Verify totals */
	if (j > 42 && j < 54) break;
    }

    /* Each stat is 5 + 1d3 + 1d4 + 1d5 */
    for (i = 0; i < 6; i++) {
	j = 5 + dice[3*i] + dice[3*i+1] + dice[3*i+2];
	p_ptr->stats.max_stat[i] = j;
    }
}

/*
 * Generate all stats and modify for race.
 * Needed in a separate module so looping of character
 * selection would be allowed     -RGM- 
 */
static void get_all_stats(void)
{
    register int        j;

    player_race *rp_ptr = &race[p_ptr->misc.prace];

    get_stats();

    /* Modify the stats for "class" */
    change_stat(A_STR, rp_ptr->str_adj);
    change_stat(A_INT, rp_ptr->int_adj);
    change_stat(A_WIS, rp_ptr->wis_adj);
    change_stat(A_DEX, rp_ptr->dex_adj);
    change_stat(A_CON, rp_ptr->con_adj);
    change_stat(A_CHR, rp_ptr->chr_adj);

    /* Analyze the stats */
    for (j = 0; j < 6; j++) {
	p_ptr->stats.cur_stat[j] = p_ptr->stats.max_stat[j];
	p_ptr->stats.use_stat[j] = modify_stat(j, p_ptr->stats.mod_stat[j]);
    }

    p_ptr->misc.srh = rp_ptr->srh;
    p_ptr->misc.bth = rp_ptr->bth;
    p_ptr->misc.bthb = rp_ptr->bthb;
    p_ptr->misc.fos = rp_ptr->fos;
    p_ptr->misc.stl = rp_ptr->stl;
    p_ptr->misc.save = rp_ptr->bsav;
    p_ptr->misc.hitdie = rp_ptr->bhitdie;
    p_ptr->misc.lev = 1;
    p_ptr->misc.ptodam = todam_adj();
    p_ptr->misc.ptohit = tohit_adj();
    p_ptr->misc.ptoac = 0;
    p_ptr->misc.pac = toac_adj();
    p_ptr->misc.expfact = rp_ptr->b_exp;
    p_ptr->flags.see_infra = rp_ptr->infra;
}


#ifdef AUTOROLLER

/*
 * used for auto-roller.  Just "put_stats()" w/o the extra info -CFT
 */
static void put_auto_stats()
{
    register int i;
    vtype        buf;

    /* Put the stats */
    for (i = 0; i < 6; i++) {
	cnv_stat(p_ptr->stats.use_stat[i], buf);
	put_str(stat_names[i], 2 + i, 61);
	put_str(buf, 2 + i, 66);
	if (p_ptr->stats.max_stat[i] > p_ptr->stats.cur_stat[i]) {
	    cnv_stat(p_ptr->stats.max_stat[i], buf);
	    put_str(buf, 2 + i, 73);
	}
    }
}

#endif


/*
 * Will print the history of a character			-JWT-	 
 */
static void put_history()
{
    register int        i;

    clear_from(15);
    
    put_str("Character Background", 15, 27);
    for (i = 0; i < 4; i++) {
	put_str(p_ptr->misc.history[i], i + 16, 10);
    }
}


/*
 * Get the racial history, determines social class	-RAK-
 * Assumptions:	Each race has init history beginning at
 * (race-1)*3+1.  All history parts are in ascending order
 */

static void get_history(void)
{
    int                      hist_idx, cur_idx, test_roll, flag;
    register int             start_pos, end_pos, cur_len;
    int                      line_ctr, new_start = 0, social_class;
    char                     history_block[240];
    player_background		*bp_ptr;

    /* Get a block of history text */
    /* Special race */
    if (p_ptr->misc.prace == 8) {
	hist_idx = 1;
    }

    /* Special race */
    else if (p_ptr->misc.prace > 8) {
	hist_idx = 2 * 3 + 1;
    }

    /* Normal races */
    else {
	hist_idx = p_ptr->misc.prace * 3 + 1;
    }

    history_block[0] = '\0';
    social_class = randint(4);
    cur_idx = 0;

    /* Process the history */
    while (hist_idx >= 1) {
	for (flag = FALSE; !flag; ) {
	    if (background[cur_idx].chart == hist_idx) {
		test_roll = randint(100);
		while (test_roll > background[cur_idx].roll) cur_idx++;
		bp_ptr = &background[cur_idx];
		(void)strcat(history_block, bp_ptr->info);
		social_class += bp_ptr->bonus - 50;
		if (hist_idx > bp_ptr->next) cur_idx = 0;
		hist_idx = bp_ptr->next;
		flag = TRUE;
	    }
	    else {
		cur_idx++;
	    }
	}
    }

    /* clear the previous history strings */
    for (hist_idx = 0; hist_idx < 4; hist_idx++) {
	p_ptr->misc.history[hist_idx][0] = '\0';
    }

    /* Process block of history text for pretty output	 */
    start_pos = 0;
    end_pos = strlen(history_block) - 1;
    line_ctr = 0;
    flag = FALSE;
    while (history_block[end_pos] == ' ') end_pos--;

    for (flag = FALSE; !flag; ) {
	while (history_block[start_pos] == ' ') start_pos++;
	cur_len = end_pos - start_pos + 1;
	if (cur_len > 60) {
	    cur_len = 60;
	    while (history_block[start_pos + cur_len - 1] != ' ') cur_len--;
	    new_start = start_pos + cur_len;
	    while (history_block[start_pos + cur_len - 1] == ' ') cur_len--;
	}
	else {
	    flag = TRUE;
	}

	(void)strncpy(p_ptr->misc.history[line_ctr],
		&history_block[start_pos], cur_len);
	p_ptr->misc.history[line_ctr][cur_len] = '\0';
	line_ctr++;
	start_pos = new_start;
    }

    /* Verify social class */
    if (social_class > 100) social_class = 100;
    else if (social_class < 1) social_class = 1;

    /* Save the social class */
    p_ptr->misc.sc = social_class;
}


static void get_prev_history()
{
    register int        i;

    background->info = prev.bg.info;
    background->roll = prev.bg.roll;
    background->chart = prev.bg.chart;
    background->next = prev.bg.next;
    background->bonus = prev.bg.bonus;
    p_ptr->misc.sc = prev.sc;

    for (i = 0; i < 4; i++)
	strncpy(p_ptr->misc.history[i], prev.history[i], 60);
}

static void set_prev_history()
{
    prev.bg.info = background->info;
    prev.bg.roll = background->roll;
    prev.bg.chart = background->chart;
    prev.bg.next = background->next;
    prev.bg.bonus = background->bonus;
    prev.sc = p_ptr->misc.sc;

    (void)strncpy(prev.history[0], p_ptr->misc.history[0], 60);
    (void)strncpy(prev.history[1], p_ptr->misc.history[1], 60);
    (void)strncpy(prev.history[2], p_ptr->misc.history[2], 60);
    (void)strncpy(prev.history[3], p_ptr->misc.history[3], 60);

    return;
}


/*
 * Computes character's age, height, and weight
 */
static void get_ahw(void)
{
    register int        i;

    i = p_ptr->misc.prace;
    
    /* Calculate the starting age */
    p_ptr->misc.age = race[i].b_age + randint((int)race[i].m_age);

    /* Calculate the height/weight for males */
    if (p_ptr->misc.male) {
	p_ptr->misc.ht = randnor((int)race[i].m_b_ht, (int)race[i].m_m_ht);
	p_ptr->misc.wt = randnor((int)race[i].m_b_wt, (int)race[i].m_m_wt);
    }

    /* Calculate the height/weight for females */
    else {
	p_ptr->misc.ht = randnor((int)race[i].f_b_ht, (int)race[i].f_m_ht);
	p_ptr->misc.wt = randnor((int)race[i].f_b_wt, (int)race[i].f_m_wt);
    }
    p_ptr->misc.disarm += race[i].b_dis;
}


static void set_prev_ahw()
{
    prev.age = p_ptr->misc.age;
    prev.wt = p_ptr->misc.wt;
    prev.ht = p_ptr->misc.ht;
    prev.disarm = p_ptr->misc.disarm;

    return;
}


static void get_prev_ahw()
{
    p_ptr->misc.age = prev.age;
    p_ptr->misc.wt = prev.wt;
    p_ptr->misc.ht = prev.ht;
    p_ptr->misc.disarm = prev.disarm;
    prev.age = prev.wt = prev.ht = prev.disarm = 0;
}


/*
 * Given a stat value, return a monetary value,
 * which affects the amount of gold a player has. 
 */
static int monval(int i)
{
    return (5 * (i - 10));
}

/*
 * Get the player's starting money
 */
static void get_money(void)
{
    register int        gold;

    /* Social Class adj */
    gold = p_ptr->misc.sc * 6 + randint(25) + 325;

    /* Stat adj */
    gold -= monval(p_ptr->stats.max_stat[A_STR]);
    gold -= monval(p_ptr->stats.max_stat[A_INT]);
    gold -= monval(p_ptr->stats.max_stat[A_WIS]);
    gold -= monval(p_ptr->stats.max_stat[A_CON]);
    gold -= monval(p_ptr->stats.max_stat[A_DEX]);

    /* Charisma adj */
    gold += monval(a_ptr[A_CHR]);

    /* Minimum 80 gold */
    if (gold < 80) gold = 80;

    /* She charmed the banker into it! -CJS- */
    /* She slept with the banker.. :) -GDH-  */
    if (!p_ptr->misc.male) gold += 50;

    /* Save the gold */
    p_ptr->misc.au = gold;
}



void rerate()
{
    int         min_value, max_value, i, percent;
    char        buf[50];
    struct misc *m_ptr = &p_ptr->misc;

    min_value = (MAX_PLAYER_LEVEL * 3 * (m_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    max_value = (MAX_PLAYER_LEVEL * 5 * (m_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    player_hp[0] = m_ptr->hitdie;
    do {
	for (i = 1; i < MAX_PLAYER_LEVEL; i++) {
	    player_hp[i] = randint((int)m_ptr->hitdie);
	    player_hp[i] += player_hp[i - 1];
	}
    }
    while ((player_hp[MAX_PLAYER_LEVEL - 1] < min_value) ||
	   (player_hp[MAX_PLAYER_LEVEL - 1] > max_value));

    percent = (int)(((long)player_hp[MAX_PLAYER_LEVEL - 1] * 200L) /
		(m_ptr->hitdie + ((MAX_PLAYER_LEVEL - 1) * m_ptr->hitdie)));

    sprintf(buf, "%d%% Life Rating", percent);
    calc_hitpoints();
    prt_stat_block();
    msg_print(buf);
}




/*
 * Create a character.  Then wait for a moment.
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise. -JWT-
 */
void player_birth()
{
    char		c;

    player_class	*cp_ptr;
    player_race		*rp_ptr;

#ifdef AUTOROLLER

    register int	i;
    u32b		auto_round = 0;
    int			stat[6];
    int			autoroll = 0;
    int			msstat = 0; /* Max autoroll w/ look for -SAC */
    int			stat_idx = 0;

    char		inp[60];

#endif
    int previous_exists = 0;	/* flag to prevent prev from garbage values */

    town_seed = random();	/* Change random seeds for new characters -CWS */
    randes_seed = random();

    put_character();

    /* Choose a sex */
    choose_sex();

    /* Choose a race */
    choose_race();

    /* Choose a class */
    choose_class();

    /* Access the race/class */
    cp_ptr = &class[p_ptr->misc.pclass];
    rp_ptr = &race[p_ptr->misc.prace];


#ifdef AUTOROLLER

/*
 * This auto-roller stolen from a post on rec.games.moria, which I belive was
 * taken from druid moria 5.something.  If this intrudes on someone's
 * copyright, take it out, and someone let me know -CFT 
 */

    /* Prompt for it */
    put_str("Do you want to use automatic rolling? (? for Help) ", 20, 2);

    /* allow multiple key entry, so they can ask for help and still get back to this menu... -CFT */
    while (1) {
	move_cursor(20, 52);
	c = inkey();
	if (c == '?') helpfile(ANGBAND_WELCOME);
	else if (strchr("ynYN", c)) break;
    }


    /* Prepare the autoroller */
    if ((c == 'Y') || (c == 'y')) {

	autoroll = TRUE;

	clear_from(15);
	put_str("Enter minimum attribute for: ", 15, 2);

	/* Check the stats */
	for (i = 0; i < 6; i++) {

		clear_from(16 + i);

	    switch (i) {
	      case 0:
		stat_idx = A_STR;
		msstat = cp_ptr->madj_str + rp_ptr->str_adj;
		break;
	      case 1:
		stat_idx = A_INT;
		msstat = cp_ptr->madj_int + rp_ptr->int_adj;
		break;
	      case 2:
		stat_idx = A_WIS;
		msstat = cp_ptr->madj_wis + rp_ptr->wis_adj;
		break;
	      case 3:
		stat_idx = A_DEX;
		msstat = cp_ptr->madj_dex + rp_ptr->dex_adj;
		break;
	      case 4:
		stat_idx = A_CON;
		msstat = cp_ptr->madj_con + rp_ptr->con_adj;
		break;
	      case 5:
		stat_idx = A_CHR;
		msstat = cp_ptr->madj_chr + rp_ptr->chr_adj;
		break;
	    } /* switch */

	    msstat = adjust_stat(17, msstat, TRUE);

	    sprintf(inp, "%-12s (Max of %2d): ", stat_name[i], msstat);
	    put_str(inp, 16 + i, 5);

	    do {

		inp[0] = '\000';
		get_string(inp, 16 + i, 32, 3);

		/* Parse the input */
		stat[stat_idx] = atoi(inp);

		/* have return give a stat of 3 */
		if (inp[0] == '\015' || inp[0] == '\012' || inp[0] == '\000')
		    stat[stat_idx] = 3;

		/* Use negative numbers to avoid "max stat" setting */
		if (stat[stat_idx] < 0) {
		    stat[stat_idx] = (-stat[stat_idx]);
		    if (stat[stat_idx] > msstat) msstat = stat[stat_idx];
		}
	    } while (stat[stat_idx] > msstat || stat[stat_idx] < 3);
	} /* for i 0 - 5 */

	/* Dump results */
	Term_fresh();
    }

#endif				   /* AUTOROLLER - main setup code */


    /* Actually Generate */
    do {

	/* Clear the old data */
	clear_from(9);

#ifdef AUTOROLLER

	if (autoroll) for (i = 2; i < 6; i++) erase_line(i, 30);

	/* Start of AUTOROLLing loop */
	do {
#endif
	    get_all_stats();
	    get_class();

#ifdef AUTOROLLER
	    if (autoroll) {
		put_auto_stats();

	    /* Advance the round */
	    auto_round++;

		sprintf(inp, "auto-rolling round #%lu.", (long)auto_round);
		put_str(inp, 20, 2);

#if defined(NICE)

	    /* Wait 1/10 second per roll */
	    delay(10);

#endif

	    /* Make sure they see everything */
	    Term_fresh();

	    } else
		put_stats();
	} while ((autoroll) &&

	    /* Break if "happy" */
		 ((stat[A_STR] > p_ptr->stats.cur_stat[A_STR]) ||
		  (stat[A_INT] > p_ptr->stats.cur_stat[A_INT]) ||
		  (stat[A_WIS] > p_ptr->stats.cur_stat[A_WIS]) ||
		  (stat[A_DEX] > p_ptr->stats.cur_stat[A_DEX]) ||
		  (stat[A_CON] > p_ptr->stats.cur_stat[A_CON]) ||
		  (stat[A_CHR] > p_ptr->stats.cur_stat[A_CHR]))

#if (defined (unix) || defined(ATARI_ST)) /* CFT's if/elif/else    */
		 && (!check_input(1)));	  /* unix needs flush here */
#elif (defined(MSDOS) || defined(VMS))
	&&(!kbhit()));
    if (kbhit())
	    flush();
#else
    );
#endif				   /* character checks */
#endif				   /* AUTOROLLER main looping section */

	/* Calculate Common stuff */
       get_history();
       get_ahw();

	calc_bonuses();
	put_history();
	put_misc1();
	clear_from(20);

	/* Input loop */
	do {

	    /* Prepare a prompt */
	    if (previous_exists) {
		put_str("Hit space: Reroll, ^P: Previous or ESC: Accept: ", 20, 2);
		move_cursor(20, 50);
	    } else {
		put_str("Hit space: Reroll, or ESC: Accept: ", 20, 2);
		move_cursor(20, 37);
	    }

	    /* Prompt and get a command */
	    c = inkey();

	    if ((previous_exists) && (c == CTRL('P'))) {
		previous_exists = FALSE;
		if (load_prev_data()) {
		    get_prev_history();
		    get_prev_ahw();
		    put_history();
		    put_misc1();
		    calc_bonuses();
		    put_stats();
		    clear_from(20);
		}
	    } else if ((c != ' ') && (c != ESCAPE))	/* Prolly better way to do this */
		bell();
	} while ((c != ' ') && (c != ESCAPE));

	/* Not going to waste space w/ a check here. So ESC takes a little longer. -SAC */
	save_prev_data();
	set_prev_history();
	set_prev_ahw();
	previous_exists = TRUE;
    } while (c == ' ');

    get_money();
    put_stats();
    put_misc2();
    put_misc3();
    
    /* Get a name */
    get_name();
    msg_print(NULL);

/* This delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.
 */
    pause_exit(23, PLAYER_EXIT_PAUSE);
}

/* Pauses for user response before returning		-RAK-	 */
/* NOTE: Delay is for players trying to roll up "perfect"	 */
/* characters.  Make them wait a bit.			 */
void pause_exit(int prt_line, int delay)
{
    char dummy;

    prt("[Press any key to continue, or Q to exit.]", prt_line, 10);
    dummy = inkey();
    if (dummy == 'Q') {
	erase_line(prt_line, 0);
#ifndef MSDOS			   /* PCs are slow enough as is  -dgk */
	if (delay > 0)
	    (void)sleep((unsigned)delay);
#else
    /* prevent message about delay unused */
	dummy = delay;
#endif
#ifdef MACINTOSH
	enablefilemenu(FALSE);
	exit_game();
	enablefilemenu(TRUE);
#else
	exit_game();
#endif
    }
    erase_line(prt_line, 0);
}
