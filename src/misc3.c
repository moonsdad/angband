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
 * Pushs a record back onto free space list -RAK-
 *
 * Delete_object() should always be called instead, unless the object in
 * question is not in the dungeon, e.g. in store1.c and files.c 
 */
void pusht(int my_x)
{
    s16b        x = (s16b) my_x;
    register int i, j;

    if (x != i_max - 1) {
	i_list[x] = t_list[i_max - 1];

    /* must change the i_idx in the cave of the object just moved */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++)
		if (cave[i][j].i_idx == i_max - 1)
		    cave[i][j].i_idx = x;
    }
    i_max--;
    invcopy(&i_list[i_max], OBJ_NOTHING);
}


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
    pusht(c_ptr->i_idx);

    /* There is nothing here */
    c_ptr->i_idx = 0;

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
	for (i = 0; i < cur_height; i++) {
	    for (j = 0; j < cur_width; j++) {

		/* Get the location */
		c_ptr = &cave[i][j];

		if ((c_ptr->i_idx != 0)
		    && (distance(i, j, char_row, char_col) > cur_dis)) {

		/* Every object gets a "saving throw" */
		switch (i_list[c_ptr->i_idx].tval) {
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
			if ((i_list[c_ptr->i_idx].tval >= TV_MIN_WEAR) &&
			    (i_list[c_ptr->i_idx].tval <= TV_MAX_WEAR) &&
			    (i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT))
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
	case 6: i_ptr->flags2 |= TR2_RES_NETHER; break;
	case 7: i_ptr->flags2 |= TR2_RES_SHARDS; break;
	case 8: i_ptr->flags2 |= TR2_RES_NEXUS; break;
	case 9: i_ptr->flags2 |= TR2_RES_BLIND; break;
	case 10: i_ptr->flags2 |= TR2_RES_DISEN; break;
    }
}


/*
 * Charge a wand  XXX Redo this cleanly!
 */
static void charge_wand(inven_type *i_ptr)
{
    switch (i_ptr->sval) {
      case 0:
	i_ptr->pval = randint(20) + 12;
	break;
      case 1:
	i_ptr->pval = randint(8) + 6;
	break;
      case 2:
	i_ptr->pval = randint(5) + 6;
	break;
      case 3:
	i_ptr->pval = randint(20) + 12;
	break;
      case 4:
	i_ptr->pval = randint(15) + 6;
	break;
      case 5:
	i_ptr->pval = randint(4) + 5;
	break;
      case 6:
	i_ptr->pval = randint(5) + 3;
	break;
      case 7:
	i_ptr->pval = randint(3) + 1;
	i_ptr->level = 10;
	break;
      case 8:
	i_ptr->pval = randint(3) + 1;
	break;
      case 9:
	i_ptr->pval = randint(5) + 6;
	break;
      case 10:
	i_ptr->pval = randint(10) + 12;
	break;
      case 11:
	i_ptr->pval = randint(5) + 6;
	break;
      case 12:
	i_ptr->pval = randint(5) + 6;
	break;
      case 13:
	i_ptr->pval = randint(5) + 6;
	break;
      case 14:
	i_ptr->pval = randint(10) + 12;
	break;
      case 15:
	i_ptr->pval = randint(3) + 4;
	break;
      case 16:
	i_ptr->pval = randint(5) + 6;
	break;
      case 17:
	i_ptr->pval = randint(5) + 6;
	break;
      case 18:
	i_ptr->pval = randint(3) + 4;
	break;
      case 19:
	i_ptr->pval = randint(10) + 12;
	break;
      case 20:
	i_ptr->pval = randint(3) + 4;
	break;
  case 21:
	i_ptr->pval = randint(3) + 4;
	break;
  case 22:
	i_ptr->pval = randint(10) + 6;
	i_ptr->level = 5;
	break;
      case 23:
	i_ptr->pval = randint(2) + 1;
	break;
      case 24:
	i_ptr->pval = randint(3) + 1;
	break;
      case 25:
	i_ptr->pval = randint(2) + 2;
	break;
      case 26:
	i_ptr->pval = randint(15) + 5;
	break;
  case 27:
	i_ptr->pval = randint(2) + 2;
	break;
      case 28:
	i_ptr->pval = randint(5) + 5;
	break;
      case 29:
	i_ptr->pval = randint(2) + 1;
	break;
      case 30:
	i_ptr->pval = randint(6) + 2;
	break;
      default:
	break;
    }
}


/*
 * Charge a staff  XXX Redo this cleanly!
 */
static void charge_staff(inven_type *i_ptr)
{        
    switch (i_ptr->sval) {
      case 0:
	i_ptr->pval = randint(20) + 12;
	break;
      case 1:
	i_ptr->pval = randint(8) + 6;
	break;
      case 2:
	i_ptr->pval = randint(5) + 6;
	break;
      case 3:
	i_ptr->pval = randint(20) + 12;
	break;
      case 4:
	i_ptr->pval = randint(15) + 6;
	break;
      case 5:
	i_ptr->pval = randint(4) + 5;
	break;
      case 6:
	i_ptr->pval = randint(5) + 3;
	break;
      case 7:
	i_ptr->pval = randint(3) + 1;
	i_ptr->level = 10;
	break;
      case 8:
	i_ptr->pval = randint(3) + 1;
	break;
      case 9:
	i_ptr->pval = randint(5) + 6;
	break;
      case 10:
	i_ptr->pval = randint(10) + 12;
	break;
      case 11:
	i_ptr->pval = randint(5) + 6;
	break;
      case 12:
	i_ptr->pval = randint(5) + 6;
	break;
      case 13:
	i_ptr->pval = randint(5) + 6;
	break;
      case 14:
	i_ptr->pval = randint(10) + 12;
	break;
      case 15:
	i_ptr->pval = randint(3) + 4;
	break;
      case 16:
	i_ptr->pval = randint(5) + 6;
	break;
      case 17:
	i_ptr->pval = randint(5) + 6;
	break;
      case 18:
	i_ptr->pval = randint(3) + 4;
	break;
      case 19:
	i_ptr->pval = randint(10) + 12;
	break;
      case 20:
	i_ptr->pval = randint(3) + 4;
	break;
      case 21:
	i_ptr->pval = randint(3) + 4;
	break;
      case 22:
	i_ptr->pval = randint(10) + 6;
	i_ptr->level = 5;
	break;
      case 23:
	i_ptr->pval = randint(2) + 1;
	break;
      case 24:
	i_ptr->pval = randint(3) + 1;
	break;
      case 25:
	i_ptr->pval = randint(2) + 2;
	break;
      case 26:
	i_ptr->pval = randint(15) + 5;
	break;
      case 27:
	i_ptr->pval = randint(2) + 2;
	break;
      case 28:
	i_ptr->pval = randint(5) + 5;
	break;
      case 29:
	i_ptr->pval = randint(2) + 1;
	break;
      case 30:
	i_ptr->pval = randint(6) + 2;
	break;
      default:
	break;
    }
}



/*
 * Chance of treasure having magic abilities		-RAK-
 * Chance increases with each dungeon level			 
 *
 * some objects appear multiple times in the k_list with different
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

    i_ptr = &i_list[x];

    /* Depending on treasure type, it can have certain magical properties */
    switch (i_ptr->tval) {

      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:
      case TV_SHIELD:

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
	    if (!stricmp(k_list[i_ptr->index].name, "& Robe") &&
		((magik(special) && randint(30) == 1)
		 || (good == 666 && magik(special)))) {

		    i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD | 
				      TR2_RES_ACID | TR2_RES_FIRE |
				      TR2_HOLD_LIFE |
				      TR_SUST_STAT);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE | TR3_IGNORE_COLD |
				      TR3_IGNORE_ELEC | TR3_IGNORE_ACID);
		    i_ptr->ident |= ID_NOSHOW_P1;
		    i_ptr->toac += 10 + randint(5);
		    i_ptr->cost = 10000L + (i_ptr->toac * 100);
		    give_1_hi_resist(i_ptr);	/* JLS */
		    i_ptr->name2 = EGO_MAGI;
		    i_ptr->pval = 10;
		    rating += 30;
		    if (wizard || peek) msg_print("Robe of the Magi");
	    } else if (magik(special) || good == 666)

		/* Make it "Excellent" */
		switch (randint(9)) {

		  case 1:
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD |
				      TR2_RES_ACID | TR2_RES_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_ELEC | TR3_IGNORE_COLD |
				      TR3_IGNORE_ACID | TR3_IGNORE_FIRE);
		    if (randint(3) == 1) {
			i_ptr->flags1 |= TR1_STEALTH;
			i_ptr->pval = randint(3);
			i_ptr->toac += 15;
			i_ptr->cost += 15000L;
			give_1_hi_resist(i_ptr); /* JLS */
			i_ptr->name2 = EGO_ELVENKIND;
			rating += 25;
			if (peek) msg_print("Elvenkind");
		    }
		    else {
			i_ptr->toac += 8;
			i_ptr->cost += 12500L;
			i_ptr->name2 = EGO_R;
			rating += 20;
			if (peek) msg_print("Resist");
		    }
		    break;

		  case 2: /* Resist Acid */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    if (!strncmp(k_list[i_ptr->index].name,
				 "Mithril", 7) ||
			!strncmp(k_list[i_ptr->index].name,
				 "Adamantite", 10))
			break;
		    i_ptr->flags2 |= (TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
		    i_ptr->cost += 1000L;
		    i_ptr->name2 = EGO_RESIST_A;
		    rating += 15;
		    if (peek) msg_print("Resist Acid");
		    break;

		  case 3: case 4: /* Resist Fire */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE);
		    i_ptr->cost += 600L;
		    i_ptr->name2 = EGO_RESIST_F;
		    rating += 17;
		    if (peek) msg_print("Resist Fire");
		    break;

		  case 5: case 6: /* Resist Cold */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_COLD);
		    i_ptr->flags3 |= (TR3_IGNORE_COLD);
		    i_ptr->cost += 600L;
		    i_ptr->name2 = EGO_RESIST_C;
		    rating += 16;
		    if (peek) msg_print("Resist Cold");
		    break;

		  case 7: case 8: case 9: /* Resist Lightning */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_ELEC);
		    i_ptr->flags3 |= (TR3_IGNORE_ELEC);
		    i_ptr->cost += 500L;
		    i_ptr->name2 = EGO_RESIST_E;
		    rating += 15;
		    if (peek) msg_print("Resist Lightning");
		    break;
		}
	}

	/* Cursed armor */
	else if (magik(cursed)) {
	    i_ptr->cost = 0L;
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac = 0 - randint(3) - m_bonus(0, 10, level);
	}

	break;


      case TV_GLOVES:

	/* Good */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);

	    if ((((randint(2) == 1) && magik(5 * special / 2)) || (good == 666)) &&
		!stricmp(k_list[i_ptr->index].name,
			 "& Set of Leather Gloves") &&
		!not_unique && unique_armour(i_ptr));
	    else if ((((randint(4) == 1) && magik(special)) || (good == 666))
		     && !stricmp(k_list[i_ptr->index].name,
				 "& Set of Gauntlets") &&
		     !not_unique && unique_armour(i_ptr));
	    else if ((((randint(5) == 1) && magik(special)) || (good == 666))
		     && !stricmp(k_list[i_ptr->index].name,
				 "& Set of Cesti") &&
		     !not_unique && unique_armour(i_ptr));

	/* don't forget cesti -CFT */
	    else if (magik(special) || (good == 666)) {
		switch (randint(10)) {

		  case 1: case 2: case 3:
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->cost += 1000L;
		    i_ptr->name2 = EGO_FREE_ACTION;
		    rating += 11;
		    if (peek) msg_print("Free action");
		    break;

		  case 4: case 5: case 6:
		    i_ptr->flags3 |= (TR3_SHOW_MODS);
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 250;
		    i_ptr->name2 = EGO_SLAYING;
		    rating += 17;
		    if (peek) msg_print("Slaying");
		    break;

		  case 7: case 8: case 9:
		    i_ptr->flags1 |= (TR1_DEX);
		    i_ptr->pval = 2 + randint(2);	/* +N DEX */
		    i_ptr->cost += (i_ptr->pval) * 400;
		    i_ptr->name2 = EGO_AGILITY;
		    rating += 14;
		    if (peek) msg_print("Agility");
		    break;

		  case 10:
		    if (((randint(3) == 1) || (good == 666)) && !not_unique &&
			unique_armour(i_ptr))
			break;
		    i_ptr->flags1 |= (TR1_STR);
		    i_ptr->flags3 |= (TR3_SHOW_MODS | TR3_HIDE_TYPE);
		    i_ptr->pval = 1 + randint(4);	/* +N STR */
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->cost += ((i_ptr->tohit + i_ptr->todam +
				     i_ptr->pval) * 300);
		    i_ptr->name2 = EGO_POWER;
		    rating += 22;
		    if (peek) msg_print("Power");
		    break;
		}
	    }
	}

	/* Cursed gloves */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac = (-m_bonus(1, 20, level));
	    i_ptr->cost = 0;

	    /* Permanently damaged */
	    if (magik(special)) {

		/* Strength or Dexterity */
		if (randint(2) == 1) {
		    i_ptr->flags1 |= TR1_DEX;
		    i_ptr->pval = (randint(3) - m_bonus(0, 10, level));
		    i_ptr->name2 = EGO_CLUMSINESS;
		}
		else {
		    i_ptr->flags1 |= TR1_STR;
		    i_ptr->pval = (randint(3) - m_bonus(0, 10, level));
		    i_ptr->name2 = EGO_WEAKNESS;
		}
	    }
	}

	break;


      case TV_BOOTS:

	/* Good */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(1, 10, level);

	    /* Apply more magic */
	    if (magik(special) || (good == 666)) {

		tmp = randint(12);
		if (tmp == 1) {

		/* Make it "excellent" */
		    if (!((randint(2) == 1) && !not_unique
			  && unique_armour(i_ptr))) {

		    i_ptr->flags1 |= TR1_SPEED;
		    i_ptr->name2 = EGO_SPEED;

		    i_ptr->pval = 1;
		    
		    /* Calculate the cost */
		    i_ptr->cost += 300000L;
		    
		    /* Increase the rating */
		    rating += 30;
		    if (wizard || peek) msg_print("Boots of Speed");
		    }
		}

		  else if (stricmp("& Pair of Metal Shod Boots",
				   k_list[i_ptr->index].name))	/* not metal */
		    if (tmp > 6) {
			i_ptr->flags3 |= TR3_FEATHER;
			i_ptr->cost += 250;
			i_ptr->name2 = EGO_SLOW_DESCENT;
			rating += 7;
		    }

		  else if (tmp < 5) {
			i_ptr->flags1 |= TR1_STEALTH;
			i_ptr->pval = randint(3);	/* +N Stealth */
			i_ptr->cost += 500;
			i_ptr->name2 = EGO_STEALTH;
			rating += 16;
		    }

		  else {	   /* 5,6 */
			i_ptr->flags2 |= TR2_FREE_ACT;
			i_ptr->cost += 500;
			i_ptr->cost *= 2;
			i_ptr->name2 = EGO_FREE_ACTION;
			rating += 15;
		    }

		else
		 /* is metal boots, different odds since no stealth */
		    if (tmp < 5) {
			i_ptr->flags2 |= TR2_FREE_ACT;
			i_ptr->cost += 500;
			i_ptr->cost *= 2;
			i_ptr->name2 = EGO_FREE_ACTION;
			rating += 15;
		    }

		  else {	   /* tmp > 4 */
			i_ptr->flags3 |= TR3_FEATHER;
			i_ptr->cost += 250;
			i_ptr->name2 = EGO_SLOW_DESCENT;
			rating += 7;
		    }
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->cost = 0;
	    i_ptr->toac = (-m_bonus(2, 20, level));

		/* Pick some damage */
		switch (randint(3)) {
		    case 1:
			i_ptr->flags1 |= TR1_SPEED;
			i_ptr->pval = -1;
			i_ptr->name2 = EGO_SLOWNESS;
			break;
		    case 2:
			i_ptr->flags3 |= TR3_AGGRAVATE;
			i_ptr->name2 = EGO_NOISE;
			break;
		    case 3:
			i_ptr->weight = i_ptr->weight * 5;
			i_ptr->name2 = EGO_GREAT_MASS;
			break;
	    }
	}

	break;
	

      case TV_HELM: /* Helms */

	/* give crowns a higher chance for magic */
	if ((i_ptr->sval >= 6) && (i_ptr->subval <= 8)) {
	    chance += i_ptr->cost / 100;
	    special += special;
	}

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);

	    /* Apply more magic */
	    if (magik(special) || (good == 666)) {

		if (i_ptr->sval < 6) {

		    tmp = randint(14);

		    if (tmp < 3) {

		      if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags1 |= TR1_INT;
			i_ptr->pval = randint(2);	/* +N INT */
			i_ptr->cost += i_ptr->pval * 500;
			i_ptr->name2 = EGO_INTELLIGENCE;
			rating += 13;
			if (peek) msg_print("Intelligence");
			}
		    }

		      else if (tmp < 6) {
			if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags1 |= TR1_WIS;
			i_ptr->pval = randint(2);	/* +N Wis */
			i_ptr->cost += i_ptr->pval * 500;
			i_ptr->name2 = EGO_WISDOM;
			rating += 13;
			if (peek) msg_print("Wisdom");
			}
		    }

		      else if (tmp < 10) {
			if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags1 |= TR1_INFRA;
			i_ptr->pval = 1 + randint(4);	/* +N Infra */
			i_ptr->cost += i_ptr->pval * 250;
			i_ptr->name2 = EGO_INFRAVISION;
			rating += 11;
			}
		    }

		      else if (tmp < 12) {
			if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags2 |= (TR2_RES_LITE);
			i_ptr->flags3 |= (TR3_LITE);
			i_ptr->cost += 500;
			i_ptr->name2 = EGO_LITE;
			rating += 6;
			if (peek) msg_print("Light");
			}
		    }

		      else if (tmp < 14) {
			if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags2 |= TR2_RES_BLIND;
			i_ptr->flags3 |= TR3_SEE_INVIS;
			i_ptr->cost += 1000;
			i_ptr->name2 = EGO_SEEING;
			rating += 8;
			if (peek) msg_print("Helm of Seeing");
			}
		    }

		     else {
			if (!((randint(2) == 1) && !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags3 |= TR3_TELEPATHY;
			i_ptr->cost += 50000L;
			i_ptr->name2 = EGO_TELEPATHY;
			rating += 20;
			if (peek) msg_print("Telepathy");
			}
		    }
		}

		/* Process "crowns" */
		else {

		    /* Make it "excellent" */
		    switch (randint(6)) {

		      case 1:
			if (!(((randint(2) == 1) || (good == 666)) &&
			      !not_unique && unique_armour(i_ptr))) {
			i_ptr->flags1 |= (TR1_STR | TR1_DEX | TR1_CON);
			i_ptr->flags2 |= (TR2_FREE_ACT);
			i_ptr->pval = randint(3);	/* +N STR/DEX/CON */
			i_ptr->cost += 1000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_MIGHT;
			rating += 19;
			if (peek) msg_print("Crown of Might");
			}
			break;

		      case 2:
			i_ptr->flags1 |= (TR1_CHR | TR1_WIS);
			i_ptr->pval = randint(3);	/* +N WIS/CHR */
			i_ptr->cost += 1000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_LORDLINESS;
			rating += 17;
			if (peek) msg_print("Lordliness");
			break;

		      case 3:
			i_ptr->flags1 |= (TR1_INT);
			i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD |
					  TR2_RES_ACID | TR2_RES_FIRE);
			i_ptr->flags3 |= (TR3_IGNORE_ELEC | TR3_IGNORE_COLD |
					  TR3_IGNORE_ACID | TR3_IGNORE_FIRE);
			i_ptr->pval = randint(3);	/* +N INT */
			i_ptr->cost += 3000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_MAGI;
			rating += 15;
			if (peek) msg_print("Crown of the Magi");
			break;

		      case 4:
			i_ptr->flags1 |= TR1_CHR;
			i_ptr->pval = randint(4);	/* +N CHR */
			i_ptr->cost += 750;
			i_ptr->name2 = EGO_BEAUTY;
			rating += 8;
			if (peek) msg_print("Beauty");
			break;

		      case 5:
			i_ptr->flags1 |= (TR1_SEARCH);
			i_ptr->flags3 |= (TR3_SEE_INVIS);
			i_ptr->pval = 5 * (1 + randint(4));	/* +N Search */
			i_ptr->cost += 1000 + i_ptr->pval * 100;
			i_ptr->name2 = EGO_SEEING;
			rating += 8;
			if (peek) msg_print("Seeing");
			break;

		      case 6:
			i_ptr->flags3 |= TR3_REGEN;
			i_ptr->cost += 1500;
			i_ptr->name2 = EGO_REGENERATION;
			rating += 10;
			if (peek) msg_print("Regeneration");
			break;
		    }
		}
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac -= m_bonus(1, 20, level);
	    i_ptr->cost = 0;

	    if (magik(special)) {

		/* Choose some damage */
		switch (randint(7)) {
		  case 1:
		    i_ptr->flags1 |= TR1_INT;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    i_ptr->flags1 |= TR1_WIS;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    i_ptr->flags1 |= TR1_STR;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_WEAKNESS;
		    break;
		  case 6:
		    i_ptr->flags3 |= TR3_TELEPORT;
		    i_ptr->name2 = EGO_TELEPORTATION;
		    break;
		  case 7:
		    i_ptr->flags1 |= TR1_CHR;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_UGLINESS;
		    break;
		}
	    }
	}
	break;


      case TV_CLOAK:

	/* Apply some magic */
	if (good || magik(chance)) {

	    int                 made_art_cloak= 0;

	    /* Make it better */
	    i_ptr->toac += 1 + m_bonus(0, 20, level);

	    /* Apply more magic */
	    if (magik(special) || (good == 666)) {

		if (!not_unique &&
		    !stricmp(k_list[i_ptr->index].name, "& Cloak")
		    && randint(10) == 1) {

		    switch (randint(9)) {
		      case 1:
		      case 2:
			if (COLLUIN) break;
			if (wizard || peek) msg_print("Colluin");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_COLLUIN;
			i_ptr->toac = 15;
			i_ptr->flags2 |= (TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_POIS |
					 TR2_RES_ELEC | TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags3 |= (TR3_ACTIVATE);
			i_ptr->cost = 10000L;
			made_art_cloak = 1;
			COLLUIN = 1;
			break;

		      case 3:
		      case 4:
			if (HOLCOLLETH) break;
			if (wizard || peek) msg_print("Holcolleth");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_HOLCOLLETH;
			i_ptr->toac = 4;
			i_ptr->pval = 2;
			i_ptr->flags1 |= (TR1_INT | TR1_WIS | TR1_STEALTH );
			i_ptr->flags2 |= (TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags3 |= (TR3_ACTIVATE);
			i_ptr->ident |= TR3_HIDE_TYPE;
			i_ptr->cost = 13000L;
			made_art_cloak = 1;
			HOLCOLLETH = 1;
			break;

		      case 5:
			if (THINGOL) break;
			if (wizard || peek) msg_print("Thingol");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_THINGOL;
			i_ptr->toac = 18;
			i_ptr->flags1 = (TR1_DEX | TR1_CHR);
			i_ptr->flags2 = (TR2_RES_FIRE |TR2_RES_ACID | TR2_RES_COLD | TR2_FREE_ACT | TR_ARTIFACT);
			i_ptr->flags3 = (TR3_ACTIVATE);
			i_ptr->pval = 3;
			i_ptr->cost = 35000L;
			made_art_cloak = 1;
			THINGOL = 1;
			break;

		      case 6:
		      case 7:
			if (THORONGIL) break;
			if (wizard || peek) msg_print("Thorongil");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_THORONGIL;
			i_ptr->toac = 10;
			i_ptr->flags2 = ( TR2_FREE_ACT | TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags3 |= (TR3_SEE_INVIS);
			i_ptr->cost = 8000L;
			made_art_cloak = 1;
			THORONGIL = 1;
			break;

		      case 8:
		      case 9:
			if (COLANNON) break;
			if (wizard || peek) msg_print("Colannon");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_COLANNON;
			i_ptr->toac = 15;
			i_ptr->flags1 |= (TR1_STEALTH);
			i_ptr->flags1 |= (TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags2 |= (TR3_ACTIVATE);
			i_ptr->pval = 3;
			i_ptr->cost = 11000L;
			made_art_cloak = 1;
			COLANNON = 1;
			break;
		    }

		} else if (!not_unique &&
			   !stricmp(k_list[i_ptr->index].name,
				    "& Shadow Cloak")
			   && randint(20) == 1) {

		    switch (randint(2)) {
		      case 1:
			if (LUTHIEN) break;
			if (wizard || peek) msg_print("Luthien");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_LUTHIEN;
			i_ptr->toac = 20;
			i_ptr->flags1 = (TR1_INT | TR1_WIS | TR1_CHR);
			i_ptr->flags2 = (TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags3 = (TR3_ACTIVATE);
			i_ptr->pval = 2;
			i_ptr->cost = 45000L;
			made_art_cloak = 1;
			LUTHIEN = 1;
			break;

		      case 2:
			if (TUOR) break;
			if (wizard || peek) msg_print("Tuor");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_TUOR;
			i_ptr->toac = 12;
			i_ptr->flags1 = (TR1_STEALTH);
			i_ptr->flags2 |= (TR2_FREE_ACT | TR2_IM_ACID | TR2_RES_ACID | TR_ARTIFACT);
			i_ptr->flags2 |= (TR3_SEE_INVIS);
			i_ptr->pval = 4;
			i_ptr->cost = 35000L;
			made_art_cloak = 1;
			TUOR = 1;
			break;
		    }
		}
		if (!made_art_cloak) {
		    if (randint(2) == 1) {
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
			i_ptr->toac += m_bonus(0, 10, level) + (5 + randint(3));
			i_ptr->cost += 250L;
			i_ptr->name2 = EGO_PROTECTION;
			rating += 8;
		    } else if (randint(10) < 10) {
			i_ptr->toac += m_bonus(3, 10, level);
			i_ptr->pval = randint(3);
			i_ptr->flags1 |= TR1_STEALTH;
			i_ptr->name2 = EGO_STEALTH;
			i_ptr->cost += 500 + (50 * i_ptr->pval);
			rating += 9;
		    } else {
			i_ptr->toac += 10 + randint(10);
			i_ptr->pval = randint(3);
			i_ptr->flags1 |= (TR1_STEALTH);
			i_ptr->flags2 |= (TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
			i_ptr->name2 = EGO_AMAN;
			i_ptr->cost += 4000 + (100 * i_ptr->toac);
			rating += 16;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		i_ptr->flags3 |= TR3_AGGRAVATE;
		i_ptr->name2 = EGO_IRRITATION;
		i_ptr->toac -= m_bonus(1, 10, level);
		i_ptr->flags3 |= TR3_SHOW_MODS;
		i_ptr->tohit -= m_bonus(1, 10, level);
		i_ptr->todam -= m_bonus(1, 10, level);
		i_ptr->cost = 0;
	    } else if (tmp == 2) {
		i_ptr->name2 = EGO_VULNERABILITY;
		i_ptr->toac -= m_bonus(10, 20, level + 50);
		i_ptr->cost = 0;
	    } else {
		i_ptr->name2 = EGO_ENVELOPING;
		i_ptr->toac -= m_bonus(1, 10, level);
		i_ptr->flags3 |= TR3_SHOW_MODS;
		i_ptr->tohit -= m_bonus(2, 15, level + 10);
		i_ptr->todam -= m_bonus(2, 15, level + 10);
		i_ptr->cost = 0;
	    }
	    i_ptr->flags3 |= TR3_CURSED;
	}
	break;


      case TV_DIGGING:

    /* always show tohit/todam values if identified */
	i_ptr->ident |= TR3_SHOW_MODS;

	/* Apply some magic */
	if (magik(chance) || (good == 666)) {

	    tmp = randint(3);

	    /* Add in a digging bonus */
	    if (tmp == 1) i_ptr->pval += m_bonus(0, 5, level);
	    if (tmp == 2); /* do not give additional plusses -CWS */

	    else {
	/* a cursed digging tool */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->pval = (-m_bonus(1, 15, level));
	    i_ptr->cost = 0L;
	    }
	}
	
	break;


      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:

    /* always show tohit/todam values if identified */
	i_ptr->flags3 |= TR3_SHOW_MODS;

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->tohit += randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam += randint(3) + m_bonus(0, 10, level);
	/*
	 * the 3*special/2 is needed because weapons are not as common as
	 * before change to treasure distribution, this helps keep same
	 * number of ego weapons same as before, see also missiles 
	 */
	    if (magik(3*special/2)||good==666) { /* was 2 */
		if (!stricmp("& Whip", k_list[i_ptr->index].name)
		    && randint(2)==1) {

		    i_ptr->flags1 |= (TR1_BRAND_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE);

		    /* Better stats */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;
		    /* this should allow some WICKED whips -CFT */
		    while (randint(5 * (int)i_ptr->dd) == 1) {
			i_ptr->dd++;
			i_ptr->cost += 2500;
			i_ptr->cost *= 2;
		    }

		    i_ptr->name2 = EGO_FIRE;

		    rating += 20;
		    if (peek) msg_print("Whip of Fire");

		} else {

		/* Make it "excellent" */
		switch (randint(30)) {	/* was 16 */

		  case 1: /* Holy Avenger */
		    if (((randint(2) == 1) || (good == 666))
		        && !not_unique && unique_weapon(i_ptr)) break;
		    i_ptr->flags1 |= (TR1_SLAY_DEMON | TR1_WIS |
				      TR1_SLAY_UNDEAD | TR1_SLAY_EVIL);
		    i_ptr->flags3 |= (TR3_BLESSED | TR3_SEE_INVIS);
		    i_ptr->pval = randint(4);  /* Wisdom bonus */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;
		    i_ptr->toac += randint(4);

		    /* Hack --the value in pval is used for stat increase */
		    /* pval is also used for sustain stat */
		    i_ptr->flags2 |= TR_SUST_STAT

		    i_ptr->cost += i_ptr->pval * 500;
		    i_ptr->cost += 10000L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_HA;
		    rating += 30;
		    if (peek) msg_print("Holy Avenger");
		    break;

		  case 2: /* Defender */
		    if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr)) break;
		    i_ptr->flags1 |= (TR1_STEALTH);
		    i_ptr->flags2 |= (TR2_FREE_ACT |
				      TR2_RES_FIRE | TR2_RES_COLD |
				      TR2_RES_ELEC | TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_FEATHER | TR3_REGEN | TR3_SEE_INVIS |
				      TR3_IGNORE_FIRE | TR3_IGNORE_COLD |
				      TR3_IGNORE_ELEC | TR3_IGNORE_ACID);
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->toac += 5 + randint(5);
		    /* the value in pval is used for stealth */
		    i_ptr->pval = randint(3);	/* +X Stealth */
		    i_ptr->cost += i_ptr->pval * 500;
		    i_ptr->cost += 7500L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_DF;
		    rating += 23;
		    if (peek) msg_print("Defender");
		    break;

		  case 3: case 4: /* Flame Tongue */
		    if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr)) break;
		    i_ptr->flags1 |= (TR1_BRAND_FIRE);
		    i_ptr->flags2 |= (TR2_RES_FIRE);
		    i_ptr->flags2 |= (TR3_IGNORE_FIRE);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 3;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_FT;
		    rating += 20;
		    if (peek) msg_print("Flame");
		    break;

		  case 5: case 6: /* Frost Brand */
		    if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr)) break;
		    i_ptr->flags1 |= (TR1_BRAND_COLD);
		    i_ptr->flags2 |= (TR2_RES_COLD);
		    i_ptr->flags3 |= (TR3_IGNORE_COLD);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 2200L;
		    i_ptr->name2 = EGO_FB;
		    rating += 20;
		    if (peek) msg_print("Frost");
		    break;

		  case 7: case 8:
		    i_ptr->flags1 |= TR1_SLAY_ANIMAL;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 2000L;
		    i_ptr->name2 = EGO_SLAY_A;
		    rating += 15;
		    if (peek) msg_print("Slay Animal");
		    break;

		  case 9: case 10:
		    i_ptr->flags1 |= TR1_SLAY_DRAGON;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 4000L;
		    i_ptr->name2 = EGO_SLAY_D;
		    rating += 18;
		    if (peek) msg_print("Slay Dragon");
		    break;

		  case 11: case 12:
		    i_ptr->flags1 |= TR1_SLAY_EVIL;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 4000L;
		    i_ptr->name2 = EGO_SLAY_E;

		    /* One in three is also a blessed wisdom booster */
		    if (randint(3) == 1) {
			i_ptr->flags1 |= (TR1_WIS);
			i_ptr->flags3 |= (TR3_BLESSED);
			i_ptr->pval = m_bonus(0, 3, level);
			i_ptr->cost += (200L * i_ptr->pval);
		    }

		    rating += 18;
		    if (peek) msg_print("Slay Evil");

		    break;

		  case 13: case 14:
		    i_ptr->flags1 |= (TR1_SLAY_UNDEAD);
		    i_ptr->flags3 |= (TR3_SEE_INVIS);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_SLAY_U;

		    /* One in three is also a Life Holder */
		    if (randint(3) == 1) {
			i_ptr->flags2 |= (TR2_HOLD_LIFE);
			i_ptr->cost += 1000L;
		    }

		    rating += 18;
		    if (peek) msg_print("Slay Undead");

		    break;

		  case 15: case 16: case 17:
		    i_ptr->flags1 |= TR1_SLAY_ORC;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_O;
		    rating += 13;
		    if (peek) msg_print("Slay Orc");
		    break;

		  case 18: case 19: case 20:
		    i_ptr->flags1 |= TR1_SLAY_TROLL;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_T;
		    rating += 13;
		    if (peek) msg_print("Slay Troll");
		    break;

		  case 21: case 22: case 23:
		    i_ptr->flags1 |= TR1_SLAY_GIANT;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_G;
		    rating += 14;
		    if (peek) msg_print("Slay Giant");
		    break;

		  case 24: case 25: case 26:
		    i_ptr->flags1 |= TR1_SLAY_DEMON;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_DEMON;
		    rating += 16;
		    if (peek) msg_print("Slay Demon");
		    break;

		  case 27: /* of Westernesse */
		if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(i_ptr)) break;
		    i_ptr->flags1 |= (TR1_SLAY_ORC |
				      TR1_DEX | TR1_CON | TR1_STR);
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->flags3 |= (TR3_SEE_INVIS);
		    i_ptr->tohit += 3 + randint(5);
		    i_ptr->todam += 3 + randint(5);
		    i_ptr->pval = 1;
		    i_ptr->cost += 10000L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_WEST;
		    rating += 20;
		    if (peek) msg_print("Westernesse");
		    break;

		  /* Blessed Blade -DGK *
		  case 28: case 29:
			if ((i_ptr->tval != TV_SWORD) &&
			    (i_ptr->tval != TV_POLEARM))
			    break;
		    i_ptr->flags3 |= TR3_BLESSED;
		    i_ptr->flags1 |= TR1_WIS;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->pval = randint(3);
		    i_ptr->cost += i_ptr->pval * 1000;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_BLESS_BLADE;
		    rating += 20;
		    if (peek) msg_print("Blessed");
		    break;

		  /* of Speed -DGK */
		  case 30:
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique && unique_weapon(i_ptr))
			    break;
		    i_ptr->tohit += randint(5);
		    i_ptr->todam += randint(3);
		    i_ptr->flags1 |= (TR1_ATTACK_SPD);
		    if (i_ptr->weight <= 80) {
			i_ptr->pval = randint(3);
		    }
		    else if (i_ptr->weight <= 130) {
			i_ptr->pval = randint(2);
		    }
		    else {
			i_ptr->pval = 1;
		    }
		    i_ptr->cost += (i_ptr->pval * 2000);
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_ATTACKS;
		    rating += 20;
		    if (wizard || peek) msg_print("Weapon of Extra Attacks");
		    break;
		    }
		}
	    }
	}

	/* Cursed Weapons */
	else if (magik(cursed)) {
	
	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->tohit = 0 - randint(3) - m_bonus(1, 20, level);
	    i_ptr->todam = 0 - randint(3) - m_bonus(1, 20, level);

	    /* Permanently cursed Weapon of Morgul */
	    if ((level > (20 + randint(15))) && (randint(10) == 1)) {
		i_ptr->flags3 |= (R3_AGGRAVATE | TR3_SEE_INVIS);
		i_ptr->tohit -= 15;
		i_ptr->todam -= 15;
		i_ptr->toac = -10;
		i_ptr->weight += 100;
		i_ptr->name2 = EGO_MORGUL;
	    }
	    i_ptr->cost = 0L;
	}
	break;


      case TV_BOW:

    /* always show tohit/todam values if identified */
	i_ptr->flags3 |= TR3_SHOW_MODS;

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->tohit = randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam = randint(3) + m_bonus(0, 10, level);

	    switch (randint(15)) {

	      case 1: case 2: case 3:
		if (((randint(3)==1)||(good==666)) && !not_unique &&
		    !stricmp(k_list[i_ptr->index].name, "& Long Bow") &&
		    (((i=randint(2))==1 && !BELEG) || (i==2 && !BARD))) {
		    switch (i) {
		    case 1:
			  if (BELEG) break;
			if (wizard || peek) msg_print("Belthronding");
			else good_item_flag = TRUE;
			i_ptr->name2 = SN_BELEG;
			i_ptr->flags3 |= TR3_HIDE_TYPE;
			i_ptr->sval = 4; /* make do x5 damage!! -CFT */
			i_ptr->tohit = 20;
			i_ptr->todam = 22;
			i_ptr->pval = 3;
			i_ptr->flags1 |= (TR1_STEALTH | TR1_DEX);
			i_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_DISEN);
			i_ptr->cost = 35000L;
			BELEG = 1;
			break;

		      case 2:
			if (BARD) break;
			if (wizard || peek) msg_print("Bard");
			else good_item_flag = TRUE;
			i_ptr->name2 = ART_BARD;
			i_ptr->sval = 3; /* make do x4 damage!! -CFT */
			i_ptr->tohit = 17;
			i_ptr->todam = 19;
			i_ptr->pval = 3;
			i_ptr->flags1 |= (TR1_DEX);
			i_ptr->flags2 |= (TR2_FREE_ACT) | TR_ARTIFACT);
			i_ptr->cost = 20000L;
			BARD = 1;
			break;
		    }
		    break;
		}
		if (((randint(5) == 1) || (good == 666)) && !not_unique &&
		    !stricmp(k_list[i_ptr->index].name, "& Light Crossbow")
		    && !CUBRAGOL) {
		    if (CUBRAGOL) break;
		    if (wizard || peek) msg_print("Cubragol");
		    i_ptr->name2 = ART_CUBRAGOL;
		    i_ptr->sval = 11;
		    i_ptr->tohit = 10;
		    i_ptr->todam = 14;
		    i_ptr->pval = 1;
		    i_ptr->flags1 |= (TR1_SPEED);
		    i_ptr->flags2 |= (TR2_RES_FIRE | TR_ARTIFACT);
		    i_ptr->flags3 |= (TR3_ACTIVATE);
		    i_ptr->cost = 38000L;
		    CUBRAGOL = 1;
		    break;
		}
		    i_ptr->sval++; /* make it do an extra multiple of damage */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 10;
		    i_ptr->name2 = EGO_MIGHT;
		    rating += 15;
		    if (peek) msg_print("Bow of Might");
		    break;

		  case 4: case 5: case 6: case 7: case 8:
		    i_ptr->tohit += 5;
		    i_ptr->todam += 12;
		    i_ptr->name2 = EGO_MIGHT;
		    rating += 11;
		    if (peek) msg_print("Bow of Might");
		    break;

		  case 9: case 10: case 11: case 12:
		  case 13: case 14: case 15:
		    i_ptr->tohit += 12;
		    i_ptr->todam += 5;
		    i_ptr->name2 = EGO_ACCURACY;
		    rating += 11;
		    if (peek) msg_print("Accuracy");
		    break;
	    }
	}

	/* Cursed bows */
	else if (magik(cursed)) {
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->tohit = 0 - m_bonus(5, 30, level);
	    i_ptr->todam = 0 - m_bonus(5, 20, level);	/* add damage. -CJS- */
	    i_ptr->cost = 0L;
	}
	
	break;


      case TV_BOLT:
      case TV_ARROW:
      case TV_SHOT:

     /* this fn makes ammo for player's missile weapon more common -CFT */
      magic_ammo(i_ptr, good, chance, special, cursed, level);
      break;



      case TV_RING: /* Rings */

	if (!((randint(10) == 1) && !not_unique && unique_armour(i_ptr))) {

	switch (i_ptr->sval) {

	  case 0:
	  case 1:
	  case 2:
	  case 3: /* 132-135 */
	    i_ptr->pval = m_bonus(1, 6, level);
	    i_ptr->cost += i_ptr->pval * 100;
	    if (magik(cursed)) {
		i_ptr->pval = -m_bonus(1, 10, level);
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  case 4: /* 136 */

	    /* Cursed Ring */
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -randint(3);
		i_ptr->cost = -i_ptr->cost;
		break;
	    }

	    rating += 35;
	    if (randint(888) == 1) i_ptr->pval = 2;
	    else i_ptr->pval = 1;

	    if (peek) msg_print("Ring of Speed");
	    break;

	  case 5:
	    i_ptr->pval = 5 * m_bonus(1, 10, level);
	    i_ptr->cost += i_ptr->pval * 30;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -i_ptr->pval;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Flames, Acid, Ice */
	  case 14:
	  case 15:
	  case 16:
	    i_ptr->toac = m_bonus(1, 10, level);
	    i_ptr->toac += 5 + randint(7);
	    i_ptr->cost += i_ptr->toac * 100;
	    break;

	  /* WOE, Stupidity */
	  case 17:
	  case 18:
	    i_ptr->toac = (-5) - m_bonus(1,10,level);
	    i_ptr->pval = (-randint(4));
	    break;

	  /* Increase damage */
	  case 19:
	    i_ptr->todam = m_bonus(1, 10, level);
	    i_ptr->todam += 3 + randint(10);
	    i_ptr->cost += i_ptr->todam * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->todam = 0 - i_ptr->todam;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Increase To-Hit */
	  case 20:
	    i_ptr->tohit = m_bonus(1, 10, level);
	    i_ptr->tohit += 3 + randint(10);
	    i_ptr->cost += i_ptr->tohit * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->tohit = 0 - i_ptr->tohit;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Protection */
	  case 21:
	    i_ptr->toac = m_bonus(0, 10, level);
	    i_ptr->toac += 4 + randint(5);
	    i_ptr->cost += i_ptr->toac * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->toac = 0 - i_ptr->toac;
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

	  /* Slaying */
	  case 30:
	    i_ptr->ident |= TR3_SHOW_MODS;
	    i_ptr->todam = m_bonus(1, 10, level);
	    i_ptr->todam += 2 + randint(3);
	    i_ptr->tohit = m_bonus(1, 10, level);
	    i_ptr->tohit += 2 + randint(3);
	    i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->tohit = 0 - i_ptr->tohit;
		i_ptr->todam = 0 - i_ptr->todam;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  default: break;
	    }
	}
	break;

      case TV_AMULET: /* Amulets */

	if (i_ptr->sval < 2) {
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -m_bonus(1, 5, level);
		i_ptr->cost = -i_ptr->cost;
	    } else {
		i_ptr->pval = m_bonus(1, 5, level);
		i_ptr->cost += i_ptr->pval * 100;
	    }
	}
	else if (i_ptr->sval == 2) { /* searching */
	    i_ptr->pval = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -i_ptr->pval;
		i_ptr->cost = -i_ptr->cost;
	    } else
		i_ptr->cost += 20 * i_ptr->pval;
	}
	else if (i_ptr->sval == 8) {
	    rating += 25;
	    i_ptr->pval = 5 * (randint(2) + m_bonus(0, 10, level));
	    i_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    i_ptr->cost += 20 * i_ptr->pval + 50 * i_ptr->toac;
	    if (i_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		i_ptr->flags |= TR3_CURSED;
	}
	else if (i_ptr->sval == 9) { /* amulet of DOOM */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->pval = 0 - randint(5) - m_bonus(2, 10, level);
	    i_ptr->toac = 0 - randint(3) - m_bonus(0, 6, level);
	}
	break;


      case TV_WAND:
	charge_wand(i_ptr);
	break;


      case TV_STAFF:
	charge_staff(i_ptr);
	break;


    /* Subval should be even for store, odd for dungeon */
    /* Dungeon found ones will be partially charged	 */
      case TV_LITE:

	if ((i_ptr->sval % 2) == 1) {
	    i_ptr->pval = randint(i_ptr->pval);
	    i_ptr->sval -= 1;
	}

	break;


      case TV_CHEST:

	/* Chests have "goodness" based on the level */
	/* Pick a trap (or set of traps) for the chest */
	/* Note that the traps give an idea of the "value" */

	switch (randint(level + 4)) {
	  case 1:
	    i_ptr->flags1 = 0L;
	    i_ptr->flags2 = 0L;
	    break;
	  case 2:
	    i_ptr->flags2 |= CH2_LOCKED;
	    break;
	  case 3:
	  case 4:
	    i_ptr->flags2 |= (CH2_LOSE_STR | CH2_LOCKED);
	    break;
	  case 5:
	  case 6:
	    i_ptr->flags2 |= (CH2_POISON | CH2_LOCKED);
	    break;
	  case 7:
	  case 8:
	  case 9:
	    i_ptr->flags2 |= (CH2_PARALYSED | CH2_LOCKED);
	    break;
	  case 10:
	  case 11:
	    i_ptr->flags2 |= (CH2_EXPLODE | CH2_LOCKED);
	    break;
	  case 12:
	  case 13:
	  case 14:
	    i_ptr->flags2 |= (CH2_SUMMON | CH2_LOCKED);
	    break;
	  case 15:
	  case 16:
	  case 17:
	    i_ptr->flags2 |= (CH2_PARALYSED | CH2_POISON | CH2_LOSE_STR | CH2_LOCKED);
	    break;
	  default:
	    i_ptr->flags2 |= (CH2_SUMMON | CH2_EXPLODE | CH2_LOCKED);
	    break;
	}
	if (not_unique)		/* if bought from store - dbd */
	    i_ptr->pval = randint(i_ptr->level);
	else			/* store the level chest's found on - dbd */
	    i_ptr->pval = dun_level;
	break;

      case TV_SPIKE:
	i_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    i_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	i_ptr->pval = missile_ctr;
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
static int special_place_object(int y, int x)
{
    register int	cur_pos, tmp;
    char         str[100];
    int          done = 0;

    /* Is this cave grid sacred? */
    if (!in_bounds(y,x)) return (FALSE); /* abort! -CFT */

    if (cave[y][x].i_idx != 0)
	if ((i_list[cave[y][x].i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT)))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(70) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(100) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(30) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(35) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(30) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(65) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
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
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
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
    cave[y][x].i_idx = cur_pos;
    invcopy(&i_list[cur_pos], tmp);
    i_list[cur_pos].timeout = 0;
    i_list[cur_pos].flags3 |= TR3_HIDE_TYPE; /* don't show (+x of yyy) for these */
    if (k_list[tmp].level > object_level) {
	rating += 2 * (k_list[sorted_objects[tmp]].level - object_level);
    }

    /* Is it on the player? */
    if (cave[y][x].cptr == 1) {
	msg_print("You feel something roll beneath your feet.");
    }

    /* Success */
    return (-1);
}


/*
 * Attempts to places a random object at the given location -RAK-
 */
void place_object(int y, int x)
{
    register int cur_pos, tmp;

    if (!in_bounds(y,x)) return; /* abort! -CFT */
    if (cave[y][x].i_idx != 0)
	if ((i_list[cave[y][x].i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT)))
	    return; /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y,x);
    
    if (randint(MAX_OBJECTS)>SPECIAL_OBJ && randint(10)==1)
	if (special_place_object(y,x)==(-1))
	    return;

    /* Make it */
    cur_pos = i_pop();
    cave[y][x].i_idx = cur_pos;

    do {	   /* don't generate another chest if opening_chest is true -CWS */
	tmp = get_obj_num(dun_level, FALSE);
    } while (opening_chest && (k_list[sorted_objects[tmp]].tval == TV_CHEST));
	
    invcopy(&i_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, dun_level, FALSE, 0);
    if (k_list[sorted_objects[tmp]].level > dun_level)
	rating += k_list[sorted_objects[tmp]].level - dun_level;

    if (peek) {
	if (k_list[sorted_objects[tmp]].level > dun_level) {
	    char buf[200];
	    byte temp;
	    
	    temp=i_list[cur_pos].ident;
	    i_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &i_list[cur_pos], TRUE);
	    i_list[cur_pos].ident = temp;
	    msg_print(buf);
	}
    }

    /* Under the player */
    if (cave[y][x].cptr == 1) {
	msg_print ("You feel something roll beneath your feet.");
    }
}


/*
 * Places a "GOOD" object at given row, column co-ordinate ~Ludwig 
 */
void place_good(int y, int x, u32b good)
{
    register int cur_pos, k_idx;
    int          tv, is_good = FALSE;
    cave_type *c_ptr;

    if (!in_bounds(y, x)) return; /* abort! -CFT */

    /* Get the grid */
    c_ptr = &cave[y][x];

    if (c_ptr->i_idx != 0)
	if ((i_list[c_ptr->i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[c_ptr->i_idx].tval == TV_UP_STAIR) ||
	    (i_list[c_ptr->i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[c_ptr->i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[c_ptr->i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);

    /* Hack -- much higher chance of doing "Special Objects" */
    if (randint(10) == 1) {
	if (special_place_object(y, x)) return;
    }

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;

    /* Pick a good "base object" */
    do {

	/* Pick a random object, based on "object_level" */
	k_idx = get_obj_num((object_level + 10), TRUE);

	/* Examine the object */
	tv = k_list[sorted_objects[k_idx]].tval;

	/* Normal weapons/armour are okay (except "shots" or "shovels") */
	if ((tv == TV_HELM) || (tv == TV_SHIELD) || (tv == TV_CLOAK) ||
	    (tv == TV_HAFTED) || (tv == TV_POLEARM) ||
	    (tv == TV_BOW) || (tv == TV_BOLT) || (tv == TV_ARROW) ||
	    (tv == TV_BOOTS) || (tv == TV_GLOVES))
	    is_good = TRUE;

	if ((tv == TV_SWORD) &&
	    strncmp("& Broken", k_list[sorted_objects[k_idx]].name, 8))
	    is_good = TRUE;	   /* broken swords/daggers are NOT good!
				    * -CFT */
	if ((tv == TV_HARD_ARMOR) &&
	    strncmp("Rusty", k_list[sorted_objects[k_idx]].name, 5))
	    is_good = TRUE;	   /* rusty chainmail is NOT good! -CFT */
	if ((tv == TV_SOFT_ARMOR) &&
	 stricmp("some filthy rags", k_list[sorted_objects[k_idx]].name))
	    is_good = TRUE;	   /* nor are rags! -CFT */
	if ((tv == TV_MAGIC_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Raal's */
	    (k_list[sorted_objects[k_idx]].sval > ((good & MF2_SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
	if ((tv == TV_PRAYER_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Wrath of
					 * God */
	    (k_list[sorted_objects[k_idx]].sval > ((good & MF2_SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
    } while (!is_good);

    invcopy(&i_list[cur_pos], sorted_objects[k_idx]);
    magic_treasure(cur_pos, object_level, (good & MF2_SPECIAL) ? 666 : 1, 0);

	/* Hack -- look at it */
	if (peek) {
	    char buf[200];
	    if (k_list[sorted_objects[k_idx]].level > object_level) {
	    byte               t;

	    t = i_list[cur_pos].ident;
	    i_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &i_list[cur_pos], TRUE);
	    i_list[cur_pos].ident = t;
	    msg_print(buf);
	}
    }

    if (c_ptr->m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}




/*
 * Creates objects nearby the coordinates given -RAK-	 
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

	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->i_idx == 0)) {
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
    /* Place them */
    for (; num > 0; --num) {

	/* Try up to 10 spots looking for empty space */
	for (i = 0; i < 11; ++i) {
	    
	    /* Pick a random spot */
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = &cave[j][k];

	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->i_idx == 0)) {

	    /* Perhaps attempt to place a "Special Object" */
	    if (randint(5) == 1) {
		if (special_place_object(j, k)) break;
	    }

	    /* Place a "great" object */
	    place_good(j, k, MF2_SPECIAL);

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

    if (!in_bounds(y, x)) return; /* abort! -CFT */

    /* don't put rubble under monsters, it's annoying -CFT */
    if (cave[y][x].cptr >= MIN_M_IDX) return;

    if (cave[y][x].i_idx != 0)
    /* don't replace stairs, stores, artifacts */
	if ((i_list[cave[y][x].i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT)))
	    return;
	else
	    delete_object(y, x);

    /* Make a new object */
    cur_pos = i_pop();
    cave[y][x].i_idx = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_TRAP_LIST + sval);
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

    if (cave[y][x].i_idx != 0)

   /* don't replace stairs, stores, artifacts */
	if ((i_list[cave[y][x].i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT)))
	    return;
	else
    /* Delete whatever is there */
    delete_object(y, x);

    cur_pos = i_pop();
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_RUBBLE);

    /* Put the rubble in the cave */
    c_ptr = &cave[y][x];
    c_ptr->i_idx = cur_pos;

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
 * Places a treasure (Gold or Gems) at given row, column -RAK-	
 */
void place_gold(int y, int x)
{
    register int        i, cur_pos;
    register inven_type *i_ptr;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].i_idx != 0)
	if ((i_list[cave[y][x].i_idx].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].i_idx].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].i_idx].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].i_idx].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT)))
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
    cave[y][x].i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_GOLD_LIST + i);

    /* Determine the "cost" */
    i_ptr->cost += (8L * (long)randint((int)i_ptr->cost)) + randint(8);

    /* Hack -- average the values to make sure "creeping coins" are not too valuable */
    if (coin_type) {
	i_ptr->cost = ((8L * (long)randint((int)k_list[OBJ_GOLD_LIST + i].cost))
		       + (i_ptr->cost)) >> 1;
    }

    /* Under the player */
    if (cave[y][x].cptr == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}


/*
 * Returns the array number of a random object -RAK-
 */
int get_obj_num(int level, int good)
{
    register int i, j;

    do {

	/* Town level is easy */
	if (level == 0) {

	    /* Pick a level 0 entry */
	    i = rand_int(t_level[0]);
	}

	/* Other levels sometimes have great stuff */
	else {

	    /* Never exceed a given level */
	    if (level >= MAX_K_LEV) level = MAX_K_LEV;

	    /* Occasionally, get a "better" object */
	    else if (randint(GREAT_OBJ) == 1) {

		/* What a bizarre calculation */
		level = 1 + (level * MAX_K_LEV / randint(MAX_K_LEV));
		if (level > MAX_K_LEV) level = MAX_K_LEV;
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
		j = k_list[sorted_objects[i]].level;
		if (j == 0) i = rand_int(t_level[0]);
		else i = randint(t_level[j] - t_level[j - 1]) - 1 + t_level[j - 1];
	    }
	}
    } while (((k_list[sorted_objects[i]].rare ?
	       (randint(k_list[sorted_objects[i]].rare) - 1) : 0) && !good)
	     || (k_list[sorted_objects[i]].rare == 255));

    /* Accept that object */
    return (i);
}



