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

static cptr stat_names[] = {
    "STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};


/*
 * Print character info in given row, column   -RAK-
 * the longest title is 13 characters, so only pad to 13
 */
void prt_field(cptr info, int row, int col)
{
    put_buffer(&blank_string[BLANK_LENGTH - 13], row, col);
    put_buffer(info, row, col);
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
    vtype out_val1;

    cnv_stat(p_ptr->stats.use_stat[stat], out_val1);
    put_buffer(stat_names[stat], 5 + stat, STAT_COLUMN);
    put_buffer(out_val1, 5 + stat, STAT_COLUMN + 6);
}




/*
 * Adjustment for wisdom/intelligence -JWT-
 */
int stat_adj(int stat)
{
    register int value;

    value = p_ptr->stats.use_stat[stat];
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
    if (value == 118) return (7);
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

    charisma = p_ptr->stats.use_stat[A_CHR];

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

    con = p_ptr->stats.use_stat[A_CON];
    if (con < 7) return (con - 7);
    if (con < 17) return (0);
    if (con == 17) return (1);
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

    if (p_ptr->misc.lev < 1) {
	p = "Babe in arms";
    }
    else if (p_ptr->misc.lev <= MAX_PLAYER_LEVEL) {
	p = player_title[p_ptr->misc.pclass][p_ptr->misc.lev - 1];
    }
    else if (p_ptr->misc.male) {
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
    prt_field(title_string(), 3, STAT_COLUMN);
}


/*
 * Prints level	-RAK-
 */
void prt_level()
{
    prt_int((int)p_ptr->misc.lev, 12, STAT_COLUMN + 6);
}


/*
 * Prints players current mana points. -RAK-
 */
void prt_cmana()
{
    prt_int(p_ptr->misc.cmana, 14, STAT_COLUMN + 6);
}


/*
 * Prints Max hit points -RAK-
 */
void prt_mhp()
{
    prt_int(p_ptr->misc.mhp, 15, STAT_COLUMN + 6);
}


/*
 * Prints players current hit points -RAK-
 */
void prt_chp()
{
    prt_int(p_ptr->misc.chp, 16, STAT_COLUMN + 6);
}


/*
 * prints current AC -RAK-
 */
void prt_pac()
{
    prt_int(p_ptr->misc.dis_ac, 18, STAT_COLUMN + 6);
}


/*
 * Prints current gold					-RAK-
 */
void prt_gold()
{
    prt_long(p_ptr->misc.au, 19, STAT_COLUMN + 3);
}


/*
 * Prints depth in stat area				-RAK-	 
 */
void prt_depth()
{
    vtype               depths;
    register int        depth;

    depth = dun_level * 50;
    if (depth == 0)
	(void)strcpy(depths, "Town    ");
    else
	(void)sprintf(depths, "%d ft", depth);
    prt(depths, 23, 70);
}


/*
 * Prints status of hunger				-RAK-	 
 */
void prt_hunger()
{
    if (PY_WEAK & p_ptr->flags.status) {
	put_buffer("Weak  ", 23, 0);
    }
    else if (PY_HUNGRY & p_ptr->flags.status) {
	put_buffer("Hungry", 23, 0);
    }
    else {
	put_buffer("      ", 23, 0);
    }
}


/*
 * Prints Blind status					-RAK-	 
 */
void prt_blind(void)
{
    if (PY_BLIND & p_ptr->flags.status) {
	put_buffer("Blind", 23, 7);
    }
    else {
	put_buffer("     ", 23, 7);
    }
}


/*
 * Prints Confusion status				-RAK-	 
 */
void prt_confused(void)
{
    if (PY_CONFUSED & p_ptr->flags.status) {
	put_buffer("Confused", 23, 13);
    }
    else {
	put_buffer("        ", 23, 13);
    }
}


/*
 * Prints Fear status					-RAK-	 
 */
void prt_afraid()
{
    if (PY_FEAR & p_ptr->flags.status) {
	put_buffer("Afraid", 23, 22);
    else {
	put_buffer("      ", 23, 22);
    }
}


/*
 * Prints Poisoned status				-RAK-	 
 */
void prt_poisoned()
{
    if (PY_POISONED & p_ptr->flags.status) {
	put_buffer("Poisoned", 23, 29);
    }
    else {
	put_buffer("        ", 23, 29);
    }
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status	-RAK-
 */
void prt_state(void)
{
    char tmp[16];

    /* Turn off the flag */
    p_ptr->flags.status &= ~PY_REPEAT;

    /* Most important info is paralyzation */
    if (p_ptr->flags.paralysis > 1) {
	put_buffer("Paralysed ", 23, 38);
    }

    /* Then comes resting */
    else if (PY_REST & p_ptr->flags.status) {
	if (p_ptr->flags.rest > 0) {
	    (void)sprintf(tmp, "Rest %-5d", p_ptr->flags.rest);
    }
    else if (p_ptr->flags.rest == -1) {
	    (void)sprintf(tmp, "Rest *****");
    }
    else if (p_ptr->flags.rest == -2) {
	    (void)sprintf(tmp, "Rest &&&&&");
    }
	put_buffer(tmp, 23, 38);
    }

    /* Then comes repeating */
    else if (command_count > 0) {

	(void)sprintf(tmp, "Repeat %-3d", command_count);

	/* Hack -- we need to redraw this */
	p_ptr->flags.status |= PY_REPEAT;

	put_buffer(tmp, 23, 38);

	if (PY_SEARCH & p_ptr->flags.status)
	    put_buffer("Search    ", 23, 38);
    }

    else if (PY_SEARCH & p_ptr->flags.status) {
	put_buffer("Searching ", 23, 38);
    }
    else			   /* "repeat 999" is 10 characters */
	put_buffer("          ", 23, 38);
}


/*
 * Prints the speed of a character.			-CJS-
 */
void prt_speed()
{
    int i = p_ptr->flags.speed;

    if (PY_SEARCH & p_ptr->flags.status) i--;	/* Search mode. */


    if (i > 2)
	put_buffer("Extremely Slow", 23, 49);
    else if (i == 2)
	put_buffer("Very Slow     ", 23, 49);
    else if (i == 1)
	put_buffer("Slow          ", 23, 49);
    else if (i == 0)
	put_buffer("              ", 23, 49);
    else if (i == -1)
	put_buffer("Fast          ", 23, 49);
    else if (i == -2)
	put_buffer("Very Fast     ", 23, 49);
    else if (i == -3)
	put_buffer("Extremely Fast", 23, 49);
    else if (i == -4)
	put_buffer("Deadly Speed  ", 23, 49);
    else
	put_buffer("Light Speed   ", 23, 49);
}


void prt_study()
{
    p_ptr->flags.status &= ~PY_STUDY;
    if (p_ptr->flags.new_spells != 0) {
	put_buffer("Study", 23, 64);
    }
    else {
	put_buffer("     ", 23, 64);
    }
}


void prt_cut()
{
    int c = p_ptr->flags.cut;

    if (c > 900)
	put_buffer("Mortal wound", 21, 0);
    else if (c > 300)
	put_buffer("Deep gash   ", 21, 0);
    else if (c > 200)
	put_buffer("Severe cut  ", 21, 0);
    else if (c > 45)
	put_buffer("Nasty cut   ", 21, 0);
    else if (c > 15)
	put_buffer("Bad cut     ", 21, 0);
    else if (c > 5)
	put_buffer("Light cut   ", 21, 0);
    else if (c > 0)
	put_buffer("Graze       ", 21, 0);
    else
	put_buffer("            ", 21, 0);
}



void prt_stun(void)
{
    int s = p_ptr->flags.stun;

    if (!p_ptr->flags.resist_sound) {
	if (s > 100) {
	    put_buffer("Knocked out ", 22, 0);
	}
	else if (s > 50) {
	    put_buffer("Heavy stun  ", 22, 0);
	}
	else if (s > 0) {
	    put_buffer("Stun        ", 22, 0);
	}
	else {
	    put_buffer("            ", 22, 0);
	}
    }
}


/*
 * Prints winner/wizard status on display			-RAK-	
 */
void prt_winner(void)
{
    if (wizard) {
	put_buffer("Wizard", 20, 0);
    }
    else if (total_winner) {
	put_buffer("Winner", 20, 0);
    }
    else {
	put_buffer("       ", 20, 0);
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
                  
    vtype out_val;

    out_val[1]='\0';

    for (i = INVEN_WIELD; i < INVEN_TOTAL; i++) {
	i_ptr = &inventory[i];                           
	j = i - INVEN_WIELD;
	
	if (!equippy_chars || (i_ptr->tval == TV_NOTHING)) out_val[0] = ' ';

	else out_val[0] = (int)(i_ptr->tchar);                

	put_buffer(out_val, 4, j);
    }
}



/*
 * Prints character-screen info				-RAK-	 
 */
void prt_stat_block()
{
    register struct misc *m_ptr = &p_ptr->misc;
    register int          i;

    prt_field(race[p_ptr->misc.prace].trace, 1, STAT_COLUMN);
    prt_field(class[p_ptr->misc.pclass].title, 2, STAT_COLUMN);

    prt_field(title_string(), 3, STAT_COLUMN);

    for (i = 0; i < 6; i++) prt_stat(i);

    prt_num("LEV", (int)m_ptr->lev, 12, STAT_COLUMN);

    prt_lnum("EXP", m_ptr->exp, 13, STAT_COLUMN);

    prt_num("MNA", m_ptr->cmana, 14, STAT_COLUMN);

    prt_num("MHP", m_ptr->mhp, 15, STAT_COLUMN);

    prt_num("CHP", m_ptr->chp, 16, STAT_COLUMN);

    prt_chp(); /* this will overwrite hp, in color, if needed. -CFT */

    prt_num("AC ", m_ptr->dis_ac, 18, STAT_COLUMN);

    prt_lnum("AU ", m_ptr->au, 19, STAT_COLUMN);

    prt_winner();
    prt_cut();
    prt_stun();
    prt_study();

    if ((PY_HUNGRY | PY_WEAK) & p_ptr->flags.status)
	prt_hunger();
    if (PY_BLIND & p_ptr->flags.status)
	prt_blind();
    if (PY_CONFUSED & p_ptr->flags.status)
	prt_confused();
    if (PY_FEAR & p_ptr->flags.status)
	prt_afraid();
    if (PY_POISONED & p_ptr->flags.status)
	prt_poisoned();
    if ((PY_SEARCH | PY_REST) & p_ptr->flags.status)
	prt_state();

    /* if speed non zero, print it, modify speed if Searching */
    if (p_ptr->flags.speed - ((PY_SEARCH & status) >> 8) != 0)
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
    c += p_ptr->flags.cut;
    p_ptr->flags.cut = c;

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

    if (!p_ptr->flags.resist_sound) {
	t = p_ptr->flags.stun;
	p_ptr->flags.stun += s;
	s = p_ptr->flags.stun;
	if (s > 100) {
	    msg_print("You have been knocked out.");
	    if (t == 0) {
		p_ptr->misc.ptohit -= 20;
		p_ptr->misc.ptodam -= 20;
		p_ptr->misc.dis_th -= 20;
		p_ptr->misc.dis_td -= 20;
	    } else if (t <= 50) {
		p_ptr->misc.ptohit -= 15;
		p_ptr->misc.ptodam -= 15;
		p_ptr->misc.dis_th -= 15;
		p_ptr->misc.dis_td -= 15;
	    }
	}
    else if (s > 50) {
	    msg_print("You've been heavily stunned.");
	    if (t == 0) {
		p_ptr->misc.ptohit -= 20;
		p_ptr->misc.ptodam -= 20;
		p_ptr->misc.dis_th -=20;
		p_ptr->misc.dis_td -=20;
	    } else if (t <= 50) {
		p_ptr->misc.ptohit -= 15;
		p_ptr->misc.ptodam -= 15;
        p_ptr->misc.dis_th -= 15;
        p_ptr->misc.dis_td -= 15;
	    }
	}
    else if (s > 0) {
	    msg_print("You've been stunned.");
	    if (t == 0) {
		p_ptr->misc.ptohit -= 5;
		p_ptr->misc.ptodam -= 5;
        p_ptr->misc.dis_th -= 5;
        p_ptr->misc.dis_td -= 5;
	    }
	}
    }
}


u16b modify_stat(int stat, int amount)
{
    register int    loop, i;
    register u16b tmp_stat;

    tmp_stat = p_ptr->stats.cur_stat[stat];

    loop = (amount < 0 ? -amount : amount);

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
    p_ptr->stats.use_stat[stat] = modify_stat(stat, p_ptr->stats.mod_stat[stat]);

    /* Calculate various effects */
    if (stat == A_STR) {
	p_ptr->flags.status |= PY_STR_WGT;
	calc_bonuses();
    }
    else if (stat == A_DEX) {
	calc_bonuses();
    }
    else if (stat == A_CON) {
	calc_hitpoints();
    }
    else if (stat == A_INT && class[p_ptr->misc.pclass].spell == MAGE) {
	calc_spells(A_INT);
	calc_mana(A_INT);
    }
    else if (stat == A_WIS && class[p_ptr->misc.pclass].spell == PRIEST) {
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
    tmp_stat = p_ptr->stats.cur_stat[stat];

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

	p_ptr->stats.cur_stat[stat] = tmp_stat;
	if (tmp_stat > p_ptr->stats.max_stat[stat]) {
	    p_ptr->stats.max_stat[stat] = tmp_stat;
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

    tmp_stat = p_ptr->stats.cur_stat[stat];
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

	p_ptr->stats.cur_stat[stat] = tmp_stat;
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
    if (p_ptr->stats.cur_stat[stat] != p_ptr->stats.max_stat[stat]) {
	p_ptr->stats.cur_stat[stat] = p_ptr->stats.max_stat[stat];
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

    stat = p_ptr->stats.use_stat[A_DEX];
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
    stat = p_ptr->stats.use_stat[A_STR];
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

    stat = p_ptr->stats.use_stat[A_DEX];
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

    stat = p_ptr->stats.use_stat[A_DEX];
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

    stat = p_ptr->stats.use_stat[A_STR];
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
    put_buffer(out_val, row, col);
}

/*
 * Print number with header at given row, column	-RAK-
 */
static void prt_num(cptr header, int num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%s   %6d", header, num);
    put_buffer(out_val, row, col);
}


/*
 * Print long number at given row, column */
static void prt_long(s32b num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%9ld", (long)num);
    put_buffer(out_val, row, col);
}


/*
 * Print number at given row, column	-RAK-	 */
static void prt_int(int num, int row, int col)
{
    vtype out_val;

    (void)sprintf(out_val, "%6d", num);
    put_buffer(out_val, row, col);
}


/*
 * Prints the following information on the screen.	-JWT-	 
 */
void put_character()
{
    register struct misc *m_ptr = &p_ptr->misc;

    clear_screen();

    put_buffer("Name        :", 2, 1);
    put_buffer("Race        :", 3, 1);
    put_buffer("Sex         :", 4, 1);
    put_buffer("Class       :", 5, 1);

    if (character_generated) {
	put_buffer(m_ptr->name, 2, 15);
	put_buffer(race[m_ptr->prace].trace, 3, 15);
	put_buffer((m_ptr->male ? "Male" : "Female"), 4, 15);
	put_buffer(class[m_ptr->pclass].title, 5, 15);
    }
}



/*
 * Prints some information on the screen	-JWT-
 */
void put_stats()
{
    struct misc *m_ptr = &p_ptr->misc;
    int          i, temp;
    vtype        buf;

    /* Display the stats */
    for (i = 0; i < 6; i++) {

	cnv_stat(p_ptr->stats.use_stat[i], buf);
	put_buffer(stat_names[i], 2 + i, 61);
	put_buffer(buf, 2 + i, 66);
	if (p_ptr->stats.max_stat[i] > p_ptr->stats.cur_stat[i]) {
	    /* this looks silly, but it happens because modify_stat() only
	     * looks at cur_stat -CFT */
	    temp = p_ptr->stats.cur_stat[i];
	    p_ptr->stats.cur_stat[i] = p_ptr->stats.max_stat[i];
	    cnv_stat (modify_stat(i,p_ptr->stats.mod_stat[i]), buf);
	    p_ptr->stats.cur_stat[i] = temp; /* DON'T MS2_FORGET! -CFT */
	    put_buffer(buf, 2 + i, 73);
	}
    }
    prt_num("+ To Hit    ", m_ptr->dis_th, 9, 1);
    prt_num("+ To Damage ", m_ptr->dis_td, 10, 1);
    prt_num("+ To AC     ", m_ptr->dis_tac, 11, 1);
    prt_num("  Total AC  ", m_ptr->dis_ac, 12, 1);
}


/*
 * Returns a "rating" of x depending on y			-JWT-
 */
cptr likert(int x, int y)
{
    if ((x/y) < 0) {
	return ("Very Bad");
    }

    switch ((x / y)) {
      case 0:
      case 1:
	return ("Bad");
      case 2:
	return ("Poor");
      case 3:
      case 4:
	return ("Fair");
      case 5:
	return ("Good");
      case 6:
	return ("Very Good");
      case 7:
      case 8:
	return ("Excellent");
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
	return ("Superb");
      case 14:
      case 15:
      case 16:
      case 17:
	return ("Heroic");
      default:
	return ("Legendary");
    }
}


/*
 * Prints age, height, weight, and Social Class			-JWT-
 */
void put_misc1()
{
    register struct misc *m_ptr = &p_ptr->misc;
    prt_num("Age          ", (int)m_ptr->age, 2, 32);
    prt_num("Height       ", (int)m_ptr->ht, 3, 32);
    prt_num("Weight       ", (int)m_ptr->wt, 4, 32);
    prt_num("Social Class ", (int)m_ptr->sc, 5, 32);
}


/*
 * Prints the following information on the screen.	-JWT-
 *
 * For this to look right, the following should be spaced the
 * same as in the prt_lnum code... -CFT
 */
void put_misc2()
{
    register struct misc *m_ptr = &p_ptr->misc;

    prt_num("Level      ", (int)m_ptr->lev, 9, 28);
    prt_lnum("Experience ", m_ptr->exp, 10, 28);
    prt_lnum("Max Exp    ", m_ptr->max_exp, 11, 28);
    if (m_ptr->lev >= MAX_PLAYER_LEVEL) {
	char                buf[40];

	sprintf(buf, "%s%9s", "Exp to Adv.", "****");
	put_buffer(buf, 12, 28);
    }
    else {
	prt_lnum("Exp to Adv.", (s32b) (player_exp[m_ptr->lev - 1] * m_ptr->expfact / 100),
	    12, 28);
    }
    prt_lnum("Gold       ", m_ptr->au, 13, 28);
    prt_num("Max Hit Points ", m_ptr->mhp, 9, 52);
    prt_num("Cur Hit Points ", m_ptr->chp, 10, 52);
    prt_num("Max Mana       ", m_ptr->mana, 11, 52);
    prt_num("Cur Mana       ", m_ptr->cmana, 12, 52);
}


/*
 * Prints ratings on certain abilities			-RAK-
 */
void put_misc3()
{
    int			xbth, xbthb, xfos, xsrh;
    int			xstl, xdis, xsave, xdev;
    vtype                 xinfra;

    /* XXX XXX XXX Skill with current weapon */
    xbth = p_ptr->misc.bth + p_ptr->misc.ptohit * BTH_PLUS_ADJ +
	(class_level_adj[p_ptr->misc.pclass][CLA_BTH] * p_ptr->misc.lev);
    xbthb = p_ptr->misc.bthb + p_ptr->misc.ptohit * BTH_PLUS_ADJ +
	   (class_level_adj[p_ptr->misc.pclass][CLA_BTHB] * p_ptr->misc.lev);

    /* this results in a range from 0 to 29 */
    xfos = 40 - p_ptr->fos;
    if (xfos < 0) xfos = 0;
    xsrh = p_ptr->misc.srh;
    /* this results in a range from 0 to 9 */
    xstl = p_ptr->misc.stl + 1;
    xdis = p_ptr->misc.disarm + 2 * todis_adj() + stat_adj(A_INT) +
	   (class_level_adj[p_ptr->misc.pclass][CLA_DISARM] * p_ptr->misc.lev / 3);
    xsave = p_ptr->misc.save + stat_adj(A_WIS) +
	    (class_level_adj[p_ptr->misc.pclass][CLA_SAVE] * p_ptr->misc.lev / 3);
    xdev = p_ptr->misc.save + stat_adj(A_INT) +
	   (class_level_adj[p_ptr->misc.pclass][CLA_DEVICE] * p_ptr->misc.lev / 3);

    /* Infravision string */
    (void)sprintf(xinfra, "%d feet", p_ptr->flags.see_infra * 10);

    /* Clear it */
    clear_from(14);

    put_buffer("(Miscellaneous Abilities)", 15, 25);

    put_buffer("Fighting    :", 16, 1);
    put_buffer(likert(xbth, 12), 16, 15);

    put_buffer("Bows/Throw  :", 17, 1);
    put_buffer(likert(xbthb, 12), 17, 15);

    put_buffer("Saving Throw:", 18, 1);
    put_buffer(likert(xsave, 6), 18, 15);

    put_buffer("Stealth     :", 16, 28);
    put_buffer(likert(xstl, 1), 16, 42);

    put_buffer("Disarming   :", 17, 28);
    put_buffer(likert(xdis, 8), 17, 42);

    put_buffer("Magic Device:", 18, 28);
    put_buffer(likert(xdev, 6), 18, 42);

    put_buffer("Perception  :", 16, 55);
    put_buffer(likert(xfos, 3), 16, 69);

    put_buffer("Searching   :", 17, 55);
    put_buffer(likert(xsrh, 6), 17, 69);

    put_buffer("Infra-Vision:", 18, 55);
    put_buffer(xinfra, 18, 69);
}


/*
 * Used to display the character on the screen.		-RAK-
 */
void display_char()
{
    put_character();
    put_misc1();
    put_stats();
    put_misc2();
    put_misc3();
}




/*
 * Gets a name for the character			-JWT-
 */
void get_name()
{
    char tmp[100];

    strcpy(tmp, p_ptr->misc.name);
    prt("Enter your player's name  [press <RETURN> when finished]", 21, 2);
    put_buffer(&blank_string[BLANK_LENGTH - 15], 2, 15);

#ifdef MACINTOSH
/*
 * Force player to give a name, would be nice to get name from chooser (STR
 * -16096), but that name might be too long 
 */
    while (!get_string(p_ptr->misc.name, 2, 15, 15) || p_ptr->misc.name[0] == 0);
#else
    if (!get_string(p_ptr->misc.name, 2, 15, 15) || p_ptr->misc.name[0] == 0) {
	strcpy(p_ptr->misc.name, tmp);
	put_buffer(tmp, 2, 15);
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

    display_char();

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
	    if (file_character()) flag = TRUE;
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
 * Computes current weight limit.			-RAK-
 */
int weight_limit(void)
{
    register s32b weight_cap;

    /* Factor in strength */
    weight_cap = (long)p_ptr->stats.use_stat[A_STR] * (long)PLAYER_WEIGHT_CAP;

    /* Hack -- large players can carry more */
    weight_cap += (long)p_ptr->misc.wt;

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

    else if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN)

    /* Scan every possible match */
    for (i = 0; i < inven_ctr; i++) {

	    if (inventory[i].tval == t_ptr->tval &&
		inventory[i].subval == t_ptr->subval &&
	/* make sure the number field doesn't overflow */
		((int)inventory[i].number + (int)t_ptr->number < 256) &&
	/* they always stack (subval < 192), or else they have same p1 */
		((t_ptr->subval < ITEM_GROUP_MIN) || (inventory[i].p1 == t_ptr->p1))
	/* only stack if both or neither are identified */
		&& (known1_p(&inventory[i]) == known1_p(t_ptr)))
		return TRUE;
    }

    /* And there was no room in the inn... */
    return FALSE;
}

/*
 * Add an item to the players inventory, and return the item position.
 * for a description if needed.
 *
 * -RAK- this code must be identical to the inven_check_num() code above 
 *
 * Okay, here's my inven_carry() function (from misc2.c, I think).  Just
 * replace the existing inven_carry() with this one.
 *
 * Items will sort into place, with mage spellbooks coming first for mages,
 * rangers, and rogues.  Also, this will make Tenser's book sort after all
 * the mage books except Raals, instead of in the middle of them (which
 * always seemed strange to me). -CFT 
 */
int inven_carry(inven_type *i_ptr)
{
    register int         locn = 0, i;
    register int         typ, subt;
    register inven_type *t_ptr;
    int                  known1p, always_known1p;
    int                  tval_tmp;  /* used to make magic books before pray books if magicuser */
    int                  stacked = FALSE;

    typ = i_ptr->tval;
    subt = i_ptr->subval;
    known1p = known1_p(i_ptr);
    always_known1p = (object_offset(i_ptr) == -1);

    if (inven_ctr >= INVEN_WIELD) /* sanity checking to prevent the inv from */
	inven_ctr = INVEN_WIELD;  /* running over the equipment list -CWS */

/*
 * to prevent nasty losses of objects, we first look through entire inven for
 * a place to stack, w/o assuming the inventory is sorted. -CFT 
 */
    if (subt >= ITEM_SINGLE_STACK_MIN) {
	for (locn = 0; locn < inven_ctr; locn++) {
	    t_ptr = &inventory[locn];
	    if (t_ptr->tval == typ &&
		t_ptr->subval == subt &&
	/* make sure the number field doesn't overflow */
		((int)t_ptr->number + (int)i_ptr->number < 256) &&
	/* they always stack (subval < 192), or else they have same p1 */
		((subt < ITEM_GROUP_MIN) || (t_ptr->p1 == i_ptr->p1))
	/* only stack if both or neither are identified */
		&& (known1_p(&inventory[locn]) == known1p)) {
		stacked = TRUE;	   /* note that we did process the item -CFT */
		t_ptr->number += i_ptr->number;

	/* if player bought at bargin price, then make sure he can't sell back
	 * for normal value.  This is unfair, since it robs the value from items,
	 * but it does prevent the player from "milking" the stores for cash.
	 */
		if (i_ptr->cost < t_ptr->cost)
		    t_ptr->cost = i_ptr->cost;
		break;
	    } /* if it stacks here */
	} /* for loop */
    } /* if it stacks, try to stack it... */

    if (!stacked) {
    /* either it doesn't stack anyway, or it didn't match anything in the inventory.
     * Now try to insert. -CFT */

	for (locn = 0;; locn++) {
	    t_ptr = &inventory[locn];

	/* For items which are always known1p, i.e. never have a 'color',
	 * insert them into the inventory in sorted order.  
	 */
	    if ((typ == TV_PRAYER_BOOK) && (class[p_ptr->misc.pclass].spell == MAGE))
		typ = TV_MAGIC_BOOK - 1;
	/* sort is in descending, so this will be immediately after magic books.
	 * It helps that there is no tval that uses this. -CFT
	 */
	    tval_tmp = t_ptr->tval;
	    if ((tval_tmp == TV_PRAYER_BOOK) &&
		(class[p_ptr->misc.pclass].spell == MAGE))
		tval_tmp = TV_MAGIC_BOOK - 1;
	/* sort is in descending, so this will be immediately after magic books.
	 * It helps that there is no tval that uses this. -CFT
	 */
	    if ((typ > tval_tmp) ||     /* sort by desc tval */
		(always_known1p &&      /* if always known, then sort by inc level, */
		 (typ == tval_tmp) &&	/* then by inc subval */
		 ((i_ptr->level < t_ptr->level) ||
	     ((i_ptr->level == t_ptr->level) && (subt < t_ptr->subval))))) {
		for (i = inven_ctr - 1; i >= locn; i--)
		    inventory[i + 1] = inventory[i];
		inventory[locn] = *i_ptr;
		inven_ctr++;
		break;
	    }
	}
    }
    /* Increase the weight, prepare to redraw */
    inven_weight += i_ptr->number * i_ptr->weight;

    /* Remember to re-calculate bonuses */
    p_ptr->flags.status |= PY_STR_WGT;

    /* Say where it went */
    return locn;
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

    s_ptr = &magic_spell[p_ptr->misc.pclass - 1][spell];
    chance = s_ptr->sfail - 3 * (p_ptr->misc.lev - s_ptr->slevel);
    stat = (class[p_ptr->misc.pclass].spell == MAGE) ? A_INT : A_WIS;
    chance -= 3 * (stat_adj(stat) - 1);
    if (s_ptr->smana > p_ptr->misc.cmana) {
	chance += 5 * (s_ptr->smana - p_ptr->misc.cmana);
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
    if ((minfail < 5) && (p_ptr->misc.pclass != 1) && (p_ptr->misc.pclass != 2)) {
	minfail = 5;
    }

    /* Big prayer penalty for edged weapons  -DGK */
    /* XXX So, like, switch weapons and then pray? */
    if (p_ptr->misc.pclass == 2) {
	register inven_type *i_ptr;
	i_ptr = &inventory[INVEN_WIELD];
	if ((i_ptr->tval == TV_SWORD) || (i_ptr->tval == TV_POLEARM)) {
	    if ((i_ptr->flags2 & TR_BLESS_BLADE) == 0) {
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

    offset = (class[p_ptr->misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);
    erase_line(1, col);
    put_buffer("Name", 1, col + 5);
    put_buffer("Lv Mana Fail", 1, col + 35);

    /* only show the first 22 choices */
    if (num > 22) num = 22;

    for (i = 0; i < num; i++) {
	j = spell[i];
	s_ptr = &magic_spell[p_ptr->misc.pclass - 1][j];
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
    offset = (class[p_ptr->misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    /* Get a spell from the user */
    while (flag == FALSE && get_com(out_str, &choice)) {

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
		s_ptr = &magic_spell[p_ptr->misc.pclass - 1][*sn];
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
		(class[p_ptr->misc.pclass].spell == MAGE) ? "spell" : "prayer");
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
    register player_class *c_ptr;

    p_ptr->misc.lev++;
    (void)sprintf(out_val, "Welcome to level %d.", (int)p_ptr->misc.lev);
    msg_print(out_val);
    calc_hitpoints();
    prt_level();
    prt_title();
    c_ptr = &class[p_ptr->misc.pclass];

    if (c_ptr->spell == MAGE) {
	calc_spells(A_INT);
	calc_mana(A_INT);
    }
    else if (c_ptr->spell == PRIEST) {
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
    
    if (p_ptr->misc.exp > MAX_EXP) p_ptr->misc.exp = MAX_EXP;
    if (p_ptr->misc.lev < MAX_PLAYER_LEVEL) {
	while ((player_exp[p_ptr->misc.lev-1] * p_ptr->misc.expfact / 100)
	       <= p_ptr->misc.exp && (p_ptr->misc.lev < MAX_PLAYER_LEVEL)) {

	    gain_level();

	    /* Level was actually gained, not restored */
	    if (p_ptr->misc.exp > p_ptr->misc.max_exp) {

		/* level was actually gained, not restored:
		 * this 300 is arbitrary, but it makes human ages work okay,
		 * and I chose the other racial age adjs based on this as well -CFT
		 */
		p_ptr->misc.age += randint((u16b)class[p_ptr->misc.pclass].age_adj *
				       (u16b)race[p_ptr->misc.prace].m_age)/300;
	    }
	}
    }

    if (p_ptr->misc.exp > p_ptr->misc.max_exp) p_ptr->misc.max_exp = p_ptr->misc.exp;

    (void) sprintf(out_val, "%8ld", (long)p_ptr->misc.exp);
    put_buffer(out_val, 13, STAT_COLUMN+4);
}



/*
 * Calculate the players hit points 
 */
void calc_hitpoints()
{
    register int          hitpoints;
    register s32b        value;

    /* Calculate hitpoints */
    hitpoints = player_hp[p_ptr->misc.lev - 1] + (con_adj() * p_ptr->misc.lev);

    /* always give at least one point per level + 1 */
    if (hitpoints < (p_ptr->misc.lev + 1)) {
	hitpoints = p_ptr->misc.lev + 1;
    }

    /* Factor in the hero / superhero values */
    if (p_ptr->flags.status & PY_HERO) hitpoints += 10;
    if (p_ptr->flags.status & PY_SHERO) hitpoints += 30;

    /* mhp can equal zero while character is being created */
    if ((hitpoints != p_ptr->misc.mhp) && (p_ptr->misc.mhp != 0)) {

        /* change current hit points proportionately to change of mhp */
        /* divide first to avoid overflow, little loss of accuracy */
	value = (((long)p_ptr->misc.chp << 16) + p_ptr->misc.chp_frac) / p_ptr->misc.mhp;
	value = value * hitpoints;
	p_ptr->misc.chp = value >> 16;
	p_ptr->misc.chp_frac = value & 0xFFFF;

	/* Save the new max-hitpoints */
	p_ptr->misc.mhp = hitpoints;

    /* can't print hit points here, may be in store or inventory mode */
	p_ptr->flags.status |= PY_HP;
    }
}


/*
 * Inserts a string into a string
 */
void insert_str(char *object_str, cptr mtc_str, cptr insert)
{
    int            obj_len;
    char          *bound, *pc;
    register int   i, mtc_len;
    register char *temp_obj;
    cptr           temp_mtc;
    char           out_val[80];

    mtc_len = strlen(mtc_str);
    obj_len = strlen(object_str);
    bound = object_str + obj_len - mtc_len;
    for (pc = object_str; pc <= bound; pc++) {
	temp_obj = pc;
	temp_mtc = mtc_str;
	for (i = 0; i < mtc_len; i++)
	    if (*temp_obj++ != *temp_mtc++)
		break;
	if (i == mtc_len)
	    break;
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
 * Lets anyone enter wizard mode after a disclaimer...		- JEW -
 */
int enter_wiz_mode(void)
{
    register int answer;

    if (!is_wizard(player_uid)) return FALSE;

    if (!noscore) {
	msg_print("Wizard mode is for debugging and experimenting.");
	answer = get_check("The game will not be scored if you enter wizard mode. Are you sure?");
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

    /* Start with the strength/dexterity */
    int s = p_ptr->stats.use_stat[A_STR];
    int d = p_ptr->stats.use_stat[A_DEX];

    if (s * 15 < weight) {

	*wtohit = s * 15 - weight;
	return 1;
    } else {
	*wtohit = 0;

	if (d < 10) dex_index = 0;
	else if (d < 19) dex_index = 1;
	else if (d < 68) dex_index = 2;
	else if (d < 108) dex_index = 3;
	else if (d < 118) dex_index = 4;
	else if (d == 118) dex_index = 5;
	else if (d < 128) dex_index = 6;
	else if (d < 138) dex_index = 7;
	else if (d < 148) dex_index = 8;
	else if (d < 158) dex_index = 9;
	else if (d < 168) dex_index = 10;
	else dex_index = 11;

	/* new class-based weight penalties -CWS */
	switch (p_ptr->misc.pclass) {
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
	    if (inventory[d].flags2 & TR_ATTACK_SPD)
		s += inventory[d].p1;

	d = (int)blows_table[str_index][dex_index];

	/* Non-warrior attack penalty */
	if (p_ptr->misc.pclass != 0) {
	    if (d > 5) d = 5;
	}
	
	/* Mage attack penalty */
	if (p_ptr->misc.pclass == 1) {
	    if (d > 4) d = 4;
	}

	d += s;

    /* Always get at least one attack */
    if (d < 1) d = 1;

    /* Return the attack count */
	return (d);
    }
}


/*
 * Special damage due to magical abilities of object	-RAK-
 */
int tot_dam(inven_type *i_ptr, int tdam, int monster)
{
    register monster_race *m_ptr;
    register monster_lore   *r_ptr;
    int                     reduced = FALSE;

    /* don't resist more than one thing.... -CWS */
    if ((((i_ptr->tval >= TV_SHOT) && (i_ptr->tval <= TV_ARROW)) ||
	 ((i_ptr->tval >= TV_HAFTED) && (i_ptr->tval <= TV_SWORD)) ||
	 (i_ptr->tval == TV_FLASK))) {
	m_ptr = &r_list[monster];
	r_ptr = &l_list[monster];
    /* Mjollnir? :-> */
	if (!(m_ptr->cdefense & MF2_IM_ELEC) && (i_ptr->flags2 & TR_LIGHTNING)) {
	    tdam *= 5;
	}

    /* Execute Dragon */
	else if ((m_ptr->cdefense & DRAGON) && (i_ptr->flags & TR1_KILL_DRAGON)) {
	    tdam *= 5;
	    r_ptr->r_cdefense |= DRAGON;
	}

    /* Slay Dragon  */
	else if ((m_ptr->cdefense & DRAGON) && (i_ptr->flags & TR1_SLAY_DRAGON)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= DRAGON;
	}

    /* Slay Undead  */
	else if ((m_ptr->cdefense & UNDEAD) && (i_ptr->flags & TR1_SLAY_UNDEAD)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= UNDEAD;
	}

    /* Slay ORC     */
	else if ((m_ptr->cdefense & ORC) && (i_ptr->flags2 & TR1_SLAY_ORC)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= ORC;
	}

    /* Slay MF2_TROLL     */
	else if ((m_ptr->cdefense & MF2_TROLL) && (i_ptr->flags2 & TR1_SLAY_TROLL)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= MF2_TROLL;
	}

    /* Slay GIANT     */
	else if ((m_ptr->cdefense & MF2_GIANT) && (i_ptr->flags2 & TR1_SLAY_GIANT)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= MF2_GIANT;
	}

    /* Slay DEMON     */
	else if ((m_ptr->cdefense & DEMON) && (i_ptr->flags2 & TR1_SLAY_DEMON)) {
	    tdam *= 3;
	    r_ptr->r_cdefense |= DEMON;
	}

    /* Frost	       */
	else if ((!(m_ptr->cdefense & MF2_IM_COLD))
		 && (i_ptr->flags & TR1_BRAND_COLD)) {
	    tdam *= 3;
	}

    /* Fire	      */
	else if ((!(m_ptr->cdefense & MF2_IM_FIRE))
		 && (i_ptr->flags & TR1_BRAND_FIRE)) {
	    tdam *= 3;
	}

    /* Slay Evil     */
	else if ((m_ptr->cdefense & EVIL) && (i_ptr->flags & TR1_SLAY_EVIL)) {
	    tdam *= 2;
	    r_ptr->r_cdefense |= EVIL;
	}

    /* Slay Animal  */
	else if ((m_ptr->cdefense & MF2_ANIMAL) && (i_ptr->flags & TR1_SLAY_ANIMAL)) {
	    tdam *= 2;
	    r_ptr->r_cdefense |= MF2_ANIMAL;
	}

				/* let's do the resistances */
	if (((m_ptr->cdefense & MF2_IM_COLD)) && (i_ptr->flags & TR1_BRAND_COLD)) {
	    r_ptr->r_cdefense |= MF2_IM_COLD;
	    tdam = (tdam * 3) / 4;
	    reduced = TRUE;
	}

	if (((m_ptr->cdefense & MF2_IM_FIRE)) && (i_ptr->flags & TR1_BRAND_FIRE)) {
	    r_ptr->r_cdefense |= MF2_IM_FIRE;
	    if (!reduced) {
		tdam = (tdam * 3) / 4;
		reduced = TRUE;
	    }
	}

	if (((m_ptr->cdefense & MF2_IM_ELEC)) && (i_ptr->flags2 & TR_LIGHTNING)) {
	    r_ptr->r_cdefense |= MF2_IM_ELEC;
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
			     + (class_level_adj[p_ptr->misc.pclass][attack_type]
				* p_ptr->misc.lev))) {
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
    s16b temp = class_level_adj[p_ptr->misc.pclass][CLA_SAVE];

    if (randint(100) <= (p_ptr->misc.save + stat_adj(A_WIS)
			 + (temp * p_ptr->misc.lev / 3)))
	return (TRUE);
    else
	return (FALSE);
}


/*
 * Finds range of item in inventory list		-RAK-	 
 */
int find_range(int item1, int item2, register int *j, register int *k)
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
		(t_list[cave[y][x].tptr].index == OBJ_OPEN_DOOR) ||
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

    creatures(FALSE);

    /* Hack -- The teleport is dealt with */
    teleport_flag = FALSE;
}

