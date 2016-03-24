/* File: dungeon.c */ 

/* Purpose: the main command interpreter, updating player status */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/* ANGBAND game module					-RAK-	 */
/* The code in this section has gone through many revisions, and */
/* some of it could stand some more hard work.	-RAK-	       */

/* It has had a bit more hard work.			-CJS- */

/* And it has had some more... -BEN- */






/*
 * Is an item an enchanted weapon or armor and we don't know?  -CJS-
 * returns a description
 */
static cptr value_check(inven_type *i_ptr)
{
    /* Paranoia -- No item */
    if (i_ptr->tval == TV_NOTHING) return (NULL);

    /* Known items need no feeling */
    if (known2_p(i_ptr)) return (NULL);

    if (store_bought_p(i_ptr)) return (NULL);
    if (i_ptr->ident & ID_MAGIK) return (NULL);
    if (i_ptr->ident & ID_DAMD) return (NULL);
    if (i_ptr->inscrip[0] != '\0') return (NULL);
    if (i_ptr->flags & TR_CURSED && i_ptr->name2 == SN_NULL) return "worthless";
    if (i_ptr->flags & TR_CURSED && i_ptr->name2 != SN_NULL) return "terrible";
    if ((i_ptr->tval == TV_DIGGING) &&  /* also, good digging tools -CFT */
	(i_ptr->flags & TR_TUNNEL) &&
	(i_ptr->p1 > object_list[i_ptr->index].p1)) /* better than normal for this
						       type of shovel/pick? -CFT */
	return "good";
    if ((i_ptr->tohit<=0 && i_ptr->todam<=0 && i_ptr->toac<=0) &&
	i_ptr->name2==SN_NULL)  /* normal shovels will also reach here -CFT */
	return "average";
    if (i_ptr->name2 == SN_NULL)
	return "good";
    if ((i_ptr->name2 == SN_R) || (i_ptr->name2 == SN_RA) ||
	(i_ptr->name2 == SN_RF) || (i_ptr->name2 == SN_RC) ||
	(i_ptr->name2 == SN_RL) || (i_ptr->name2 == SN_SE) ||
	(i_ptr->name2 == SN_HA) || (i_ptr->name2 == SN_FT) ||
	(i_ptr->name2 == SN_DF) || (i_ptr->name2 == SN_FB) ||
	(i_ptr->name2 == SN_SA) || (i_ptr->name2 == SN_FREE_ACTION) ||
	(i_ptr->name2 == SN_SD) || (i_ptr->name2 == SN_SLAYING) ||
	(i_ptr->name2 == SN_SU) || (i_ptr->name2 == SN_SLOW_DESCENT) ||
	(i_ptr->name2 == SN_SPEED) || (i_ptr->name2 == SN_STEALTH) ||
	(i_ptr->name2 == SN_INTELLIGENCE) || (i_ptr->name2 == SN_WISDOM) ||
	(i_ptr->name2 == SN_INFRAVISION) || (i_ptr->name2 == SN_MIGHT) ||
	(i_ptr->name2 == SN_LORDLINESS) || (i_ptr->name2 == SN_MAGI) ||
	(i_ptr->name2 == SN_BEAUTY) || (i_ptr->name2 == SN_SEEING) ||
     (i_ptr->name2 == SN_REGENERATION) || (i_ptr->name2 == SN_PROTECTION) ||
	(i_ptr->name2 == SN_FIRE) || (i_ptr->name2 == SN_SLAY_EVIL) ||
	(i_ptr->name2 == SN_DRAGON_SLAYING) || (i_ptr->name2 == SN_SLAY_ANIMAL) ||
	(i_ptr->name2 == SN_ACCURACY) || (i_ptr->name2 == SN_SO) ||
	(i_ptr->name2 == SN_POWER) || (i_ptr->name2 == SN_WEST) ||
	(i_ptr->name2 == SN_SDEM) || (i_ptr->name2 == SN_ST) ||
	(i_ptr->name2 == SN_LIGHT) || (i_ptr->name2 == SN_AGILITY) ||
	(i_ptr->name2 == SN_SG) || (i_ptr->name2 == SN_TELEPATHY) ||
	(i_ptr->name2 == SN_DRAGONKIND) || (i_ptr->name2 == SN_AMAN) ||
	(i_ptr->name2 == SN_ELVENKIND) || (i_ptr->name2 == SN_WOUNDING) ||
	(i_ptr->name2 == SN_BLESS_BLADE) || (i_ptr->name2 == SN_ATTACKS))
	return "excellent";
    return "special";
}


/*
 *  Regenerate hit points				-RAK-	 
 */
static void regenhp(int percent)
{
    register s32b        new_chp, new_chp_frac;
    int                   old_chp;

    old_chp = p_ptr->misc.chp;
    new_chp = ((long)p_ptr->misc.mhp) * percent + PLAYER_REGEN_HPBASE;
    p_ptr->misc.chp += new_chp >> 16;   /* div 65536 */

    /* check for overflow */
    if (p_ptr->misc.chp < 0 && old_chp > 0) p_ptr->misc.chp = MAX_SHORT;
    new_chp_frac = (new_chp & 0xFFFF) + p_ptr->misc.chp_frac;	/* mod 65536 */
    if (new_chp_frac >= 0x10000L) {
	p_ptr->misc.chp_frac = new_chp_frac - 0x10000L;
	p_ptr->misc.chp++;
    }
    else {
	p_ptr->misc.chp_frac = new_chp_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->misc.chp >= p_ptr->misc.mhp) {
	p_ptr->misc.chp = p_ptr->misc.mhp;
	p_ptr->misc.chp_frac = 0;
    }

    /* Notice changes */
    if (old_chp != p_ptr->misc.chp) prt_chp();
}


/* 
 * Regenerate mana points				-RAK-	 
 */
static void regenmana(int percent)
{
    register s32b        new_mana, new_mana_frac;
    int                   old_cmana;

    old_cmana = p_ptr->misc.cmana;
    new_mana = ((long)p_ptr->misc.mana) * percent + PLAYER_REGEN_MNBASE;
    p_ptr->misc.cmana += new_mana >> 16;	/* div 65536 */
    /* check for overflow */
    if (p_ptr->misc.cmana < 0 && old_cmana > 0) {
	p_ptr->misc.cmana = MAX_SHORT;
    }
    new_mana_frac = (new_mana & 0xFFFF) + p_ptr->misc.cmana_frac;	/* mod 65536 */
    if (new_mana_frac >= 0x10000L) {
	p_ptr->misc.cmana_frac = new_mana_frac - 0x10000L;
	p_ptr->misc.cmana++;
    }
    else {
	p_ptr->misc.cmana_frac = new_mana_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->misc.cmana >= p_ptr->misc.mana) {
	p_ptr->misc.cmana = p_ptr->misc.mana;
	p_ptr->misc.cmana_frac = 0;
    }
    
    /* Redraw mana */
    if (old_cmana != p_ptr->misc.cmana) prt_cmana();
}



static void regen_monsters(void)
{
    register int i;
    monster_type *m_ptr;
    int          frac;

    /* Regenerate everyone */
    for (i = 0; i < MAX_M_IDX; i++) {

	/* Check the i'th monster */
	m_ptr = &m_list[i];

	if (m_ptr->hp >= 0) {
	    if (m_ptr->maxhp == 0) {	/* then we're just going to fix it!  -CFT */
		if ((r_list[m_ptr->mptr].cdefense & MF2_MAX_HP) || be_nasty)
		    m_ptr->maxhp = max_hp(r_list[m_ptr->mptr].hd);
		else
		    m_ptr->maxhp = pdamroll(r_list[m_ptr->mptr].hd);
	    }
	/* Allow regeneration */
	    if (m_ptr->hp < m_ptr->maxhp)
		m_ptr->hp += ((frac = 2 * m_ptr->maxhp / 100) > 0) ? frac : 1;
	    if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
	} /* if hp >= 0 */
    } /* for loop */
}



/*
 * Main procedure for dungeon.			-RAK-
 * Note: There is a lot of preliminary magic going on here at first
 */
void dungeon(void)
{
    int                    i, find_count;

    register inven_type		*i_ptr;

    /* Regenerate hp and mana */
    int                    regen_amount;

    /* Last command           */
    char                   command;

    /* init pointers. */
    i_ptr = &inventory[INVEN_LITE];

    /* Check light status for setup	   */
    if (i_ptr->p1 > 0 || p_ptr->flags.light)
	player_light = TRUE;
    else
	player_light = FALSE;
    if (i_ptr->tval == TV_LITE)
	i = i_ptr->sval;
    else
	i = 195;
    cur_lite = 1 + (i < 190) + (i == 4 || i == 6);

    /* Check for a maximum level		   */
    /* Added check to avoid -50' being "deepest", since max_dlv unsigned -CFT */
    if ((dun_level >= 0) && ((unsigned) dun_level > p_ptr->misc.max_dlv))
	p_ptr->misc.max_dlv = dun_level;

    /* Reset flags and initialize variables  (most of it is overkill)  */
    new_level_flag = FALSE;
    teleport_flag  = FALSE;
    find_flag      = FALSE;
    eof_flag       = FALSE;
    opening_chest  = FALSE;

    /* Reset the "command" vars (again, mostly overkill) */
    command_count  = 0;
    find_count     = 0;
    mon_tot_mult   = 0;
    coin_type      = 0;
    old_lite        = (-1);

#ifdef TARGET
    /* target code taken from Morgul -CFT */
    target_mode = FALSE;
#endif
    cave[char_row][char_col].cptr = 1;

	/* just in case... */
    if (create_up_stair && (dun_level == 0)) create_up_stair = FALSE;

    /* Make a stairway. */
    if (create_up_stair || create_down_stair) {

	register cave_type *c_ptr;
	register int        cur_pos;

	c_ptr = &cave[char_row][char_col];
	if ((c_ptr->tptr == 0) ||
	    ((t_list[c_ptr->tptr].tval != TV_STORE_DOOR) && /* if not store */
	     ((t_list[c_ptr->tptr].tval < TV_MIN_WEAR) ||   /* if no artifact here -CFT */
	      (t_list[c_ptr->tptr].tval > TV_MIN_WEAR) ||
	      !(t_list[c_ptr->tptr].flags2 & TR_ARTIFACT)))) {
	    if (c_ptr->tptr != 0)
		(void)delete_object(char_row, char_col);
	    cur_pos = i_pop();
	    c_ptr->tptr = cur_pos;
	    if (create_up_stair)
		invcopy(&t_list[cur_pos], OBJ_UP_STAIR);
	    else if (create_down_stair && !is_quest(dun_level))
		invcopy(&t_list[cur_pos], OBJ_DOWN_STAIR);
	} else
	    msg_print("The object resists your attempt to transform it into a stairway.");

	/* Cancel the stair request */
	create_down_stair = FALSE;
	create_up_stair = FALSE;
    }

    /* Ensure we display the panel. Used to do this with a global var. -CJS- */
    panel_row = panel_col = (-1);

    /*  Check the view */
    check_view();

/* must do this after panel_row/col set to -1, because search_off() will call
 * check_view(), and so the panel_* variables must be valid before
 * search_off() is called 
 */
    if (p_ptr->flags.status & PY_SEARCH)
	search_off();
/* Light,  but do not move critters	    */
    creatures(FALSE);

    /* Print the depth */
    prt_depth();

    /* FIXME: figure this out */
    if (((turn - old_turn) > randint(50) + 50) && dun_level) {
	unfelt = FALSE;
	print_feeling();
    }
    old_turn = turn;

/* Loop until dead,  or new level		 */
    do {

	/* Advance the turn counter */
	turn++;


	/*** Check the Load ***/

#ifdef CHECK_HOURS
	/* Check for game hours			       */
	if (((turn % 100) == 1) && !check_time()) {
	    if (closing_flag > 2) {
		msg_print("The gates to ANGBAND are now closed.");
		(void)strcpy(died_from, "(closing gate: saved)");
		if (!save_char()) {
		(void)strcpy(died_from, "a slammed gate");
		death = TRUE;
		}
		exit_game();
	    }
	    else {
		disturb(0, 0);
		closing_flag++;
		msg_print("The gates to ANGBAND are closing due to high load.");
		msg_print("Please finish up or save your game.");
	    }
	}
#endif				   /* CHECK_HOURS */

	/*** Update the Stores ***/
    /* turn over the store contents every, say, 1000 turns */
	/* Update the stores once a day */
	if ((dun_level != 0) && ((turn % 1000) == 0)) {

	/* if (peek) msg_print("Store update: "); */
	    store_maint();
	/* if (peek) msg_print("Complete "); */
	}


	/*** Make, and Heal, the Monsters ***/

	/* Check for creature generation */
	if (randint(MAX_M_ALLOC_CHANCE) == 1) {
	    alloc_monster(1, MAX_SIGHT, FALSE);
	}

	/* Check for creature regeneration */
	if (!(turn % 20)) regen_monsters();


	/*** Handle the Lights ***/

	/* Check light status */
	i_ptr = &inventory[INVEN_LITE];

	if (player_light)
	    if (i_ptr->p1 > 0) {
		if (!(i_ptr->flags2 & TR_LIGHT))
		    i_ptr->p1--;   /* don't dec if perm light -CFT */
		if (i_ptr->p1 == 0) {
		    player_light = FALSE;
		    disturb(0, 1);
		/* unlight creatures */
		    creatures(FALSE);
		    msg_print("Your light has gone out!");
		}

		/* The light is getting dim */
		else if ((i_ptr->p1 < 40) && (randint(5) == 1) &&
			   (p_ptr->flags.blind < 1) &&
			   !(i_ptr->flags2 & TR_LIGHT)) { /* perm light doesn't dim -CFT */
		    disturb(0, 0);
		    msg_print("Your light is growing faint.");
		}
	    } else {
		if (!p_ptr->flags.light) {
		    player_light = FALSE;
		    disturb(0, 1);
		/* unlight creatures */
		    creatures(FALSE);
		}
	    }
	else if (i_ptr->p1 > 0 || p_ptr->flags.light) {
	    if (!(i_ptr->flags2 & TR_LIGHT))
		i_ptr->p1--;	   /* don't dec if perm light -CFT */
	    player_light = TRUE;
	    disturb(0, 1);
	/* light creatures */
	    creatures(FALSE);
	}

	/*** Check the Food, and Regenerate ***/

	/* Default regeneration */
	regen_amount = PLAYER_REGEN_NORMAL;

	/* Check food status	       */
	if (p_ptr->flags.food < PLAYER_FOOD_ALERT) {
	    if (p_ptr->flags.food < PLAYER_FOOD_WEAK) {

		if (p_ptr->flags.food < 0) {
		    regen_amount = 0;
		}
		else if (p_ptr->flags.food < PLAYER_FOOD_FAINT) {
		    regen_amount = PLAYER_REGEN_FAINT;
		}
		else if (p_ptr->flags.food < PLAYER_FOOD_WEAK) {
		    regen_amount = PLAYER_REGEN_WEAK;
		}

		if ((PY_WEAK & p_ptr->flags.status) == 0) {
		    p_ptr->flags.status |= PY_WEAK;
		    msg_print("You are getting weak from hunger.");
		    disturb(0, 0);
		    prt_hunger();
		}
		if ((p_ptr->flags.food < PLAYER_FOOD_FAINT) && (randint(8) == 1)) {
		    p_ptr->flags.paralysis += randint(5);
		    msg_print("You faint from the lack of food.");
		    disturb(1, 0);
		}
	    }
	    else if ((PY_HUNGRY & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_HUNGRY;
		msg_print("You are getting hungry.");
		disturb(0, 0);
		prt_hunger();
	    }
	}

	/* Food consumption */
	/* Note: Speeded up characters really burn up the food!  */
	/* now summation, not square, since spd less powerful -CFT */
       if (p_ptr->flags.speed < 0)
	   p_ptr->flags.food -=  (p_ptr->flags.speed*p_ptr->flags.speed - p_ptr->flags.speed) / 2;

	/* Digest some food */
	p_ptr->flags.food -= p_ptr->flags.food_digested;
	if (p_ptr->flags.food < 0) {
	    take_hit(-p_ptr->flags.food / 16, "starvation");	/* -CJS- */
	    disturb(1, 0);
	}

	/* Regeneration ability */
	if (p_ptr->flags.regenerate) {
	    regen_amount = regen_amount * 3 / 2;
	}

	/* Searching or Resting */
	if ((p_ptr->flags.status & PY_SEARCH) || p_ptr->flags.rest > 0 ||
	    p_ptr->flags.rest == -1 || p_ptr->flags.rest == -2) {
	    regen_amount = regen_amount * 2;
	}

	if ((p_ptr->flags.poisoned < 1) && (p_ptr->flags.cut < 1) &&
	    (p_ptr->misc.chp < p_ptr->misc.mhp))
	    regenhp(regen_amount);

	/* Regenerate the mana (even if poisoned or cut (?)) */
	if (p_ptr->misc.cmana < p_ptr->misc.mana) {
	    regenmana(regen_amount);
	}


	/*** Assorted Maladies ***/

	/* Blindness */
	if (p_ptr->flags.blind > 0) {
	    if ((PY_BLIND & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_BLIND;
		prt_map();
		prt_blind();
		disturb(0, 1);
	    /* unlight creatures */
		creatures(FALSE);
	    }
	    p_ptr->flags.blind--;
	    if (p_ptr->flags.blind == 0) {
		p_ptr->flags.status &= ~PY_BLIND;
		prt_blind();
		prt_map();
	    /* light creatures */
		disturb(0, 1);
		creatures(FALSE);
		msg_print("The veil of darkness lifts.");
	    }
	}

	/* Confusion */
	if (p_ptr->flags.confused > 0) {
	    if ((PY_CONFUSED & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_CONFUSED;
		prt_confused();
	    }
	    p_ptr->flags.confused--;
	    if (p_ptr->flags.confused == 0) {
		p_ptr->flags.status &= ~PY_CONFUSED;
		msg_print("You feel less confused now.");
		prt_confused();

		if (p_ptr->flags.rest > 0 || p_ptr->flags.rest == -1) rest_off();
	    }
	}

	/* Afraid */
	if (p_ptr->flags.afraid > 0) {
	    if ((PY_FEAR & p_ptr->flags.status) == 0) {
		if ((p_ptr->flags.shero + p_ptr->flags.hero + p_ptr->flags.resist_fear) > 0)
		    p_ptr->flags.afraid = 0;
		else {
		    p_ptr->flags.status |= PY_FEAR;
		    prt_afraid();
		}
	    } else if ((p_ptr->flags.shero + p_ptr->flags.hero + p_ptr->flags.resist_fear) > 0)
		p_ptr->flags.afraid = 1;
	    p_ptr->flags.afraid--;
	    if (p_ptr->flags.afraid == 0) {
		p_ptr->flags.status &= ~PY_FEAR;
		prt_afraid();
		msg_print("You feel bolder now.");
		disturb(0, 0);
	    }
	}

	/* Cut */
	if (p_ptr->flags.cut > 0) {
	    if (p_ptr->flags.cut>1000) {
		take_hit(3 , "a fatal wound");
		disturb(1,0);
	    }
	    else if (p_ptr->flags.cut>200) {
		take_hit(3, "a fatal wound");
		p_ptr->flags.cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else if (p_ptr->flags.cut>100) {
		take_hit(2, "a fatal wound");
		p_ptr->flags.cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else {
		take_hit(1, "a fatal wound");
		p_ptr->flags.cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    prt_cut();
	    if (p_ptr->flags.cut <= 0) {
		p_ptr->flags.cut = 0;
		if (p_ptr->misc.chp >= 0)
		    msg_print("Your wound heals.");
	    }
	}

	/* Always redraw "cuts" */
	prt_cut();

	/* Stun */
	if (p_ptr->flags.stun > 0) {
	    int                 oldstun = p_ptr->flags.stun;

	    p_ptr->flags.stun -= (con_adj() <= 0 ? 1 : (con_adj() / 2 + 1));
				/* fixes endless stun if bad con. -CFT */
	    if ((oldstun > 50) && (p_ptr->flags.stun <= 50)) { 
				/* if crossed 50 mark... */
		p_ptr->misc.ptohit += 15;
		p_ptr->misc.ptodam += 15;
		p_ptr->misc.dis_th += 15;
		p_ptr->misc.dis_td += 15;
	    }
	    if (p_ptr->flags.stun <= 0) {
		p_ptr->flags.stun = 0;
		msg_print("Your head stops stinging.");
		p_ptr->misc.ptohit += 5;
		p_ptr->misc.ptodam += 5;
		p_ptr->misc.dis_th += 5;
		p_ptr->misc.dis_td += 5;
	    }
	}

	/* Hack -- Always redraw the stun */
	prt_stun();

	/* Poisoned */
	if (p_ptr->flags.poisoned > 0) {
	    if ((PY_POISONED & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_POISONED;
		prt_poisoned();
	    }
	    p_ptr->flags.poisoned--;
	    if (p_ptr->flags.poisoned == 0 ||
		 p_ptr->flags.immune_pois ||
		p_ptr->flags.resist_pois ||
		p_ptr->flags.oppose_pois) {
		p_ptr->flags.poisoned = 0;
		p_ptr->flags.status &= ~PY_POISONED;
		prt_poisoned();
		msg_print("You feel better.");
		disturb(0, 0);
	    }
	    else {
		switch (con_adj()) {
		  case -4: i = 4; break;
		  case -3:
		  case -2: i = 3; break;
		  case -1: i = 2; break;
		  case 0: i = 1; break;
		  case 1:
		  case 2:
		  case 3: i = ((turn % 2) == 0); break;
		  case 4:
		  case 5: i = ((turn % 3) == 0); break;
		  case 6: i = ((turn % 4) == 0); break;
		  case 7: i = ((turn % 5) == 0); break;
		  default: i = ((turn % 6) == 0); break;
		}
		take_hit(i, "poison");
		disturb(1, 0);
	    }
	}

	/*** Check the Speed ***/

	/* Fast */
	if (p_ptr->flags.fast > 0) {
	    if ((PY_FAST & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_FAST;
		change_speed(-1);
		msg_print("You feel yourself moving faster.");
		disturb(0, 0);
	    }
	    p_ptr->flags.fast--;
	    if (p_ptr->flags.fast == 0) {
		p_ptr->flags.status &= ~PY_FAST;
		change_speed(1);
		msg_print("You feel yourself slow down.");
		disturb(0, 0);
	    }
	}

	/* Slow */
	if (p_ptr->flags.slow > 0) {
	    if ((PY_SLOW & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_SLOW;
		change_speed(1);
		msg_print("You feel yourself moving slower.");
		disturb(0, 0);
	    }
	    p_ptr->flags.slow--;
	    if (p_ptr->flags.slow == 0) {
		p_ptr->flags.status &= ~PY_SLOW;
		change_speed(-1);
		msg_print("You feel yourself speed up.");
		disturb(0, 0);
	    }
	}
    /* Resting is over?      */
	if (p_ptr->flags.rest > 0 || p_ptr->flags.rest == -1 || p_ptr->flags.rest == -2) {
	    if (p_ptr->flags.rest > 0) {
		p_ptr->flags.rest--;
		if (p_ptr->flags.rest == 0)	/* Resting over */
		    rest_off();
	    } else if (p_ptr->flags.rest == -1) {
		if (p_ptr->misc.chp == p_ptr->misc.mhp && p_ptr->misc.cmana == p_ptr->misc.mana) {
		    p_ptr->flags.rest = 0;
		    rest_off();
		}
	    } else if (p_ptr->flags.rest == -2) {	/* rest until
						 * blind/conf/stun/
						 * HP/mana/fear/halluc over */
		if ((p_ptr->flags.blind < 1) && (p_ptr->flags.confused < 1) &&
		    (p_ptr->flags.afraid < 1) && (p_ptr->flags.stun < 1) &&
		    (p_ptr->flags.image < 1) && (p_ptr->flags.word_recall < 1) &&
		    (p_ptr->flags.slow < 1) && (p_ptr->misc.chp == p_ptr->misc.mhp) &&
		    (p_ptr->misc.cmana == p_ptr->misc.mana)) {
		    p_ptr->flags.rest = 0;
		    rest_off();
		}
	    }
	}
    /* Check for interrupts to find or rest. */
	if ((command_count > 0 || find_flag || p_ptr->flags.rest > 0 || p_ptr->flags.rest == -1
	     || p_ptr->flags.rest == -2)
#if defined(MSDOS) || defined(VMS) /* stolen from Um55 src -CFT */
	    && kbhit()
#else
	    && (check_input(find_flag ? 0 : 10000))
#endif
	    ) {
#ifdef MSDOS
	    (void)msdos_getch();
#endif
#ifdef VMS
	/* Get and ignore the key used to interrupt resting/running.  */
	    (void)vms_getch();
#endif
	    disturb(0, 0);
	}
    /* Hallucinating?	 (Random characters appear!) */
	if (p_ptr->flags.image > 0) {
	    end_find();
	    p_ptr->flags.image--;
	    if (p_ptr->flags.image == 0)
		prt_map();	   /* Used to draw entire screen! -CJS- */
	}
    /* Paralysis	       */
	if (p_ptr->flags.paralysis > 0) {
	/* when paralysis true, you can not see any movement that occurs */
	    p_ptr->flags.paralysis--;
	    disturb(1, 0);
	}


	/*** All good things must come to an end... ***/

	/* Protection from evil counter */
	if (p_ptr->flags.protevil > 0) {
	    p_ptr->flags.protevil--;
	    if (!p_ptr->flags.protevil) {
		msg_print("You no longer feel safe from evil.");
	    }
	}

	/* Invulnerability */
	if (p_ptr->flags.invuln > 0) {
	    if ((PY_INVULN & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_INVULN;
		disturb(0, 0);
		p_ptr->misc.ptoac += 100;	/* changed to ptoac -CFT */
		p_ptr->misc.dis_tac += 100;
		msg_print("Your skin turns to steel!");
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->flags.invuln--;
	    if (p_ptr->flags.invuln == 0) {
		p_ptr->flags.status &= ~PY_INVULN;
		disturb(0, 0);
		p_ptr->misc.ptoac -= 100;	/* changed to ptoac -CFT */
		p_ptr->misc.dis_tac -= 100;
		msg_print("Your skin returns to normal.");
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Heroism */
	if (p_ptr->flags.hero > 0) {
	    if ((PY_HERO & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_HERO;
		disturb(0, 0);
		p_ptr->misc.mhp += 10;
		p_ptr->misc.chp += 10;
		p_ptr->misc.ptohit += 12;
		p_ptr->misc.dis_th += 12;
		msg_print("You feel like a HERO!");
		prt_mhp();
		prt_chp();
	    }
	    p_ptr->flags.hero--;
	    if (p_ptr->flags.hero == 0) {
		p_ptr->flags.status &= ~PY_HERO;
		disturb(0, 0);
		p_ptr->mhp -= 10;
		if (p_ptr->misc.chp > p_ptr->mhp) {
		    p_ptr->misc.chp = p_ptr->mhp;
		    p_ptr->misc.chp_frac = 0;
		    prt_chp();
		}
		p_ptr->misc.ptohit -= 12;
		p_ptr->misc.dis_th -= 12;
		msg_print("The heroism wears off.");
		prt_mhp();
	    }
	}

	/* Super Heroism */
	if (p_ptr->flags.shero > 0) {
	    if ((PY_SHERO & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_SHERO;
		disturb(0, 0);
		p_ptr->mhp += 30;
		p_ptr->misc.chp += 30;
		p_ptr->misc.ptohit += 24;
		p_ptr->misc.dis_th += 24;
		p_ptr->misc.ptoac -= 10;
		p_ptr->misc.dis_tac -= 10;
		msg_print("You feel like a killing machine!");
		prt_mhp();
		prt_chp();
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->flags.shero--;
	    if (p_ptr->flags.shero == 0) {
		p_ptr->flags.status &= ~PY_SHERO;
		disturb(0, 0);
		p_ptr->mhp -= 30;
		p_ptr->misc.ptoac += 10;
		p_ptr->misc.dis_tac += 10;
		if (p_ptr->misc.chp > p_ptr->mhp) {
		    p_ptr->misc.chp = p_ptr->mhp;
		    p_ptr->misc.chp_frac = 0;
		    prt_chp();
		}
		p_ptr->misc.ptohit -= 24;
		p_ptr->misc.dis_th -= 24;
		msg_print("You feel less Berserk.");
		prt_mhp();
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Blessed */
	if (p_ptr->flags.blessed > 0) {
	    if ((PY_BLESSED & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_BLESSED;
		disturb(0, 0);
		p_ptr->misc.ptohit += 10;
		p_ptr->misc.dis_th += 10;
		p_ptr->misc.ptoac += 5; /* changed to ptoac -CFT */
		p_ptr->misc.dis_tac += 5;
		msg_print("You feel righteous!");
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->flags.blessed--;
	    if (p_ptr->flags.blessed == 0) {
		p_ptr->flags.status &= ~PY_BLESSED;
		disturb(0, 0);
		p_ptr->misc.ptohit -= 10;
		p_ptr->misc.dis_th -= 10;
		p_ptr->misc.ptoac -= 5; /* changed to ptoac -CFT */
		p_ptr->misc.dis_tac -= 5;
		msg_print("The prayer has expired.");
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Shield */
	if (p_ptr->flags.shield > 0) {
	    p_ptr->flags.shield--;
	    if (p_ptr->flags.shield == 0) {
		disturb(0, 0);
		msg_print("Your mystic shield crumbles away.");
		p_ptr->misc.ptoac -= 50;	/* changed to ptoac -CFT */
		p_ptr->misc.dis_tac -= 50;
		p_ptr->flags.status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/*** Timed resistance must end eventually ***/

    /* Resist Heat   */
	if (p_ptr->flags.oppose_fire > 0) {
	    p_ptr->flags.oppose_fire--;
	    if (p_ptr->flags.oppose_fire == 0) {
		msg_print("You no longer feel safe from flame.");
	    }
	}

    /* Resist Cold   */
	if (p_ptr->flags.oppose_cold > 0) {
	    p_ptr->flags.oppose_cold--;
	    if (!p_ptr->flags.oppose_cold) {
		msg_print("You no longer feel safe from cold.");
	    }
	}

    /* Resist Acid   */
	if (p_ptr->flags.oppose_acid > 0) {
	    p_ptr->flags.oppose_acid--;
	    if (!p_ptr->flags.oppose_acid) {
		msg_print("You no longer feel safe from acid.");
	    }
	}
    /* Resist Lightning   */
	if (p_ptr->flags.oppose_elec > 0) {
	    p_ptr->flags.oppose_elec--;
	    if (!p_ptr->flags.oppose_elec) {
		msg_print("You no longer feel safe from lightning.");
	    }
	}

    /* Resist Poison   */
	if (p_ptr->flags.oppose_pois > 0) {
	    p_ptr->flags.oppose_pois--;
	    if (!p_ptr->flags.oppose_pois) {
		msg_print("You no longer feel safe from poison.");
		}
	}

    /* Timeout Artifacts  */
	for (i = 22; i < (INVEN_TOTAL - 1); i++) {
	    i_ptr = &inventory[i];
	    if (i_ptr->tval != TV_NOTHING && (i_ptr->flags2 & TR_ACTIVATE)) {
		if (i_ptr->timeout > 0)
		    i_ptr->timeout--;
		if ((i_ptr->tval == TV_RING) &&
		    (!stricmp(object_list[i_ptr->index].name, "Power")) &&
		    (randint(20) == 1) && (p_ptr->misc.exp > 0))
		    p_ptr->misc.exp--, p_ptr->misc.max_exp--, prt_experience();
	    }
	}

    /* Timeout rods  */
	for (i = 0; i < 22; i++) {
	    i_ptr = &inventory[i];
	    if (i_ptr->tval == TV_ROD && (i_ptr->flags2 & TR_ACTIVATE)) {
		if (i_ptr->timeout > 0)
		    i_ptr->timeout--;
	    }
	}

    /* Detect Invisible      */
	if (p_ptr->flags.detect_inv > 0) {
	    if ((PY_DET_INV & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_DET_INV;
		p_ptr->flags.see_inv = TRUE;
	    /* light but don't move creatures */
		creatures(FALSE);
	    }
	    p_ptr->flags.detect_inv--;
	    if (p_ptr->flags.detect_inv == 0) {
		p_ptr->flags.status &= ~PY_DET_INV;
	    /* may still be able to see_inv if wearing magic item */
		if (p_ptr->misc.prace == 9)
		    p_ptr->flags.see_inv = TRUE;
		else {
		    p_ptr->flags.see_inv = FALSE;	/* unless item grants it */
		    for (i = INVEN_WIELD; i <= INVEN_LITE; i++)
			if (TR_SEE_INVIS & inventory[i].flags)
			    p_ptr->flags.see_inv = TRUE;
		}
	    /* unlight but don't move creatures */
		creatures(FALSE);
	    }
	}
    /* Timed infra-vision    */
	if (p_ptr->flags.tim_infra > 0) {
	    if ((PY_TIM_INFRA & p_ptr->flags.status) == 0) {
		p_ptr->flags.status |= PY_TIM_INFRA;
		p_ptr->flags.see_infra++;
	    /* light but don't move creatures */
		creatures(FALSE);
	    }
	    p_ptr->flags.tim_infra--;
	    if (p_ptr->flags.tim_infra == 0) {
		p_ptr->flags.status &= ~PY_TIM_INFRA;
		p_ptr->flags.see_infra--;
	    /* unlight but don't move creatures */
		creatures(FALSE);
	    }
	}

	/*** Involuntary Movement ***/

	/* Word-of-Recall -- Note: Word-of-Recall is a delayed action */
	if (p_ptr->flags.word_recall > 0) {
	    if (p_ptr->flags.word_recall == 1) {
		new_level_flag = TRUE;
		p_ptr->flags.paralysis++;
		p_ptr->flags.word_recall = 0;
		if (dun_level > 0) {
		    dun_level = 0;
		    msg_print("You feel yourself yanked upwards! ");
		}
		else if (p_ptr->misc.max_dlv) {
		    dun_level = p_ptr->misc.max_dlv;
		    msg_print("You feel yourself yanked downwards! ");
		}
	    }
		else {
		p_ptr->flags.word_recall--;
		}
	}

    /* Random teleportation  */
	if ((p_ptr->flags.teleport) && (randint(100) == 1)) {
	    disturb(0, 0);
	    teleport(40);
	}
    /* See if we are too weak to handle the weapon or pack.  -CJS- */
	if (p_ptr->flags.status & PY_STR_WGT)
	    check_strength();
	if (p_ptr->flags.status & PY_STUDY)
	    prt_study();
	if (p_ptr->flags.status & PY_SPEED) {
	    p_ptr->flags.status &= ~PY_SPEED;
	    prt_speed();
	}
	if ((p_ptr->flags.status & PY_PARALYSED) && (p_ptr->flags.paralysis < 1)) {
	    prt_state();
	    p_ptr->flags.status &= ~PY_PARALYSED;
	} else if (p_ptr->flags.paralysis > 0) {
	    prt_state();
	    p_ptr->flags.status |= PY_PARALYSED;
	} else if (p_ptr->flags.rest > 0 || p_ptr->flags.rest == -1 || p_ptr->flags.rest == -2)
	    prt_state();

	if ((p_ptr->flags.status & PY_ARMOR) != 0) {
	    p_ptr->misc.dis_ac = p_ptr->misc.pac + p_ptr->misc.dis_tac;	/* use updated ac */
	    prt_pac();
	    p_ptr->flags.status &= ~PY_ARMOR;
	}
	if ((p_ptr->flags.status & PY_STATS) != 0) {
	    for (i = 0; i < 6; i++)
		if ((PY_STR << i) & p_ptr->flags.status)
		    prt_stat(i);
	    p_ptr->flags.status &= ~PY_STATS;
	}
	if (p_ptr->flags.status & PY_HP) {
	    prt_mhp();
	    prt_chp();
	    p_ptr->flags.status &= ~PY_HP;
	}
	if (p_ptr->flags.status & PY_MANA) {
	    prt_cmana();
	    p_ptr->flags.status &= ~PY_MANA;
	}
    /* Allow for a slim chance of detect enchantment -CJS- */
    /*
     * for 1st level char, check once every 2160 turns for 40th level char,
     * check once every 416 turns 
     */
	if (p_ptr->misc.pclass == 2 ?
	    ((p_ptr->flags.confused == 0) && (p_ptr->misc.pclass != 0) &&
	(randint((int)(10000 / (p_ptr->misc.lev * p_ptr->misc.lev + 40)) + 1) == 1))
	    :
	    (((turn & 0xF) == 0) && (p_ptr->flags.confused == 0)
	     && (randint((int)(10 + 750 / (5 + p_ptr->misc.lev))) == 1))
	    ) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_TOTAL; i++) {
		if (i == inven_ctr)
		    i = 22;
		i_ptr = &inventory[i];

	    /*
	     * if in inventory, succeed 1 out of 50 times, if in equipment
	     * list, success 1 out of 10 times, unless you're a priest or
	     * rogue... 
	     */
		if (((i_ptr->tval >= TV_MIN_WEAR) && (i_ptr->tval <= TV_MAX_WEAR)) &&
		    special_check(i_ptr) &&
		    ((p_ptr->misc.pclass == 2 || p_ptr->misc.pclass == 3) ?
		     (randint(i < 22 ? 5 : 1) == 1) :
		     (randint(i < 22 ? 50 : 10) == 1))) {

		    if (p_ptr->misc.pclass == 0 || p_ptr->misc.pclass == 3 ||
			p_ptr->misc.pclass == 5)
			if ((i_ptr->tval == TV_SWORD) ||
			    (i_ptr->tval == TV_HAFTED) ||
			    (i_ptr->tval == TV_POLEARM) ||
			    (i_ptr->tval == TV_BOW) ||
			    (i_ptr->tval == TV_BOLT) ||
			    (i_ptr->tval == TV_ARROW) ||
			    (i_ptr->tval == TV_DIGGING) ||
			    (i_ptr->tval == TV_SHOT) ||
			    (i_ptr->tval == TV_SOFT_ARMOR) ||
			    (i_ptr->tval == TV_HARD_ARMOR) ||
			    (i_ptr->tval == TV_HELM) ||
			    (i_ptr->tval == TV_BOOTS) ||
			    (i_ptr->tval == TV_CLOAK) ||
			    (i_ptr->tval == TV_GLOVES) ||
			    (i_ptr->tval == TV_SHIELD))
			    continue;
		    (void)sprintf(tmp_str,
			    "There's something %s about what you are %s...",
				  special_check(i_ptr) > 0 ? "good" : "bad",
				  describe_use(i));
		    disturb(0, 0);
		    msg_print(tmp_str);
		    add_inscribe(i_ptr, (special_check(i_ptr) > 0) ?
				 ID_MAGIK : ID_DAMD);
		}
	    }
	}
    /* Warriors, Rogues and paladins inbuilt ident */
	if (((p_ptr->misc.pclass == 0) && (p_ptr->flags.confused == 0) &&
	 (randint((int)(9000 / (p_ptr->misc.lev * p_ptr->misc.lev + 40)) + 1) == 1))
	    ||
	    ((p_ptr->misc.pclass == 3) && (p_ptr->flags.confused == 0) &&
	(randint((int)(20000 / (p_ptr->misc.lev * p_ptr->misc.lev + 40)) + 1) == 1))
	    ||
	    ((p_ptr->misc.pclass == 5) && (p_ptr->flags.confused == 0) &&
	     (randint((int)(80000L / (p_ptr->misc.lev * p_ptr->misc.lev + 40)) + 1) == 1))) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_TOTAL; i++) {
		if (i == inven_ctr)
		    i = 22;
		i_ptr = &inventory[i];
	    /*
	     * if in inventory, succeed 1 out of 5 times, if in equipment
	     * list, always succeed! 
	     */
		if (((i_ptr->tval == TV_SOFT_ARMOR) ||
		     (i_ptr->tval == TV_HARD_ARMOR) ||
		     (i_ptr->tval == TV_SWORD) ||
		     (i_ptr->tval == TV_HAFTED) ||
		     (i_ptr->tval == TV_POLEARM) ||
		     (i_ptr->tval == TV_SHIELD) ||
		     (i_ptr->tval == TV_HELM) ||
		     (i_ptr->tval == TV_BOOTS) ||
		     (i_ptr->tval == TV_GLOVES) ||
		     (i_ptr->tval == TV_DIGGING) ||
		     (i_ptr->tval == TV_SHOT) ||
		     (i_ptr->tval == TV_BOLT) ||
		     (i_ptr->tval == TV_ARROW) ||
		     (i_ptr->tval == TV_BOW) ||
		     (i_ptr->tval == TV_CLOAK))
		    && value_check(i_ptr) &&
		    (randint(i < 22 ? 5 : 1) == 1)) {
		    char                out_val[100], tmp[100], *ptr;
		    int                 sp;

		    (void)strcpy(tmp, object_list[i_ptr->index].name);

		    ptr = tmp;
		    sp = 0;
		    while (tmp[sp] == ' ' || tmp[sp] == '&')
			ptr = &tmp[++sp];

		    (void)strcpy(out_val, ptr);

		    ptr = out_val;

		    while (*ptr) {
			if (*ptr == '~')
			    *ptr = 's';
			ptr++;
		    }

		    (void)sprintf(tmp_str,
				  "You feel the %s (%c) you are %s %s %s...",
				  out_val,
		    ((i < INVEN_WIELD) ? i + 'a' : (i + 'a' - INVEN_WIELD)),
				  describe_use(i),
				  ((i_ptr->tval == TV_BOLT) ||
				   (i_ptr->tval == TV_ARROW) ||
				   (i_ptr->tval == TV_SHOT) ||
				   (i_ptr->tval == TV_BOOTS) ||
				 (i_ptr->tval == TV_GLOVES)) ? "are" : "is",
				  value_check(i_ptr));
		    disturb(0, 0);
		    msg_print(tmp_str);
		    if (!stricmp(value_check(i_ptr), "terrible"))
			add_inscribe(i_ptr, ID_DAMD);
		    else if (!stricmp(value_check(i_ptr), "worthless"))
			add_inscribe(i_ptr, ID_DAMD);
		    else
			inscribe(i_ptr, value_check(i_ptr));
		}
	    }
	}
    /*
     * Check the state of the monster list, and delete some monsters if the
     * monster list is nearly full.  This helps to avoid problems in
     * creature.c when monsters try to multiply.  Compact_monsters() is much
     * more likely to succeed if called from here, than if called from within
     * creatures().  
     */
	if (MAX_M_IDX - mfptr < 10)
	    (void)compact_monsters();

	if ((p_ptr->flags.paralysis < 1) &&	/* Accept a command?     */
	    (p_ptr->flags.rest == 0) &&
	    (p_ptr->flags.stun < 100) &&
	    (!death))
	/* Accept a command and execute it				 */
	{
	    do {
		if (p_ptr->flags.status & PY_REPEAT)
		    prt_state();
		default_dir = FALSE;
		free_turn_flag = FALSE;

		if ((old_lite >= 0) && (!find_flag)) {
		    cur_lite = old_lite;
		    old_lite = (-1);
		}
		if (find_flag) {
		    find_run();
		    find_count--;
		    if (find_count == 0)
			end_find();
		    put_qio();
		} else if (doing_inven)
		    inven_command(doing_inven);
		else {
		/* move the cursor to the players character */
		    move_cursor_relative(char_row, char_col);
		    if (command_count > 0) {
			command_count--;
			msg_flag = FALSE;
			default_dir = TRUE;
		    } else {
#ifdef TARGET
/* This bit of targetting code taken from Morgul -CFT */
/* If we are in targetting mode, with a creature target, make the targetted */
/* row and column match the creature's.  This optimizes a lot of code.  CDW */
			if ((target_mode)&&(target_mon<(unsigned) mfptr)) {
			    target_row = m_list[target_mon].fy;
			    target_col = m_list[target_mon].fx;
			}
#endif
			msg_flag = FALSE;
			command = inkey();
			i = 0;
		    /* Get a count for a command. */
			if ((rogue_like_commands
			     && command >= '0' && command <= '9')
			    || (!rogue_like_commands && command == '#')) {
			    char                tmp[8];

			    prt("Repeat count:", 0, 0);
			    if (command == '#')
				command = '0';
			    i = 0;
			    while (TRUE) {
				if (command == DELETE || command == CTRL('H')) {
				    i = i / 10;
				    (void)sprintf(tmp, "%d", i);
				    prt(tmp, 0, 14);
				} else if (command >= '0' && command <= '9') {
				    if (i > 99)
					bell();
				    else {
					i = i * 10 + command - '0';
					(void)sprintf(tmp, "%d", i);
					prt(tmp, 0, 14);
				    }
				} else
				    break;
				command = inkey();
			    }
			    if (i == 0) {
				i = 99;
				(void)sprintf(tmp, "%d", i);
				prt(tmp, 0, 14);
			    }
			/* a special hack to allow numbers as commands */
			    if (command == ' ') {
				prt("Command:", 0, 20);
				command = inkey();
			    }
			}
		    /* Another way of typing control codes -CJS- */
			if (command == '^') {
			    if (command_count > 0)
				prt_state();
			    if (get_com("Control-", &command)) {
				if (command >= 'A' && command <= 'Z')
				    command -= 'A' - 1;
				else if (command >= 'a' && command <= 'z')
				    command -= 'a' - 1;
				else {
				    msg_print("Type ^ <letter> for a control char");
				    command = ' ';
				}
			    } else
				command = ' ';
			}
		    /* move cursor to player char again, in case it moved */
			move_cursor_relative(char_row, char_col);
		    /* Commands are always converted to rogue form. -CJS- */
			if (rogue_like_commands == FALSE)
			    command = original_commands(command);
			if (i > 0) {
			    if (!valid_countcommand(command)) {
				free_turn_flag = TRUE;
				msg_print("Invalid command with a count.");
				command = ' ';
			    } else {
				command_count = i;
				prt_state();
				command_count--;	/* count this pass as
							 * one */
			    }
			}
		    }
		/* Flash the message line. */
		    erase_line(MSG_LINE, 0);
		    move_cursor_relative(char_row, char_col);
		    put_qio();

		    do_command(command);
		/* Find is counted differently, as the command changes. */
		    if (find_flag) {
			find_count = command_count;
			command_count = 0;
		    } else if (old_lite >= 0) {
			cur_lite = old_lite;
			old_lite = (-1);
		    }
		    if (free_turn_flag)
			command_count = 0;
		}
	    /* End of commands				     */
	    }
	    while (free_turn_flag && !new_level_flag && !eof_flag);
	} else {
	/* if paralyzed, resting, or dead, flush output */
	/* but first move the cursor onto the player, for aesthetics */
	    move_cursor_relative(char_row, char_col);
	    put_qio();
	}

    /* Teleport?		       */
	if (teleport_flag)
	    teleport(100);
    /* Move the creatures	       */
	if (!new_level_flag)
	    creatures(TRUE);
    /* Exit when new_level_flag is set   */
    }
    while (!new_level_flag && !eof_flag);
}


