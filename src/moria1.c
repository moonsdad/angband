/* File: moria1.c */

/* Purpose: player inventory (and related commands) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * Check a char for "vowel-hood"
 */
int is_a_vowel(int ch)
{
    switch (ch & 127) {
      case 'a':
      case 'e':
      case 'i':
      case 'o':
      case 'u':
      case 'A':
      case 'E':
      case 'I':
      case 'O':
      case 'U':
	return (TRUE);
      default:
	return (FALSE);
    }
}



/*
 * Return a string describing how a given item is carried. -CJS- 
 */
cptr describe_use(int i)
{
    register cptr p;

    switch (i) {
      case INVEN_WIELD: p = "wielding"; break;
      case INVEN_AUX:   p = "holding ready by your side"; break;
      case INVEN_LEFT:  p = "wearing on your left hand"; break;
      case INVEN_RIGHT: p = "wearing on your right hand"; break;
      case INVEN_NECK:  p = "wearing around your neck"; break;
      case INVEN_LIGHT: p = "using to light the way"; break;
      case INVEN_BODY:  p = "wearing on your body"; break;
      case INVEN_OUTER: p = "wearing about your body"; break;
      case INVEN_ARM:   p = "wearing on your arm"; break;
      case INVEN_HEAD:  p = "wearing on your head"; break;
      case INVEN_HANDS: p = "wearing on your hands"; break;
      case INVEN_FEET:  p = "wearing on your feet"; break;
      default:          p = "carrying in your pack"; break;
    }
    return p;
}



/*
 * Displays inventory items from r1 to r2	-RAK-
 *
 * Designed to keep the display as far to the right as possible.  -CJS-
 *
 * The parameter col gives a column at which to start, but if the display does
 * not fit, it may be moved left.  The return value is the left edge used. 
 */
int show_inven(int r1, int r2, int weight, int col, int (*test) ())
{
    register int i, j, k;
    int          total_weight, len, l, lim;
    bigvtype     tmp_val;
    vtype        out_val[23];

    len = 79 - col;
    if (weight)
	lim = 68;
    else
	lim = 76;

    for (i = 0; i < 23; i++)
	out_val[i][0] = '\0';

    k = 0;
    for (i = r1; i <= r2; i++) {
	if (test) {
	    if ((*test) (object_list[inventory[i].index].tval)) {
		objdes(tmp_val, &inventory[i], TRUE);
		tmp_val[lim] = 0;  /* Truncate if too long. */
		(void)sprintf(out_val[i], "  %c) %s", 'a' + i, tmp_val);
		l = strlen(out_val[i]);
		if (weight)
		    l += 9;
		if (l > len)
		    len = l;
		k++;
	    }
	} else {
	    objdes(tmp_val, &inventory[i], TRUE);
	    tmp_val[lim] = 0;	   /* Truncate if too long. */
	    (void)sprintf(out_val[i], "  %c) %s", 'a' + i, tmp_val);
	    l = strlen(out_val[i]);
	    if (weight)
		l += 9;
	    if (l > len)
		len = l;
	    k++;
	}
    }

    col = 79 - len;
    if (col < 0) col = 0;

    j = 0;
    for (i = r1; (i <= r2) && k; i++) {
	if (out_val[i][0]) {
	    k--;
	/* don't need first two spaces if in first column */
	    if (col == 0)
		prt(&out_val[i][2], 1 + j, col);
	    else
		prt(out_val[i], 1 + j, col);
	    if (weight) {
		total_weight = inventory[i].weight * inventory[i].number;
		(void)sprintf(tmp_val, "%3d.%d lb",
			      (total_weight) / 10, (total_weight) % 10);
		prt(tmp_val, 1 + j, 71);
	    }
	    j++;
	}
    }
    erase_line(1+j,col);
    return col;
}





/*
 * Displays equipment items from r1 to end   -RAK-
 * Keep display as far right as possible. -CJS-
 */
int show_equip(int weight, int col)
{
    register int         i, line;
    register inven_type *i_ptr;
    int                  total_weight, l, len, lim;
    register cptr prt1;
    bigvtype             prt2;
    vtype                out_val[INVEN_ARRAY_SIZE - INVEN_WIELD];

    line = 0;
    len = 79 - col;
    if (weight)
	lim = 52;
    else
	lim = 60;
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
	i_ptr = &inventory[i];
	if (i_ptr->tval != TV_NOTHING) {
	    switch (i) {
	      case INVEN_WIELD:
		if (py.stats.use_stat[A_STR] * 15 < i_ptr->weight)
		    prt1 = "Just lifting";
		else
		    prt1 = "Wielding";
		break;
	      case INVEN_HEAD:
		prt1 = "On head";
		break;
	      case INVEN_NECK:
		prt1 = "Around neck";
		break;
	      case INVEN_BODY:
		prt1 = "On body";
		break;
	      case INVEN_ARM:
		prt1 = "On arm";
		break;
	      case INVEN_HANDS:
		prt1 = "On hands";
		break;
	      case INVEN_RIGHT:
		prt1 = "On right hand";
		break;
	      case INVEN_LEFT:
		prt1 = "On left hand";
		break;
	      case INVEN_FEET:
		prt1 = "On feet";
		break;
	      case INVEN_OUTER:
		prt1 = "About body";
		break;
	      case INVEN_LIGHT:
		prt1 = "Light source";
		break;
	      case INVEN_AUX:
		prt1 = "Spare weapon";
		break;
	      default:
		prt1 = "Unknown value";
		break;
	    }
	    objdes(prt2, &inventory[i], TRUE);
	    prt2[lim] = 0;	   /* Truncate if necessary */
	    (void)sprintf(out_val[line], "  %c) %-14s: %s", line + 'a',
			  prt1, prt2);
	    l = strlen(out_val[line]);
	    if (weight)
		l += 9;
	    if (l > len)
		len = l;
	    line++;
	}
    }
    col = 79 - len;
    if (col < 0)
	col = 0;

    line = 0;
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
	i_ptr = &inventory[i];
	if (i_ptr->tval != TV_NOTHING) {
	/* don't need first two spaces when using whole screen */
	    if (col == 0)
		prt(&out_val[line][2], line + 1, col);
	    else
		prt(out_val[line], line + 1, col);
	    if (weight) {
		total_weight = i_ptr->weight * i_ptr->number;
		(void)sprintf(prt2, "%3d.%d lb",
			      (total_weight) / 10, (total_weight) % 10);
		prt(prt2, line + 1, 71);
	    }
	    line++;
	}
    }
    erase_line(line + 1, col);
    return col;
}






/*
 * Get the ID of an item and return the CTR value of it	-RAK-
 */
int get_item(int *com_val, cptr pmt, int i, int j, int       (*test) ())
{
    vtype        out_val;
    char         which;
    register int test_flag, item;
    int          full, i_scr, redraw;

    int on_floor, ih;
    cave_type *c_ptr;
 
    /* check we're a) identifying and b) on the floor is an object
     * and c) it is a object wich could be picked up
     */

    c_ptr = &cave[char_row][char_col];
    ih = t_list[c_ptr->tptr].tval;
    on_floor = ( (strcmp("Item you wish identified?",pmt) == 0) &&
		 !( (c_ptr->tptr == 0) || ih == TV_NOTHING
		    || ih > TV_MAX_PICK_UP) );

    item = FALSE;
    redraw = FALSE;
    *com_val = 0;
    i_scr = 1;
    if (j > INVEN_WIELD) {
	full = TRUE;
	if (inven_ctr == 0) {
	    i_scr = 0;
	    j = equip_ctr - 1;
	} else
	    j = inven_ctr - 1;
    } else
	full = FALSE;

    if (inven_ctr > 0 || (full && equip_ctr > 0)) {
	do {
	    if (redraw) {
		if (i_scr > 0)
		    (void)show_inven(i, j, FALSE, 80, test);
		else
		    (void)show_equip(FALSE, 80);
	    }
	    if (full)
		(void)sprintf(out_val,
			      "(%s: %c-%c,%s%s / for %s, or ESC) %s",
			     (i_scr > 0 ? "Inven" : "Equip"), i + 'a', j + 'a',
			      (on_floor ? " - floor," : ""),
			      (redraw ? "" : " * to see,"),
			      (i_scr > 0 ? "Equip" : "Inven"), pmt);
	    else
		(void)sprintf(out_val,
			"(Items %c-%c,%s ESC to exit) %s", i + 'a', j + 'a',
			      (redraw ? "" : " * for inventory list,"), pmt);
	    test_flag = FALSE;
	    prt(out_val, 0, 0);
	    do {
		which = inkey();
		switch (which) {
		  case ESCAPE:
		    test_flag = TRUE;
		    free_turn_flag = TRUE;
		    i_scr = (-1);
		    break;
		  case '/':
		    if (full) {
			if (i_scr > 0) {
			    if (equip_ctr == 0) {
				prt("But you're not using anything -more-", 0, 0);
				(void)inkey();
			    } else {
				i_scr = 0;
				test_flag = TRUE;
				if (redraw) {
				    j = equip_ctr;
				    while (j < inven_ctr) {
					j++;
					erase_line(j, 0);
				    }
				}
				j = equip_ctr - 1;
			    }
			    prt(out_val, 0, 0);
			} else {
			    if (inven_ctr == 0) {
				prt("But you're not carrying anything -more-", 0, 0);
				(void)inkey();
			    } else {
				i_scr = 1;
				test_flag = TRUE;
				if (redraw) {
				    j = inven_ctr;
				    while (j < equip_ctr) {
					j++;
					erase_line(j, 0);
				    }
				}
				j = inven_ctr - 1;
			    }
			}

		    }
		    break;
		  case '*':
		    if (!redraw) {
			test_flag = TRUE;
			save_screen();
			redraw = TRUE;
		    }
		    break;
		case '-':
		/* not identified from INVEN or EQU but not aborted */
		    if (on_floor) {
			item = FUZZY;
			test_flag = TRUE;
			i_scr = -1;
			break;
		    }
		  default:
		    if (isupper((int)which))
			*com_val = which - 'A';
		    else
			*com_val = which - 'a';
		    if ((*com_val >= i) && (*com_val <= j)) {
			if (i_scr == 0) {
			    i = 21;
			    j = *com_val;
			    do {
				while (inventory[++i].tval == TV_NOTHING);
				j--;
			    }
			    while (j >= 0);
			    *com_val = i;
			}
			if (isupper((int)which) && !verify("Try", *com_val)) {
			    test_flag = TRUE;
			    free_turn_flag = TRUE;
			    i_scr = (-1);
			    break;
			}
			test_flag = TRUE;
			item = TRUE;
			i_scr = (-1);
		    } else
			bell();
		    break;
		}
	    }
	    while (!test_flag);
	}
	while (i_scr >= 0);
	if (redraw)
	    restore_screen();
	erase_line(MSG_LINE, 0);
    } else
	prt("You are not carrying anything.", 0, 0);
    return (item);
}




/*
 * Recalculate the effect of all the stuff we use.		  -CJS- 
 * Also initialise race intrinsics    SM
 */
void calc_bonuses()
{
    register s32b        item_flags;
    register s32b        item_flags2;
    int                    old_dis_ac;
    register struct flags *p_ptr;
    register struct misc  *m_ptr;
    register inven_type   *i_ptr;
    register int           i;

    p_ptr = &py.flags;
    m_ptr = &py.misc;
    if (p_ptr->slow_digest)
	p_ptr->food_digested++;
    if (p_ptr->regenerate)
	p_ptr->food_digested -= 3;
    if (py.misc.prace == 9)
	p_ptr->see_inv = TRUE;
    else
	p_ptr->see_inv = FALSE;
    p_ptr->teleport = FALSE;
    if (py.misc.prace == 4)
	p_ptr->free_act = TRUE;
    else
	p_ptr->free_act = FALSE;
    p_ptr->slow_digest = FALSE;
    p_ptr->aggravate = FALSE;
    if (py.misc.prace == 7)
	p_ptr->sustain_str = TRUE;
    else
	p_ptr->sustain_str = FALSE;
    p_ptr->sustain_int = FALSE;
    p_ptr->sustain_wis = FALSE;
    if (py.misc.prace == 8)
	p_ptr->sustain_con = TRUE;
    else
	p_ptr->sustain_con = FALSE;
    if (py.misc.prace == 3)
	p_ptr->sustain_dex = TRUE;
    else
	p_ptr->sustain_dex = FALSE;
    p_ptr->sustain_chr = FALSE;
    p_ptr->fire_resist = FALSE;
    p_ptr->acid_resist = FALSE;
    p_ptr->cold_resist = FALSE;
    p_ptr->regenerate = FALSE;
    p_ptr->lght_resist = FALSE;
    if (py.misc.prace == 9)
	p_ptr->ffall = TRUE;
    else
	p_ptr->ffall = FALSE;
    p_ptr->poison_resist = FALSE;
    p_ptr->hold_life = FALSE;
    p_ptr->telepathy = FALSE;
    p_ptr->fire_im = FALSE;
    p_ptr->acid_im = FALSE;
    p_ptr->poison_im = FALSE;
    p_ptr->cold_im = FALSE;
    p_ptr->light_im = FALSE;
    p_ptr->light = FALSE;
    p_ptr->confusion_resist = FALSE;
    p_ptr->sound_resist = FALSE;
    if (py.misc.prace == 2)
	p_ptr->light_resist = TRUE;
    else
	p_ptr->light_resist = FALSE;
    if (py.misc.prace == 6)
	p_ptr->dark_resist = TRUE;
    else
	p_ptr->dark_resist = FALSE;
    p_ptr->chaos_resist = FALSE;
    p_ptr->disenchant_resist = FALSE;
    p_ptr->shards_resist = FALSE;
    p_ptr->nexus_resist = FALSE;
    if (py.misc.prace == 5)
	p_ptr->blindness_resist = TRUE;
    else
	p_ptr->blindness_resist = FALSE;
    p_ptr->nether_resist = FALSE;
    p_ptr->fear_resist = FALSE;

    old_dis_ac = m_ptr->dis_ac;
    m_ptr->ptohit = tohit_adj();   /* Real To Hit   */
    m_ptr->ptodam = todam_adj();   /* Real To Dam   */
    m_ptr->ptoac = toac_adj();	   /* Real To AC    */
    m_ptr->pac = 0;		   /* Real AC	     */
    m_ptr->dis_th = m_ptr->ptohit; /* Display To Hit	    */
    m_ptr->dis_td = m_ptr->ptodam; /* Display To Dam	    */
    m_ptr->dis_ac = 0;		   /* Display AC		 */
    m_ptr->dis_tac = m_ptr->ptoac; /* Display To AC	    */
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	i_ptr = &inventory[i];
	if (i_ptr->tval != TV_NOTHING) {
	    if ((TR_CURSED & i_ptr->flags) == 0) {
		m_ptr->pac += i_ptr->ac;
		m_ptr->dis_ac += i_ptr->ac;
	    }
	    m_ptr->ptohit += i_ptr->tohit;
	    if (i_ptr->tval != TV_BOW)            	/* Bows can't damage. -CJS- */
		m_ptr->ptodam += i_ptr->todam;
	    m_ptr->ptoac += i_ptr->toac;
	    if (known2_p(i_ptr)) {
		m_ptr->dis_th += i_ptr->tohit;
		if (i_ptr->tval != TV_BOW)
		    m_ptr->dis_td += i_ptr->todam;	/* Bows can't damage. -CJS- */
		m_ptr->dis_tac += i_ptr->toac;
	    }
	}
    }

    if (py.misc.pclass == 2) {
	i_ptr = &inventory[INVEN_WIELD];
	if (!(i_ptr->flags2 & TR_BLESS_BLADE) && /* blessed blade == no penalty -CWS */
	    (i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)) {
	    m_ptr->ptohit -= 2;
	    m_ptr->ptodam -= 2;
	    m_ptr->dis_th -= 2;
	    m_ptr->dis_td -= 2;
	}
    }
    if (weapon_heavy)
	m_ptr->dis_th += (py.stats.use_stat[A_STR] * 15 -
			  inventory[INVEN_WIELD].weight);

/* don't forget stun adj, or we'll get incorrect values... -CFT */
    if (p_ptr->stun > 50) {
	m_ptr->ptohit -= 20;
	m_ptr->dis_th -= 20;
	m_ptr->ptodam -= 20;
	m_ptr->dis_td -= 20;
    } else if (p_ptr->stun > 0) {
	m_ptr->ptohit -= 5;
	m_ptr->dis_th -= 5;
	m_ptr->ptodam -= 5;
	m_ptr->dis_td -= 5;
    }
/* Add in temporary spell increases	 */
/*
 * these changed from pac to ptoac, since mana now affected by high pac (to
 * sim. encumberence), and these really should be magical bonuses -CFT 
 */
    if (p_ptr->status & PY_INVULN) {
	m_ptr->ptoac += 100;
	m_ptr->dis_tac += 100;
    }
    if (p_ptr->status & PY_BLESSED) {	/* changed to agree w/ code in
					 * dungeon()... -CFT */
	m_ptr->ptoac += 5;
	m_ptr->dis_tac += 5;
	m_ptr->ptohit += 10;
	m_ptr->dis_th += 10;
    }
    if (p_ptr->shield > 0) {
	m_ptr->ptoac += 50;
	m_ptr->dis_tac += 50;
    }
    if (p_ptr->detect_inv > 0)
	p_ptr->see_inv = TRUE;
    if (p_ptr->status & PY_HERO) { /* now agrees w/ code in dungeon() -CFT */
	m_ptr->ptohit += 12;
	m_ptr->dis_th += 12;
    }
    if (p_ptr->status & PY_SHERO) {/* now agrees w/ code in dungeon() -CFT */
	m_ptr->ptohit += 24;
	m_ptr->dis_th += 24;
	m_ptr->ptoac -= 10;	   /* berserk, so not being careful... -CFT */
	m_ptr->dis_tac -= 10;
    }
    m_ptr->dis_ac += m_ptr->dis_tac;	/* this moved from above, so it will
					 * show ac adjustments from spells...
					 * -CFT */

/* can't print AC here because might be in a store */
    p_ptr->status |= PY_ARMOR;	   /* This was in an if, but I want to be
				    * sure ac is shown properly... -CFT */

    item_flags = 0L;
    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	item_flags |= i_ptr->flags;
	i_ptr++;
    }
    item_flags2 = 0L;
    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	item_flags2 |= i_ptr->flags2;
	i_ptr++;
    }

    /* Process the item flags */
    if (TR_SLOW_DIGEST & item_flags) p_ptr->slow_digest = TRUE;
    if (TR_AGGRAVATE & item_flags) p_ptr->aggravate = TRUE;
    if (TR_TELEPORT & item_flags) p_ptr->teleport = TRUE;
    if (TR_REGEN & item_flags) p_ptr->regenerate = TRUE;
    if (TR_RES_FIRE & item_flags) p_ptr->fire_resist = TRUE;
    if (TR_RES_ACID & item_flags) p_ptr->acid_resist = TRUE;
    if (TR_RES_COLD & item_flags) p_ptr->cold_resist = TRUE;
    if (TR_POISON & item_flags) p_ptr->poison_resist = TRUE;
    if (TR_HOLD_LIFE & item_flags2) ptr->hold_life = TRUE;
    if (TR_TELEPATHY & item_flags2) p_ptr->telepathy = TRUE;
    if (TR_IM_FIRE & item_flags2) p_ptr->fire_im = TRUE;
    if (TR_IM_ACID & item_flags2) p_ptr->acid_im = TRUE;
    if (TR_IM_COLD & item_flags2) p_ptr->cold_im = TRUE;
    if (TR_IM_LIGHT & item_flags2) p_ptr->light_im = TRUE;
    if (TR_IM_POISON & item_flags2) p_ptr->poison_im = TRUE;
    if (TR_LIGHT & item_flags2) _ptr->light = TRUE;
    if (TR_FREE_ACT & item_flags) p_ptr->free_act = TRUE;
    if (TR_SEE_INVIS & item_flags) p_ptr->see_inv = TRUE;
    if (TR_RES_LIGHT & item_flags) p_ptr->lght_resist = TRUE;
    if (TR_FFALL & item_flags) p_ptr->ffall = TRUE;
    if (TR_RES_CONF & item_flags2) p_ptr->confusion_resist = TRUE;
    if (TR_RES_SOUND & item_flags2) p_ptr->sound_resist = TRUE;
    if (TR_RES_LT & item_flags2) p_ptr->light_resist = TRUE;
    if (TR_RES_DARK & item_flags2) p_ptr->dark_resist = TRUE;
    if (TR_RES_CHAOS & item_flags2) p_ptr->chaos_resist = TRUE;
    if (TR_RES_DISENCHANT & item_flags2) p_ptr->disenchant_resist = TRUE;
    if (TR_RES_SHARDS & item_flags2) p_ptr->shards_resist = TRUE;
    if (TR_RES_NEXUS & item_flags2) p_ptr->nexus_resist = TRUE;
    if (TR_RES_BLIND & item_flags2) p_ptr->blindness_resist = TRUE;
    if (TR_RES_NETHER & item_flags2) p_ptr->nether_resist = TRUE;
    if (TR_RES_FEAR & item_flags2) p_ptr->fear_resist = TRUE;

    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
	if (TR_SUST_STAT & i_ptr->flags)
	    switch (i_ptr->p1) {
	      case 1:
		p_ptr->sustain_str = TRUE;
		break;
	      case 2:
		p_ptr->sustain_int = TRUE;
		break;
	      case 3:
		p_ptr->sustain_wis = TRUE;
		break;
	      case 4:
		p_ptr->sustain_con = TRUE;
		break;
	      case 5:
		p_ptr->sustain_dex = TRUE;
		break;
	      case 6:
		p_ptr->sustain_chr = TRUE;
		break;
	      case 10:		   /* :-) ~Ludwig the Hacker!!! */
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;
	      default:
		break;
	    }
	i_ptr++;
    }

    if (p_ptr->slow_digest)
	p_ptr->food_digested--;
    if (p_ptr->regenerate)
	p_ptr->food_digested += 3;

    if (class[py.misc.pclass].spell == MAGE) {
	calc_mana(A_INT);
    } else if (class[py.misc.pclass].spell == PRIEST) {
	calc_mana(A_WIS);
    }
}




/* 
 * Remove item from equipment list		-RAK-
 */
void takeoff(int item_val, int posn)
{
    register cptr p;
    bigvtype             out_val, prt2;
    register inven_type *t_ptr;

    equip_ctr--;
    t_ptr = &inventory[item_val];
    inven_weight -= t_ptr->weight * t_ptr->number;
    py.flags.status |= PY_STR_WGT;

    if (item_val == INVEN_WIELD || item_val == INVEN_AUX)
	p = "Was wielding ";
    else if (item_val == INVEN_LIGHT)
	p = "Light source was ";
    else
	p = "Was wearing ";

    objdes(prt2, t_ptr, TRUE);
    if (posn >= 0)
	(void)sprintf(out_val, "%s%s. (%c)", p, prt2, 'a' + posn);
    else if (posn == -1)
	(void)sprintf(out_val, "%s%s.", p, prt2);
    msg_print(out_val);
    if (item_val != INVEN_AUX)	   /* For secondary weapon  */
	py_bonuses(t_ptr, -1);
    invcopy(t_ptr, OBJ_NOTHING);
}


/*
 * Used to verify if this really is the item we wish to wear or read.
 */
int verify(cptr prompt, int item)
{
    bigvtype out_str, object;

    objdes(object, &inventory[item], TRUE);
    (void)sprintf(out_str, "%s %s? ", prompt, object);
    return (get_check(out_str));
}



/*
 * Drops an item from inventory to given location	-RAK-	
 */
void inven_drop(int item_val, int drop_all)
{
    int                  i;
    register inven_type *i_ptr;
    vtype                prt2;
    bigvtype             prt1;

    /* Access the slot to be dropped */
    i_ptr = &inventory[item_val];

    if (cave[char_row][char_col].tptr != 0)
	(void)delete_object(char_row, char_col);
    i = popt();
    t_list[i] = *i_ptr;
    cave[char_row][char_col].tptr = i;

    if (item_val >= INVEN_WIELD)
	takeoff(item_val, -1);
    else {
	if (drop_all || i_ptr->number == 1) {
	    inven_weight -= i_ptr->weight * i_ptr->number;
	    inven_ctr--;
	    while (item_val < inven_ctr) {
		inventory[item_val] = inventory[item_val + 1];
		item_val++;
	    }
	    invcopy(&inventory[inven_ctr], OBJ_NOTHING);
	} else {
	    t_list[i].number = 1;
	    inven_weight -= i_ptr->weight;
	    i_ptr->number--;
	}
	objdes(prt1, &t_list[i], TRUE);
	(void)sprintf(prt2, "Dropped %s.", prt1);
	msg_print(prt2);
    }
    py.flags.status |= PY_STR_WGT;
}




