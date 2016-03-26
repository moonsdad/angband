/* File: misc3.c */ 

/* Purpose: misc code for objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * Deletes object from given location			-RAK-	
 */
int delete_object(int y, int x)
{
    register int        delete;
    register cave_type *c_ptr;

    /* Find where it was */
    c_ptr = &cave[y][x];

    if (c_ptr->fval == BLOCKED_FLOOR) c_ptr->fval = CORR_FLOOR;

    /* then eliminate it */
    pusht(c_ptr->tptr);

    /* There is nothing here */
    c_ptr->tptr = 0;

    c_ptr->fm = FALSE;

    lite_spot(y, x);

    if (test_lite(y, x)) delete = TRUE;
    else delete = FALSE;

    return (delete);
}




/*
 * When too many objects gather on the floor, delete some of them
 */
static void compact_objects()
{
    register int        i, j;
    register cave_type *c_ptr;
    int                 ctr, cur_dis, chance;


    /* Debugging message */
    msg_print("Compacting objects...");

    ctr = 0;
    cur_dis = 66;
    do {

	/* Examine the dungeon */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++) {

		/* Get the location */
		c_ptr = &cave[i][j];

		if ((c_ptr->tptr != 0)
		    && (distance(i, j, char_row, char_col) > cur_dis)) {

		/* Every object gets a "saving throw" */
		    switch (t_list[c_ptr->tptr].tval) {
		      case TV_VIS_TRAP:
			chance = 15;
			break;
		    case TV_RUBBLE:
		    case TV_INVIS_TRAP:
		    case TV_OPEN_DOOR:
		    case TV_CLOSED_DOOR:
			chance = 5;
			break;
		    case TV_UP_STAIR:
		    case TV_DOWN_STAIR:
		    case TV_STORE_DOOR:
			chance = 0;
			break;
		    case TV_SECRET_DOOR:
			chance = 3;
			break;
		    default:
			if ((t_list[c_ptr->tptr].tval >= TV_MIN_WEAR) &&
			    (t_list[c_ptr->tptr].tval <= TV_MAX_WEAR) &&
			    (t_list[c_ptr->tptr].flags2 & TR_ARTIFACT))
			    chance = 0;	/* don't compact artifacts -CFT */
			else
			    chance = 10;
		    }
		    if (randint(100) <= chance) {
			(void)delete_object(i, j);
			ctr++;
		    }
		}
	    }
	if (ctr == 0) cur_dis -= 6;
    }
    while (ctr <= 0);

    /* Redraw */
    if (cur_dis < 66) prt_map();
}


/*
 * Gives index of next free space			-RAK-
 */
int i_pop(void)
{
    /* Compact if needed */
    if (i_max == MAX_I_IDX) compact_objects();

    /* Return the next free space */
    return (i_max++);
}

/*
 * Pushs a record back onto free space list		-RAK-	 
 *
 * Delete_object() should always be called instead, unless the object in
 * question is not in the dungeon, e.g. in store1.c and files.c 
 */
void pusht(int my_x)
{
    s16b        x = (s16b) my_x;
    register int i, j;

    if (x != i_max - 1) {
	t_list[x] = t_list[i_max - 1];

    /* must change the tptr in the cave of the object just moved */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++)
		if (cave[i][j].tptr == i_max - 1)
		    cave[i][j].tptr = x;
    }
    i_max--;
    invcopy(&t_list[i_max], OBJ_NOTHING);
}


/*
 * Boolean : is object enchanted	  -RAK- 
 */
int magik(int chance)
{
    if (randint(100) <= chance) return (TRUE);

    return (FALSE);
}


#if 0

/*
 * Enchant a bonus based on degree desired -RAK-
 *
 * Lets just change this to make sense.  Now it goes from base to limit,
 * roughly proportional to the level.... -CWS
 */
int m_bonus(int base, int limit, int level)
{
    register int x, stand_dev, tmp, diff = limit - base;

#ifdef USE_FLOATING_POINT

    /* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100.0)) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);

    /* Extract a weird value */
    x = (tmp * diff / 150.0) + (level * limit / 200.0) + base;

#else

    /* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100)) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);

    /* Extract a weird value */
    x = (tmp * diff / 150) + (level * limit / 200) + base;

#endif

    /* Enforce minimum value */
    if (x < base) return (base);

    /* Return the extracted value */
    return (x);
}

#endif






/*
 * JLS: Give an item one of the "powerful resistances"
 */		   
static void give_1_hi_resist(inven_type *i_ptr)
{
    switch (randint(10)) {
	case 1: i_ptr->flags2 |= TR2_RES_CONF; break;
	case 2: i_ptr->flags2 |= TR2_RES_SOUND; break;
	case 3: i_ptr->flags2 |= TR2_RES_LITE; break;
	case 4: i_ptr->flags2 |= TR2_RES_DARK; break;
	case 5: i_ptr->flags2 |= TR2_RES_CHAOS; break;
	case 6: i_ptr->flags2 |= TR2_RES_DISEN; break;
	case 7: i_ptr->flags2 |= TR2_RES_SHARDS; break;
	case 8: i_ptr->flags2 |= TR2_RES_NEXUS; break;
	case 9: i_ptr->flags2 |= TR2_RES_BLIND; break;
	case 10: i_ptr->flags2 |= TR2_RES_NETHER; break;
    }
}

/*
 * Chance of treasure having magic abilities		-RAK-
 * Chance increases with each dungeon level			 
 *
 * some objects appear multiple times in the object_list with different
 * levels, this is to make the object occur more often, however, for
 * consistency, must set the level of these duplicates to be the same as the
 * object with the lowest level 
 */
void magic_treasure(int x, int level, int good, int not_unique)
{
    register inven_type *i_ptr;
    register u32b      chance, special, cursed, i;
    u32b               tmp;

    /* Extract the "chance" of "goodness" */
    chance = OBJ_BASE_MAGIC + level;
    if (chance > OBJ_BASE_MAX) chance = OBJ_BASE_MAX;

    /* Extract the "chance" of "greatness" (approx range 3-18 percent) */
    special = (10 * chance) / OBJ_DIV_SPECIAL;

    /* Extract the "chance" of ickiness (approx range 11-54 percent) */
    cursed = (10 * chance) / OBJ_DIV_CURSED;

    i_ptr = &t_list[x];

    /* Depending on treasure type, it can have certain magical properties */
    switch (i_ptr->tval) {

      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:

	if ((i_ptr->index >= 389 && i_ptr->index <= 394)
	    || (i_ptr->index >= 408 && i_ptr->index <= 409)
	    || (i_ptr->index >= 415 && i_ptr->index <= 419)) {

	    byte               artifact = FALSE;

	/* all DSM are enchanted, I guess -CFT */
	    i_ptr->toac += m_bonus(0, 5, level) + randint(5);
	    rating += 30;
	    if ((magik(chance) && magik(special)) || (good == 666)) {
		i_ptr->toac += randint(5);	/* even better... */
		if ((randint(3) == 1 || good == 666) && !not_unique
		    && unique_armour(i_ptr))	/* ...but is it an artifact? */
		    artifact = TRUE;
	    }
	    if (!artifact) {	   /* assume cost&mesg done if it was an artifact */
		if (wizard || peek)
		    msg_print("Dragon Scale Mail");
		i_ptr->cost += ((s32b) i_ptr->toac * 500L);
	    }
	} /* end if is a DSM */ 

	else if (magik(chance) || good) {

	    i_ptr->toac += randint(3) + m_bonus(0, 5, level);
	    if (!stricmp(object_list[i_ptr->index].name, "& Robe") &&
		((magik(special) && randint(30) == 1)
		 || (good == 666 && magik(special)))) {
		i_ptr->flags |= (TR2_RES_LIGHT | TR_RES_COLD | TR2_RES_ACID |
				 TR2_RES_FIRE | TR_SUST_STAT);
		if (wizard || peek)
		    msg_print("Robe of the Magi");
		rating += 30;
		i_ptr->flags2 |= TR_HOLD_LIFE;
		i_ptr->ident |= ID_NOSHOW_P1;
		give_1_hi_resist(i_ptr);	/* JLS */
		i_ptr->p1 = 10;
		i_ptr->toac += 10 + randint(5);
		i_ptr->name2 = SN_MAGI;
		i_ptr->cost = 10000L + (i_ptr->toac * 100);
	    } else if (magik(special) || good == 666)

		switch (randint(9)) {

		  case 1:
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags |= (TR2_RES_LIGHT | TR_RES_COLD | TR2_RES_ACID |
				     TR2_RES_FIRE);
		    if (randint(3) == 1) {
			if (peek) msg_print("Elvenkind");
			rating += 25;
			give_1_hi_resist(i_ptr);	/* JLS */
			i_ptr->flags |= TR1_STEALTH;
			i_ptr->p1 = randint(3);
			i_ptr->name2 = SN_ELVENKIND;
			i_ptr->toac += 15;
			i_ptr->cost += 15000L;
		    }
		    else {
			if (peek) msg_print("Resist");
			rating += 20;
			i_ptr->name2 = SN_R;
			i_ptr->toac += 8;
			i_ptr->cost += 12500L;
		    }
		    break;

		  case 2:	   /* Resist Acid	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (!strncmp(object_list[i_ptr->index].name,
				 "Mithril", 7) ||
			!strncmp(object_list[i_ptr->index].name,
				 "Adamantite", 10))
			break;
		    if (peek) msg_print("Resist Acid");
		    rating += 15;
		    i_ptr->flags |= TR2_RES_ACID;
		    i_ptr->name2 = SN_RA;
		    i_ptr->cost += 1000L;
		    break;

		  case 3: case 4:	   /* Resist Fire	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (peek) msg_print("Resist Fire");
		    rating += 17;
		    i_ptr->flags |= TR2_RES_FIRE;
		    i_ptr->name2 = SN_RF;
		    i_ptr->cost += 600L;
		    break;

		  case 5:  case :	   /* Resist Cold	 */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (peek) msg_print("Resist Cold");
		    rating += 16;
		    i_ptr->flags |= TR2_RES_COLD;
		    i_ptr->name2 = SN_RC;
		    i_ptr->cost += 600L;
		    break;

		  case 7: case 8: case 9:	   /* Resist Lightning */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (peek) msg_print("Resist Lightning");
		    rating += 15;
		    i_ptr->flags |= TR2_RES_LIGHT;
		    i_ptr->name2 = SN_RL;
		    i_ptr->cost += 500L;
		    break;
		}
	}

	/* Cursed armor */
	else if (magik(cursed)) {
	    i_ptr->toac = -randint(3) - m_bonus(0, 10, level);
	    i_ptr->cost = 0L;
	    i_ptr->flags |= TR_CURSED;
	}

	break;

      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:

    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    i_ptr->tohit += randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam += randint(3) + m_bonus(0, 10, level);
	/*
	 * the 3*special/2 is needed because weapons are not as common as
	 * before change to treasure distribution, this helps keep same
	 * number of ego weapons same as before, see also missiles 
	 */
	    if (magik(3*special/2)||good==666) { /* was 2 */
		if (!stricmp("& Whip", object_list[i_ptr->index].name)
		    && randint(2)==1) {
		    if (peek) msg_print("Whip of Fire");
		    rating += 20;
		    i_ptr->name2 = SN_FIRE;
		    i_ptr->flags |= (TR1_BRAND_FIRE | TR2_RES_FIRE);
		    /* this should allow some WICKED whips -CFT */
		    while (randint(5*(int)i_ptr->dd)==1) {
			i_ptr->dd++;
			i_ptr->cost += 2500;
			i_ptr->cost *= 2;
		    }
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;
		} else {
		    switch (randint(30)) {	/* was 16 */
		      case 1:	   /* Holy Avenger	 */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique &&
			    unique_weapon(i_ptr))
			    break;
			if (peek)
			    msg_print("Holy Avenger");
			rating += 30;
			i_ptr->flags |= (TR_SEE_INVIS | TR_SUST_STAT |
				      TR1_SLAY_UNDEAD | TR1_SLAY_EVIL | TR1_WIS);
			i_ptr->flags2 |= (TR1_SLAY_DEMON | TR_BLESS_BLADE);
			i_ptr->tohit += 5;
			i_ptr->todam += 5;
			i_ptr->toac += randint(4);
		    /* the value in p1 is used for strength increase */
		    /* p1 is also used for sustain stat */
			i_ptr->p1 = randint(4);
			i_ptr->name2 = SN_HA;
			i_ptr->cost += i_ptr->p1 * 500;
			i_ptr->cost += 10000L;
			i_ptr->cost *= 2;
			break;
		      case 2:	   /* Defender	 */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr))
			    break;
			if (peek)
			    msg_print("Defender");
			rating += 23;
			i_ptr->flags |= (TR_FFALL | TR2_RES_LIGHT | TR_SEE_INVIS
				   | TR2_FREE_ACT | TR2_RES_COLD | TR2_RES_ACID
				     | TR2_RES_FIRE | TR_REGEN | TR1_STEALTH);
			i_ptr->tohit += 3;
			i_ptr->todam += 3;
			i_ptr->toac += 5 + randint(5);
			i_ptr->name2 = SN_DF;
		    /* the value in p1 is used for stealth */
			i_ptr->p1 = randint(3);
			i_ptr->cost += i_ptr->p1 * 500;
			i_ptr->cost += 7500L;
			i_ptr->cost *= 2;
			break;
		      case 3:
		      case 4:	   /* Flame Tongue  */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr))
			    break;
			rating += 20;
			i_ptr->flags |= (TR1_BRAND_FIRE | TR2_RES_FIRE);
			if (peek)
			    msg_print("Flame");
			i_ptr->tohit += 2;
			i_ptr->todam += 3;
			i_ptr->name2 = SN_FT;
			i_ptr->cost += 3000L;
			break;
		      case 5:
		      case 6:	   /* Frost Brand   */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr))
			    break;
			if (peek)
			    msg_print("Frost");
			rating += 20;
			i_ptr->flags |= (TR1_BRAND_COLD | TR2_RES_COLD);
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_FB;
			i_ptr->cost += 2200L;
			break;
		      case 7:
		      case 8:	   /* Slay Animal  */
			i_ptr->flags |= TR1_SLAY_ANIMAL;
			rating += 15;
			if (peek)
			    msg_print("Slay Animal");
			i_ptr->tohit += 3;
			i_ptr->todam += 3;
			i_ptr->name2 = SN_SA;
			i_ptr->cost += 2000L;
			break;
		      case 9:
		      case 10:	   /* Slay Dragon	 */
			i_ptr->flags |= TR1_SLAY_DRAGON;
			if (peek)
			    msg_print("Slay Dragon");
			rating += 18;
			i_ptr->tohit += 3;
			i_ptr->todam += 3;
			i_ptr->name2 = SN_SD;
			i_ptr->cost += 4000L;
			break;
		      case 11:
		      case 12:	   /* Slay Evil   */
			i_ptr->flags |= TR1_SLAY_EVIL;
			if (randint(3) == 1) {
			    i_ptr->flags |= (TR1_WIS);
			    i_ptr->flags2 |= (TR_BLESS_BLADE);
			    i_ptr->p1 = m_bonus(0, 3, level);
			    i_ptr->cost += (200 * i_ptr->p1);
			}
			if (peek)
			    msg_print("Slay Evil");
			rating += 18;
			i_ptr->tohit += 3;
			i_ptr->todam += 3;
			i_ptr->name2 = SN_SE;
			i_ptr->cost += 4000L;
			break;
		      case 13:
		      case 14:	   /* Slay Undead	  */
			i_ptr->flags |= (TR_SEE_INVIS | TR1_SLAY_UNDEAD);
			if (randint(3) == 1) {
			    i_ptr->flags2 |= (TR_HOLD_LIFE);
			    i_ptr->cost += 1000;
			}
			if (peek)
			    msg_print("Slay Undead");
			rating += 18;
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_SU;
			i_ptr->cost += 3000L;
			break;
		      case 15:
		      case 16:
		      case 17:	   /* Slay Orc */
			i_ptr->flags2 |= TR1_SLAY_ORC;
			if (peek)
			    msg_print("Slay Orc");
			rating += 13;
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_SO;
			i_ptr->cost += 1200L;
			break;
		      case 18:
		      case 19:
		      case 20:	   /* Slay Troll */
			i_ptr->flags2 |= TR1_SLAY_TROLL;
			if (peek)
			    msg_print("Slay Troll");
			rating += 13;
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_ST;
			i_ptr->cost += 1200L;
			break;
		      case 21:
		      case 22:
		      case 23:
			i_ptr->flags2 |= TR1_SLAY_GIANT;
			if (peek)
			    msg_print("Slay Giant");
			rating += 14;
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_SG;
			i_ptr->cost += 1200L;
			break;
		      case 24:
		      case 25:
		      case 26:
			i_ptr->flags2 |= TR1_SLAY_DEMON;
			if (peek)
			    msg_print("Slay Demon");
			rating += 16;
			i_ptr->tohit += 2;
			i_ptr->todam += 2;
			i_ptr->name2 = SN_SDEM;
			i_ptr->cost += 1200L;
			break;
		      case 27:	   /* of Westernesse */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr))
			    break;
			if (peek)
			    msg_print("Westernesse");
			rating += 20;
			i_ptr->flags |= (TR_SEE_INVIS | TR1_DEX | TR1_CON | TR1_STR |
					 TR2_FREE_ACT);
			i_ptr->flags2 |= TR1_SLAY_ORC;
			i_ptr->tohit += randint(5) + 3;
			i_ptr->todam += randint(5) + 3;
			i_ptr->p1 = 1;
			i_ptr->cost += 10000L;
			i_ptr->cost *= 2;
			i_ptr->name2 = SN_WEST;
			break;
		      case 28:
		      case 29:	   /* Blessed Blade -DGK */
			if ((i_ptr->tval != TV_SWORD) &&
			    (i_ptr->tval != TV_POLEARM))
			    break;
			if (peek)
			    msg_print("Blessed");
			rating += 20;
			i_ptr->flags = TR1_WIS;
			i_ptr->flags2 = TR_BLESS_BLADE;
			i_ptr->tohit += 3;
			i_ptr->todam += 3;
			i_ptr->p1 = randint(3);
			i_ptr->name2 = SN_BLESS_BLADE;
			i_ptr->cost += i_ptr->p1 * 1000;
			i_ptr->cost += 3000L;
			break;
		      case 30:	   /* of Speed -DGK */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique && unique_weapon(i_ptr))
			    break;
			if (wizard || peek)
			    msg_print("Weapon of Extra Attacks");
			rating += 20;
			i_ptr->tohit += randint(5);
			i_ptr->todam += randint(3);
			i_ptr->flags2 = TR_ATTACK_SPD;
			if (i_ptr->weight <= 80)
			    i_ptr->p1 = randint(3);
			else if (i_ptr->weight <= 130)
			    i_ptr->p1 = randint(2);
			else
			    i_ptr->p1 = 1;
			i_ptr->name2 = SN_ATTACKS;
			i_ptr->cost += (i_ptr->p1 * 2000);
			i_ptr->cost *= 2;
			break;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    i_ptr->tohit = (-randint(3) - m_bonus(1, 20, level));
	    i_ptr->todam = (-randint(3) - m_bonus(1, 20, level));
	    i_ptr->flags |= TR_CURSED;
	    if (level > (20 + randint(15)) && randint(10) == 1) {
		i_ptr->name2 = SN_MORGUL;
		i_ptr->flags |= (TR_SEE_INVIS | TR_AGGRAVATE);
		i_ptr->tohit -= 15;
		i_ptr->todam -= 15;
		i_ptr->toac = -10;
		i_ptr->weight += 100;
	    }
	    i_ptr->cost = 0L;
	}
	break;
      case TV_BOW:
    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    i_ptr->tohit = randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam = randint(3) + m_bonus(0, 10, level);
	    switch (randint(15)) {
	      case 1: case 2: case 3:
		if (((randint(3)==1)||(good==666)) && !not_unique &&
		    !stricmp(object_list[i_ptr->index].name, "& Long Bow") &&
		    (((i=randint(2))==1 && !BELEG) || (i==2 && !BARD))) {
		    switch (i) {
		    case 1:
			  if (BELEG)
			    break;
			if (wizard || peek)
			    msg_print("Belthronding");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_BELEG;
			i_ptr->ident |= ID_NOSHOW_TYPE;
			i_ptr->sval = 4; /* make do x5 damage!! -CFT */
			i_ptr->tohit = 20;
			i_ptr->todam = 22;
			i_ptr->p1 = 3;
			i_ptr->flags |= (TR1_STEALTH | TR1_DEX);
			i_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_DISEN);
			i_ptr->cost = 35000L;
			BELEG = 1;
			break;
		      case 2:
			if (BARD)
			    break;
			if (wizard || peek)
			    msg_print("Bard");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_BARD;
			i_ptr->sval = 3; /* make do x4 damage!! -CFT */
			i_ptr->tohit = 17;
			i_ptr->todam = 19;
			i_ptr->p1 = 3;
			i_ptr->flags |= (TR2_FREE_ACT | TR1_DEX);
			i_ptr->flags2 |= (TR_ARTIFACT);
			i_ptr->cost = 20000L;
			BARD = 1;
			break;
		    }
		    break;
		}
		if (((randint(5) == 1) || (good == 666)) && !not_unique &&
		    !stricmp(object_list[i_ptr->index].name, "& Light Crossbow")
		    && !CUBRAGOL) {
		    if (CUBRAGOL)
			break;
		    if (wizard || peek)
			msg_print("Cubragol");
		    i_ptr->name2 = SN_CUBRAGOL;
		    i_ptr->sval = 11;
		    i_ptr->tohit = 10;
		    i_ptr->todam = 14;
		    i_ptr->p1 = 1;
		    i_ptr->flags |= (TR1_SPEED | TR2_RES_FIRE);
		    i_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
		    i_ptr->cost = 38000L;
		    CUBRAGOL = 1;
		    break;
		}
		i_ptr->name2 = SN_MIGHT;
		if (peek)
		    msg_print("Bow of Might");
		rating += 15;
		i_ptr->sval++; /* make it do an extra multiple of damage */
		i_ptr->tohit += 5;
		i_ptr->todam += 10;
		break;
	      case 4: case 5: case 6: case 7: case 8:
		i_ptr->name2 = SN_MIGHT;
		if (peek) msg_print("Bow of Might");
		rating += 11;
		i_ptr->tohit += 5;
		i_ptr->todam += 12;
		break;

	      case 9: case 10: case 11: case 12:
	      case 13: case 14: case 15:
		i_ptr->name2 = SN_ACCURACY;
		rating += 11;
		if (peek)
		    msg_print("Accuracy");
		i_ptr->tohit += 12;
		i_ptr->todam += 5;
		break;
	    }
	} else if (magik(cursed)) {
	    i_ptr->tohit = (-m_bonus(5, 30, level));
	    i_ptr->todam = (-m_bonus(5, 20, level));	/* add damage. -CJS- */
	    i_ptr->flags |= TR_CURSED;
	    i_ptr->cost = 0L;
	}
	break;

      case TV_DIGGING:
    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || (good == 666)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		i_ptr->p1 += m_bonus(0, 5, level);
	    }
	    if (tmp == 2)	/* do not give additional plusses -CWS */
		;
	    else {
	    /* a cursed digging tool */
		i_ptr->p1 = (-m_bonus(1, 15, level));
		i_ptr->cost = 0L;
		i_ptr->flags |= TR_CURSED;
	    }
	}
	break;

      case TV_GLOVES:
	if (magik(chance) || good) {
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if ((((randint(2) == 1) && magik(5 * special / 2)) || (good == 666)) &&
		!stricmp(object_list[i_ptr->index].name,
			 "& Set of Leather Gloves") &&
		!not_unique && unique_armour(i_ptr));
	    else if ((((randint(4) == 1) && magik(special)) || (good == 666))
		     && !stricmp(object_list[i_ptr->index].name,
				 "& Set of Gauntlets") &&
		     !not_unique && unique_armour(i_ptr));
	    else if ((((randint(5) == 1) && magik(special)) || (good == 666))
		     && !stricmp(object_list[i_ptr->index].name,
				 "& Set of Cesti") &&
		     !not_unique && unique_armour(i_ptr));

	/* don't forget cesti -CFT */
	    else if (magik(special) || (good == 666)) {
		switch (randint(10)) {

		  case 1: case 2: case 3:
		    if (peek)	msg_print("Free action");
		    rating += 11;
		    i_ptr->flags |= TR2_FREE_ACT;
		    i_ptr->name2 = SN_FREE_ACTION;
		    i_ptr->cost += 1000L;
		    break;

		  case 4: case 5: case 6:
		    i_ptr->ident |= ID_SHOW_HITDAM;
		    rating += 17;
		    if (peek)
			msg_print("Slaying");
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->name2 = SN_SLAYING;
		    i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 250;
		    break;

		  case 7: case 8: case 9:
		    i_ptr->name2 = SN_AGILITY;
		    if (peek)
			msg_print("Agility");
		    rating += 14;
		    i_ptr->p1 = 2 + randint(2);
		    i_ptr->flags |= TR1_DEX;
		    i_ptr->cost += (i_ptr->p1) * 400;
		    break;

		  case 10:
		    if (((randint(3) == 1) || (good == 666)) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (peek)
			msg_print("Power");
		    rating += 22;
		    i_ptr->name2 = SN_POWER;
		    i_ptr->p1 = 1 + randint(4);
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->flags |= TR1_STR;
		    i_ptr->ident |= ID_SHOW_HITDAM;
		    i_ptr->ident |= ID_NOSHOW_TYPE;
		    i_ptr->cost += (i_ptr->tohit + i_ptr->todam + i_ptr->p1) * 300;
		    break;
		}
	    }
	}

	else if (magik(cursed)) {
	    if (magik(special)) {
		if (randint(2) == 1) {
		    i_ptr->flags |= TR1_DEX;
		    i_ptr->name2 = SN_CLUMSINESS;
		}
		else {
		    i_ptr->flags |= TR1_STR;
		    i_ptr->name2 = SN_WEAKNESS;
		}
		i_ptr->p1 = (randint(3) - m_bonus(0, 10, level));
	    }
	    i_ptr->toac = (-m_bonus(1, 20, level));
	    i_ptr->flags |= TR_CURSED;
	    i_ptr->cost = 0;
	}

	break;


      case TV_BOOTS:
	if (magik(chance) || good) {

	    i_ptr->toac = randint(3) + m_bonus(1, 10, level);

	    if (magik(special) || (good == 666)) {
		tmp = randint(12);
		if (tmp == 1) {
		    if (!((randint(2) == 1) && !not_unique
			  && unique_armour(i_ptr))) {
			i_ptr->flags |= TR1_SPEED;
			if (wizard || peek)
			    msg_print("Boots of Speed");
			i_ptr->name2 = SN_SPEED;
			rating += 30;
			i_ptr->p1 = 1;
			i_ptr->cost += 300000L;
		    }
		} else if (stricmp("& Pair of Metal Shod Boots",
				   object_list[i_ptr->index].name))	/* not metal */
		    if (tmp > 6) {
			i_ptr->flags |= TR_FFALL;
			rating += 7;
			i_ptr->name2 = SN_SLOW_DESCENT;
			i_ptr->cost += 250;
		    } else if (tmp < 5) {
			i_ptr->flags |= TR1_STEALTH;
			rating += 16;
			i_ptr->p1 = randint(3);
			i_ptr->name2 = SN_STEALTH;
			i_ptr->cost += 500;
		    } else {	   /* 5,6 */
			i_ptr->flags |= TR2_FREE_ACT;
			rating += 15;
			i_ptr->name2 = SN_FREE_ACTION;
			i_ptr->cost += 500;
			i_ptr->cost *= 2;
		    }
		else
		 /* is metal boots, different odds since no stealth */
		    if (tmp < 5) {
			i_ptr->flags |= TR2_FREE_ACT;
			rating += 15;
			i_ptr->name2 = SN_FREE_ACTION;
			i_ptr->cost += 500;
			i_ptr->cost *= 2;
		    } else {	   /* tmp > 4 */
			i_ptr->flags |= TR_FFALL;
			rating += 7;
			i_ptr->name2 = SN_SLOW_DESCENT;
			i_ptr->cost += 250;
		    }
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		i_ptr->flags |= TR1_SPEED;
		i_ptr->name2 = SN_SLOWNESS;
		i_ptr->p1 = -1;
	    } else if (tmp == 2) {
		i_ptr->flags |= TR_AGGRAVATE;
		i_ptr->name2 = SN_NOISE;
	    } else {
		i_ptr->name2 = SN_GREAT_MASS;
		i_ptr->weight = i_ptr->weight * 5;
	    }
	    i_ptr->cost = 0;
	    i_ptr->toac = (-m_bonus(2, 20, level));
	    i_ptr->flags |= TR_CURSED;
	}
	break;

      case TV_HELM:		   /* Helms */

	if ((i_ptr->sval >= 6) && (i_ptr->subval <= 8)) {
	/* give crowns a higher chance for magic */
	    chance += i_ptr->cost / 100;
	    special += special;
	}
	if (magik(chance) || good) {
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if (magik(special) || (good == 666)) {
		if (i_ptr->sval < 6) {
		    tmp = randint(14);
		    if (tmp < 3) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Intelligence");
			    i_ptr->p1 = randint(2);
			    rating += 13;
			    i_ptr->flags |= TR1_INT;
			    i_ptr->name2 = SN_INTELLIGENCE;
			    i_ptr->cost += i_ptr->p1 * 500;
			}
		    } else if (tmp < 6) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Wisdom");
			    rating += 13;
			    i_ptr->p1 = randint(2);
			    i_ptr->flags |= TR1_WIS;
			    i_ptr->name2 = SN_WISDOM;
			    i_ptr->cost += i_ptr->p1 * 500;
			}
		    } else if (tmp < 10) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    i_ptr->p1 = 1 + randint(4);
			    rating += 11;
			    i_ptr->flags |= TR_INFRA;
			    i_ptr->name2 = SN_INFRAVISION;
			    i_ptr->cost += i_ptr->p1 * 250;
			}
		    } else if (tmp < 12) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Light");
			    i_ptr->flags2 |= (TR2_RES_LITE | TR_LIGHT);
			    rating += 6;
			    i_ptr->name2 = SN_LIGHT;
			    i_ptr->cost += 500;
			}
		    } else if (tmp < 14) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Helm of Seeing");
			    i_ptr->flags |= TR_SEE_INVIS;
			    i_ptr->flags2 |= TR2_RES_BLIND;
			    rating += 8;
			    i_ptr->name2 = SN_SEEING;
			    i_ptr->cost += 1000;
			}
		    } else {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Telepathy");
			    rating += 20;
			    i_ptr->flags2 |= TR_TELEPATHY;
			    i_ptr->name2 = SN_TELEPATHY;
			    i_ptr->cost += 50000L;
			}
		    }
		} else {
		    switch (randint(6)) {
		      case 1:
			if (!(((randint(2) == 1) || (good == 666)) &&
			      !not_unique && unique_armour(i_ptr))) {
			    if (peek)
				msg_print("Crown of Might");
			    rating += 19;
			    i_ptr->p1 = randint(3);
			    i_ptr->flags |= (TR2_FREE_ACT | TR1_CON |
					     TR1_DEX | TR1_STR);
			    i_ptr->name2 = SN_MIGHT;
			    i_ptr->cost += 1000 + i_ptr->p1 * 500;
			}
			break;
		      case 2:
			if (peek)
			    msg_print("Lordliness");
			i_ptr->p1 = randint(3);
			rating += 17;
			i_ptr->flags |= (TR1_CHR | TR1_WIS);
			i_ptr->name2 = SN_LORDLINESS;
			i_ptr->cost += 1000 + i_ptr->p1 * 500;
			break;
		      case 3:
			if (peek)
			    msg_print("Crown of the Magi");
			rating += 15;
			i_ptr->p1 = randint(3);
			i_ptr->flags |= (TR2_RES_LIGHT | TR_RES_COLD
				      | TR2_RES_ACID | TR2_RES_FIRE | TR1_INT);
			i_ptr->name2 = SN_MAGI;
			i_ptr->cost += 3000 + i_ptr->p1 * 500;
			break;
		      case 4:
			rating += 8;
			if (peek)
			    msg_print("Beauty");
			i_ptr->p1 = randint(4);
			i_ptr->flags |= TR1_CHR;
			i_ptr->name2 = SN_BEAUTY;
			i_ptr->cost += 750;
			break;
		      case 5:
			if (peek)
			    msg_print("Seeing");
			rating += 8;
			i_ptr->p1 = 5 * (1 + randint(4));
			i_ptr->flags |= (TR_SEE_INVIS | TR1_SEARCH);
			i_ptr->name2 = SN_SEEING;
			i_ptr->cost += 1000 + i_ptr->p1 * 100;
			break;
		      case 6:
			i_ptr->flags |= TR_REGEN;
			rating += 10;
			if (peek)
			    msg_print("Regeneration");
			i_ptr->name2 = SN_REGENERATION;
			i_ptr->cost += 1500;
			break;

		    }
		}
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {
	    i_ptr->toac -= m_bonus(1, 20, level);
	    i_ptr->flags |= TR_CURSED;
	    i_ptr->cost = 0;
	    if (magik(special))
		switch (randint(7)) {
		  case 1:
		    i_ptr->p1 = -randint(5);
		    i_ptr->flags |= TR1_INT;
		    i_ptr->name2 = SN_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    i_ptr->p1 = -randint(5);
		    i_ptr->flags |= TR1_WIS;
		    i_ptr->name2 = SN_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    i_ptr->p1 = -randint(5);
		    i_ptr->flags |= TR1_STR;
		    i_ptr->name2 = SN_WEAKNESS;
		    break;
		  case 6:
		    i_ptr->flags |= TR_TELEPORT;
		    i_ptr->name2 = SN_TELEPORTATION;
		    break;
		  case 7:
		    i_ptr->p1 = -randint(5);
		    i_ptr->flags |= TR1_CHR;
		    i_ptr->name2 = SN_UGLINESS;
		    break;
		}
	}
	break;

      case TV_RING:		   /* Rings	      */
	if (!((randint(10) == 1) && !not_unique && unique_armour(i_ptr))) {
	    switch (i_ptr->sval) {
	      case 0:
	      case 1:
	      case 2:
	      case 3:		   /* 132-135 */
		if (magik(cursed)) {
		    i_ptr->p1 = -m_bonus(1, 10, level);
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		} else {
		    i_ptr->p1 = m_bonus(1, 6, level);
		    i_ptr->cost += i_ptr->p1 * 100;
		}
		break;
	      case 4:		   /* 136 */
		if (magik(cursed)) {
		    i_ptr->p1 = -randint(3);
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		} else {
		    if (peek)
			msg_print("Ring of Speed");
		    rating += 35;
		    if (randint(888) == 1)
			i_ptr->p1 = 2;
		    else
			i_ptr->p1 = 1;
		}
		break;
	      case 5:
		i_ptr->p1 = 5 * m_bonus(1, 10, level);
		i_ptr->cost += i_ptr->p1 * 30;
		if (magik(cursed)) {
		    i_ptr->p1 = -i_ptr->p1;
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		}
		break;
	      case 14:
	      case 15:
	      case 16:		   /* Flames, Acid, Ice */
		i_ptr->toac = m_bonus(1, 10, level);
		i_ptr->toac += 5 + randint(7);
		i_ptr->cost += i_ptr->toac * 100;
		break;
              case 17:
              case 18:		   /* WOE, Stupidity */
		i_ptr->toac = (-5) - m_bonus(1,10,level);
		i_ptr->p1 = (-randint(4));
		break;
	      case 19:		   /* Increase damage	      */
		i_ptr->todam = m_bonus(1, 10, level);
		i_ptr->todam += 3 + randint(10);
		i_ptr->cost += i_ptr->todam * 100;
		if (magik(cursed)) {
		    i_ptr->todam = -i_ptr->todam;
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		}
		break;
	      case 20:		   /* Increase To-Hit	      */
		i_ptr->tohit = m_bonus(1, 10, level);
		i_ptr->tohit += 3 + randint(10);
		i_ptr->cost += i_ptr->tohit * 100;
		if (magik(cursed)) {
		    i_ptr->tohit = -i_ptr->tohit;
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		}
		break;
	      case 21:		   /* Protection	      */
		i_ptr->toac = m_bonus(0, 10, level);
		i_ptr->toac += 4 + randint(5);
		i_ptr->cost += i_ptr->toac * 100;
		if (magik(cursed)) {
		    i_ptr->toac = -i_ptr->toac;
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		}
		break;
	      case 24:
	      case 25:
	      case 26:
	      case 27:
	      case 28:
	      case 29:
		i_ptr->ident |= ID_NOSHOW_P1;
		break;
	      case 30:		   /* Slaying	      */
		i_ptr->ident |= ID_SHOW_HITDAM;
		i_ptr->todam = m_bonus(1, 10, level);
		i_ptr->todam += 2 + randint(3);
		i_ptr->tohit = m_bonus(1, 10, level);
		i_ptr->tohit += 2 + randint(3);
		i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 100;
		if (magik(cursed)) {
		    i_ptr->tohit = -i_ptr->tohit;
		    i_ptr->todam = -i_ptr->todam;
		    i_ptr->flags |= TR_CURSED;
		    i_ptr->cost = -i_ptr->cost;
		}
		break;
	      default:
		break;
	    }
	}
	break;

      case TV_AMULET:		   /* Amulets	      */
	if (i_ptr->sval < 2) {
	    if (magik(cursed)) {
		i_ptr->p1 = -m_bonus(1, 5, level);
		i_ptr->flags |= TR_CURSED;
		i_ptr->cost = -i_ptr->cost;
	    } else {
		i_ptr->p1 = m_bonus(1, 5, level);
		i_ptr->cost += i_ptr->p1 * 100;
	    }
	} else if (i_ptr->sval == 2) { /* searching */
	    i_ptr->p1 = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		i_ptr->p1 = -i_ptr->p1;
		i_ptr->cost = -i_ptr->cost;
		i_ptr->flags |= TR_CURSED;
	    } else
		i_ptr->cost += 20 * i_ptr->p1;
	} else if (i_ptr->sval == 8) {
	    rating += 25;
	    i_ptr->p1 = 5 * (randint(2) + m_bonus(0, 10, level));
	    i_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    i_ptr->cost += 20 * i_ptr->p1 + 50 * i_ptr->toac;
	    if (i_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		i_ptr->flags |= TR_CURSED;
	} else if (i_ptr->sval == 9) {
	/* amulet of DOOM */
	    i_ptr->p1 = (-randint(5) - m_bonus(2, 10, level));
	    i_ptr->toac = (-randint(3) - m_bonus(0, 6, level));
	    i_ptr->flags |= TR_CURSED;
	}
	break;

    /* Subval should be even for store, odd for dungeon */
    /* Dungeon found ones will be partially charged	 */
      case TV_LITE:
	if ((i_ptr->sval % 2) == 1) {
	    i_ptr->p1 = randint(i_ptr->p1);
	    i_ptr->sval -= 1;
	}
	break;

      case TV_WAND:
	switch (i_ptr->sval) {
	  case 0:
	    i_ptr->p1 = randint(10) + 6;
	    break;
	  case 1:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 4:
	    i_ptr->p1 = randint(4) + 3;
	    break;
	  case 5:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 6:
	    i_ptr->p1 = randint(20) + 12;
	    break;
	  case 7:
	    i_ptr->p1 = randint(20) + 12;
	    break;
	  case 8:
	    i_ptr->p1 = randint(10) + 6;
	    break;
	  case 9:
	    i_ptr->p1 = randint(12) + 6;
	    break;
	  case 10:
	    i_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    i_ptr->p1 = randint(3) + 3;
	    break;
	  case 12:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 13:
	    i_ptr->p1 = randint(10) + 6;
	    break;
	  case 14:
	    i_ptr->p1 = randint(5) + 3;
	    break;
	  case 15:
	    i_ptr->p1 = randint(5) + 3;
	    break;
	  case 16:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    i_ptr->p1 = randint(5) + 4;
	    break;
	  case 18:
	    i_ptr->p1 = randint(8) + 4;
	    break;
	  case 19:
	    i_ptr->p1 = randint(6) + 2;
	    break;
	  case 20:
	    i_ptr->p1 = randint(4) + 2;
	    break;
	  case 21:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 22:
	    i_ptr->p1 = randint(5) + 2;
	    break;
	  case 23:
	    i_ptr->p1 = randint(12) + 12;
	    break;
	  case 24:
	    i_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    i_ptr->p1 = randint(3) + 1;
	    break;
	  case 26:
	    i_ptr->p1 = randint(3) + 1;
	    break;
	  case 27:
	    i_ptr->p1 = randint(2) + 1;
	    break;
	  case 28:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  default:
	    break;
	}
	break;

      case TV_STAFF:
	switch (i_ptr->sval) {
	  case 0:
	    i_ptr->p1 = randint(20) + 12;
	    break;
	  case 1:
	    i_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    i_ptr->p1 = randint(20) + 12;
	    break;
	  case 4:
	    i_ptr->p1 = randint(15) + 6;
	    break;
	  case 5:
	    i_ptr->p1 = randint(4) + 5;
	    break;
	  case 6:
	    i_ptr->p1 = randint(5) + 3;
	    break;
	  case 7:
	    i_ptr->p1 = randint(3) + 1;
	    i_ptr->level = 10;
	    break;
	  case 8:
	    i_ptr->p1 = randint(3) + 1;
	    break;
	  case 9:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 10:
	    i_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 12:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 13:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 14:
	    i_ptr->p1 = randint(10) + 12;
	    break;
	  case 15:
	    i_ptr->p1 = randint(3) + 4;
	    break;
	  case 16:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    i_ptr->p1 = randint(5) + 6;
	    break;
	  case 18:
	    i_ptr->p1 = randint(3) + 4;
	    break;
	  case 19:
	    i_ptr->p1 = randint(10) + 12;
	    break;
	  case 20:
	    i_ptr->p1 = randint(3) + 4;
	    break;
	  case 21:
	    i_ptr->p1 = randint(3) + 4;
	    break;
	  case 22:
	    i_ptr->p1 = randint(10) + 6;
	    i_ptr->level = 5;
	    break;
	  case 23:
	    i_ptr->p1 = randint(2) + 1;
	    break;
	  case 24:
	    i_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    i_ptr->p1 = randint(2) + 2;
	    break;
	  case 26:
	    i_ptr->p1 = randint(15) + 5;
	    break;
	  case 27:
	    i_ptr->p1 = randint(2) + 2;
	    break;
	  case 28:
	    i_ptr->p1 = randint(5) + 5;
	    break;
	  case 29:
	    i_ptr->p1 = randint(2) + 1;
	    break;
	  case 30:
	    i_ptr->p1 = randint(6) + 2;
	    break;
	  default:
	    break;
	}
	break;

      case TV_CLOAK:
	if (magik(chance) || good) {
	    int                 made_art_cloak;

	    made_art_cloak = 0;
	    i_ptr->toac += 1 + m_bonus(0, 20, level);
	    if (magik(special) || (good == 666)) {
		if (!not_unique &&
		    !stricmp(object_list[i_ptr->index].name, "& Cloak")
		    && randint(10) == 1) {
		    switch (randint(9)) {
		      case 1:
		      case 2:
			if (COLLUIN)
			    break;
			if (wizard || peek)
			    msg_print("Colluin");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_COLLUIN;
			i_ptr->toac = 15;
			i_ptr->flags |= (TR2_RES_FIRE | TR_RES_COLD | TR_POISON |
					 TR2_RES_LIGHT | TR2_RES_ACID);
			i_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			i_ptr->cost = 10000L;
			made_art_cloak = 1;
			COLLUIN = 1;
			break;
		      case 3:
		      case 4:
			if (HOLCOLLETH)
			    break;
			if (wizard || peek)
			    msg_print("Holcolleth");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_HOLCOLLETH;
			i_ptr->toac = 4;
			i_ptr->p1 = 2;
			i_ptr->flags |= (TR1_INT | TR1_WIS | TR1_STEALTH |
					 TR2_RES_ACID);
			i_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			i_ptr->ident |= ID_NOSHOW_TYPE;
			i_ptr->cost = 13000L;
			made_art_cloak = 1;
			HOLCOLLETH = 1;
			break;
		      case 5:
			if (THINGOL)
			    break;
			if (wizard || peek)
			    msg_print("Thingol");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_THINGOL;
			i_ptr->toac = 18;
			i_ptr->flags = (TR1_DEX | TR1_CHR | TR2_RES_FIRE |
				   TR2_RES_ACID | TR2_RES_COLD | TR2_FREE_ACT);
			i_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			i_ptr->p1 = 3;
			i_ptr->cost = 35000L;
			made_art_cloak = 1;
			THINGOL = 1;
			break;
		      case 6:
		      case 7:
			if (THORONGIL)
			    break;
			if (wizard || peek)
			    msg_print("Thorongil");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_THORONGIL;
			i_ptr->toac = 10;
			i_ptr->flags = (TR_SEE_INVIS | TR2_FREE_ACT |
					TR2_RES_ACID);
			i_ptr->flags2 |= (TR_ARTIFACT);
			i_ptr->cost = 8000L;
			made_art_cloak = 1;
			THORONGIL = 1;
			break;
		      case 8:
		      case 9:
			if (COLANNON)
			    break;
			if (wizard || peek)
			    msg_print("Colannon");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_COLANNON;
			i_ptr->toac = 15;
			i_ptr->flags |= (TR1_STEALTH | TR2_RES_ACID);
			i_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			i_ptr->p1 = 3;
			i_ptr->cost = 11000L;
			made_art_cloak = 1;
			COLANNON = 1;
			break;
		    }

		} else if (!not_unique &&
			   !stricmp(object_list[i_ptr->index].name,
				    "& Shadow Cloak")
			   && randint(20) == 1) {
		    switch (randint(2)) {
		      case 1:
			if (LUTHIEN)
			    break;
			if (wizard || peek)
			    msg_print("Luthien");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_LUTHIEN;
			i_ptr->toac = 20;
			i_ptr->flags = (TR2_RES_FIRE | TR_RES_COLD |
				    TR1_INT | TR1_WIS | TR1_CHR | TR2_RES_ACID);
			i_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			i_ptr->p1 = 2;
			i_ptr->cost = 45000L;
			made_art_cloak = 1;
			LUTHIEN = 1;
			break;
		      case 2:
			if (TUOR)
			    break;
			if (wizard || peek)
			    msg_print("Tuor");
			else
			    good_item_flag = TRUE;
			i_ptr->name2 = SN_TUOR;
			i_ptr->toac = 12;
			i_ptr->flags = (TR1_STEALTH |
				  TR2_FREE_ACT | TR_SEE_INVIS | TR2_RES_ACID);
			i_ptr->flags2 |= (TR2_IM_ACID | TR_ARTIFACT);
			i_ptr->p1 = 4;
			i_ptr->cost = 35000L;
			made_art_cloak = 1;
			TUOR = 1;
			break;
		    }
		}
		if (!made_art_cloak) {
		    if (randint(2) == 1) {
			i_ptr->name2 = SN_PROTECTION;
			i_ptr->toac += m_bonus(0, 10, level) + (5 + randint(3));
			i_ptr->cost += 250L;
			rating += 8;
		    } else if (randint(10) < 10) {
			i_ptr->toac += m_bonus(3, 10, level);
			i_ptr->p1 = randint(3);
			i_ptr->flags |= TR1_STEALTH;
			i_ptr->name2 = SN_STEALTH;
			i_ptr->cost += 500 + (50 * i_ptr->p1);
			rating += 9;
		    } else {
			i_ptr->toac += 10 + randint(10);
			i_ptr->p1 = randint(3);
			i_ptr->flags |= (TR1_STEALTH | TR2_RES_ACID);
			i_ptr->name2 = SN_AMAN;
			i_ptr->cost += 4000 + (100 * i_ptr->toac);
			rating += 16;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		i_ptr->flags |= TR_AGGRAVATE;
		i_ptr->name2 = SN_IRRITATION;
		i_ptr->toac -= m_bonus(1, 10, level);
		i_ptr->ident |= ID_SHOW_HITDAM;
		i_ptr->tohit -= m_bonus(1, 10, level);
		i_ptr->todam -= m_bonus(1, 10, level);
		i_ptr->cost = 0;
	    } else if (tmp == 2) {
		i_ptr->name2 = SN_VULNERABILITY;
		i_ptr->toac -= m_bonus(10, 20, level + 50);
		i_ptr->cost = 0;
	    } else {
		i_ptr->name2 = SN_ENVELOPING;
		i_ptr->toac -= m_bonus(1, 10, level);
		i_ptr->ident |= ID_SHOW_HITDAM;
		i_ptr->tohit -= m_bonus(2, 15, level + 10);
		i_ptr->todam -= m_bonus(2, 15, level + 10);
		i_ptr->cost = 0;
	    }
	    i_ptr->flags |= TR_CURSED;
	}
	break;

      case TV_CHEST:
	switch (randint(level + 4)) {
	  case 1:
	    i_ptr->flags = 0;
	    i_ptr->name2 = SN_EMPTY;
	    break;
	  case 2:
	    i_ptr->flags |= CH2_LOCKED;
	    i_ptr->name2 = SN_LOCKED;
	    break;
	  case 3:
	  case 4:
	    i_ptr->flags |= (CH2_LOSE_STR | CH2_LOCKED);
	    i_ptr->name2 = SN_POISON_NEEDLE;
	    break;
	  case 5:
	  case 6:
	    i_ptr->flags |= (CH2_POISON | CH2_LOCKED);
	    i_ptr->name2 = SN_POISON_NEEDLE;
	    break;
	  case 7:
	  case 8:
	  case 9:
	    i_ptr->flags |= (CH2_PARALYSED | CH2_LOCKED);
	    i_ptr->name2 = SN_GAS_TRAP;
	    break;
	  case 10:
	  case 11:
	    i_ptr->flags |= (CH2_EXPLODE | CH2_LOCKED);
	    i_ptr->name2 = SN_EXPLOSION_DEVICE;
	    break;
	  case 12:
	  case 13:
	  case 14:
	    i_ptr->flags |= (CH2_SUMMON | CH2_LOCKED);
	    i_ptr->name2 = SN_SUMMONING_RUNES;
	    break;
	  case 15:
	  case 16:
	  case 17:
	    i_ptr->flags |= (CH2_PARALYSED | CH2_POISON | CH2_LOSE_STR |
			     CH2_LOCKED);
	    i_ptr->name2 = SN_MULTIPLE_TRAPS;
	    break;
	  default:
	    i_ptr->flags |= (CH2_SUMMON | CH2_EXPLODE | CH2_LOCKED);
	    i_ptr->name2 = SN_MULTIPLE_TRAPS;
	    break;
	}
	if (not_unique)		/* if bought from store - dbd */
	    i_ptr->p1 = randint(i_ptr->level);
	else			/* store the level chest's found on - dbd */
	    i_ptr->p1 = dun_level;
	break;

      case TV_SPIKE:
	i_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    i_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	i_ptr->p1 = missile_ctr;
	break;

    case TV_BOLT: case TV_ARROW: case TV_SHOT:
     /* this fn makes ammo for player's missile weapon more common -CFT */
      magic_ammo(i_ptr, good, chance, special, cursed, level);
      break;

      case TV_FOOD:
    /* make sure all food rations have the same level */
	if (i_ptr->sval == 90)
	    i_ptr->level = 0;
    /* give all elvish waybread the same level */
	else if (i_ptr->sval == 92)
	    i_ptr->level = 6;
	break;

      case TV_SCROLL1:
    /* give all identify scrolls the same level */
	if (i_ptr->sval == 67)
	    i_ptr->level = 1;
    /* scroll of light */
	else if (i_ptr->sval == 69)
	    i_ptr->level = 0;
    /* scroll of trap detection */
	else if (i_ptr->sval == 80)
	    i_ptr->level = 5;
    /* scroll of door/stair location */
	else if (i_ptr->sval == 81)
	    i_ptr->level = 5;
	break;

      case TV_POTION1:		   /* potions */
    /* cure light */
	if (i_ptr->sval == 76)
	    i_ptr->level = 0;
	break;

      default:
	break;
    }
}

/*
 * XXX Mega-Hack -- attempt to place one of the "Special Objects"
 */
int special_place_object(int y, int x)
{
    register int	cur_pos, tmp;
    char         str[100];
    int          done = 0;

    /* Is this cave grid sacred? */
    if (!in_bounds(y, x)) return 0;		   /* abort! -CFT */

    if (cave[y][x].tptr != 0)
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return 0;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    str[0] = 0;

again:
    if (done > 20)
	return 0;
    tmp = randint(MAX_OBJECTS - (SPECIAL_OBJ - 1)) + (SPECIAL_OBJ - 1) - 1;
    switch (tmp) {
      case (SPECIAL_OBJ - 1):
	done++;
	if (randint(30) > 1)
	    goto again;
	if (NARYA)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Narya");
	else
	    good_item_flag = TRUE;
	NARYA = TRUE;
	break;
      case (SPECIAL_OBJ):
	done++;
	if (randint(35) > 1)
	    goto again;
	if (NENYA)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Nenya");
	else
	    good_item_flag = TRUE;
	NENYA = TRUE;
	break;
      case (SPECIAL_OBJ + 1):
	done++;
	if (randint(40) > 1)
	    goto again;
	if (VILYA)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(70) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Vilya");
	else
	    good_item_flag = TRUE;
	VILYA = TRUE;
	break;
      case (SPECIAL_OBJ + 2):
	done++;
	if (randint(60) > 1)
	    goto again;
	if (POWER)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(100) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Power (The One Ring)");
	else
	    good_item_flag = TRUE;
	POWER = TRUE;
	break;
      case (SPECIAL_OBJ + 3):
	done++;
	if (PHIAL)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(30) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Phial of Galadriel");
	else
	    good_item_flag = TRUE;
	PHIAL = TRUE;
	break;
      case (SPECIAL_OBJ + 4):
	done++;
	if (randint(10) > 1)
	    goto again;
	if (INGWE)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Amulet of Ingwe");
	else
	    good_item_flag = TRUE;
	INGWE = TRUE;
	break;
      case (SPECIAL_OBJ + 5):
	done++;
	if (randint(6) > 1)
	    goto again;
	if (CARLAMMAS)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(35) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Amulet of Carlammas");
	else
	    good_item_flag = TRUE;
	CARLAMMAS = TRUE;
	break;
      case (SPECIAL_OBJ + 6):
	done++;
	if (randint(8) > 1)
	    goto again;
	if (ELENDIL)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(30) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Star of Elendil");
	else
	    good_item_flag = TRUE;
	ELENDIL = TRUE;
	break;
      case (SPECIAL_OBJ + 7):
	done++;
	if (randint(18) > 1)
	    goto again;
	if (THRAIN)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Arkenstone of Thrain");
	else
	    good_item_flag = TRUE;
	THRAIN = TRUE;
	break;
      case (SPECIAL_OBJ + 8):
	done++;
	if (randint(25) > 1)
	    goto again;
	if (TULKAS)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(65) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Ring of Tulkas");
	else
	    good_item_flag = TRUE;
	TULKAS = TRUE;
	break;
      case (SPECIAL_OBJ + 9):
	done++;
	if (randint(25) > 1)
	    goto again;
	if (NECKLACE)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Necklace of the Dwarves");
	else
	    good_item_flag = TRUE;
	NECKLACE = TRUE;
	break;
      case (SPECIAL_OBJ + 10):
	done++;
	if (randint(20) > 1)
	    goto again;
	if (BARAHIR)
	    goto again;
	if ((object_list[tmp].level - 40) > object_level)
	    goto again;
	if ((object_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Ring of Barahir");
	else
	    good_item_flag = TRUE;
	BARAHIR = TRUE;
	break;
    }
    if (strlen(str) > 0 && (wizard || peek))
	msg_print(str);
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    invcopy(&t_list[cur_pos], tmp);
    t_list[cur_pos].timeout = 0;
    t_list[cur_pos].ident |= ID_NOSHOW_TYPE; /* don't show (+x of yyy) for these */
    if (object_list[tmp].level > object_level) {
	rating += 2 * (object_list[sorted_objects[tmp]].level - object_level);
    }
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
    return (-1);
}


/*
 * Places an object at given row, column co-ordinate    -RAK-
 */
void place_object(int y, int x)
{
    register int cur_pos, tmp;

    if (!in_bounds(y,x)) return; /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return; /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y,x);
    
    if (randint(MAX_OBJECTS)>SPECIAL_OBJ && randint(10)==1)
	if (special_place_object(y,x)==(-1))
	    return;
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;

    do {	   /* don't generate another chest if opening_chest is true -CWS */
	tmp = get_obj_num(dun_level, FALSE);
    } while (opening_chest && (object_list[sorted_objects[tmp]].tval == TV_CHEST));
	
    invcopy(&t_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, dun_level, FALSE, 0);
    if (object_list[sorted_objects[tmp]].level > dun_level)
	rating += object_list[sorted_objects[tmp]].level - dun_level;
    if (peek) {
	if (object_list[sorted_objects[tmp]].level > dun_level) {
	    char buf[200];
	    byte temp;
	    
	    temp=t_list[cur_pos].ident;
	    t_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &t_list[cur_pos], TRUE);
	    t_list[cur_pos].ident = temp;
	    msg_print(buf);
	}
    }
    if (cave[y][x].cptr == 1)
	msg_print ("You feel something roll beneath your feet.");
}


/*
 * Places a "GOOD" object at given row, column co-ordinate ~Ludwig 
 */
void place_good(int y, int x, u32b good)
{
    register int cur_pos, tmp;
    int          tv, is_good = FALSE;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);

    if (randint(10) == 1)
	if (special_place_object(y, x) == (-1))
	    return;
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    do {
	tmp = get_obj_num((object_level + 10), TRUE);
	tv = object_list[sorted_objects[tmp]].tval;

	if ((tv == TV_HELM) || (tv == TV_SHIELD) || (tv == TV_CLOAK) ||
	    (tv == TV_HAFTED) || (tv == TV_POLEARM) ||
	    (tv == TV_BOW) || (tv == TV_BOLT) || (tv == TV_ARROW) ||
	    (tv == TV_BOOTS) || (tv == TV_GLOVES))
	    is_good = TRUE;

	if ((tv == TV_SWORD) &&
	    strncmp("& Broken", object_list[sorted_objects[tmp]].name, 8))
	    is_good = TRUE;	   /* broken swords/daggers are NOT good!
				    * -CFT */
	if ((tv == TV_HARD_ARMOR) &&
	    strncmp("Rusty", object_list[sorted_objects[tmp]].name, 5))
	    is_good = TRUE;	   /* rusty chainmail is NOT good! -CFT */
	if ((tv == TV_SOFT_ARMOR) &&
	 stricmp("some filthy rags", object_list[sorted_objects[tmp]].name))
	    is_good = TRUE;	   /* nor are rags! -CFT */
	if ((tv == TV_MAGIC_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Raal's */
	    (object_list[sorted_objects[tmp]].sval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
	if ((tv == TV_PRAYER_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Wrath of
					 * God */
	    (object_list[sorted_objects[tmp]].sval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
    } while (!is_good);

    invcopy(&t_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, object_level, (good & SPECIAL) ? 666 : 1, 0);

	/* Hack -- look at it */
	if (peek) {
	    char buf[200];
	    if (object_list[sorted_objects[tmp]].level > object_level) {
	    byte               t;

	    t = t_list[cur_pos].ident;
	    t_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &t_list[cur_pos], TRUE);
	    t_list[cur_pos].ident = t;
	    msg_print(buf);
	}
    }

    if (cave[y][x].cptr == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}




/*
 * Creates objects nearby the coordinates given		-RAK-	 
 */
void random_object(int y, int x, int num)
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    /* Attempt to place 'num' objects */
    for (; num > 0; --num) {

	/* Try up to 10 spots looking for empty space */
	for (i = 0; i < 11; ++i) {

	    /* Pick a random location */
	    do {
		j = y - 3 + randint(5);
		k = x - 4 + randint(7);
	    } while (!in_bounds(j, k));
	    cave_ptr = &cave[j][k];

	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		object_level = dun_level;

	    /* Place something */
	    if (randint(100) < 75) {
		place_object(j, k);
	    }
	    else {
		place_gold(j, k);
	    }

	    /* Placement accomplished */
	    break;
	    }
	}
    }
}


/*
 * Same as above, but always "special"
 * Only really called by "scroll of *acquirement*"
 */
void special_random_object(int y, int x, int num)
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    object_level = dun_level;
    for (; num > 0; --num) {

	/* Try up to 10 spots looking for empty space */
	for (i = 0; i < 11; ++i) {
	    
	    /* Pick a random spot */
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = &cave[j][k];

	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		if (randint(5) == 1) {
		    if (!special_place_object(j, k))
			place_good(j, k, SPECIAL);
		} else {
		    place_good(j, k, SPECIAL);
		}

	    /* Placement accomplished */
	    break;
	    }
	}
    }
}




/*
 * Places a particular trap at location y, x		-RAK-	 
 */
void place_trap(int y, int x, int sval)
{
    register int cur_pos;

    if (!in_bounds(y, x))
	return;	       /* abort! -CFT */
    if (cave[y][x].cptr >= MIN_M_IDX)
	return;	       /* don't put rubble under monsters, it's annoying -CFT */

    if (cave[y][x].tptr != 0)
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    invcopy(&t_list[cur_pos], OBJ_TRAP_LIST + sval);
}


/*
 * Places rubble at location y, x			-RAK-	
 */
void place_rubble(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    register inven_type *i_ptr;

    if (!in_bounds(y, x)) return;			   /* abort! -CFT */

    if (cave[y][x].tptr != 0)

   /* don't replace stairs, stores, artifacts */
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;
	else
    /* Delete whatever is there */
    delete_object(y, x);

    cur_pos = i_pop();
    i_ptr = &t_list[cur_pos];
    invcopy(i_ptr, OBJ_RUBBLE);

    /* Put the rubble in the cave */
    c_ptr = &cave[y][x];
    c_ptr->tptr = cur_pos;

    /* Hack -- nuke any walls */
    c_ptr->fval = BLOCKED_FLOOR;
}

/*
 * Return the "coin type" of a monster race
 * Used to allocate proper treasure for killing creeping coins -CWS
 */
void get_coin_type(monster_race *r_ptr)
{
    cptr name;

    name = r_ptr->name;
    if (!stricmp(name, "Creeping copper coins")) coin_type = 2;
    if (!stricmp(name, "Creeping silver coins")) coin_type = 5;
    if (!stricmp(name, "Creeping gold coins")) coin_type = 10;
    if (!stricmp(name, "Creeping mithril coins")) coin_type = 16;
    if (!stricmp(name, "Creeping adamantite coins")) coin_type = 17;
}

/*
 * Places a treasure (Gold or Gems) at given row, column -RAK-	 */
void place_gold(int y, int x)
{
    register int        i, cur_pos;
    register inven_type *i_ptr;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((t_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (t_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (t_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((t_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (t_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (t_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    cur_pos = i_pop();

    /* Pick a Treasure variety */
    i = ((randint(object_level + 2) + 2) / 2) - 1;

    /* Apply "extra" magic */
    if (randint(GREAT_OBJ) == 1) {
	i += randint(object_level + 1);
    }

    /* Hack -- Creeping Coins only generate "themselves" */
    if (coin_type) i = coin_type;

    /* Do not create "illegal" Treasure Types */
    if (i >= MAX_GOLD) i = MAX_GOLD - 1;

    /* Make it */
    cave[y][x].tptr = cur_pos;
    i_ptr = &t_list[cur_pos];
    invcopy(i_ptr, OBJ_GOLD_LIST + i);

    /* Determine the "cost" */
    i_ptr->cost += (8L * (long)randint((int)i_ptr->cost)) + randint(8);

    /* Hack -- average the values to make sure "creeping coins" are not too valuable */
    if (coin_type) {
	i_ptr->cost = ((8L * (long)randint((int)object_list[OBJ_GOLD_LIST + i].cost))
		       + (i_ptr->cost)) >> 1;
    }

    /* Under the player */
    if (cave[y][x].cptr == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}


/*
 * Returns the array number of a random object		-RAK-
 */
int get_obj_num(int level, int good)
{
    register int i, j;

    do {
	if (level == 0) i = rand_int(t_level[0]);
	else {
	    if (level >= MAX_OBJ_LEVEL) level = MAX_OBJ_LEVEL;
	    else if (randint(GREAT_OBJ) == 1) {
		level = level * MAX_OBJ_LEVEL / randint(MAX_OBJ_LEVEL) + 1;
		if (level > MAX_OBJ_LEVEL) level = MAX_OBJ_LEVEL;
	    }


	/*
	 * This code has been added to make it slightly more likely to get
	 * the higher level objects.	Originally a uniform distribution
	 * over all objects less than or equal to the dungeon level.  This
	 * distribution makes a level n objects occur approx 2/n% of the time
	 * on level n, and 1/2n are 0th level. 
	 */

	    if (randint(2) == 1)
		i = rand_int(t_level[level]);
	    else {		   /* Choose three objects, pick the highest level. */
		i = rand_int(t_level[level]);
		j = rand_int(t_level[level]);
		if (i < j) i = j;
		j = rand_int(t_level[level]);
		if (i < j) i = j;
		j = object_list[sorted_objects[i]].level;
		if (j == 0) i = rand_int(t_level[0]);
		else i = randint(t_level[j] - t_level[j - 1]) - 1 + t_level[j - 1];
	    }
	}
    } while (((object_list[sorted_objects[i]].rare ?
	       (randint(object_list[sorted_objects[i]].rare) - 1) : 0) && !good)
	     || (object_list[sorted_objects[i]].rare == 255));

    /* Accept that object */
    return (i);
}



