/* File: misc4.c */ 

/* Purpose: misc dungeon/player code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

#define BLANK_LENGTH	24
static char blank_string[] = "                        ";

#define COL_STAT 0

#define ROW_RACE    1
#define ROW_CLASS   2
#define ROW_TITLE   3

#define ROW_LEVEL   (new_screen_layout ? 4 : 12)
#define ROW_EXP     (new_screen_layout ? 5 : 13)

#define ROW_STAT    (new_screen_layout ? 7 : 5)

#define ROW_AC      (new_screen_layout ? 14 : 17)
#define ROW_CURHP   (new_screen_layout ? 15 : 16)
#define ROW_MAXHP   (new_screen_layout ? 16 : 15)
#define ROW_MANA    (new_screen_layout ? 17 : 14)
#define ROW_GOLD    18

#define ROW_WINNER  (new_screen_layout ? 19 : 20)
#define ROW_EQUIPPY (new_screen_layout ? 20 : 4)
#define ROW_CUT     21
#define ROW_STUN    22


static cptr stat_names[] = {
    "STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};

static cptr stat_names_reduced[] = {
    "Str: ", "Int: ", "Wis: ", "Dex: ", "Con: ", "Chr: "
};




/*
 * Print character info in given row, column   -RAK-
 * the longest title is 13 characters, so only pad to 13
 * XXX There has got to be a cleaner way
 */
void prt_field(cptr info, int row, int col)
{
    char tmp[16];
    sprintf(tmp, "%-13.13s", info);
    c_put_str(TERM_L_BLUE, tmp, row, col);
}




/*
 * Converts stat num into a string (six chars plus a space)
 */
void cnv_stat(int my_stat, char *out_val)
{
    register u16b stat = my_stat;
    register int    part1, part2;

    if (stat > 18) {
	part1 = 18;
	part2 = stat - 18;
	if (part2 >= 220)
	    (void)sprintf(out_val, "%2d/*** ", part1);
	else if (part2 >= 100)
	    (void)sprintf(out_val, "%2d/%03d ", part1, part2);
	else
	    (void)sprintf(out_val, " %2d/%02d ", part1, part2);
    }
    else {
	(void)sprintf(out_val, "%6d ", stat);
    }
}


/*
 * Print character stat in given row, column -RAK-
 */
void prt_stat(int stat)
{
    char tmp[32];

	put_str(stat_names[stat], ROW_STAT + stat, COL_STAT);
	cnv_stat(p_ptr->use_stat[stat], tmp);
	c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
}




/*
 * Adjustment for wisdom/intelligence -JWT-
 */
int stat_adj(int stat)
{
    register int value;

    value = p_ptr->use_stat[stat];
    if (value > 228) return (20);
    if (value > 218) return (18);
    if (value > 198) return (16);
    if (value > 188) return (15);
    if (value > 178) return (14);
    if (value > 168) return (13);
    if (value > 158) return (12);
    if (value > 148) return (11);
    if (value > 138) return (10);
    if (value > 128) return (9);
    if (value > 118) return (8);
    if (value > 117) return (7);
    if (value > 107) return (6);
    if (value > 87) return (5);
    if (value > 67) return (4);
    if (value > 17) return (3);
    if (value > 14) return (2);
    if (value > 7) return (1);
    return (0);
}


/*
 * Adjustment for charisma -RAK-
 * Percent decrease or increase in price of goods		
 */
int chr_adj()
{
    register int charisma;

    charisma = p_ptr->use_stat[A_CHR];

    if (charisma > 217) return (80);
    if (charisma > 187) return (86);
    if (charisma > 147) return (88);
    if (charisma > 117) return (90);
    if (charisma > 107) return (92);
    if (charisma > 87) return (94);
    if (charisma > 67) return (96);
    if (charisma > 18) return (98);

    switch (charisma) {
	case 18:
	    return (100);
	case 17:
	    return (101);
	case 16:
	    return (102);
	case 15:
	    return (103);
	case 14:
	    return (104);
	case 13:
	    return (106);
	case 12:
	    return (108);
	case 11:
	    return (110);
	case 10:
	    return (112);
	case 9:
	    return (114);
	case 8:
	    return (116);
	case 7:
	    return (118);
	case 6:
	    return (120);
	case 5:
	    return (122);
	case 4:
	    return (125);
	case 3:
	    return (130);
	default:
	    return (140);
      }
}


/*
 * Returns a character's adjustment to hit points -JWT-
 */
int con_adj()
{
    register int con;

    con = p_ptr->use_stat[A_CON];

    if (con < 7) return (con - 7);
    if (con < 17) return (0);
    if (con < 18) return (1);
    if (con < 94) return (2);
    if (con < 117) return (3);
    if (con < 119) return (4);
    if (con < 128) return (5);
    if (con < 138) return (6);
    if (con < 158) return (7);
    if (con < 168) return (8);
    if (con < 178) return (9);
    if (con < 188) return (10);
    if (con < 198) return (11);
    if (con < 208) return (12);
    if (con < 228) return (13);
    return (14);
}


/*
 * Determine a "title" for the player
 */
cptr title_string()
{
    cptr p;

    if (p_ptr->lev < 1) {
	p = "Babe in arms";
    }
    else if (p_ptr->lev <= MAX_PLAYER_LEVEL) {
	p = player_title[p_ptr->pclass][p_ptr->lev - 1];
    }
    else if (p_ptr->male) {
	p = "**KING**";
    }
    else {
	p = "**QUEEN**";
    }

    return p;
}


/*
 * Prints title of character				-RAK-	
 */
void prt_title()
{
    prt_field(title_string(), ROW_TITLE, COL_STAT);
}


/*
 * Prints level	-RAK-
 */
void prt_level()
{
    char tmp[32];
    sprintf(tmp, "%6d", p_ptr->lev);
    c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_STAT + 6);
}


/*
 * Prints players current mana points. -RAK-
 */
void prt_cmana()
{
    char tmp[32];
    byte color;

    sprintf(tmp, "%6d", p_ptr->cmana);
	color = TERM_L_GREEN;

    c_put_str(color, tmp, ROW_MANA, COL_STAT + 6);
}


/*
 * Prints Max hit points -RAK-
 */
void prt_mhp()
{
    char tmp[32];
    sprintf(tmp, "%6d", p_ptr->mhp);
    c_put_str(TERM_L_GREEN, tmp, ROW_MAXHP, COL_STAT + 6);
}


/*
 * Prints players current hit points -RAK-
 */
void prt_chp()
{
    char tmp[32];
    byte color;
    sprintf(tmp, "%6d", p_ptr->chp);
	color = TERM_L_GREEN;

    c_put_str(color, tmp, ROW_CURHP, COL_STAT + 6);
}


/*
 * prints current AC -RAK-
 */
void prt_pac()
{
    char tmp[32];
    sprintf(tmp, "%6d", p_ptr->dis_ac);
    c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_STAT + 6);
}


/*
 * Prints current gold -RAK-
 */
void prt_gold()
{
    char tmp[32];

    sprintf(tmp, "%9ld", (long)p_ptr->au);
    c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_STAT + 3);
}


/*
 * Prints depth in stat area				-RAK-	 
 */
void prt_depth()
{
    char depths[32];

    if (!dun_level) {
	(void)strcpy(depths, "Town");
    }
    else if (depth_in_feet) {
	(void)sprintf(depths, "%d ft", dun_level * 50);
    }
    else {
	(void)sprintf(depths, "Lev %d", dun_level);
    }

    /* Right-Adjust the "depth", but clear old values */
    prt(format("%7s", depths), 23, 70);
}


/*
 * Prints status of hunger				-RAK-	 
 */
void prt_hunger()
{
    if (PY_WEAK & p_ptr->status) {
	c_put_str(TERM_ORANGE, "Weak  ", 23, 0);
    }
    else if (PY_HUNGRY & p_ptr->status) {
	c_put_str(TERM_YELLOW, "Hungry", 23, 0);
    }
    else {
	put_str("      ", 23, 0);
    }
}


/*
 * Prints Blind status					-RAK-	 
 */
void prt_blind(void)
{
    if (PY_BLIND & p_ptr->status) {
	put_str("Blind", 23, 7);
    }
    else {
	put_str("     ", 23, 7);
    }
}


/*
 * Prints Confusion status				-RAK-	 
 */
void prt_confused(void)
{
    if (PY_CONFUSED & p_ptr->status) {
	c_put_str(TERM_ORANGE, "Confused", 23, 13);
    }
    else {
	put_str("        ", 23, 13);
    }
}


/*
 * Prints Fear status					-RAK-	 
 */
void prt_afraid()
{
    if (PY_FEAR & p_ptr->status) {
	c_put_str(TERM_ORANGE, "Afraid", 23, 22);
    }
    else {
	put_str("      ", 23, 22);
    }
}


/*
 * Prints Poisoned status				-RAK-	 
 */
void prt_poisoned(void)
{
    if (PY_POISONED & p_ptr->status) {
	c_put_str(TERM_L_GREEN, "Poisoned", 23, 29);
    }
    else {
	put_str("        ", 23, 29);
    }
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status	-RAK-
 * Display is always exactly 10 characters wide (see below)
 */
void prt_state(void)
{
    /* Default to white */
    byte attr = TERM_WHITE;

    /* Default to ten spaces */
    char text[16] = "          ";

    /* Turn off the flag */
    p_ptr->status &= ~PY_REPEAT;

    /* Most important info is paralyzation */
    if (p_ptr->paralysis > 1) {
	attr = TERM_RED;
	strcpy(text, "Paralyzed!");
    }

    /* Then comes resting */
    else if (PY_REST & p_ptr->status) {
	if (p_ptr->rest > 0) {
	    (void)sprintf(text, "Rest %-5d", p_ptr->rest);
	}
	else if (p_ptr->rest == -1) {
	    (void)sprintf(text, "Rest *****");
	}
	else if (p_ptr->rest == -2) {
	    (void)sprintf(text, "Rest &&&&&");
	}
    }

    /* Then comes repeating */
    else if (command_rep > 0) {

	/* Hack -- we need to redraw this */
	p_ptr->status |= PY_REPEAT;

	/* Hack -- ignore searching info */
	/* if (PY_SEARCH & p_ptr->status) ... */


	    (void)sprintf(text, "Repeat %3d", command_rep); /* "repeat 999" is 10 characters */
    }

    else if (PY_SEARCH & p_ptr->status) {
	strcpy(text, "Searching ");
    }

    /* Display the info (or blanks) */
    c_put_str(attr, text, 23, 38);
}


/*
 * Prints the speed of a character.			-CJS-
 */
void prt_speed()
{
    int i = p_ptr->speed;

    if (PY_SEARCH & p_ptr->status) i--;	/* Search mode. */


    if (i > 2)
	put_str("Extremely Slow", 23, 49);
    else if (i == 2)
	put_str("Very Slow     ", 23, 49);
    else if (i == 1)
	put_str("Slow          ", 23, 49);
    else if (i == 0)
	put_str("              ", 23, 49);
    else if (i == -1)
	put_str("Fast          ", 23, 49);
    else if (i == -2)
	put_str("Very Fast     ", 23, 49);
    else if (i == -3)
	put_str("Extremely Fast", 23, 49);
    else if (i == -4)
	put_str("Deadly Speed  ", 23, 49);
    else
	put_str("Light Speed   ", 23, 49);
}


void prt_study()
{
    p_ptr->status &= ~PY_STUDY;
    if (p_ptr->new_spells != 0) {
	put_str("Study", 23, 64);
    }
    else {
	put_str("     ", 23, 64);
    }
}


void prt_cut()
{
    int c = p_ptr->cut;

    if (c > 900)
	c_put_str(TERM_L_RED,"Mortal wound", ROW_CUT, 0);
    else if (c > 300)
	c_put_str(TERM_RED,"Deep gash   ", ROW_CUT, 0);
    else if (c > 200)
	c_put_str(TERM_RED,"Severe cut  ", ROW_CUT, 0);
    else if (c > 45)
	c_put_str(TERM_ORANGE,"Nasty cut   ", ROW_CUT, 0);
    else if (c > 15)
	c_put_str(TERM_ORANGE,"Bad cut     ", ROW_CUT, 0);
    else if (c > 5)
	c_put_str(TERM_YELLOW,"Light cut   ", ROW_CUT, 0);
    else if (c > 0)
	c_put_str(TERM_YELLOW,"Graze       ", ROW_CUT, 0);
    else
	put_str("            ", ROW_CUT, 0);
}



void prt_stun(void)
{
    int s = p_ptr->stun;

    if (!p_ptr->resist_sound) {
	if (s > 100) {
	    c_put_str(TERM_RED, "Knocked out ", ROW_STUN, 0);
	}
	else if (s > 50) {
	    c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, 0);
	}
	else if (s > 0) {
	    c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, 0);
	}
	else {
	    put_str("            ", ROW_STUN, 0);
	}
    }
}


/*
 * Prints winner/wizard status on display			-RAK-	
 */
void prt_winner(void)
{
    if (wizard) {
	put_str("Wizard", ROW_WINNER, 0);
    }
    else if (total_winner) {
	put_str("Winner", ROW_WINNER, 0);
    }
    else {
	put_str("       ", ROW_WINNER, 0);
    }
}



/*
 * Display the "equippy chars"
 * EQUIPMENT CHARACTER HANDLER  - DGK
 */
void prt_equippy_chars(void)
{                                        
    int i, j;                              
    inven_type *i_ptr;   
                  
    char out_val[2];


    /* Analyze the pack */
    for (j = 0, i = INVEN_WIELD; i < INVEN_TOTAL; i++, j++) {

	/* Get the item */
	i_ptr = &inventory[i];                           
	
	if (!equippy_chars || (i_ptr->tval == TV_NOTHING)) out_val[0] = ' ';

	else                

	/* Extract the symbol */
	out_val[0] = (int)(i_ptr->tchar);                

	/* Hack -- terminate the string */
	out_val[1] = '\0';

	/* Display the item symbol */
	put_str(out_val, ROW_EQUIPPY, j);
    }
}



/*
 * Prints character-screen info				-RAK-	 
 */
void prt_stat_block()
{

    register int          i;

    prt_field(race[p_ptr->prace].trace, ROW_RACE, COL_STAT);
    prt_field(class[p_ptr->pclass].title, ROW_CLASS, COL_STAT);

    prt_field(title_string(), ROW_TITLE, COL_STAT);

    for (i = 0; i < 6; i++) prt_stat(i);

    prt_num("LEV", (int)p_ptr->lev, ROW_LEVEL, COL_STAT);

    prt_lnum("EXP", p_ptr->exp, ROW_EXP, COL_STAT);

    prt_num("MNA", p_ptr->cmana, ROW_MANA, COL_STAT);

    prt_num("MHP", p_ptr->mhp, ROW_MAXHP, COL_STAT);

    prt_num("CHP", p_ptr->chp, ROW_CURHP, COL_STAT);

    prt_chp(); /* this will overwrite hp, in color, if needed. -CFT */

    prt_num("AC ", p_ptr->dis_ac, ROW_AC, COL_STAT);

    prt_lnum("AU ", p_ptr->au, ROW_GOLD, COL_STAT);

    prt_winner();
    prt_cut();
    prt_stun();
    prt_study();

    if ((PY_HUNGRY | PY_WEAK) & p_ptr->status)
	prt_hunger();
    if (PY_BLIND & p_ptr->status)
	prt_blind();
    if (PY_CONFUSED & p_ptr->status)
	prt_confused();
    if (PY_FEAR & p_ptr->status)
	prt_afraid();
    if (PY_POISONED & p_ptr->status)
	prt_poisoned();
    if ((PY_SEARCH | PY_REST) & p_ptr->status)
	prt_state();

    /* if speed non zero, print it, modify speed if Searching */
    if (p_ptr->speed - ((PY_SEARCH & status) >> 8) != 0)
	prt_speed();

	prt_equippy_chars();
}


/*
 * Draws entire screen					-RAK-	
 */
void draw_cave(void)
{
    /* Redraw everything */
    clear_screen();
    prt_stat_block();
    prt_map();
    prt_depth();
}




/*
 * Cut the player
 */
void cut_player(int c)
{
    c += p_ptr->cut;
    p_ptr->cut = c;

    if (c > 5000) {
	msg_print("You have been given a mortal wound.");
    }
    else if (c > 900) {
	msg_print("You have been given a deep gash.");
    }
    else if (c > 200) {
	msg_print("You have been given a severe cut.");
    }
    else if (c > 100) {
	msg_print("You have been given a nasty cut.");
    }
    else if (c > 50) {
	msg_print("You have been given a bad cut.");
    }
    else if (c > 10) {
	msg_print("You have been given a light cut.");
    }
    else if (c > 0) {
	msg_print("You have been given a graze.");
    }
}


/*
 * Stun the player
 */
void stun_player(int s)
{
    int t;

    if (p_ptr->resist_sound) return;

	t = p_ptr->stun;
	p_ptr->stun += s;
	s = p_ptr->stun;

    if (s > 100) {
	msg_print("You have been knocked out.");
	    if (t == 0) {
		p_ptr->ptohit -= 20;
		p_ptr->ptodam -= 20;
		p_ptr->dis_th -= 20;
		p_ptr->dis_td -= 20;
	    } else if (t <= 50) {
		p_ptr->ptohit -= 15;
		p_ptr->ptodam -= 15;
		p_ptr->dis_th -= 15;
		p_ptr->dis_td -= 15;
	    }
    }
    else if (s > 50) {
	msg_print("You've been heavily stunned.");
	    if (t == 0) {
		p_ptr->ptohit -= 20;
		p_ptr->ptodam -= 20;
		p_ptr->dis_th -=20;
		p_ptr->dis_td -=20;
	    } else if (t <= 50) {
		p_ptr->ptohit -= 15;
		p_ptr->ptodam -= 15;
		p_ptr->dis_th -= 15;
		p_ptr->dis_td -= 15;
	    }
    }
    else if (s > 0) {
	msg_print("You've been stunned.");
	    if (t == 0) {
		p_ptr->ptohit -= 5;
		p_ptr->ptodam -= 5;
		p_ptr->dis_th -= 5;
		p_ptr->dis_td -= 5;
	    }
	}
}


/*
 * Modify a stat by a "modifier", return new value
 */
u16b modify_stat(int stat, int amount)
{
    register int    loop, i;
    register u16b tmp_stat;

    tmp_stat = p_ptr->cur_stat[stat];

    loop = ABS(amount);

    for (i = 0; i < loop; i++) {
	if (amount > 0) {
	    if (tmp_stat < 18) tmp_stat++;
	    else tmp_stat += 10;
	}
	else {
	    if (tmp_stat > 27) tmp_stat -= 10;
	    else if (tmp_stat > 18) tmp_stat = 18;
	    else if (tmp_stat > 3) tmp_stat--;
	}
    }

    return tmp_stat;
}



/*
 * Set the value of the stat which is actually used.	 -CJS-
 * Also, make sure to recalculate any changed values.
 */
void set_use_stat(int stat)
{
    /* Calculate the "total" stat value */
    p_ptr->use_stat[stat] = modify_stat(stat, p_ptr->mod_stat[stat]);

    /* Calculate various effects */
    if (stat == A_STR) {
	p_ptr->status |= PY_STR_WGT;
	calc_bonuses();
    }
    else if (stat == A_DEX) {
	calc_bonuses();
    }
    else if (stat == A_CON) {
	calc_hitpoints();
    }
    else if (stat == A_INT && class[p_ptr->pclass].spell == MAGE) {
	calc_spells(A_INT);
	calc_mana(A_INT);
    }
    else if (stat == A_WIS && class[p_ptr->pclass].spell == PRIEST) {
	calc_spells(A_WIS);
	calc_mana(A_WIS);
    }
}


/*
 * Increases a stat by one randomized level		-RAK-	 
 */
int inc_stat(int stat)
{
    register int tmp_stat, gain;

    /* Hmmm -- First, restore the stat */
    res_stat(stat);

    /* Then augment the current/max stat */
    tmp_stat = p_ptr->cur_stat[stat];

    /* Cannot go above 18/100 */
    if (tmp_stat < 118) {
	if (tmp_stat < 18) {	   
	    gain = randint(2);		/* let's be able to monitor the increase -CWS */
	    tmp_stat += gain;
	}
	else if (tmp_stat < 116) {
	    /* stat increases by 1/6 to 1/3 of difference from max */
	    gain = ((118 - tmp_stat) / 2 + 3) >> 1;
	    tmp_stat += randint(gain) + gain / 2;
	    if (tmp_stat > 117) tmp_stat = 117;
	}
	else {
	    tmp_stat++;
	}

	p_ptr->cur_stat[stat] = tmp_stat;
	if (tmp_stat > p_ptr->max_stat[stat]) {
	    p_ptr->max_stat[stat] = tmp_stat;
	}
	set_use_stat(stat);
	prt_stat(stat);
	return TRUE;
    }

    return FALSE;
}



/* 
 * Decreases a stat by one randomized level		-RAK-	
 */
int dec_stat(int stat)
{
    int tmp_stat, loss;

    tmp_stat = p_ptr->cur_stat[stat];
    if (tmp_stat > 3) {
	if (tmp_stat < 19)
	    tmp_stat--;
	else if (tmp_stat < 117) {
	    loss = (((118 - tmp_stat) >> 1) + 1) >> 1;
	    tmp_stat += -randint(loss) - loss;
	    if (tmp_stat < 18)
		tmp_stat = 18;
	} else
	    tmp_stat--;

	p_ptr->cur_stat[stat] = tmp_stat;
	set_use_stat(stat);
	prt_stat(stat);
	return TRUE;
    } else
    return FALSE;
}
int ruin_stat(register int stat)
{
    register int tmp_stat;

    tmp_stat = p_ptr->cur_stat[stat];
    if (tmp_stat > 3) {
	if (tmp_stat > 6) {
	    if (tmp_stat < 19) {
		tmp_stat -= 3;
	    } else {
		tmp_stat /= 2;
		if (tmp_stat < 18)
		    tmp_stat = 18;
	    }
	} else
	    tmp_stat--;

	p_ptr->cur_stat[stat] = tmp_stat;
	p_ptr->max_stat[stat] = tmp_stat;
	set_use_stat(stat);
	prt_stat(stat);
	return TRUE;
    } else
	return FALSE;
}


/*
 * Restore a stat.  Return TRUE only if this actually makes a difference. 
 */
int res_stat(int stat)
{
    /* Restore */
    if (p_ptr->cur_stat[stat] != p_ptr->max_stat[stat]) {
	p_ptr->cur_stat[stat] = p_ptr->max_stat[stat];
	set_use_stat(stat);
	prt_stat(stat);
	return TRUE;
    }

    /* Nothing to restore */
    return FALSE;
}


/*
 * Returns a character's adjustment to hit.		 -JWT-	 
 */
int tohit_adj()
{
    register int total, stat;

    stat = p_ptr->use_stat[A_DEX];
    if      (stat <   4)  total = -3;
    else if (stat <   6)  total = -2;
    else if (stat <   8)  total = -1;
    else if (stat <  16)  total =  0;
    else if (stat <  17)  total =  1;
    else if (stat <  18)  total =  2;
    else if (stat <  69)  total =  3;
    else if (stat < 108)  total =  4; /* 18/51 to 18/89 -CFT */
    else if (stat < 118)  total =  5; /* 18/90 to 18/99 -CFT */
    else if (stat < 128)  total =  6; /* 18/100 to 18/109 -CFT */
    else if (stat < 138)  total =  7;
    else if (stat < 148)  total =  8;
    else if (stat < 158)  total =  9;
    else if (stat < 168)  total = 10;
    else if (stat < 178)  total = 11;
    else if (stat < 188)  total = 12;
    else if (stat < 198)  total = 13;
    else if (stat < 218)  total = 14;
    else if (stat < 228)  total = 15;
    else total = 17;
    stat = p_ptr->use_stat[A_STR];
    if      (stat <   4)  total -= 3;
    else if (stat <   5)  total -= 2;
    else if (stat <   7)  total -= 1;
    else if (stat <  18)  total -= 0;
    else if (stat <  88)  total += 1; /* 18 to 18/69 -CFT */
    else if (stat <  98)  total += 2; /* 18/70 to 18/79 -CFT */
    else if (stat < 108)  total += 3; /* 18/80 to 18/89 -CFT */
    else if (stat < 118)  total += 4; /* 18/90 to 18/99 -CFT */
    else if (stat < 128)  total += 5; /* 18/100 to 18/109 -CFT */
    else if (stat < 138)  total += 6;
    else if (stat < 148)  total += 7;
    else if (stat < 158)  total += 8;
    else if (stat < 168)  total += 9;
    else if (stat < 178)  total +=10;
    else if (stat < 188)  total +=11;
    else if (stat < 198)  total +=12;
    else if (stat < 218)  total +=13;
    else if (stat < 228)  total +=14;
    else total += 16;
    return (total);
}


/*
 * Returns a character's adjustment to armor class	 -JWT-	 
 */
int toac_adj(void)
{
    register int stat;

    stat = p_ptr->use_stat[A_DEX];
    if      (stat <   4)  return(-4);
    else if (stat ==  4)  return(-3);
    else if (stat ==  5)  return(-2);
    else if (stat ==  6)  return(-1);
    else if (stat <  15)  return( 0);
    else if (stat <  18)  return( 1);
    else if (stat <  58)  return( 2); /* 18 to 18/49 -CFT */
    else if (stat <  98)  return( 3); /* 18/50 to 18/79 -CFT */
    else if (stat < 108)  return( 4); /* 18/80 to 18/89 -CFT */
    else if (stat < 118)  return( 5); /* 18/90 to /99 -CFT */
    else if (stat < 128)  return( 6); /* /100 to /109 -CFT */
    else if (stat < 138)  return( 7);
    else if (stat < 148)  return( 8);
    else if (stat < 158)  return( 9);
    else if (stat < 168)  return(10);
    else if (stat < 178)  return(11);
    else if (stat < 188)  return(12);
    else if (stat < 198)  return(13);
    else if (stat < 218)  return(14);
    else if (stat < 228)  return(15);
    else                  return(17);
}


/*
 * Returns a character's adjustment to disarm		 -RAK-	 
 */
int todis_adj(void)
{
    register int stat;

    stat = p_ptr->use_stat[A_DEX];
    if      (stat <=  3)  return(-8);
    else if (stat ==  4)  return(-6);
    else if (stat ==  5)  return(-4);
    else if (stat ==  6)  return(-2);
    else if (stat ==  7)  return(-1);
    else if (stat <  13)  return( 0);
    else if (stat <  16)  return( 1);
    else if (stat <  18)  return( 2);
    else if (stat <  58)  return( 4); /* 18 to 18/49 -CFT */
    else if (stat <  88)  return( 5); /* 18/50 to 18/69 -CFT */
    else if (stat < 108)  return( 6); /* 18/70 to 18/89 -CFT */
    else if (stat < 118)  return( 7); /* 18/90 to 18/99 -CFT */
    else                  return( 8); /* 18/100 and over -CFT */
}


/*
 * Returns a character's adjustment to damage	 -JWT-
 */
int todam_adj(void)
{
    register int stat;

    stat = p_ptr->use_stat[A_STR];
    if      (stat <   4)  return(-2);
    else if (stat <   5)  return(-1);
    else if (stat <  16)  return( 0);
    else if (stat <  17)  return( 1);
    else if (stat <  18)  return( 2);
    else if (stat <  88)  return( 3); /* 18 to 18/69 -CFT */
    else if (stat <  98)  return( 4); /* 18/70 to 18/79 -CFT */
    else if (stat < 108)  return( 5); /* 18/80 to 18/89 -CFT */
    else if (stat < 118)  return( 5); /* 18/90 to 18/99 -CFT */
    else if (stat < 128)  return( 6); /* 18/100 to /109 -CFT */
    else if (stat < 138)  return( 7);
    else if (stat < 148)  return( 8);
    else if (stat < 158)  return( 9);
    else if (stat < 168)  return(10);
    else if (stat < 178)  return(11);
    else if (stat < 188)  return(12);
    else if (stat < 198)  return(13);
    else if (stat < 218)  return(14);
    else if (stat < 228)  return(16);
    else                  return(20);
}


/*
 * Print long number with header at given row, column
 */
static void prt_lnum(cptr header, s32b num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%s%9ld", header, (long)num);
    put_str(out_val, row, col);
}

/*
 * Print number with header at given row, column	-RAK-
 */
static void prt_num(cptr header, int num, int row, int col)
{
    char out_val[32];

    (void)sprintf(out_val, "%s   %6d", header, num);
    put_str(out_val, row, col);
}


/*
 * Print long number at given row, column */
static void prt_long(s32b num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%9ld", (long)num);
    put_str(out_val, row, col);
}


/*
 * Print number at given row, column	-RAK-	 */
static void prt_int(int num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%6d", num);
    put_str(out_val, row, col);
}


/*
 * Prints the following information on the screen.	-JWT-	 
 */
void put_character()
{


    clear_screen();

    put_str("Name        :", 2, 1);
    put_str("Sex         :", 3, 1);
    put_str("Race        :", 4, 1);
    put_str("Class       :", 5, 1);

    if (character_generated) {
    c_put_str(TERM_L_BLUE, p_ptr->name, 2, 15);
    c_put_str(TERM_L_BLUE, (p_ptr->male ? "Male" : "Female"), 4, 15);
    c_put_str(TERM_L_BLUE, race[p_ptr->prace].trace, 3, 15);
    c_put_str(TERM_L_BLUE, class[p_ptr->pclass].title, 5, 15);
    }
}



/*
 * Prints some information on the screen	-JWT-
 */
void put_stats()
{

    int          i, temp;
    vtype        buf;

    /* Display the stats */
    for (i = 0; i < 6; i++) {

	cnv_stat(p_ptr->use_stat[i], buf);
	put_str(stat_names[i], 2 + i, 61);
	put_str(buf, 2 + i, 66);
	if (p_ptr->max_stat[i] > p_ptr->cur_stat[i]) {
	    /* this looks silly, but it happens because modify_stat() only
	     * looks at cur_stat -CFT */
	    temp = p_ptr->cur_stat[i];
	    p_ptr->cur_stat[i] = p_ptr->max_stat[i];
	    cnv_stat (modify_stat(i,p_ptr->mod_stat[i]), buf);
	    p_ptr->cur_stat[i] = temp; /* DON'T MS2_FORGET! -CFT */
	    put_str(buf, 2 + i, 73);
	}
    }
    prt_num("+ To Hit    ", p_ptr->dis_th, 9, 1);
    prt_num("+ To Damage ", p_ptr->dis_td, 10, 1);
    prt_num("+ To AC     ", p_ptr->dis_tac, 11, 1);
    prt_num("  Total AC  ", p_ptr->dis_ac, 12, 1);
}


/*
 * Used to pass color info around
 */
static byte likert_color;

/*
 * Returns a "rating" of x depending on y			-JWT-
 */
cptr likert(int x, int y)
{
    if ((x/y) < 0) {
	likert_color = TERM_RED;
	return ("Very Bad");
    }

    switch ((x / y)) {
      case 0:
      case 1:
	likert_color = TERM_RED;
	return ("Bad");
      case 2:
	likert_color = TERM_RED;
	return ("Poor");
      case 3:
      case 4:
	likert_color = TERM_YELLOW;
	return ("Fair");
      case 5:
	likert_color = TERM_YELLOW;
	return ("Good");
      case 6:
	likert_color = TERM_YELLOW;
	return ("Very Good");
      case 7:
      case 8:
	likert_color = TERM_L_GREEN;
	return ("Excellent");
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
	likert_color = TERM_L_GREEN;
	return ("Superb");
      case 14:
      case 15:
      case 16:
      case 17:
	likert_color = TERM_L_GREEN;
	return ("Heroic");
      default:
	likert_color = TERM_L_GREEN;
	return ("Legendary");
    }
}


/*
 * Prints age, height, weight, and Social Class			-JWT-
 */
void put_misc1()
{

    prt_num("Age          ", (int)p_ptr->age, 2, 32);
    prt_num("Height       ", (int)p_ptr->ht, 3, 32);
    prt_num("Weight       ", (int)p_ptr->wt, 4, 32);
    prt_num("Social Class ", (int)p_ptr->sc, 5, 32);
}


/*
 * Prints the following information on the screen.	-JWT-
 *
 * For this to look right, the following should be spaced the
 * same as in the prt_lnum code... -CFT
 */
void put_misc2()
{


    prt_num("Level      ", (int)p_ptr->lev, 9, 28);
    prt_lnum("Experience ", p_ptr->exp, 10, 28);
    prt_lnum("Max Exp    ", p_ptr->max_exp, 11, 28);
    if (p_ptr->lev >= MAX_PLAYER_LEVEL) {
	char                buf[40];

	sprintf(buf, "%s%9s", "Exp to Adv.", "****");
	put_str(buf, 12, 28);
    }
    else {
	prt_lnum("Exp to Adv.", (s32b) (player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100),
	    12, 28);
    }
    prt_lnum("Gold       ", p_ptr->au, 13, 28);
    prt_num("Max Hit Points ", p_ptr->mhp, 9, 52);
    prt_num("Cur Hit Points ", p_ptr->chp, 10, 52);
    prt_num("Max Mana       ", p_ptr->mana, 11, 52);
    prt_num("Cur Mana       ", p_ptr->cmana, 12, 52);
}


/*
 * Prints ratings on certain abilities			-RAK-
 *
 * This code is "repeated" elsewhere to "dump" a character sheet.
 */
void put_misc3()
{
    int			xbth, xbthb, xfos, xsrh;
    int			xstl, xdis, xsave, xdev;
    char                xinfra[32];
    cptr		desc;

    /* XXX XXX XXX Skill with current weapon */
    xbth = p_ptr->bth + p_ptr->ptohit * BTH_PLUS_ADJ +
	(class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
    xbthb = p_ptr->bthb + p_ptr->ptohit * BTH_PLUS_ADJ +
	   (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);

    /* this results in a range from 0 to 29 */
    xfos = 40 - p_ptr->fos;
    if (xfos < 0) xfos = 0;
    xsrh = p_ptr->srh;
    /* this results in a range from 0 to 9 */
    xstl = p_ptr->stl + 1;
    xdis = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) +
	   (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
    xsave = p_ptr->save + stat_adj(A_WIS) +
	    (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
    xdev = p_ptr->save + stat_adj(A_INT) +
	   (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

    /* Infravision string */
    (void)sprintf(xinfra, "%d feet", p_ptr->see_infra * 10);

    /* Clear it */
    clear_from(14);

    put_str("(Miscellaneous Abilities)", 15, 25);

    put_str("Fighting    :", 16, 1);
    desc=likert(xbth, 12);
    c_put_str(likert_color, desc, 16, 15);

    put_str("Bows/Throw  :", 17, 1);
    desc=likert(xbthb, 12);
    c_put_str(likert_color, desc, 17, 15);

    put_str("Saving Throw:", 18, 1);
    desc=likert(xsave, 6);
    c_put_str(likert_color, desc, 18, 15);

    put_str("Stealth     :", 16, 28);
    desc=likert(xstl, 1);
    c_put_str(likert_color, desc, 16, 42);

    put_str("Disarming   :", 17, 28);
    desc=likert(xdis, 8);
    c_put_str(likert_color, desc, 17, 42);

    put_str("Magic Device:", 18, 28);
    desc=likert(xdev, 6);
    c_put_str(likert_color, desc, 18, 42);

    put_str("Perception  :", 16, 55);
    desc=likert(xfos, 3);
    c_put_str(likert_color, desc, 16, 69);

    put_str("Searching   :", 17, 55);
    desc=likert(xsrh, 6);
    c_put_str(likert_color, desc, 17, 69);

    put_str("Infra-Vision:", 18, 55);
    put_str(xinfra, 18, 69);
}


/*
 * Used to display the character on the screen.		-RAK-
 */
void display_player()
{
    put_character();
    put_stats();
    put_misc1();
    put_misc2();
    put_misc3();
}




/*
 * Gets a name for the character			-JWT-
 */
void get_name()
{
    char tmp[100];

    strcpy(tmp, p_ptr->name);

    /* Prompt and ask */
    prt("Enter your player's name  [press <RETURN> when finished]", 21, 2);


#ifdef MACINTOSH
/*
 * Force player to give a name, would be nice to get name from chooser (STR
 * -16096), but that name might be too long 
 */
    while (!get_string(p_ptr->name, 2, 15, 15) || p_ptr->name[0] == 0);
#else
    if (!get_string(p_ptr->name, 2, 15, 15) || p_ptr->name[0] == 0) {
	strcpy(p_ptr->name, tmp);
	put_str(tmp, 2, 15);
    }
#endif

    /* Erase the prompt, etc */
    clear_from(20);

#ifdef MACINTOSH
/* Use the new name to set save file default name. */
    initsavedefaults();
#endif
}


/*
 * Display character, allow name change or character dump. -JWT-
 */
void change_name()
{
    register char c;
    register int  flag;
    vtype         temp;

    display_player();

    for (flag = FALSE; !flag; ) {

	prt("<f>ile character description. <c>hange character name.", 21, 2);
	c = inkey();
	switch (c) {

	  case 'c':
	    get_name();
	    flag = TRUE;
	    break;

	  case 'f':
#ifdef MACINTOSH
	/* On mac, file_character() gets filename with std file dialog. */
	    if (mac_file_character()) flag = TRUE;
#else
	    prt("File name:", 0, 0);
	    if (get_string(temp, 0, 10, 60) && temp[0]) {
		 flag = TRUE;
	    }
#endif
	    break;

	  case ESCAPE:
	  case ' ':
	  case '\n':
	  case '\r':
	    flag = TRUE;
	    break;

	  default:
	    bell();
	    break;
	}
    }
}




/*
 * Describe number of remaining charges.		-RAK-	
 */
void inven_item_charges(int item_val)
{
    register int rem_num;
    vtype        out_val;

    if (known2_p(&inventory[item_val])) {
	rem_num = inventory[item_val].pval;
	(void)sprintf(out_val, "You have %d charges remaining.", rem_num);
	msg_print(out_val);
    }
}


/*
 * Describe amount of item remaining.			-RAK-	
 */
void inven_item_describe(int i_idx)
{
    inven_type *i_ptr;
    bigvtype tmp_str;

    i_ptr = &inventory[i_idx];

    /* Get a description */
    i_ptr->number--;
    objdes(tmp_str, i_ptr, TRUE);
    i_ptr->number++;

    /* Print a message */
    (void)sprintf(out_val, "You have %s.", tmp_str);
    msg_print(out_val);
}




/*
 * Computes current weight limit.			-RAK-
 */
int weight_limit(void)
{
    register s32b weight_cap;

    /* Factor in strength */
    weight_cap = (long)p_ptr->use_stat[A_STR] * (long)PLAYER_WEIGHT_CAP;

    /* Hack -- large players can carry more */
    weight_cap += (long)p_ptr->wt;

    /* Nobody can carry more than 300 pounds */
    if (weight_cap > 3000L) weight_cap = 3000L;

    /* Return the result */
    return ((int)weight_cap);
}



/*
 * this code must be identical to the inven_carry() code below
 */
int inven_check_num(inven_type *t_ptr)
{
    register int i;

    /* If there is an empty space, we are fine */
    if (inven_ctr < INVEN_WIELD) return TRUE;

    else if (t_ptr->sval >= ITEM_SINGLE_STACK_MIN)

    /* Scan every possible match */
    for (i = 0; i < inven_ctr; i++) {

	    if (inventory[i].tval == t_ptr->tval &&
		inventory[i].sval == t_ptr->subval &&
	/* make sure the number field doesn't overflow */
		((int)inventory[i].number + (int)t_ptr->number < 256) &&
	/* they always stack (sval < 192), or else they have same pval */
		((t_ptr->sval < ITEM_GROUP_MIN) || (inventory[i].pval == t_ptr->pval))
	/* only stack if both or neither are identified */
		&& (inven_aware_p(&inventory[i]) == inven_aware_p(t_ptr)))
		return TRUE;
    }

    /* And there was no room in the inn... */
    return FALSE;
}

/*
 * Add an item to the players inventory, and return the slot used.
 * for a description if needed.
 *
 * Okay, here's my inven_carry() function (from misc2.c, I think).  Just
 * replace the existing inven_carry() with this one.
 *
 * Items will sort into place, with mage spellbooks coming first for mages,
 * rangers, and rogues.  Also, this will make Tenser's book sort after all
 * the mage books except Raals, instead of in the middle of them (which
 * always seemed strange to me). -CFT 
 *
 * -RAK- this code must be identical to the "inven_check_num()" code above.
 */
int inven_carry(inven_type *i_ptr)
{
    register int         slot, i;
    register int         typ, subt;
    register inven_type *j_ptr;
    int                  known1p, always_known1p;

    int                  tval_tmp;  /* used to make magic books before pray books if magicuser */
    int                  stacked = FALSE;

    typ = i_ptr->tval;
    subt = i_ptr->sval;
    known1p = inven_aware_p(i_ptr);
    always_known1p = (flavor_p(i_ptr) == -1);

    if (inven_ctr >= INVEN_WIELD) /* sanity checking to prevent the inv from */
	inven_ctr = INVEN_WIELD;  /* running over the equipment list -CWS */

/*
 * to prevent nasty losses of objects, we first look through entire inven for
 * a place to stack, w/o assuming the inventory is sorted. -CFT 
 */
    if (subt >= ITEM_SINGLE_STACK_MIN) {

    for (slot = 0; slot < inven_ctr; slot++) {

	/* Access that inventory item */
	j_ptr = &inventory[slot];

	    if (j_ptr->tval == typ &&
		j_ptr->sval == subt &&
	/* make sure the number field doesn't overflow */
		((int)j_ptr->number + (int)i_ptr->number < 256) &&
	/* they always stack (sval < 192), or else they have same pval */
		((subt < ITEM_GROUP_MIN) || (j_ptr->pval == i_ptr->pval))
	/* only stack if both or neither are identified */
		&& (inven_aware_p(&inventory[slot]) == known1p)) {
		stacked = TRUE;	   /* note that we did process the item -CFT */

	    /* Add together the item counts */
	    j_ptr->number += i_ptr->number;

	    /* Hack -- maintain the MINIMUM cost */
	/* if player bought at bargin price, then make sure he can't sell back
	 * for normal value.  This is unfair, since it robs the value from items,
	 * but it does prevent the player from "milking" the stores for cash.
	 */
	    if (j_ptr->cost > i_ptr->cost) j_ptr->cost = i_ptr->cost;
		break;
	    } /* if it stacks here */
	} /* for loop */
    } /* if it stacks, try to stack it... */

    if (!stacked) {
    /* either it doesn't stack anyway, or it didn't match anything in the inventory.
     * Now try to insert. -CFT */

	for (slot = 0;; slot++) {
	    j_ptr = &inventory[slot];

	/* For items which are always known1p, i.e. never have a 'color',
	 * insert them into the inventory in sorted order.  
	 */
	    if ((typ == TV_PRAYER_BOOK) && (class[p_ptr->pclass].spell == MAGE))
		typ = TV_MAGIC_BOOK - 1;
	/* sort is in descending, so this will be immediately after magic books.
	 * It helps that there is no tval that uses this. -CFT
	 */
	    tval_tmp = j_ptr->tval;
	    if ((tval_tmp == TV_PRAYER_BOOK) &&
		(class[p_ptr->pclass].spell == MAGE))
		tval_tmp = TV_MAGIC_BOOK - 1;
	/* sort is in descending, so this will be immediately after magic books.
	 * It helps that there is no tval that uses this. -CFT
	 */
	    if ((typ > tval_tmp) ||     /* sort by desc tval */
		(always_known1p &&      /* if always known, then sort by inc level, */
		 (typ == tval_tmp) &&	/* then by inc sval */
		 ((i_ptr->level < j_ptr->level) ||
	     ((i_ptr->level == j_ptr->level) && (subt < j_ptr->sval))))) {

    /* Structure slide (make room) */
    for (i = inven_ctr; i > slot; i--) {
	inventory[i] = inventory[i-1];
    }

    /* Structure copy to insert the new item */
    inventory[slot] = *i_ptr;

    /* One more item present now */
    inven_ctr++;

		break;
	    }
	}
    }
    /* Increase the weight, prepare to redraw */
    inven_weight += i_ptr->number * i_ptr->weight;

    /* Remember to re-calculate bonuses */
    p_ptr->status |= PY_STR_WGT;

    /* Say where it went */
    return slot;
}


/*
 * Returns spell chance of failure for spell		-RAK-	 
 */
int spell_chance(int spell)
{
    register spell_type *s_ptr;
    register int         chance;
    register int         stat;
    int                  minfail;

    s_ptr = &magic_spell[p_ptr->pclass - 1][spell];
    chance = s_ptr->sfail - 3 * (p_ptr->lev - s_ptr->slevel);
    stat = (class[p_ptr->pclass].spell == MAGE) ? A_INT : A_WIS;
    chance -= 3 * (stat_adj(stat) - 1);
    if (s_ptr->smana > p_ptr->cmana) {
	chance += 5 * (s_ptr->smana - p_ptr->cmana);
    }
    
    switch (stat_adj(stat)) {
      case 0:
	minfail = 50;
	break;			   /* I doubt can cast spells with stat this low, anyways... */
      case 1:
	minfail = 12;
	break;			   /* 8-14 stat */
      case 2:
	minfail = 8;
	break;			   /* 15-17 stat */
      case 3:
	minfail = 5;
	break;			   /* 18-18/49 stat */
      case 4:
	minfail = 4;
	break;			   /* 18/50-18/69 */
      case 5:
	minfail = 4;
	break;			   /* 18/70-18/89 */
      case 6:
	minfail = 3;
	break;			   /* 18/90-18/99 */
      case 7:
	minfail = 3;
	break;			   /* 18/100 */
      case 8:
      case 9:
      case 10:
	minfail = 2;
	break;			   /* 18/101 - /130 */
      case 11:
      case 12:
	minfail = 2;
	break;			   /* /131 - /150 */
      case 13:
      case 14:
	minfail = 1;
	break;			   /* /151 - /170 */
      case 15:
      case 16:
	minfail = 1;
	break;			   /* /171 - /200 */
      default:
	minfail = 0;
	break;			   /* > 18/200 */
    }

     /* only mages/priests can get best chances... */
    if ((minfail < 5) && (p_ptr->pclass != 1) && (p_ptr->pclass != 2)) {
	minfail = 5;
    }

    /* Big prayer penalty for edged weapons  -DGK */
    /* XXX So, like, switch weapons and then pray? */
    if (p_ptr->pclass == 2) {
	register inven_type *i_ptr;
	i_ptr = &inventory[INVEN_WIELD];
	if ((i_ptr->tval == TV_SWORD) || (i_ptr->tval == TV_POLEARM)) {
	    if ((i_ptr->flags3 & TR3_BLESSED) == 0) {
		chance += 25;
	    }
	}
    }

    if (chance > 95) return (95);

    if (chance < minfail) return (minfail);

    return chance;
}


/*
 * Print list of spells					-RAK-
 *
 * if (nonconsec < 0) spells numbered consecutively from
 * 'a' to 'a'+num, for example, when learning spells.
 *
 * Otherwise, spells numbered by offset from nonconsec, such as when
 * asking which spell to cast 
 */
void print_spells(int *spell, int num, int comment, int nonconsec)
{
    register int         i, j;
    register spell_type *s_ptr;
    int                  col, offset;
    cptr		 p;
    char                 spell_char;
    vtype                out_val;

    col = (comment ? 22 : 31);

    offset = (class[p_ptr->pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);
    erase_line(1, col);
    put_str("Name", 1, col + 5);
    put_str("Lv Mana Fail", 1, col + 35);

    /* only show the first 22 choices */
    if (num > 22) num = 22;

    for (i = 0; i < num; i++) {
	j = spell[i];
	s_ptr = &magic_spell[p_ptr->pclass - 1][j];
	if (comment == FALSE) {
	    p = "";
	}
	else if (j >= 32 ?
		 ((spell_forgotten2 & (1L << (j - 32))) != 0) :
		 ((spell_forgotten & (1L << j)) != 0)) {
	    p = " forgotten";
	}
	else if (j >= 32 ?
		 ((spell_learned2 & (1L << (j - 32))) == 0) :
		 ((spell_learned & (1L << j)) == 0)) {
	    p = " unknown";
	}
	else if (j >= 32 ?
		 ((spell_worked2 & (1L << (j - 32))) == 0) :
		 ((spell_worked & (1L << j)) == 0)) {
	    p = " untried";
	}
	else {
	    p = "";
	}

        /* determine whether or not to leave holes in character choices,
         * nonconsec -1 when learning spells, consec offset>=0 when asking which
         * spell to cast  */
	if (nonconsec == -1) {
	    spell_char = 'a' + i;
	}
	else {
	    spell_char = 'a' + j - nonconsec;
	}

	(void)sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char,
		      spell_names[j + offset], s_ptr->slevel, s_ptr->smana,
		      spell_chance(j), p);
	prt(out_val, 2 + i, col);
    }
}


/*
 * Let the player pick a spell from a book.
 * Attempt to maintain some spell naming consistency
 * Returns spell pointer				-RAK-
 */
int get_spell(int *spell, int num, int *sn, int *sc, cptr prompt, int first_spell)
{
    register spell_type *s_ptr;
    int                  flag, redraw, offset, i;
    char                 choice;
    vtype                out_str, tmp_str;

    *sn = (-1);
    flag = FALSE;
    (void)sprintf(out_str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s",
	   spell[0] + 'a' - first_spell, spell[num - 1] + 'a' - first_spell,
		  prompt);
    redraw = FALSE;
    offset = (class[p_ptr->pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    /* Get a spell from the user */
    while (!flag && get_com(out_str, &choice)) {

	if (isupper((int)choice)) {

	    *sn = choice - 'A' + first_spell;

	    /* verify that this is in spell[], at most 22 entries in spell[] */
	    for (i = 0; i < num; i++) {
		if (*sn == spell[i]) break;
	    }
	    if (i == num) {
		*sn = (-2);
	    }
	    else {
		s_ptr = &magic_spell[p_ptr->pclass - 1][*sn];
		(void)sprintf(tmp_str, "Cast %s (%d mana, %d%% fail)?",
			      spell_names[*sn + offset], s_ptr->smana,
			      spell_chance(*sn));
		if (get_check(tmp_str)) {
		    flag = TRUE;
		}
		else {
		    *sn = (-1);
		}
	    }
	}

	else if (islower((int)choice)) {

	    *sn = choice - 'a' + first_spell;

	    /* verify that this is in spell[], at most 22 entries in spell[] */
	    for (i = 0; i < num; i++) {
		if (*sn == spell[i]) break;
	    }
	    if (i == num) {
		*sn = (-2);
	    }
	    else {
		flag = TRUE;
	    }
	}
	else if (choice == '*') {
	    /* only do this drawing once */
	    if (!redraw) {
		save_screen();
		redraw = TRUE;
		print_spells(spell, num, FALSE, first_spell);
	    }
	}
	else if (isalpha((int)choice)) {
	    *sn = (-2);
	}
	else {
	    *sn = (-1);
	    bell();
	}

	if (*sn == -2) {
	    sprintf(tmp_str, "You don't know that %s.",
		(class[p_ptr->pclass].spell == MAGE) ? "spell" : "prayer");
	    msg_print(tmp_str);
	}
    }
    
    if (redraw) restore_screen();

    erase_line(MSG_LINE, 0);
    if (flag) *sc = spell_chance(*sn);

    /* Return TRUE if choice made */
    return (flag);
}



/*
 * Increases hit points and level			-RAK-	 
 */
static void gain_level(void)
{
    vtype               out_val;

    p_ptr->lev++;
    (void)sprintf(out_val, "Welcome to level %d.", (int)p_ptr->lev);
    msg_print(out_val);
    calc_hitpoints();
    prt_level();
    prt_title();

    if (class[p_ptr->pclass].spell == MAGE) {
	calc_spells(A_INT);
	calc_mana(A_INT);
    }
    else if (class[p_ptr->spell == PRIEST) {
	calc_spells(A_WIS);
	calc_mana(A_WIS);
    }
}


/*
 * Prints experience					-RAK-	 
 */
void prt_experience()
{
    char out_val[100];

    if (p_ptr->exp > MAX_EXP) p_ptr->exp = MAX_EXP;

    if (p_ptr->lev < MAX_PLAYER_LEVEL) {

	while ((player_exp[p_ptr->lev-1] * p_ptr->expfact / 100)
	       <= p_ptr->exp && (p_ptr->lev < MAX_PLAYER_LEVEL)) {

	    gain_level();

	    /* Level was actually gained, not restored */
	    if (p_ptr->exp > p_ptr->max_exp) {

		/* level was actually gained, not restored:
		 * This "300" is arbitrary, but it makes human ages work okay,
		 * and I chose the other racial age adjustments based on this as well -CFT
		 */
		p_ptr->age += randint((u16b)class[p_ptr->pclass].age_adj *
				       (u16b)race[p_ptr->prace].m_age)/300;
	    }
	}
    }

    if (p_ptr->exp > p_ptr->max_exp) p_ptr->max_exp = p_ptr->exp;

    (void) sprintf(out_val, "%8ld", (long)p_ptr->exp);
    put_str(out_val, 13, COL_STAT+4);
}



/*
 * Calculate the players hit points
 */
void calc_hitpoints()
{
    register int          hitpoints;
    register s32b        value;

    /* Calculate hitpoints */
    hitpoints = player_hp[p_ptr->lev - 1] + (con_adj() * p_ptr->lev);

    /* Hack -- minimum hitpoints: always give at least one point per level + 1 */
    if (hitpoints < (p_ptr->lev + 1)) {
	hitpoints = p_ptr->lev + 1;
    }

    /* Factor in the hero / superhero values */
    if (p_ptr->status & PY_HERO) hitpoints += 10;
    if (p_ptr->status & PY_SHERO) hitpoints += 30;

    /* mhp can equal zero while character is being created */
    if (p_ptr->mhp && (hitpoints != p_ptr->mhp)) {

        /* change current hit points proportionately to change of mhp */
        /* divide first to avoid overflow, little loss of accuracy */
	value = (((long)p_ptr->chp << 16) + p_ptr->chp_frac) / p_ptr->mhp;
	value = value * hitpoints;
	p_ptr->chp = value >> 16;
	p_ptr->chp_frac = value & 0xFFFF;

	/* Save the new max-hitpoints */
	p_ptr->mhp = hitpoints;

	/* can't print hit points here, may be in store or inventory mode */
	/* Remember to redisplay the hitpoints */
	p_ptr->status |= PY_HP;
    }
}


/*
 * Inserts a string into a string
 */
void insert_str(char *object_str, cptr mtc_str, cptr insert)
{
    register int   i, mtc_len;
    int            obj_len;
    char          *bound, *pc;
    register char *temp_obj;
    cptr           temp_mtc;
    char           out_val[80];

    /* Extract some lengths */
    mtc_len = strlen(mtc_str);
    obj_len = strlen(object_str);

    bound = object_str + obj_len - mtc_len;

    for (pc = object_str; pc <= bound; pc++) {
	temp_obj = pc;
	temp_mtc = mtc_str;
	for (i = 0; i < mtc_len; i++)
	    if (*temp_obj++ != *temp_mtc++) break;
	if (i == mtc_len) break;
    }

    if (pc <= bound) {
	(void)strncpy(out_val, object_str, (int)(pc - object_str));
	out_val[(int)(pc - object_str)] = '\0';
	if (insert)
	    (void)strcat(out_val, insert);
	(void)strcat(out_val, (char *)(pc + mtc_len));
	(void)strcpy(object_str, out_val);
    }
}



/*
 * Lets certain players enter wizard mode after a disclaimer...	-JEW-
 */
int enter_wiz_mode(void)
{
    register int answer = FALSE;

    if (!can_be_wizard) return FALSE;

    if (!noscore) {
	msg_print("Wizard mode is for debugging and experimenting.");
	msg_print("The game will not be scored if you enter wizard mode.");
	answer = get_check("Are you sure you want to enter wizard mode?");
    }

    if (noscore || answer) {
	noscore |= 0x2;
	wizard = TRUE;
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Weapon weight VS strength and dexterity		-RAK-	
 */
int attack_blows(int weight, int *wtohit)
{
    int adj_weight, str_index, dex_index;

    /* Number of blows */
    int b = 0;
    
    /* Start with the strength/dexterity */
    int s = p_ptr->use_stat[A_STR];
    int d = p_ptr->use_stat[A_DEX];

    if (s * 15 < weight) {

	*wtohit = s * 15 - weight;
	return 1;
    } else {
	*wtohit = 0;

	/* Access the dexterity */
	if (d < 10) dex_index = 0;
	else if (d < 19) dex_index = 1;
	else if (d < 68) dex_index = 2;
	else if (d < 108) dex_index = 3;
	else if (d < 118) dex_index = 4;
	else if (d < 119) dex_index = 5;
	else if (d < 128) dex_index = 6;
	else if (d < 138) dex_index = 7;
	else if (d < 148) dex_index = 8;
	else if (d < 158) dex_index = 9;
	else if (d < 168) dex_index = 10;
	else dex_index = 11;

	/* new class-based weight penalties -CWS */
	switch (p_ptr->pclass) {
	  case 0:				/* Warriors */
	    adj_weight = ((s * 10) / ((weight < 30) ? 30 : weight));
	    break;
	  case 1:				/* Mages */
	    adj_weight = ((s * 4) / ((weight < 40) ? 40 : weight));
	    break;
	  case 2:				/* Priests */
	    adj_weight = ((s * 7) / ((weight < 35) ? 35 : weight));
	    break;
	  case 3:				/* Rogues */
	    adj_weight = ((s * 6) / ((weight < 30) ? 30 : weight));
	    break;
	  case 4:				/* Rangers */
	    adj_weight = ((s * 8) / ((weight < 35) ? 35 : weight));
	    break;
	  default:			/* Paladins */
	    adj_weight = ((s * 8) / ((weight < 30) ? 30 : weight));
	    break;
	}

	/* Access the strength vs weight */
	if (adj_weight < 2) str_index = 0;
	else if (adj_weight < 3) str_index = 1;
	else if (adj_weight < 4) str_index = 2;
	else if (adj_weight < 6) str_index = 3;
	else if (adj_weight < 8) str_index = 4;
	else if (adj_weight < 10) str_index = 5;
	else if (adj_weight < 13) str_index = 6;
	else if (adj_weight < 15) str_index = 7;
	else if (adj_weight < 18) str_index = 8;
	else if (adj_weight < 20) str_index = 9;
	else str_index = 10;

	s = 0;				/* do Weapons of Speed */
	for (d = INVEN_WIELD; d < INVEN_AUX; d++)
	    if (inventory[d].flags2 & TR1_ATTACK_SPD)
		s += inventory[d].pval;

	/* Use the blows table */
	b = (int)blows_table[str_index][dex_index];

	/* Non-warrior attack penalty */
	if (p_ptr->pclass != 0) {
	    if (b > 5) b = 5;
	}
	
	/* Mage attack penalty */
	if (p_ptr->pclass == 1) {
	    if (b > 4) b = 4;
	}

	b += s;

    /* Always get at least one attack */
    if (b < 1) b = 1;

    /* Return the attack count */
    return (b);
    }
}


/*
 * Special damage due to magical abilities of object	-RAK-
 */
int tot_dam(inven_type *i_ptr, int tdam, int r_idx)
{
    register monster_race *m_ptr;
    register monster_lore   *r_ptr;
    int                     reduced = FALSE;

    /* don't resist more than one thing.... -CWS */
    if ((((i_ptr->tval >= TV_SHOT) && 
	(i_ptr->tval <= TV_ARROW)) ||
	 ((i_ptr->tval >= TV_HAFTED) && 
	(i_ptr->tval <= TV_SWORD)) ||
	 (i_ptr->tval == TV_FLASK))) {

	m_ptr = &r_list[r_idx];
	r_ptr = &l_list[r_idx];

    /* Mjollnir? :-> */
	if (!(m_ptr->cflags2 & MF2_IM_ELEC) && (i_ptr->flags2 & TR_LIGHTNING)) {
	    tdam *= 5;
	}

	/* Execute Dragon */
	else if ((i_ptr->flags1 & TR1_KILL_DRAGON) &&
	    (m_ptr->cflags2 & MF2_DRAGON)) {

	    tdam *= 5;
	    r_ptr->r_cflags2 |= MF2_DRAGON;
	}

	/* Slay Dragon  */
	else if ((i_ptr->flags1 & TR1_SLAY_DRAGON) &&
	    (m_ptr->cflags2 & MF2_DRAGON)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_DRAGON;
	}

	/* Slay Undead */
	else if ((i_ptr->flags1 & TR1_SLAY_UNDEAD) &&
	    (m_ptr->cflags2 & MF2_UNDEAD)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_UNDEAD;
	}

	/* Slay Orc */
	else if (((i_ptr->flags2 & TR1_SLAY_ORC) &&
	    m_ptr->cflags2 & MF2_ORC)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_ORC;
	}

	/* Slay MF2_TROLL */
	else if (((i_ptr->flags2 & TR1_SLAY_TROLL) &&
	    m_ptr->cflags2 & MF2_TROLL)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_TROLL;
	}

	/* Slay Giant */
	else if ((i_ptr->flags2 & TR1_SLAY_GIANT) &&
	    (m_ptr->cflags2 & MF2_GIANT)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_GIANT;
	}

	/* Slay Demon */
	else if (((i_ptr->flags2 & TR1_SLAY_DEMON) &&
	    m_ptr->cflags2 & MF2_DEMON)) {

	    tdam *= 3;
	    r_ptr->r_cflags2 |= MF2_DEMON;
	}

	/* Frost */
	else if ((!(m_ptr->cflags2 & MF2_IM_COLD))
		 && (i_ptr->flags1 & TR1_BRAND_COLD)) {
	    tdam *= 3;
	}

	/* Fire	 */
	else if ((!(m_ptr->cflags2 & MF2_IM_FIRE))
		 && (i_ptr->flags1 & TR1_BRAND_FIRE)) {
	    tdam *= 3;
	}

	/* Slay Evil */
	else if ((i_ptr->flags1 & TR1_SLAY_EVIL) &&
	    (m_ptr->cflags2 & MF2_EVIL)) {

	    tdam *= 2;
	    r_ptr->r_cflags2 |= MF2_EVIL;
	}

	/* Slay Animal */
	else if ((i_ptr->flags1 & TR1_SLAY_ANIMAL) &&
	    (m_ptr->cflags2 & MF2_ANIMAL)) {

	    tdam *= 2;
	    r_ptr->r_cflags2 |= MF2_ANIMAL;
	}

	/* let's do the resistances */
	if (((m_ptr->cflags2 & MF2_IM_COLD)) && (i_ptr->flags1 & TR1_BRAND_COLD)) {
	    r_ptr->r_cflags2 |= MF2_IM_COLD;
	    tdam = (tdam * 3) / 4;
	    reduced = TRUE;
	}

	if (((m_ptr->cflags2 & MF2_IM_FIRE)) && (i_ptr->flags1 & TR1_BRAND_FIRE)) {
	    r_ptr->r_cflags2 |= MF2_IM_FIRE;
	    if (!reduced) {
		tdam = (tdam * 3) / 4;
		reduced = TRUE;
	    }
	}

	if (((m_ptr->cflags2 & MF2_IM_ELEC)) && (i_ptr->flags2 & TR_LIGHTNING)) {
	    r_ptr->r_cflags2 |= MF2_IM_ELEC;
	    if (!reduced) {
		tdam = (tdam * 3) / 4;
		reduced = TRUE;
	    }
	}

	if ((i_ptr->flags2 & TR1_IMPACT) && (tdam > 50))
	    earthquake();
    }


    /* Return the total damage */
    return (tdam);
}


/*
 * Critical hits, Nasty way to die.			-RAK-	 
 */
int critical_blow(int weight, int plus, int dam, int attack_type)
{
    register int critical;

    critical = dam;

/* Weight of weapon, plusses to hit, and character level all */
/* contribute to the chance of a critical	             */
    if (randint(5000) <= (int)(weight + 5 * plus
			     + (class_level_adj[p_ptr->pclass][attack_type]
				* p_ptr->lev))) {
	weight += randint(650);
	if (weight < 400) {
	    critical = 2 * dam + 5;
	    msg_print("It was a good hit!");
	} else if (weight < 700) {
	    critical = 2 * dam + 10;
	    msg_print("It was an excellent hit!");
	} else if (weight < 900) {
	    critical = 3 * dam + 15;
	    msg_print("It was a superb hit!");
	} else if (weight < 1300) {
	    critical = 3 * dam + 20;
	    msg_print("It was a *GREAT* hit!");
	} else {
	    critical = ((7 * dam) / 2) + 25;
	    msg_print("It was a *SUPERB* hit!");
	}
    }
    return (critical);
}

/*
 * Saving throws for player character.		-RAK-	 
 */
int player_saves(void)
{
    /* MPW C couldn't handle the expression, so split it into two parts */
    s16b t1 = class_level_adj[p_ptr->pclass][CLA_SAVE];
    s16b t2 = p_ptr->save + stat_adj(A_WIS);
    s16b t3 = t2 + (t1 * p_ptr->lev / 3);
    return (randint(100) <= t3);
}


/*
 * Finds range of item in inventory list		-RAK-	 
 */
int find_range(int item1, int item2, int *j, int *k)
{
    register int         i;
    register inven_type *i_ptr;
    int                  flag;

    i = 0;
    *j = (-1);
    *k = (-1);
    flag = FALSE;
    i_ptr = &inventory[0];
    while (i < inven_ctr) {
	if (!flag) {
	    if ((i_ptr->tval == item1) || (i_ptr->tval == item2)) {
		flag = TRUE;
		*j = i;
	    }
	}
	else {
	    if ((i_ptr->tval != item1) && (i_ptr->tval != item2)) {
		*k = i - 1;
		break;
	    }
	}
	i++;
	i_ptr++;
    }
    if (flag && (*k == -1)) {
	*k = inven_ctr - 1;
    }

    return (flag);
}


/*
 * Teleport the player to a new location		-RAK-
 */
void teleport(int dis)
{
    int x, y, count;

    do {
	count = 0;
	do {
	    count += 1;
	    y = randint(cur_height) - 1;
	    x = randint(cur_width) - 1;
	    while (distance(y, x, char_row, char_col) > dis) {
		y += ((char_row - y) / 2);
		x += ((char_col - x) / 2);
	    }
	}
	while (((cave[y][x].fval >= MIN_CLOSED_SPACE) ||
		(cave[y][x].cptr >= 2) ||
		(i_list[cave[y][x].i_idx].k_idx == OBJ_OPEN_DOOR) ||
		(cave[y][x].fval == NT_DARK_FLOOR) ||
		(cave[y][x].fval == NT_LIGHT_FLOOR)) && count < 1000);

	dis *= 2;
    } while (count == 1000);


    /* Move the player */
    move_rec(char_row, char_col, y, x);

    /* unlight area teleported from */
    darken_player(char_row, char_col);

    char_row = y;
    char_col = x;

    /* Check the view */
    check_view();

    /* Update the monsters */
    update_monsters();

    /* Hack -- The teleport is dealt with */
    teleport_flag = FALSE;
}

