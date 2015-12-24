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
static cptr value_check(register inven_type *t_ptr)
{
    /* Paranoia -- No item */
    if (t_ptr->tval == TV_NOTHING) return 0;
    if (known2_p(t_ptr)) return 0;
    if (store_bought_p(t_ptr)) return 0;
    if (t_ptr->ident & ID_MAGIK) return 0;
    if (t_ptr->ident & ID_DAMD) return 0;
    if (t_ptr->inscrip[0] != '\0') return 0;
    if (t_ptr->flags & TR_CURSED && t_ptr->name2 == SN_NULL) return "worthless";
    if (t_ptr->flags & TR_CURSED && t_ptr->name2 != SN_NULL) return "terrible";
    if ((t_ptr->tval == TV_DIGGING) &&  /* also, good digging tools -CFT */
	(t_ptr->flags & TR_TUNNEL) &&
	(t_ptr->p1 > object_list[t_ptr->index].p1)) /* better than normal for this
						       type of shovel/pick? -CFT */
	return "good";
    if ((t_ptr->tohit<=0 && t_ptr->todam<=0 && t_ptr->toac<=0) &&
	t_ptr->name2==SN_NULL)  /* normal shovels will also reach here -CFT */
	return "average";
    if (t_ptr->name2 == SN_NULL)
	return "good";
    if ((t_ptr->name2 == SN_R) || (t_ptr->name2 == SN_RA) ||
	(t_ptr->name2 == SN_RF) || (t_ptr->name2 == SN_RC) ||
	(t_ptr->name2 == SN_RL) || (t_ptr->name2 == SN_SE) ||
	(t_ptr->name2 == SN_HA) || (t_ptr->name2 == SN_FT) ||
	(t_ptr->name2 == SN_DF) || (t_ptr->name2 == SN_FB) ||
	(t_ptr->name2 == SN_SA) || (t_ptr->name2 == SN_FREE_ACTION) ||
	(t_ptr->name2 == SN_SD) || (t_ptr->name2 == SN_SLAYING) ||
	(t_ptr->name2 == SN_SU) || (t_ptr->name2 == SN_SLOW_DESCENT) ||
	(t_ptr->name2 == SN_SPEED) || (t_ptr->name2 == SN_STEALTH) ||
	(t_ptr->name2 == SN_INTELLIGENCE) || (t_ptr->name2 == SN_WISDOM) ||
	(t_ptr->name2 == SN_INFRAVISION) || (t_ptr->name2 == SN_MIGHT) ||
	(t_ptr->name2 == SN_LORDLINESS) || (t_ptr->name2 == SN_MAGI) ||
	(t_ptr->name2 == SN_BEAUTY) || (t_ptr->name2 == SN_SEEING) ||
     (t_ptr->name2 == SN_REGENERATION) || (t_ptr->name2 == SN_PROTECTION) ||
	(t_ptr->name2 == SN_FIRE) || (t_ptr->name2 == SN_SLAY_EVIL) ||
	(t_ptr->name2 == SN_DRAGON_SLAYING) || (t_ptr->name2 == SN_SLAY_ANIMAL) ||
	(t_ptr->name2 == SN_ACCURACY) || (t_ptr->name2 == SN_SO) ||
	(t_ptr->name2 == SN_POWER) || (t_ptr->name2 == SN_WEST) ||
	(t_ptr->name2 == SN_SDEM) || (t_ptr->name2 == SN_ST) ||
	(t_ptr->name2 == SN_LIGHT) || (t_ptr->name2 == SN_AGILITY) ||
	(t_ptr->name2 == SN_SG) || (t_ptr->name2 == SN_TELEPATHY) ||
	(t_ptr->name2 == SN_DRAGONKIND) || (t_ptr->name2 == SN_AMAN) ||
	(t_ptr->name2 == SN_ELVENKIND) || (t_ptr->name2 == SN_WOUNDING) ||
	(t_ptr->name2 == SN_BLESS_BLADE) || (t_ptr->name2 == SN_ATTACKS))
	return "excellent";
    return "special";
}


/*
 *  Regenerate hit points				-RAK-	 
 */
static void regenhp(int percent)
{
    register struct misc *p_ptr;
    register s32b        new_chp, new_chp_frac;
    int                   old_chp;

    p_ptr = &py.misc;
    old_chp = p_ptr->chp;
    new_chp = ((long)p_ptr->mhp) * percent + PLAYER_REGEN_HPBASE;
    p_ptr->chp += new_chp >> 16;   /* div 65536 */

    /* check for overflow */
    if (p_ptr->chp < 0 && old_chp > 0) p_ptr->chp = MAX_SHORT;
    new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;	/* mod 65536 */
    if (new_chp_frac >= 0x10000L) {
	p_ptr->chp_frac = new_chp_frac - 0x10000L;
	p_ptr->chp++;
    }
    else {
	p_ptr->chp_frac = new_chp_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->chp >= p_ptr->mhp) {
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;
    }

    /* Notice changes */
    if (old_chp != p_ptr->chp) prt_chp();
}


/* 
 * Regenerate mana points				-RAK-	 
 */
static void regenmana(int percent)
{
    register struct misc *p_ptr;
    register s32b        new_mana, new_mana_frac;
    int                   old_cmana;

    p_ptr = &py.misc;
    old_cmana = p_ptr->cmana;
    new_mana = ((long)p_ptr->mana) * percent + PLAYER_REGEN_MNBASE;
    p_ptr->cmana += new_mana >> 16;	/* div 65536 */
    /* check for overflow */
    if (p_ptr->cmana < 0 && old_cmana > 0) {
	p_ptr->cmana = MAX_SHORT;
    }
    new_mana_frac = (new_mana & 0xFFFF) + p_ptr->cmana_frac;	/* mod 65536 */
    if (new_mana_frac >= 0x10000L) {
	p_ptr->cmana_frac = new_mana_frac - 0x10000L;
	p_ptr->cmana++;
    }
    else {
	p_ptr->cmana_frac = new_mana_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->cmana >= p_ptr->mana) {
	p_ptr->cmana = p_ptr->mana;
	p_ptr->cmana_frac = 0;
    }
    
    /* Redraw mana */
    if (old_cmana != p_ptr->cmana) prt_cmana();
}



static void regen_monsters(void)
{
    register int i;
    int          frac;

    /* Regenerate everyone */
    for (i = 0; i < MAX_MALLOC; i++) {

	if (m_list[i].hp >= 0) {
	    if (m_list[i].maxhp == 0) {	/* then we're just going to fix it!  -CFT */
		if ((c_list[m_list[i].mptr].cdefense & MAX_HP) || be_nasty)
		    m_list[i].maxhp = max_hp(c_list[m_list[i].mptr].hd);
		else
		    m_list[i].maxhp = pdamroll(c_list[m_list[i].mptr].hd);
	    }
	/* Allow regeneration */
	    if (m_list[i].hp < m_list[i].maxhp)
		m_list[i].hp += ((frac = 2 * m_list[i].maxhp / 100) > 0) ? frac : 1;
	    if (m_list[i].hp > m_list[i].maxhp)
		m_list[i].hp = m_list[i].maxhp;
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

    register struct misc  *p_ptr;
    register inven_type   *i_ptr;
    register struct flags *f_ptr;

    /* Regenerate hp and mana */
    int                    regen_amount;

    /* Last command           */
    char                   command;

    /* init pointers. */
    f_ptr = &py.flags;
    p_ptr = &py.misc;
    i_ptr = &inventory[INVEN_LIGHT];

/* Check light status for setup	   */
    if (i_ptr->p1 > 0 || f_ptr->light)
	player_light = TRUE;
    else
	player_light = FALSE;
    if (i_ptr->tval == TV_LIGHT)
	i = i_ptr->subval;
    else
	i = 195;
    cur_lite = 1 + (i < 190) + (i == 4 || i == 6);

/* Check for a maximum level		   */
/* Added check to avoid -50' being "deepest", since max_dlv unsigned -CFT */
    if ((dun_level >= 0) && ((unsigned) dun_level > p_ptr->max_dlv))
	p_ptr->max_dlv = dun_level;

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
	    cur_pos = popt();
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
    if (py.flags.status & PY_SEARCH)
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
	if (randint(MAX_MALLOC_CHANCE) == 1) {
	    alloc_monster(1, MAX_SIGHT, FALSE);
	}

	/* Check for creature regeneration */
	if (!(turn % 20)) regen_monsters();


	/*** Handle the Lights ***/

	/* Check light status */
	i_ptr = &inventory[INVEN_LIGHT];

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
			   (py.flags.blind < 1) &&
			   !(i_ptr->flags2 & TR_LIGHT)) { /* perm light doesn't dim -CFT */
		    disturb(0, 0);
		    msg_print("Your light is growing faint.");
		}
	    } else {
		if (!f_ptr->light) {
		    player_light = FALSE;
		    disturb(0, 1);
		/* unlight creatures */
		    creatures(FALSE);
		}
	    }
	else if (i_ptr->p1 > 0 || f_ptr->light) {
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
	if (f_ptr->food < PLAYER_FOOD_ALERT) {
	    if (f_ptr->food < PLAYER_FOOD_WEAK) {

		if (f_ptr->food < 0) {
		    regen_amount = 0;
		}
		else if (f_ptr->food < PLAYER_FOOD_FAINT) {
		    regen_amount = PLAYER_REGEN_FAINT;
		}
		else if (f_ptr->food < PLAYER_FOOD_WEAK) {
		    regen_amount = PLAYER_REGEN_WEAK;
		}

		if ((PY_WEAK & f_ptr->status) == 0) {
		    f_ptr->status |= PY_WEAK;
		    msg_print("You are getting weak from hunger.");
		    disturb(0, 0);
		    prt_hunger();
		}
		if ((f_ptr->food < PLAYER_FOOD_FAINT) && (randint(8) == 1)) {
		    f_ptr->paralysis += randint(5);
		    msg_print("You faint from the lack of food.");
		    disturb(1, 0);
		}
	    }
	    else if ((PY_HUNGRY & f_ptr->status) == 0) {
		f_ptr->status |= PY_HUNGRY;
		msg_print("You are getting hungry.");
		disturb(0, 0);
		prt_hunger();
	    }
	}

	/* Food consumption */
	/* Note: Speeded up characters really burn up the food!  */
	/* now summation, not square, since spd less powerful -CFT */
       if (f_ptr->speed < 0)
	   f_ptr->food -=  (f_ptr->speed*f_ptr->speed - f_ptr->speed) / 2;

	/* Digest some food */
	f_ptr->food -= f_ptr->food_digested;
	if (f_ptr->food < 0) {
	    take_hit(-f_ptr->food / 16, "starvation");	/* -CJS- */
	    disturb(1, 0);
	}

	/* Regeneration ability */
	if (f_ptr->regenerate) {
	    regen_amount = regen_amount * 3 / 2;
	}

	/* Searching or Resting */
	if ((py.flags.status & PY_SEARCH) || f_ptr->rest > 0 ||
	    f_ptr->rest == -1 || f_ptr->rest == -2) {
	    regen_amount = regen_amount * 2;
	}

	if ((py.flags.poisoned < 1) && (py.flags.cut < 1) &&
	    (p_ptr->chp < p_ptr->mhp))
	    regenhp(regen_amount);

	/* Regenerate the mana (even if poisoned or cut (?)) */
	if (p_ptr->cmana < p_ptr->mana) {
	    regenmana(regen_amount);
	}


	/*** Assorted Maladies ***/

	/* Blindness */
	if (f_ptr->blind > 0) {
	    if ((PY_BLIND & f_ptr->status) == 0) {
		f_ptr->status |= PY_BLIND;
		prt_map();
		prt_blind();
		disturb(0, 1);
	    /* unlight creatures */
		creatures(FALSE);
	    }
	    f_ptr->blind--;
	    if (f_ptr->blind == 0) {
		f_ptr->status &= ~PY_BLIND;
		prt_blind();
		prt_map();
	    /* light creatures */
		disturb(0, 1);
		creatures(FALSE);
		msg_print("The veil of darkness lifts.");
	    }
	}

	/* Confusion */
	if (f_ptr->confused > 0) {
	    if ((PY_CONFUSED & f_ptr->status) == 0) {
		f_ptr->status |= PY_CONFUSED;
		prt_confused();
	    }
	    f_ptr->confused--;
	    if (f_ptr->confused == 0) {
		f_ptr->status &= ~PY_CONFUSED;
		msg_print("You feel less confused now.");
		prt_confused();

		if (py.flags.rest > 0 || py.flags.rest == -1) rest_off();
	    }
	}

	/* Afraid */
	if (f_ptr->afraid > 0) {
	    if ((PY_FEAR & f_ptr->status) == 0) {
		if ((f_ptr->shero + f_ptr->hero + py.flags.fear_resist) > 0)
		    f_ptr->afraid = 0;
		else {
		    f_ptr->status |= PY_FEAR;
		    prt_afraid();
		}
	    } else if ((f_ptr->shero + f_ptr->hero + py.flags.fear_resist) > 0)
		f_ptr->afraid = 1;
	    f_ptr->afraid--;
	    if (f_ptr->afraid == 0) {
		f_ptr->status &= ~PY_FEAR;
		prt_afraid();
		msg_print("You feel bolder now.");
		disturb(0, 0);
	    }
	}

	/* Cut */
	if (f_ptr->cut > 0) {
	    if (f_ptr->cut>1000) {
		take_hit(3 , "a fatal wound");
		disturb(1,0);
	    }
	    else if (f_ptr->cut>200) {
		take_hit(3, "a fatal wound");
		f_ptr->cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else if (f_ptr->cut>100) {
		take_hit(2, "a fatal wound");
		f_ptr->cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else {
		take_hit(1, "a fatal wound");
		f_ptr->cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    prt_cut();
	    if (f_ptr->cut <= 0) {
		f_ptr->cut = 0;
		if (py.misc.chp >= 0)
		    msg_print("Your wound heals.");
	    }
	}

	/* Always redraw "cuts" */
	prt_cut();

	/* Stun */
	if (f_ptr->stun > 0) {
	    int                 oldstun = f_ptr->stun;

	    f_ptr->stun -= (con_adj() <= 0 ? 1 : (con_adj() / 2 + 1));
				/* fixes endless stun if bad con. -CFT */
	    if ((oldstun > 50) && (f_ptr->stun <= 50)) { 
				/* if crossed 50 mark... */
		p_ptr->ptohit += 15;
		p_ptr->ptodam += 15;
		p_ptr->dis_th += 15;
		p_ptr->dis_td += 15;
	    }
	    if (f_ptr->stun <= 0) {
		f_ptr->stun = 0;
		msg_print("Your head stops stinging.");
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
		p_ptr->dis_th += 5;
		p_ptr->dis_td += 5;
	    }
	}

	/* Hack -- Always redraw the stun */
	prt_stun();

	/* Poisoned */
	if (f_ptr->poisoned > 0) {
	    if ((PY_POISONED & f_ptr->status) == 0) {
		f_ptr->status |= PY_POISONED;
		prt_poisoned();
	    }
	    f_ptr->poisoned--;
	    if (f_ptr->poisoned == 0 ||
		 f_ptr->poison_im ||
		f_ptr->poison_resist ||
		f_ptr->resist_poison) {
		f_ptr->poisoned = 0;
		f_ptr->status &= ~PY_POISONED;
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
	if (f_ptr->fast > 0) {
	    if ((PY_FAST & f_ptr->status) == 0) {
		f_ptr->status |= PY_FAST;
		change_speed(-1);
		msg_print("You feel yourself moving faster.");
		disturb(0, 0);
	    }
	    f_ptr->fast--;
	    if (f_ptr->fast == 0) {
		f_ptr->status &= ~PY_FAST;
		change_speed(1);
		msg_print("You feel yourself slow down.");
		disturb(0, 0);
	    }
	}

	/* Slow */
	if (f_ptr->slow > 0) {
	    if ((PY_SLOW & f_ptr->status) == 0) {
		f_ptr->status |= PY_SLOW;
		change_speed(1);
		msg_print("You feel yourself moving slower.");
		disturb(0, 0);
	    }
	    f_ptr->slow--;
	    if (f_ptr->slow == 0) {
		f_ptr->status &= ~PY_SLOW;
		change_speed(-1);
		msg_print("You feel yourself speed up.");
		disturb(0, 0);
	    }
	}
    /* Resting is over?      */
	if (f_ptr->rest > 0 || f_ptr->rest == -1 || f_ptr->rest == -2) {
	    if (f_ptr->rest > 0) {
		f_ptr->rest--;
		if (f_ptr->rest == 0)	/* Resting over */
		    rest_off();
	    } else if (f_ptr->rest == -1) {
		if (py.misc.chp == py.misc.mhp && py.misc.cmana == py.misc.mana) {
		    f_ptr->rest = 0;
		    rest_off();
		}
	    } else if (f_ptr->rest == -2) {	/* rest until
						 * blind/conf/stun/
						 * HP/mana/fear/halluc over */
		if ((py.flags.blind < 1) && (py.flags.confused < 1) &&
		    (py.flags.afraid < 1) && (py.flags.stun < 1) &&
		    (py.flags.image < 1) && (py.flags.word_recall < 1) &&
		    (py.flags.slow < 1) && (py.misc.chp == py.misc.mhp) &&
		    (py.misc.cmana == py.misc.mana)) {
		    f_ptr->rest = 0;
		    rest_off();
		}
	    }
	}
    /* Check for interrupts to find or rest. */
	if ((command_count > 0 || find_flag || f_ptr->rest > 0 || f_ptr->rest == -1
	     || f_ptr->rest == -2)
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
	if (f_ptr->image > 0) {
	    end_find();
	    f_ptr->image--;
	    if (f_ptr->image == 0)
		prt_map();	   /* Used to draw entire screen! -CJS- */
	}
    /* Paralysis	       */
	if (f_ptr->paralysis > 0) {
	/* when paralysis true, you can not see any movement that occurs */
	    f_ptr->paralysis--;
	    disturb(1, 0);
	}


	/*** All good things must come to an end... ***/

	/* Protection from evil counter */
	if (f_ptr->protevil > 0) {
	    f_ptr->protevil--;
	    if (!f_ptr->protevil) {
		msg_print("You no longer feel safe from evil.");
	    }
	}

	/* Invulnerability */
	if (f_ptr->invuln > 0) {
	    if ((PY_INVULN & f_ptr->status) == 0) {
		f_ptr->status |= PY_INVULN;
		disturb(0, 0);
		py.misc.ptoac += 100;	/* changed to ptoac -CFT */
		py.misc.dis_tac += 100;
		msg_print("Your skin turns to steel!");
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    f_ptr->invuln--;
	    if (f_ptr->invuln == 0) {
		f_ptr->status &= ~PY_INVULN;
		disturb(0, 0);
		py.misc.ptoac -= 100;	/* changed to ptoac -CFT */
		py.misc.dis_tac -= 100;
		msg_print("Your skin returns to normal.");
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Heroism */
	if (f_ptr->hero > 0) {
	    if ((PY_HERO & f_ptr->status) == 0) {
		f_ptr->status |= PY_HERO;
		disturb(0, 0);
		p_ptr->mhp += 10;
		p_ptr->chp += 10;
		p_ptr->ptohit += 12;
		p_ptr->dis_th += 12;
		msg_print("You feel like a HERO!");
		prt_mhp();
		prt_chp();
	    }
	    f_ptr->hero--;
	    if (f_ptr->hero == 0) {
		f_ptr->status &= ~PY_HERO;
		disturb(0, 0);
		p_ptr->mhp -= 10;
		if (p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		    p_ptr->chp_frac = 0;
		    prt_chp();
		}
		p_ptr->ptohit -= 12;
		p_ptr->dis_th -= 12;
		msg_print("The heroism wears off.");
		prt_mhp();
	    }
	}

	/* Super Heroism */
	if (f_ptr->shero > 0) {
	    if ((PY_SHERO & f_ptr->status) == 0) {
		f_ptr->status |= PY_SHERO;
		disturb(0, 0);
		p_ptr->mhp += 30;
		p_ptr->chp += 30;
		p_ptr->ptohit += 24;
		p_ptr->dis_th += 24;
		p_ptr->ptoac -= 10;
		p_ptr->dis_tac -= 10;
		msg_print("You feel like a killing machine!");
		prt_mhp();
		prt_chp();
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    f_ptr->shero--;
	    if (f_ptr->shero == 0) {
		f_ptr->status &= ~PY_SHERO;
		disturb(0, 0);
		p_ptr->mhp -= 30;
		p_ptr->ptoac += 10;
		p_ptr->dis_tac += 10;
		if (p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		    p_ptr->chp_frac = 0;
		    prt_chp();
		}
		p_ptr->ptohit -= 24;
		p_ptr->dis_th -= 24;
		msg_print("You feel less Berserk.");
		prt_mhp();
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Blessed */
	if (f_ptr->blessed > 0) {
	    if ((PY_BLESSED & f_ptr->status) == 0) {
		f_ptr->status |= PY_BLESSED;
		disturb(0, 0);
		p_ptr->ptohit += 10;
		p_ptr->dis_th += 10;
		p_ptr->ptoac += 5; /* changed to ptoac -CFT */
		p_ptr->dis_tac += 5;
		msg_print("You feel righteous!");
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    f_ptr->blessed--;
	    if (f_ptr->blessed == 0) {
		f_ptr->status &= ~PY_BLESSED;
		disturb(0, 0);
		p_ptr->ptohit -= 10;
		p_ptr->dis_th -= 10;
		p_ptr->ptoac -= 5; /* changed to ptoac -CFT */
		p_ptr->dis_tac -= 5;
		msg_print("The prayer has expired.");
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Shield */
	if (f_ptr->shield > 0) {
	    f_ptr->shield--;
	    if (f_ptr->shield == 0) {
		disturb(0, 0);
		msg_print("Your mystic shield crumbles away.");
		py.misc.ptoac -= 50;	/* changed to ptoac -CFT */
		py.misc.dis_tac -= 50;
		f_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/*** Timed resistance must end eventually ***/

    /* Resist Heat   */
	if (f_ptr->resist_heat > 0) {
	    f_ptr->resist_heat--;
	    if (f_ptr->resist_heat == 0) {
		msg_print("You no longer feel safe from flame.");
	    }
	}

    /* Resist Cold   */
	if (f_ptr->resist_cold > 0) {
	    f_ptr->resist_cold--;
	    if (!f_ptr->resist_cold) {
		msg_print("You no longer feel safe from cold.");
	    }
	}

    /* Resist Acid   */
	if (f_ptr->resist_acid > 0) {
	    f_ptr->resist_acid--;
	    if (!f_ptr->resist_acid) {
		msg_print("You no longer feel safe from acid.");
	    }
	}
    /* Resist Lightning   */
	if (f_ptr->resist_light > 0) {
	    f_ptr->resist_light--;
	    if (!f_ptr->resist_light) {
		msg_print("You no longer feel safe from lightning.");
	    }
	}

    /* Resist Poison   */
	if (f_ptr->resist_poison > 0) {
	    f_ptr->resist_poison--;
	    if (!f_ptr->resist_poison) {
		msg_print("You no longer feel safe from poison.");
		}
	}

    /* Timeout Artifacts  */
	for (i = 22; i < (INVEN_ARRAY_SIZE - 1); i++) {
	    i_ptr = &inventory[i];
	    if (i_ptr->tval != TV_NOTHING && (i_ptr->flags2 & TR_ACTIVATE)) {
		if (i_ptr->timeout > 0)
		    i_ptr->timeout--;
		if ((i_ptr->tval == TV_RING) &&
		    (!stricmp(object_list[i_ptr->index].name, "Power")) &&
		    (randint(20) == 1) && (py.misc.exp > 0))
		    py.misc.exp--, py.misc.max_exp--, prt_experience();
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
	if (f_ptr->detect_inv > 0) {
	    if ((PY_DET_INV & f_ptr->status) == 0) {
		f_ptr->status |= PY_DET_INV;
		f_ptr->see_inv = TRUE;
	    /* light but don't move creatures */
		creatures(FALSE);
	    }
	    f_ptr->detect_inv--;
	    if (f_ptr->detect_inv == 0) {
		f_ptr->status &= ~PY_DET_INV;
	    /* may still be able to see_inv if wearing magic item */
		if (py.misc.prace == 9)
		    f_ptr->see_inv = TRUE;
		else {
		    f_ptr->see_inv = FALSE;	/* unless item grants it */
		    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++)
			if (TR_SEE_INVIS & inventory[i].flags)
			    f_ptr->see_inv = TRUE;
		}
	    /* unlight but don't move creatures */
		creatures(FALSE);
	    }
	}
    /* Timed infra-vision    */
	if (f_ptr->tim_infra > 0) {
	    if ((PY_TIM_INFRA & f_ptr->status) == 0) {
		f_ptr->status |= PY_TIM_INFRA;
		f_ptr->see_infra++;
	    /* light but don't move creatures */
		creatures(FALSE);
	    }
	    f_ptr->tim_infra--;
	    if (f_ptr->tim_infra == 0) {
		f_ptr->status &= ~PY_TIM_INFRA;
		f_ptr->see_infra--;
	    /* unlight but don't move creatures */
		creatures(FALSE);
	    }
	}

	/*** Involuntary Movement ***/

	/* Word-of-Recall -- Note: Word-of-Recall is a delayed action */
	if (f_ptr->word_recall > 0) {
	    if (f_ptr->word_recall == 1) {
		new_level_flag = TRUE;
		f_ptr->paralysis++;
		f_ptr->word_recall = 0;
		if (dun_level > 0) {
		    dun_level = 0;
		    msg_print("You feel yourself yanked upwards! ");
		}
		else if (py.misc.max_dlv) {
		    dun_level = py.misc.max_dlv;
		    msg_print("You feel yourself yanked downwards! ");
		}
	    }
		else {
		f_ptr->word_recall--;
		}
	}

    /* Random teleportation  */
	if ((py.flags.teleport) && (randint(100) == 1)) {
	    disturb(0, 0);
	    teleport(40);
	}
    /* See if we are too weak to handle the weapon or pack.  -CJS- */
	if (py.flags.status & PY_STR_WGT)
	    check_strength();
	if (py.flags.status & PY_STUDY)
	    prt_study();
	if (py.flags.status & PY_SPEED) {
	    py.flags.status &= ~PY_SPEED;
	    prt_speed();
	}
	if ((py.flags.status & PY_PARALYSED) && (py.flags.paralysis < 1)) {
	    prt_state();
	    py.flags.status &= ~PY_PARALYSED;
	} else if (py.flags.paralysis > 0) {
	    prt_state();
	    py.flags.status |= PY_PARALYSED;
	} else if (py.flags.rest > 0 || py.flags.rest == -1 || py.flags.rest == -2)
	    prt_state();

	if ((py.flags.status & PY_ARMOR) != 0) {
	    py.misc.dis_ac = py.misc.pac + py.misc.dis_tac;	/* use updated ac */
	    prt_pac();
	    py.flags.status &= ~PY_ARMOR;
	}
	if ((py.flags.status & PY_STATS) != 0) {
	    for (i = 0; i < 6; i++)
		if ((PY_STR << i) & py.flags.status)
		    prt_stat(i);
	    py.flags.status &= ~PY_STATS;
	}
	if (py.flags.status & PY_HP) {
	    prt_mhp();
	    prt_chp();
	    py.flags.status &= ~PY_HP;
	}
	if (py.flags.status & PY_MANA) {
	    prt_cmana();
	    py.flags.status &= ~PY_MANA;
	}
    /* Allow for a slim chance of detect enchantment -CJS- */
    /*
     * for 1st level char, check once every 2160 turns for 40th level char,
     * check once every 416 turns 
     */
	if (py.misc.pclass == 2 ?
	    ((f_ptr->confused == 0) && (py.misc.pclass != 0) &&
	(randint((int)(10000 / (py.misc.lev * py.misc.lev + 40)) + 1) == 1))
	    :
	    (((turn & 0xF) == 0) && (f_ptr->confused == 0)
	     && (randint((int)(10 + 750 / (5 + py.misc.lev))) == 1))
	    ) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_ARRAY_SIZE; i++) {
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
		    ((py.misc.pclass == 2 || py.misc.pclass == 3) ?
		     (randint(i < 22 ? 5 : 1) == 1) :
		     (randint(i < 22 ? 50 : 10) == 1))) {

		    if (py.misc.pclass == 0 || py.misc.pclass == 3 ||
			py.misc.pclass == 5)
			if ((i_ptr->tval == TV_SWORD) ||
			    (i_ptr->tval == TV_HAFTED) ||
			    (i_ptr->tval == TV_POLEARM) ||
			    (i_ptr->tval == TV_BOW) ||
			    (i_ptr->tval == TV_BOLT) ||
			    (i_ptr->tval == TV_ARROW) ||
			    (i_ptr->tval == TV_DIGGING) ||
			    (i_ptr->tval == TV_SLING_AMMO) ||
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
	if (((py.misc.pclass == 0) && (f_ptr->confused == 0) &&
	 (randint((int)(9000 / (py.misc.lev * py.misc.lev + 40)) + 1) == 1))
	    ||
	    ((py.misc.pclass == 3) && (f_ptr->confused == 0) &&
	(randint((int)(20000 / (py.misc.lev * py.misc.lev + 40)) + 1) == 1))
	    ||
	    ((py.misc.pclass == 5) && (f_ptr->confused == 0) &&
	     (randint((int)(80000L / (py.misc.lev * py.misc.lev + 40)) + 1) == 1))) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_ARRAY_SIZE; i++) {
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
		     (i_ptr->tval == TV_SLING_AMMO) ||
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
				   (i_ptr->tval == TV_SLING_AMMO) ||
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
	if (MAX_MALLOC - mfptr < 10)
	    (void)compact_monsters();

	if ((py.flags.paralysis < 1) &&	/* Accept a command?     */
	    (py.flags.rest == 0) &&
	    (py.flags.stun < 100) &&
	    (!death))
	/* Accept a command and execute it				 */
	{
	    do {
		if (py.flags.status & PY_REPEAT)
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


