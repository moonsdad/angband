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

    if (test_light(y, x)) delete = TRUE;
    else delete = FALSE;

    return (delete);
}




/*
 * If too many objects on floor level, delete some of them
 */
static void compact_objects()
{
    register int        i, j;
    int                 ctr, cur_dis, chance;
    register cave_type *cave_ptr;


    /* Debugging message */
    msg_print("Compacting objects...");

    ctr = 0;
    cur_dis = 66;
    do {

	/* Examine the dungeon */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++) {

		/* Get the location */
		cave_ptr = &cave[i][j];

		if ((cave_ptr->tptr != 0)
		    && (distance(i, j, char_row, char_col) > cur_dis)) {

		/* Every object gets a "saving throw" */
		    switch (t_list[cave_ptr->tptr].tval) {
		      case TV_VIS_TRAP:
			chance = 15;
			break;
		      case TV_INVIS_TRAP:
		      case TV_RUBBLE:
		      case TV_OPEN_DOOR:
		      case TV_CLOSED_DOOR:
			chance = 5;
			break;
		      case TV_UP_STAIR:
		      case TV_DOWN_STAIR:
		      case TV_STORE_DOOR:
			chance = 0;
			break;
		      case TV_SECRET_DOOR:	/* secret doors */
			chance = 3;
			break;
		      default:
			if ((t_list[cave_ptr->tptr].tval >= TV_MIN_WEAR) &&
			    (t_list[cave_ptr->tptr].tval <= TV_MAX_WEAR) &&
			    (t_list[cave_ptr->tptr].flags2 & TR_ARTIFACT))
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
 * Gives pointer to next free space			-RAK-
 */
int popt(void)
{
    /* Compact if needed */
    if (tcptr == MAX_TALLOC) compact_objects();

    /* Return the next free space */
    return (tcptr++);
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

    if (x != tcptr - 1) {
	t_list[x] = t_list[tcptr - 1];

    /* must change the tptr in the cave of the object just moved */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++)
		if (cave[i][j].tptr == tcptr - 1)
		    cave[i][j].tptr = x;
    }
    tcptr--;
    invcopy(&t_list[tcptr], OBJ_NOTHING);
}


/*
 * Boolean : is object enchanted	  -RAK- 
 */
int magik(int chance)
{
    if (randint(100) <= chance) return (TRUE);

    return (FALSE);
}


/*
 * Enchant a bonus based on degree desired -RAK-
 *
 * Lets just change this to make sense.  Now it goes from base to limit,
 * roughly proportional to the level.... -CWS
 */
int m_bonus(int base, int limit, int level)
{
    register int x, stand_dev, tmp, diff = limit - base;

    /* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100)) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);

    /* Extract a weird value */
    x = (tmp * diff / 150) + (level * limit / 200) + base;

    /* Enforce minimum value */
    if (x < base) return (base);

    /* Return the extracted value */
    return (x);
}




int unique_weapon(inven_type *t_ptr)
{
    cptr name;

    if (be_nasty)
	return 0;
    name = object_list[t_ptr->index].name;
    if (!stricmp("& Longsword", name)) {
	switch (randint(15)) {
	  case 1:
	    if (RINGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Ringil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_RINGIL;
	    t_ptr->tohit = 22;
	    t_ptr->todam = 25;
	    t_ptr->damage[0] = 4;
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_UNDEAD | TR_SLAY_EVIL | TR_REGEN |
		     TR_SPEED | TR_RES_COLD | TR_FROST_BRAND | TR_FREE_ACT |
			    TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_LIGHT | TR_ACTIVATE
			      | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->p1 = 1;
	    t_ptr->cost = 300000L;
	    RINGIL = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	    if (ANDURIL)
		return 0;
	    if (wizard || peek)
		msg_print("Anduril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ANDURIL;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_EVIL | TR_FREE_ACT |
		     TR_SUST_STAT | TR_STR | TR_RES_FIRE | TR_FLAME_TONGUE);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_ACTIVATE | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->toac = 5;
	    t_ptr->cost = 80000L;
	    ANDURIL = 1;
	    return 1;
	  case 5:
	  case 6:
	  case 7:
	  case 8:
	    if (ANGUIREL)
		return 0;
	    if (wizard || peek)
		msg_print("Anguirel");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ANGUIREL;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 12;
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_EVIL | TR_FREE_ACT | TR_RES_LIGHT
			    | TR_STR | TR_CON);
	    t_ptr->flags2 |= (TR_ARTIFACT |
		       TR_LIGHTNING | TR_LIGHT | TR_SLAY_DEMON | TR_RES_LT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 40000L;
	    ANGUIREL = 1;
	    return 1;
	  default:
	    if (ELVAGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Elvagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ELVAGIL;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 2;
	    t_ptr->todam = 7;
	    t_ptr->flags |= (TR_SEE_INVIS | TR_CHR | TR_DEX | TR_STEALTH | TR_FFALL);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 30000L;
	    ELVAGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Two-Handed Sword", name)) {
	switch (randint(8)) {
	  case 1:
	  case 2:
	    if (GURTHANG)
		return 0;
	    if (wizard || peek)
		msg_print("Gurthang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_GURTHANG;
	    t_ptr->tohit = 13;
	    t_ptr->todam = 17;
	    t_ptr->flags = (TR_REGEN | TR_SLAY_X_DRAGON | TR_STR |
			    TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 100000L;
	    GURTHANG = 1;
	    return 1;
	  case 3:
	    if (ZARCUTHRA)
		return 0;
	    if (randint(3) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Zarcuthra");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ZARCUTHRA;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 19;
	    t_ptr->todam = 21;
	    t_ptr->flags = (TR_SLAY_X_DRAGON | TR_STR | TR_SLAY_EVIL | TR_SLAY_ANIMAL |
		  TR_SLAY_UNDEAD | TR_AGGRAVATE | TR_CHR | TR_FLAME_TONGUE |
		  TR_SEE_INVIS | TR_RES_FIRE | TR_FREE_ACT | TR_INFRA);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC | TR_SLAY_GIANT
			      | TR_SLAY_DEMON | TR_RES_CHAOS);
	    t_ptr->p1 = 4;
	    t_ptr->damage[0] = 6;
	    t_ptr->damage[1] = 4;
	    t_ptr->cost = 200000L;
	    ZARCUTHRA = 1;
	    return 1;
	  default:
	    if (MORMEGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Mormegil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_MORMEGIL;
	    t_ptr->tohit = -40;
	    t_ptr->todam = -60;
	    t_ptr->flags = (TR_SPEED | TR_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = -1;
	    t_ptr->toac = -50;
	    t_ptr->cost = 10000L;
	    MORMEGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Broadsword", name)) {
	switch (randint(12)) {
	  case 1:
	  case 2:
	    if (ARUNRUTH)
		return 0;
	    if (wizard || peek)
		msg_print("Arunruth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ARUNRUTH;
	    t_ptr->tohit = 20;
	    t_ptr->todam = 12;
	    t_ptr->damage[0] = 3;
	    t_ptr->flags = (TR_FFALL | TR_DEX |
			    TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 50000L;
	    ARUNRUTH = 1;
	    return 1;
	  case 3:
	  case 4:
	  case 5:
	  case 6:
	    if (GLAMDRING)
		return 0;
	    if (wizard || peek)
		msg_print("Glamdring");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_GLAMDRING;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SLOW_DIGEST | TR_SEARCH | TR_FLAME_TONGUE |
			    TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_RES_LT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 40000L;
	    GLAMDRING = 1;
	    return 1;
	  case 7:
	    if (AEGLIN)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_AEGLIN;
	    t_ptr->tohit = 12;
	    t_ptr->todam = 16;
	    t_ptr->flags = (TR_SLOW_DIGEST | TR_SEARCH | TR_RES_LIGHT);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_LIGHTNING);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 45000L;
	    AEGLIN = 1;
	    return 1;
	  default:
	    if (ORCRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Orcrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ORCRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SLOW_DIGEST | TR_STEALTH | TR_FROST_BRAND |
			    TR_RES_COLD);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 40000L;
	    ORCRIST = 1;
	    return 1;
	}
    } else if (!stricmp("& Bastard Sword", name)) {
	if (CALRIS)
	    return 0;
	if (wizard || peek)
	    msg_print("Calris");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_CALRIS;
	t_ptr->tohit = -20;
	t_ptr->todam = 20;
	t_ptr->damage[0] = 3;
	t_ptr->damage[1] = 7;
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_CON | TR_AGGRAVATE |
			TR_CURSED | TR_SLAY_EVIL);
	t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_RES_DISENCHANT
			  | TR_ARTIFACT);
	t_ptr->p1 = 5;
	t_ptr->cost = 100000L;
	CALRIS = 1;
	return 1;
    } else if (!stricmp("& Main Gauche", name)) {
	if (randint(4) > 1)
	    return 0;
	if (MAEDHROS)
	    return 0;
	if (wizard || peek)
	    msg_print("Maedhros");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_MAEDHROS;
	t_ptr->tohit = 12;
	t_ptr->todam = 15;
	t_ptr->damage[0] = 2;
	t_ptr->damage[1] = 6;
	t_ptr->flags = (TR_DEX | TR_INT | TR_FREE_ACT | TR_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 3;
	t_ptr->cost = 20000L;
	MAEDHROS = 1;
	return 1;
    } else if (!stricmp("& Glaive", name)) {
	if (randint(3) > 1)
	    return 0;
	if (PAIN)
	    return 0;
	if (wizard || peek)
	    msg_print("Pain!");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_PAIN;
	t_ptr->tohit = 0;
	t_ptr->todam = 30;
	t_ptr->damage[0] = 10;
	t_ptr->damage[1] = 6;
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 50000L;
	PAIN = 1;
	return 1;
    } else if (!stricmp("& Halberd", name)) {
	if (OSONDIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Osondir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_OSONDIR;
	t_ptr->tohit = 6;
	t_ptr->todam = 9;
	t_ptr->flags = (TR_FLAME_TONGUE | TR_SLAY_UNDEAD | TR_RES_FIRE |
			TR_FFALL | TR_CHR | TR_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_SOUND | TR_SLAY_GIANT);
	t_ptr->p1 = 3;
	t_ptr->cost = 22000L;
	OSONDIR = 1;
	return 1;
    } else if (!stricmp("& Lucerne Hammer", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TURMIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Turmil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_TURMIL;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 6;
	t_ptr->flags = (TR_WIS | TR_REGEN | TR_FROST_BRAND | TR_RES_COLD | TR_INFRA);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT |
			  TR_ACTIVATE | TR_RES_LT);
	t_ptr->p1 = 4;
	t_ptr->cost = 30000L;
	t_ptr->toac = 8;
	TURMIL = 1;
	return 1;
    } else if (!stricmp("& Pike", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Til-i-arc");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_TIL;
	t_ptr->tohit = 10;
	t_ptr->todam = 12;
	t_ptr->toac = 10;
	t_ptr->flags = (TR_FROST_BRAND | TR_FLAME_TONGUE | TR_RES_FIRE | TR_RES_COLD |
			TR_SLOW_DIGEST | TR_INT | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_DEMON | TR_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 2;
	t_ptr->cost = 32000L;
	TIL = 1;
	return 1;
    } else if (!stricmp("& Mace of Disruption", name)) {
	if (randint(5) > 1)
	    return 0;
	if (DEATHWREAKER)
	    return 0;
	if (wizard || peek)
	    msg_print("Deathwreaker");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_DEATHWREAKER;
	t_ptr->tohit = 18;
	t_ptr->todam = 18;
	t_ptr->damage[1] = 12;
	t_ptr->flags = (TR_STR | TR_FLAME_TONGUE | TR_SLAY_EVIL | TR_SLAY_DRAGON |
		   TR_SLAY_ANIMAL | TR_TUNNEL | TR_AGGRAVATE | TR_RES_FIRE);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_IM_FIRE | TR_RES_CHAOS
			  | TR_RES_DISENCHANT | TR_RES_DARK);
	t_ptr->p1 = 6;
	t_ptr->cost = 400000L;
	DEATHWREAKER = 1;
	return 1;
    } else if (!stricmp("& Scythe", name)) {
	if (AVAVIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Avavir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_AVAVIR;
	t_ptr->tohit = 8;
	t_ptr->todam = 8;
	t_ptr->toac = 10;
	t_ptr->flags = (TR_DEX | TR_CHR | TR_FREE_ACT | TR_RES_FIRE | TR_RES_COLD |
			TR_SEE_INVIS | TR_FLAME_TONGUE | TR_FROST_BRAND);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHT | TR_ACTIVATE | TR_RES_LT);
	t_ptr->p1 = 3;
	t_ptr->cost = 18000L;
	AVAVIR = 1;
	return 1;
    } else if (!stricmp("& Mace", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TARATOL)
	    return 0;
	if (wizard || peek)
	    msg_print("Taratol");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_TARATOL;
	t_ptr->tohit = 12;
	t_ptr->todam = 12;
	t_ptr->weight = 200;
	t_ptr->damage[1] = 7;
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHTNING | TR_ACTIVATE | TR_RES_DARK);
	t_ptr->cost = 20000L;
	TARATOL = 1;
	return 1;
    } else if (!stricmp("& Lance", name)) {
	if (randint(3) > 1)
	    return 0;
	if (EORLINGAS)
	    return 0;
	if (wizard || peek)
	    msg_print("Lance of Eorlingas");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_EORLINGAS;
	t_ptr->tohit = 3;
	t_ptr->todam = 21;
	t_ptr->weight = 360;
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_DEX);
	t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	t_ptr->p1 = 2;
	t_ptr->damage[1] = 12;
	t_ptr->cost = 55000L;
	EORLINGAS = 1;
	return 1;
    } else if (!stricmp("& Broad Axe", name)) {
	if (BARUKKHELED)
	    return 0;
	if (wizard || peek)
	    msg_print("Barukkheled");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_BARUKKHELED;
	t_ptr->tohit = 13;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_CON);
	t_ptr->flags2 |= (TR_SLAY_ORC | TR_SLAY_TROLL | TR_SLAY_GIANT | TR_ARTIFACT);
	t_ptr->p1 = 3;
	t_ptr->cost = 50000L;
	BARUKKHELED = 1;
	return 1;
    } else if (!stricmp("& Trident", name)) {
	switch (randint(3)) {
	  case 1:
	  case 2:
	    if (randint(3) > 1)
		return 0;
	    if (WRATH)
		return 0;
	    if (wizard || peek)
		msg_print("Wrath");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_WRATH;
	    t_ptr->tohit = 16;
	    t_ptr->todam = 18;
	    t_ptr->weight = 300;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 9;
	    t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_STR | TR_DEX |
			     TR_SLAY_UNDEAD);
	    t_ptr->flags2 |= (TR_RES_DARK | TR_RES_LT | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 90000L;
	    WRATH = 1;
	    return 1;
	  case 3:
	    if (randint(4) > 1)
		return 0;
	    if (ULMO)
		return 0;
	    if (wizard || peek)
		msg_print("Ulmo");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ULMO;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 19;
	    t_ptr->damage[0] = 4;
	    t_ptr->damage[1] = 10;
	    t_ptr->flags = (TR_SEE_INVIS | TR_FREE_ACT | TR_DEX | TR_REGEN |
			    TR_SLOW_DIGEST | TR_SLAY_ANIMAL | TR_SLAY_DRAGON |
			    TR_RES_ACID);
	    t_ptr->flags2 |= (TR_IM_ACID | TR_HOLD_LIFE | TR_ACTIVATE
			    | TR_RES_NETHER | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 120000L;
	    ULMO = 1;
	    return 1;
	}
    } else if (!stricmp("& Scimitar", name)) {
	if (HARADEKKET)
	    return 0;
	if (wizard || peek)
	    msg_print("Haradekket");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_HARADEKKET;
	t_ptr->tohit = 9;
	t_ptr->todam = 11;
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_DEX | TR_SLAY_UNDEAD
			 | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 30000L;
	HARADEKKET = 1;
	return 1;
    } else if (!stricmp("& Lochaber Axe", name)) {
	if (MUNDWINE)
	    return 0;
	if (wizard || peek)
	    msg_print("Mundwine");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_MUNDWINE;
	t_ptr->tohit = 12;
	t_ptr->todam = 17;
	t_ptr->flags |= (TR_SLAY_EVIL | TR_RES_FIRE | TR_RES_COLD
			 | TR_RES_LIGHT | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 30000L;
	MUNDWINE = 1;
	return 1;
    } else if (!stricmp("& Cutlass", name)) {
	if (GONDRICAM)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondricam");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_GONDRICAM;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 11;
	t_ptr->flags |= (TR_SEE_INVIS | TR_FFALL | TR_REGEN | TR_STEALTH | TR_RES_FIRE |
			 TR_RES_COLD | TR_RES_ACID | TR_RES_LIGHT | TR_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 3;
	t_ptr->cost = 28000L;
	GONDRICAM = 1;
	return 1;
    } else if (!stricmp("& Sabre", name)) {
	if (CARETH)
	    return 0;
	if (wizard || peek)
	    msg_print("Careth Asdriag");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_CARETH;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->flags |= (TR_SLAY_DRAGON | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_SLAY_GIANT | TR_SLAY_ORC | TR_SLAY_TROLL | TR_ARTIFACT |
			  TR_ATTACK_SPD);
	t_ptr->p1 = 1;
	t_ptr->cost = 25000L;
	CARETH = 1;
	return 1;
    } else if (!stricmp("& Rapier", name)) {
	if (FORASGIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Forasgil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_FORASGIL;
	t_ptr->tohit = 12;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR_RES_COLD | TR_FROST_BRAND | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_LIGHT | TR_RES_LT | TR_ARTIFACT);
	t_ptr->cost = 15000L;
	FORASGIL = 1;
	return 1;
    } else if (!stricmp("& Executioner's Sword", name)) {
	if (randint(2) > 1)
	    return 0;
	if (CRISDURIAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Crisdurian");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_CRISDURIAN;
	t_ptr->tohit = 18;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_SLAY_UNDEAD | TR_SLAY_DRAGON);
	t_ptr->flags2 |= (TR_SLAY_GIANT | TR_SLAY_ORC | TR_SLAY_TROLL | TR_ARTIFACT);
	t_ptr->cost = 100000L;
	CRISDURIAN = 1;
	return 1;
    } else if (!stricmp("& Flail", name)) {
	if (TOTILA)
	    return 0;
	if (wizard || peek)
	    msg_print("Totila");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_TOTILA;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->damage[1] = 9;
	t_ptr->flags = (TR_STEALTH | TR_RES_FIRE | TR_FLAME_TONGUE | TR_SLAY_EVIL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ACTIVATE | TR_RES_CONF);
	t_ptr->p1 = 2;
	t_ptr->cost = 55000L;
	TOTILA = 1;
	return 1;
    } else if (!stricmp("& Short sword", name)) {
	if (GILETTAR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gilettar");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_GILETTAR;
	t_ptr->tohit = 3;
	t_ptr->todam = 7;
	t_ptr->flags = (TR_REGEN | TR_SLOW_DIGEST | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 15000L;
	GILETTAR = 1;
	return 1;
    } else if (!stricmp("& Katana", name)) {
	if (randint(3) > 1)
	    return 0;
	if (AGLARANG)
	    return 0;
	if (wizard || peek)
	    msg_print("Aglarang");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_AGLARANG;
	t_ptr->tohit = 0;
	t_ptr->todam = 0;
	t_ptr->damage[0] = 6;
	t_ptr->damage[1] = 8;
	t_ptr->weight = 50;
	t_ptr->flags = (TR_DEX | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 5;
	t_ptr->cost = 40000L;
	AGLARANG = 1;
	return 1;
    } else if (!stricmp("& Spear", name)) {
	switch (randint(6)) {
	  case 1:
	    if (AEGLOS)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglos");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_AEGLOS;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 25;
	    t_ptr->damage[0] = 1;
	    t_ptr->damage[1] = 20;
	    t_ptr->flags = (TR_WIS | TR_FROST_BRAND |
			    TR_RES_COLD | TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
	    t_ptr->cost = 140000L;
	    AEGLOS = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	    if (NIMLOTH)
		return 0;
	    if (wizard || peek)
		msg_print("Nimloth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_NIMLOTH;
	    t_ptr->tohit = 11;
	    t_ptr->todam = 13;
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD | TR_SLAY_UNDEAD |
			    TR_SEE_INVIS | TR_STEALTH);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 30000L;
	    NIMLOTH = 1;
	    return 1;
	  case 6:
	    if (OROME)
		return 0;
	    if (wizard || peek)
		msg_print("Orome");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_OROME;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_SEE_INVIS | TR_SEARCH | TR_INT |
			    TR_RES_FIRE | TR_FFALL | TR_INFRA);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_SLAY_GIANT | TR_RES_LT
			      | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 60000L;
	    OROME = 1;
	    return 1;
	}
    } else if (!stricmp("& Dagger", name)) {
	switch (randint(11)) {
	  case 1:
	    if (ANGRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Angrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ANGRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 5;
	    t_ptr->flags = (TR_DEX | TR_SLAY_EVIL | TR_SUST_STAT |
			    TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_DARK | TR_ARTIFACT);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
	    t_ptr->cost = 100000L;
	    ANGRIST = 1;
	    return 1;
	  case 2:
	  case 3:
	    if (NARTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Narthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_NARTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 12000;
	    NARTHANC = 1;
	    return 1;
	  case 4:
	  case 5:
	    if (NIMTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Nimthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_NIMTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 11000L;
	    NIMTHANC = 1;
	    return 1;
	  case 6:
	  case 7:
	    if (DETHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Dethanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_DETHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR_RES_LIGHT);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHTNING | TR_ARTIFACT);
	    t_ptr->cost = 13000L;
	    DETHANC = 1;
	    return 1;
	  case 8:
	  case 9:
	    if (RILIA)
		return 0;
	    if (wizard || peek)
		msg_print("Rilia");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_RILIA;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 4;
	    t_ptr->flags = TR_POISON;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_DISENCHANT | TR_ARTIFACT);
	    t_ptr->cost = 15000L;
	    RILIA = 1;
	    return 1;
	  case 10:
	  case 11:
	    if (BELANGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Belangil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_BELANGIL;
	    t_ptr->tohit = 6;
	    t_ptr->todam = 9;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 2;
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD | TR_REGEN | TR_SLOW_DIGEST |
			    TR_DEX | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 40000L;
	    BELANGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Small sword", name)) {
	if (STING)
	    return 0;
	if (wizard || peek)
	    msg_print("Sting");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_STING;
	t_ptr->tohit = 7;
	t_ptr->todam = 8;
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL |
			 TR_SLAY_UNDEAD | TR_DEX | TR_CON | TR_STR |
			 TR_FREE_ACT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_RES_LT |
			  TR_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 100000L;
	STING = 1;
	return 1;
    } else if (!stricmp("& Great Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(6) > 1)
		return 0;
	    if (DURIN)
		return 0;
	    if (wizard || peek)
		msg_print("Durin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_DURIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->toac = 15;
	    t_ptr->flags = (TR_SLAY_X_DRAGON | TR_CON | TR_FREE_ACT |
			    TR_RES_FIRE | TR_RES_ACID);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_DARK
			      | TR_RES_LT | TR_RES_CHAOS | TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 150000L;
	    DURIN = 1;
	    return 1;
	  case 2:
	    if (randint(8) > 1)
		return 0;
	    if (EONWE)
		return 0;
	    if (wizard || peek)
		msg_print("Eonwe");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_EONWE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 18;
	    t_ptr->toac = 8;
	    t_ptr->flags = (TR_STATS | TR_SLAY_EVIL | TR_SLAY_UNDEAD | TR_FROST_BRAND |
			    TR_FREE_ACT | TR_SEE_INVIS | TR_RES_COLD);
	    t_ptr->flags2 |= (TR_IM_COLD | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 200000L;
	    EONWE = 1;
	    return 1;
	}
    } else if (!stricmp("& Battle Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (BALLI)
		return 0;
	    if (wizard || peek)
		msg_print("Balli Stonehand");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_BALLI;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 11;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 6;
	    t_ptr->toac = 5;
	    t_ptr->flags = (TR_FFALL | TR_RES_LIGHT | TR_SEE_INVIS | TR_STR | TR_CON
			    | TR_FREE_ACT | TR_RES_COLD | TR_RES_ACID
			    | TR_RES_FIRE | TR_REGEN | TR_STEALTH);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 90000L;
	    BALLI = 1;
	    return 1;
	  case 2:
	    if (LOTHARANG)
		return 0;
	    if (wizard || peek)
		msg_print("Lotharang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_LOTHARANG;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->flags = (TR_STR | TR_DEX);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 1;
	    t_ptr->cost = 21000L;
	    LOTHARANG = 1;
	    return 1;
	}
    } else if (!stricmp("& War Hammer", name)) {
	if (randint(10) > 1)
	    return 0;
	if (AULE)
	    return 0;
	if (wizard || peek)
	    msg_print("Aule");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_AULE;
	t_ptr->damage[0] = 5;
	t_ptr->damage[1] = 5;
	t_ptr->tohit = 19;
	t_ptr->todam = 21;
	t_ptr->toac = 5;
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_SLAY_EVIL | TR_SLAY_UNDEAD |
		    TR_RES_FIRE | TR_RES_ACID | TR_RES_COLD | TR_RES_LIGHT |
			TR_FREE_ACT | TR_SEE_INVIS | TR_WIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_DEMON | TR_LIGHTNING | TR_RES_NEXUS);
	t_ptr->p1 = 4;
	t_ptr->cost = 250000L;
	AULE = 1;
	return 1;
    } else if (!stricmp("& Beaked Axe", name)) {
	if (randint(2) > 1)
	    return 0;
	if (THEODEN)
	    return 0;
	if (wizard || peek)
	    msg_print("Theoden");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_THEODEN;
	t_ptr->tohit = 8;
	t_ptr->todam = 10;
	t_ptr->flags = (TR_WIS | TR_CON | TR_SEARCH | TR_SLOW_DIGEST | TR_SLAY_DRAGON);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->p1 = 3;
	t_ptr->cost = 40000L;
	THEODEN = 1;
	return 1;
    } else if (!stricmp("& Two-Handed Great Flail", name)) {
	if (randint(5) > 1)
	    return 0;
	if (THUNDERFIST)
	    return 0;
	if (wizard || peek)
	    msg_print("Thunderfist");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_THUNDERFIST;
	t_ptr->tohit = 5;
	t_ptr->todam = 18;
	t_ptr->flags = (TR_SLAY_ANIMAL | TR_STR | TR_FLAME_TONGUE |
			TR_RES_FIRE | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC
			  | TR_LIGHTNING | TR_RES_DARK);
	t_ptr->p1 = 4;
	t_ptr->cost = 160000L;
	THUNDERFIST = 1;
	return 1;
    } else if (!stricmp("& Morningstar", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(2) > 1)
		return 0;
	    if (BLOODSPIKE)
		return 0;
	    if (wizard || peek)
		msg_print("Bloodspike");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_BLOODSPIKE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 22;
	    t_ptr->flags = (TR_SLAY_ANIMAL | TR_STR | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_NEXUS);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 30000L;
	    BLOODSPIKE = 1;
	    return 1;
	  case 2:
	    if (FIRESTAR)
		return 0;
	    if (wizard || peek)
		msg_print("Firestar");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_FIRESTAR;
	    t_ptr->tohit = 5;
	    t_ptr->todam = 7;
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->toac = 2;
	    t_ptr->cost = 35000L;
	    FIRESTAR = 1;
	    return 1;
	}
    } else if (!stricmp("& Blade of Chaos", name)) {
	if (DOOMCALLER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Doomcaller");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_DOOMCALLER;
	t_ptr->tohit = 18;
	t_ptr->todam = 28;
	t_ptr->flags = (TR_CON | TR_SLAY_ANIMAL | TR_SLAY_X_DRAGON |
		TR_FROST_BRAND | TR_SLAY_EVIL | TR_FREE_ACT | TR_SEE_INVIS |
		    TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT | TR_RES_ACID |
			TR_AGGRAVATE);
	t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_TELEPATHY | TR_ARTIFACT);
	t_ptr->p1 = -5;
	t_ptr->cost = 200000L;
	DOOMCALLER = 1;
	return 1;
    } else if (!stricmp("& Quarterstaff", name)) {
	switch (randint(7)) {
	  case 1:
	  case 2:
	  case 3:
	    if (NAR)
		return 0;
	    if (wizard || peek)
		msg_print("Nar-i-vagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_NAR;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->flags = (TR_INT | TR_SLAY_ANIMAL | TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 70000L;
	    NAR = 1;
	    return 1;
	  case 4:
	  case 5:
	  case 6:
	    if (ERIRIL)
		return 0;
	    if (wizard || peek)
		msg_print("Eriril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_ERIRIL;
	    t_ptr->tohit = 3;
	    t_ptr->todam = 5;
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SEE_INVIS | TR_INT | TR_WIS);
	    t_ptr->flags2 |= (TR_LIGHT | TR_ACTIVATE | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 20000L;
	    ERIRIL = 1;
	    return 1;
	  case 7:
	    if (OLORIN)
		return 0;
	    if (randint(2) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Olorin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = SN_OLORIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 13;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 10;
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SEE_INVIS | TR_WIS | TR_INT | TR_CHR
			    | TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_HOLD_LIFE | TR_SLAY_ORC | TR_SLAY_TROLL
			      | TR_ACTIVATE | TR_RES_NETHER);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 130000L;
	    OLORIN = 1;
	    return 1;
	}
    }
    return 0;
}

int unique_armour(inven_type *t_ptr)
{
    cptr name;

    if (be_nasty)
	return 0;
    name = object_list[t_ptr->index].name;
    if (!strncmp("Adamantite", name, 10)) {
	if (SOULKEEPER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Soulkeeper");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_COLD);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR_ACTIVATE | TR_RES_CHAOS | TR_RES_DARK |
			  TR_RES_NEXUS | TR_RES_NETHER | TR_ARTIFACT);
	t_ptr->name2 = SN_SOULKEEPER;
	t_ptr->toac = 20;
	t_ptr->cost = 300000L;
	SOULKEEPER = 1;
	return 1;
    }
     /* etc..... */ 
    else if (!strncmp("Multi-Hued", name, 10)) {
	if (RAZORBACK)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Razorback");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_RES_ACID | TR_POISON |
			 TR_RES_LIGHT | TR_FREE_ACT | TR_SEE_INVIS | TR_INT |
			 TR_WIS | TR_STEALTH | TR_AGGRAVATE);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_IM_LIGHT | TR_RES_LT |
			  TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 25;
	t_ptr->p1 = -2;
	t_ptr->weight = 400;
	t_ptr->ac = 30;
	t_ptr->tohit = -3;
	t_ptr->cost = 400000L;
	t_ptr->name2 = SN_RAZORBACK;
	RAZORBACK = 1;
	return 1;
    } else if (!strncmp("Power Drag", name, 10)) {
	if (BLADETURNER)
	    return 0;
	if (wizard || peek)
	    msg_print("Bladeturner");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_RES_ACID | TR_POISON |
			 TR_RES_LIGHT | TR_DEX | TR_SEARCH | TR_REGEN);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR_RES_CONF | TR_RES_SOUND | TR_RES_LT
	      | TR_RES_DARK | TR_RES_CHAOS | TR_RES_DISENCHANT | TR_ARTIFACT
	       | TR_RES_SHARDS | TR_RES_BLIND | TR_RES_NEXUS | TR_RES_NETHER
			  | TR_ACTIVATE);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 35;
	t_ptr->p1 = -3;
	t_ptr->ac = 50;
	t_ptr->tohit = -4;
	t_ptr->weight = 500;
	t_ptr->cost = 500000L;
	t_ptr->name2 = SN_BLADETURNER;
	BLADETURNER = 1;
	return 1;
    } else if (!stricmp("& Pair of Hard Leather Boots", name)) {
	if (FEANOR)
	    return 0;
	if (randint(5) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Feanor");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_SPEED | TR_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_NEXUS | TR_ARTIFACT);
	t_ptr->name2 = SN_FEANOR;
	t_ptr->p1 = 1;
	t_ptr->toac = 20;
	t_ptr->cost = 130000L;
	FEANOR = 1;
	return 1;
    } else if (!stricmp("& Pair of Soft Leather Boots", name)) {
	if (DAL)
	    return 0;
	if (wizard || peek)
	    msg_print("Dal-i-thalion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_FREE_ACT | TR_DEX | TR_SUST_STAT | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT | TR_RES_NETHER | TR_RES_CHAOS);
	t_ptr->name2 = SN_DAL;
	t_ptr->p1 = 5;
	t_ptr->toac = 15;
	t_ptr->cost = 40000L;
	DAL = 1;
	return 1;
    } else if (!stricmp("& Small Metal Shield", name)) {
	if (THORIN)
	    return 0;
	if (randint(2) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Thorin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_CON | TR_FREE_ACT | TR_STR |
			 TR_RES_ACID | TR_SEARCH);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_RES_CHAOS | TR_ARTIFACT | TR_IM_ACID);
	t_ptr->name2 = SN_THORIN;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 0;
	t_ptr->p1 = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 60000L;
	THORIN = 1;
	return 1;
    } else if (!stricmp("Full Plate Armour", name)) {
	if (ISILDUR)
	    return 0;
	if (wizard || peek)
	    msg_print("Isildur");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 300;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_ARTIFACT | TR_RES_NEXUS);
	t_ptr->name2 = SN_ISILDUR;
	t_ptr->tohit = 0;
	t_ptr->toac = 25;
	t_ptr->cost = 40000L;
	ISILDUR = 1;
	return 1;
    } else if (!stricmp("Metal Brigandine Armour", name)) {
	if (ROHAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Rohirrim");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 200;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_STR | TR_DEX);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_RES_CONF | TR_ARTIFACT);
	t_ptr->name2 = SN_ROHAN;
	t_ptr->tohit = 0;
	t_ptr->p1 = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 30000L;
	ROHAN = 1;
	return 1;
    } else if (!stricmp("& Large Metal Shield", name)) {
	if (ANARION)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	else
	    good_item_flag = TRUE;
	if (wizard || peek)
	    msg_print("Anarion");
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = SN_ANARION;
	t_ptr->p1 = 10;
	t_ptr->ident |= ID_NOSHOW_P1;
	t_ptr->tohit = 0;
	t_ptr->toac = 20;
	t_ptr->cost = 160000L;
	ANARION = 1;
	return 1;
    } else if (!stricmp("& Set of Cesti", name)) {
	if (FINGOLFIN)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Fingolfin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_DEX | TR_FREE_ACT);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = SN_FINGOLFIN;
	t_ptr->ident |= ID_SHOW_HITDAM;
	t_ptr->p1 = 4;
	t_ptr->tohit = 10;
	t_ptr->todam = 10;
	t_ptr->toac = 20;
	t_ptr->cost = 110000L;
	FINGOLFIN = 1;
	return 1;
    } else if (!stricmp("& Set of Leather Gloves", name)) {
	if (randint(3) == 1) {
	    if (CAMBELEG)
		return 0;
	    if (wizard || peek)
		msg_print("Cambeleg");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_STR | TR_CON | TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = SN_CAMBELEG;
	    t_ptr->ident |= ID_SHOW_HITDAM;
	    t_ptr->p1 = 2;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 8;
	    t_ptr->toac = 15;
	    t_ptr->cost = 36000L;
	    CAMBELEG = 1;
	    return 1;
	} else {
	    if (CAMMITHRIM)
		return 0;
	    if (wizard || peek)
		msg_print("Cammithrim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_SUST_STAT | TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->name2 = SN_CAMMITHRIM;
	    t_ptr->ident |= ID_NOSHOW_P1;
	    t_ptr->p1 = 5;
	    t_ptr->toac = 10;
	    t_ptr->cost = 30000L;
	    CAMMITHRIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Set of Gauntlets", name)) {
	switch (randint(6)) {
	  case 1:
	    if (PAURHACH)
		return 0;
	    if (wizard || peek)
		msg_print("Paurhach");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR_RES_FIRE;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = SN_PAURHACH;
	    t_ptr->toac = 15;
	    t_ptr->cost = 15000L;
	    PAURHACH = 1;
	    return 1;
	  case 2:
	    if (PAURNIMMEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnimmen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR_RES_COLD;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = SN_PAURNIMMEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 13000L;
	    PAURNIMMEN = 1;
	    return 1;
	  case 3:
	    if (PAURAEGEN)
		return 0;
	    if (wizard || peek)
		msg_print("Pauraegen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR_RES_LIGHT;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = SN_PAURAEGEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 11000L;
	    PAURAEGEN = 1;
	    return 1;
	  case 4:
	    if (PAURNEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR_RES_ACID;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = SN_PAURNEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 12000L;
	    PAURNEN = 1;
	    return 1;
	  default:
	    if (CAMLOST)
		return 0;
	    if (wizard || peek)
		msg_print("Camlost");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_STR | TR_DEX | TR_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = SN_CAMLOST;
	    t_ptr->toac = 0;
	    t_ptr->p1 = -5;
	    t_ptr->tohit = -11;
	    t_ptr->todam = -12;
	    t_ptr->ident |= (ID_SHOW_HITDAM/* | ID_SHOW_P1*/);
	    t_ptr->cost = 0L;
	    CAMLOST = 1;
	    return 1;
	}
    } else if (!stricmp("Mithril Chain Mail", name)) {
	if (BELEGENNON)
	    return 0;
	if (wizard || peek)
	    msg_print("Belegennon");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD |
			 TR_RES_LIGHT | TR_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = SN_BELEGENNON;
	t_ptr->p1 = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 105000L;
	BELEGENNON = 1;
	return 1;
    } else if (!stricmp("Mithril Plate Mail", name)) {
	if (CELEBORN)
	    return 0;
	if (wizard || peek)
	    msg_print("Celeborn");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 250;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_STR | TR_CHR);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_DISENCHANT | TR_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = SN_CELEBORN;
	t_ptr->p1 = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 150000L;
	CELEBORN = 1;
	return 1;
    } else if (!stricmp("Augmented Chain Mail", name)) {
	if (randint(3) > 1)
	    return 0;
	if (CASPANION)
	    return 0;
	if (wizard || peek)
	    msg_print("Caspanion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_POISON | TR_CON | TR_WIS | TR_INT);
	t_ptr->flags2 |= (TR_RES_CONF | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = SN_CASPANION;
	t_ptr->p1 = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 40000L;
	CASPANION = 1;
	return 1;
    } else if (!stricmp("Soft Leather Armour", name)) {
	if (HITHLOMIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Hithlomir");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_STEALTH);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_DARK);
	t_ptr->name2 = SN_HITHLOMIR;
	t_ptr->p1 = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HITHLOMIR = 1;
	return 1;
    } else if (!stricmp("Leather Scale Mail", name)) {
	if (THALKETTOTH)
	    return 0;
	if (wizard || peek)
	    msg_print("Thalkettoth");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags |= (TR_RES_ACID | TR_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_SHARDS);
	t_ptr->name2 = SN_THALKETTOTH;
	t_ptr->toac = 25;
	t_ptr->p1 = 3;
	t_ptr->cost = 25000L;
	THALKETTOTH = 1;
	return 1;
    } else if (!stricmp("Chain Mail", name)) {
	if (ARVEDUI)
	    return 0;
	if (wizard || peek)
	    msg_print("Arvedui");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_STR | TR_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_NEXUS | TR_RES_SHARDS);
	t_ptr->name2 = SN_ARVEDUI;
	t_ptr->p1 = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 32000L;
	ARVEDUI = 1;
	return 1;
    } else if (!stricmp("& Hard Leather Cap", name)) {
	if (THRANDUIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thranduil");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_INT | TR_WIS);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_RES_BLIND | TR_ARTIFACT);
	t_ptr->name2 = SN_THRANDUIL;
	t_ptr->p1 = 2;
	t_ptr->toac = 10;
	t_ptr->cost = 50000L;
	THRANDUIL = 1;
	return 1;
    } else if (!stricmp("& Metal Cap", name)) {
	if (THENGEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thengel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_RES_ACID | TR_WIS | TR_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = SN_THENGEL;
	t_ptr->p1 = 3;
	t_ptr->toac = 12;
	t_ptr->cost = 22000L;
	THENGEL = 1;
	return 1;
    } else if (!stricmp("& Steel Helm", name)) {
	if (HAMMERHAND)
	    return 0;
	if (wizard || peek)
	    msg_print("Hammerhand");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_STR | TR_CON | TR_DEX | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_NEXUS);
	t_ptr->name2 = SN_HAMMERHAND;
	t_ptr->p1 = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HAMMERHAND = 1;
	return 1;
    } else if (!stricmp("& Large Leather Shield", name)) {
	if (CELEGORM)
	    return 0;
	if (wizard || peek)
	    msg_print("Celegorm");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR_RES_LT | TR_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = SN_CELEGORM;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	CELEGORM = 1;
	return 1;
    } else if (!stricmp("& Pair of Metal Shod Boots", name)) {
	if (THROR)
	    return 0;
	if (wizard || peek)
	    msg_print("Thror");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_CON | TR_STR | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = SN_THROR;
	t_ptr->p1 = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	THROR = 1;
	return 1;
    } else if (!stricmp("& Iron Helm", name)) {
	if (randint(6) == 1) {
	    if (DOR_LOMIN)
		return 0;
	    if (wizard || peek)
		msg_print("Dor-Lomin");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			     TR_CON | TR_DEX | TR_STR | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_TELEPATHY | TR_LIGHT | TR_RES_LT | TR_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->name2 = SN_DOR_LOMIN;
	    t_ptr->p1 = 4;
	    t_ptr->toac = 20;
	    t_ptr->cost = 300000L;
	    DOR_LOMIN = 1;
	    return 1;
	} else if (randint(2) == 1) {
	    if (HOLHENNETH)
		return 0;
	    if (wizard || peek)
		msg_print("Holhenneth");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_INT | TR_WIS | TR_SEE_INVIS | TR_SEARCH | TR_RES_ACID);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_BLIND | TR_ARTIFACT);
	    t_ptr->name2 = SN_HOLHENNETH;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->p1 = 2;
	    t_ptr->toac = 10;
	    t_ptr->cost = 100000L;
	    HOLHENNETH = 1;
	    return 1;
	} else {
	    if (GORLIM)
		return 0;
	    if (wizard || peek)
		msg_print("Gorlim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_INT | TR_WIS | TR_SEE_INVIS | TR_SEARCH | TR_CURSED
			     | TR_AGGRAVATE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = SN_GORLIM;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->p1 = -125;
	    t_ptr->toac = 10;
	    t_ptr->cost = 0L;
	    GORLIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Golden Crown", name)) {
	if (randint(3) > 1)
	    return 0;
	if (GONDOR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondor");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = SN_GONDOR;
	t_ptr->flags = (TR_STR | TR_CON | TR_WIS | TR_SEE_INVIS | TR_REGEN
			| TR_RES_ACID | TR_RES_FIRE);
	t_ptr->flags2 = (TR_ACTIVATE | TR_LIGHT | TR_RES_LT | TR_RES_BLIND |
			 TR_ARTIFACT);
	t_ptr->p1 = 3;
	t_ptr->toac = 15;
	t_ptr->cost = 100000L;
	GONDOR = 1;
	return 1;
    } else if (!stricmp("& Iron Crown", name)) {
	if (BERUTHIEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Beruthiel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR_STR | TR_DEX | TR_CON |
		      TR_RES_ACID | TR_SEE_INVIS | TR_FREE_ACT | TR_CURSED);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_ARTIFACT);
	t_ptr->name2 = SN_BERUTHIEL;
	t_ptr->p1 = -125;
	t_ptr->toac = 20;
	t_ptr->cost = 10000L;
	BERUTHIEL = 1;
	return 1;
    }
    return 0;
}


/*
 * JLS: gives one of the "new" resistances to
 */
void give_1_hi_resist(inven_type *t)
{
    switch (randint(10)) {
	case 1: t->flags2 |= TR_RES_CONF; break;
	case 2: t->flags2 |= TR_RES_SOUND; break;
	case 3: t->flags2 |= TR_RES_LT; break;
	case 4: t->flags2 |= TR_RES_DARK; break;
	case 5: t->flags2 |= TR_RES_CHAOS; break;
	case 6: t->flags2 |= TR_RES_DISENCHANT; break;
	case 7: t->flags2 |= TR_RES_SHARDS; break;
	case 8: t->flags2 |= TR_RES_NEXUS; break;
	case 9: t->flags2 |= TR_RES_BLIND; break;
	case 10: t->flags2 |= TR_RES_NETHER; break;
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
    register inven_type *t_ptr;
    register u32b      chance, special, cursed, i;
    u32b               tmp;

    /* Extract the "chance" of "goodness" */
    chance = OBJ_BASE_MAGIC + level;
    if (chance > OBJ_BASE_MAX) chance = OBJ_BASE_MAX;

    special = chance / OBJ_DIV_SPECIAL;

    cursed = (10 * chance) / OBJ_DIV_CURSED;

    t_ptr = &t_list[x];

    /* Depending on treasure type, it can have certain magical properties */
    switch (t_ptr->tval) {

      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:

	if ((t_ptr->index >= 389 && t_ptr->index <= 394)
	    || (t_ptr->index >= 408 && t_ptr->index <= 409)
	    || (t_ptr->index >= 415 && t_ptr->index <= 419)) {

	    byte               artifact = FALSE;

	/* all DSM are enchanted, I guess -CFT */
	    t_ptr->toac += m_bonus(0, 5, level) + randint(5);
	    rating += 30;
	    if ((magik(chance) && magik(special)) || (good == 666)) {
		t_ptr->toac += randint(5);	/* even better... */
		if ((randint(3) == 1 || good == 666) && !not_unique
		    && unique_armour(t_ptr))	/* ...but is it an artifact? */
		    artifact = TRUE;
	    }
	    if (!artifact) {	   /* assume cost&mesg done if it was an artifact */
		if (wizard || peek)
		    msg_print("Dragon Scale Mail");
		t_ptr->cost += ((s32b) t_ptr->toac * 500L);
	    }
	} /* end if is a DSM */ 

	else if (magik(chance) || good) {

	    t_ptr->toac += randint(3) + m_bonus(0, 5, level);
	    if (!stricmp(object_list[t_ptr->index].name, "& Robe") &&
		((magik(special) && randint(30) == 1)
		 || (good == 666 && magik(special)))) {
		t_ptr->flags |= (TR_RES_LIGHT | TR_RES_COLD | TR_RES_ACID |
				 TR_RES_FIRE | TR_SUST_STAT);
		if (wizard || peek)
		    msg_print("Robe of the Magi");
		rating += 30;
		t_ptr->flags2 |= TR_HOLD_LIFE;
		t_ptr->ident |= ID_NOSHOW_P1;
		give_1_hi_resist(t_ptr);	/* JLS */
		t_ptr->p1 = 10;
		t_ptr->toac += 10 + randint(5);
		t_ptr->name2 = SN_MAGI;
		t_ptr->cost = 10000L + (t_ptr->toac * 100);
	    } else if (magik(special) || good == 666)

		switch (randint(9)) {

		  case 1:
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    t_ptr->flags |= (TR_RES_LIGHT | TR_RES_COLD | TR_RES_ACID |
				     TR_RES_FIRE);
		    if (randint(3) == 1) {
			if (peek) msg_print("Elvenkind");
			rating += 25;
			give_1_hi_resist(t_ptr);	/* JLS */
			t_ptr->flags |= TR_STEALTH;
			t_ptr->p1 = randint(3);
			t_ptr->name2 = SN_ELVENKIND;
			t_ptr->toac += 15;
			t_ptr->cost += 15000L;
		    }
		    else {
			if (peek) msg_print("Resist");
			rating += 20;
			t_ptr->name2 = SN_R;
			t_ptr->toac += 8;
			t_ptr->cost += 12500L;
		    }
		    break;

		  case 2:	   /* Resist Acid	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (!strncmp(object_list[t_ptr->index].name,
				 "Mithril", 7) ||
			!strncmp(object_list[t_ptr->index].name,
				 "Adamantite", 10))
			break;
		    if (peek) msg_print("Resist Acid");
		    rating += 15;
		    t_ptr->flags |= TR_RES_ACID;
		    t_ptr->name2 = SN_RA;
		    t_ptr->cost += 1000L;
		    break;

		  case 3: case 4:	   /* Resist Fire	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek) msg_print("Resist Fire");
		    rating += 17;
		    t_ptr->flags |= TR_RES_FIRE;
		    t_ptr->name2 = SN_RF;
		    t_ptr->cost += 600L;
		    break;

		  case 5:  case :	   /* Resist Cold	 */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek) msg_print("Resist Cold");
		    rating += 16;
		    t_ptr->flags |= TR_RES_COLD;
		    t_ptr->name2 = SN_RC;
		    t_ptr->cost += 600L;
		    break;

		  case 7: case 8: case 9:	   /* Resist Lightning */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek) msg_print("Resist Lightning");
		    rating += 15;
		    t_ptr->flags |= TR_RES_LIGHT;
		    t_ptr->name2 = SN_RL;
		    t_ptr->cost += 500L;
		    break;
		}
	}

	/* Cursed armor */
	else if (magik(cursed)) {
	    t_ptr->toac = -randint(3) - m_bonus(0, 10, level);
	    t_ptr->cost = 0L;
	    t_ptr->flags |= TR_CURSED;
	}

	break;

      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:

    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    t_ptr->tohit += randint(3) + m_bonus(0, 10, level);
	    t_ptr->todam += randint(3) + m_bonus(0, 10, level);
	/*
	 * the 3*special/2 is needed because weapons are not as common as
	 * before change to treasure distribution, this helps keep same
	 * number of ego weapons same as before, see also missiles 
	 */
	    if (magik(3*special/2)||good==666) { /* was 2 */
		if (!stricmp("& Whip", object_list[t_ptr->index].name)
		    && randint(2)==1) {
		    if (peek) msg_print("Whip of Fire");
		    rating += 20;
		    t_ptr->name2 = SN_FIRE;
		    t_ptr->flags |= (TR_FLAME_TONGUE | TR_RES_FIRE);
		    /* this should allow some WICKED whips -CFT */
		    while (randint(5*(int)t_ptr->damage[0])==1) {
			t_ptr->damage[0]++;
			t_ptr->cost += 2500;
			t_ptr->cost *= 2;
		    }
		    t_ptr->tohit += 5;
		    t_ptr->todam += 5;
		} else {
		    switch (randint(30)) {	/* was 16 */
		      case 1:	   /* Holy Avenger	 */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Holy Avenger");
			rating += 30;
			t_ptr->flags |= (TR_SEE_INVIS | TR_SUST_STAT |
				      TR_SLAY_UNDEAD | TR_SLAY_EVIL | TR_WIS);
			t_ptr->flags2 |= (TR_SLAY_DEMON | TR_BLESS_BLADE);
			t_ptr->tohit += 5;
			t_ptr->todam += 5;
			t_ptr->toac += randint(4);
		    /* the value in p1 is used for strength increase */
		    /* p1 is also used for sustain stat */
			t_ptr->p1 = randint(4);
			t_ptr->name2 = SN_HA;
			t_ptr->cost += t_ptr->p1 * 500;
			t_ptr->cost += 10000L;
			t_ptr->cost *= 2;
			break;
		      case 2:	   /* Defender	 */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Defender");
			rating += 23;
			t_ptr->flags |= (TR_FFALL | TR_RES_LIGHT | TR_SEE_INVIS
				   | TR_FREE_ACT | TR_RES_COLD | TR_RES_ACID
				     | TR_RES_FIRE | TR_REGEN | TR_STEALTH);
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->toac += 5 + randint(5);
			t_ptr->name2 = SN_DF;
		    /* the value in p1 is used for stealth */
			t_ptr->p1 = randint(3);
			t_ptr->cost += t_ptr->p1 * 500;
			t_ptr->cost += 7500L;
			t_ptr->cost *= 2;
			break;
		      case 3:
		      case 4:	   /* Flame Tongue  */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			rating += 20;
			t_ptr->flags |= (TR_FLAME_TONGUE | TR_RES_FIRE);
			if (peek)
			    msg_print("Flame");
			t_ptr->tohit += 2;
			t_ptr->todam += 3;
			t_ptr->name2 = SN_FT;
			t_ptr->cost += 3000L;
			break;
		      case 5:
		      case 6:	   /* Frost Brand   */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Frost");
			rating += 20;
			t_ptr->flags |= (TR_FROST_BRAND | TR_RES_COLD);
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_FB;
			t_ptr->cost += 2200L;
			break;
		      case 7:
		      case 8:	   /* Slay Animal  */
			t_ptr->flags |= TR_SLAY_ANIMAL;
			rating += 15;
			if (peek)
			    msg_print("Slay Animal");
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = SN_SA;
			t_ptr->cost += 2000L;
			break;
		      case 9:
		      case 10:	   /* Slay Dragon	 */
			t_ptr->flags |= TR_SLAY_DRAGON;
			if (peek)
			    msg_print("Slay Dragon");
			rating += 18;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = SN_SD;
			t_ptr->cost += 4000L;
			break;
		      case 11:
		      case 12:	   /* Slay Evil   */
			t_ptr->flags |= TR_SLAY_EVIL;
			if (randint(3) == 1) {
			    t_ptr->flags |= (TR_WIS);
			    t_ptr->flags2 |= (TR_BLESS_BLADE);
			    t_ptr->p1 = m_bonus(0, 3, level);
			    t_ptr->cost += (200 * t_ptr->p1);
			}
			if (peek)
			    msg_print("Slay Evil");
			rating += 18;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = SN_SE;
			t_ptr->cost += 4000L;
			break;
		      case 13:
		      case 14:	   /* Slay Undead	  */
			t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_UNDEAD);
			if (randint(3) == 1) {
			    t_ptr->flags2 |= (TR_HOLD_LIFE);
			    t_ptr->cost += 1000;
			}
			if (peek)
			    msg_print("Slay Undead");
			rating += 18;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_SU;
			t_ptr->cost += 3000L;
			break;
		      case 15:
		      case 16:
		      case 17:	   /* Slay Orc */
			t_ptr->flags2 |= TR_SLAY_ORC;
			if (peek)
			    msg_print("Slay Orc");
			rating += 13;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_SO;
			t_ptr->cost += 1200L;
			break;
		      case 18:
		      case 19:
		      case 20:	   /* Slay Troll */
			t_ptr->flags2 |= TR_SLAY_TROLL;
			if (peek)
			    msg_print("Slay Troll");
			rating += 13;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_ST;
			t_ptr->cost += 1200L;
			break;
		      case 21:
		      case 22:
		      case 23:
			t_ptr->flags2 |= TR_SLAY_GIANT;
			if (peek)
			    msg_print("Slay Giant");
			rating += 14;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_SG;
			t_ptr->cost += 1200L;
			break;
		      case 24:
		      case 25:
		      case 26:
			t_ptr->flags2 |= TR_SLAY_DEMON;
			if (peek)
			    msg_print("Slay Demon");
			rating += 16;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = SN_SDEM;
			t_ptr->cost += 1200L;
			break;
		      case 27:	   /* of Westernesse */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Westernesse");
			rating += 20;
			t_ptr->flags |= (TR_SEE_INVIS | TR_DEX | TR_CON | TR_STR |
					 TR_FREE_ACT);
			t_ptr->flags2 |= TR_SLAY_ORC;
			t_ptr->tohit += randint(5) + 3;
			t_ptr->todam += randint(5) + 3;
			t_ptr->p1 = 1;
			t_ptr->cost += 10000L;
			t_ptr->cost *= 2;
			t_ptr->name2 = SN_WEST;
			break;
		      case 28:
		      case 29:	   /* Blessed Blade -DGK */
			if ((t_ptr->tval != TV_SWORD) &&
			    (t_ptr->tval != TV_POLEARM))
			    break;
			if (peek)
			    msg_print("Blessed");
			rating += 20;
			t_ptr->flags = TR_WIS;
			t_ptr->flags2 = TR_BLESS_BLADE;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->p1 = randint(3);
			t_ptr->name2 = SN_BLESS_BLADE;
			t_ptr->cost += t_ptr->p1 * 1000;
			t_ptr->cost += 3000L;
			break;
		      case 30:	   /* of Speed -DGK */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique && unique_weapon(t_ptr))
			    break;
			if (wizard || peek)
			    msg_print("Weapon of Extra Attacks");
			rating += 20;
			t_ptr->tohit += randint(5);
			t_ptr->todam += randint(3);
			t_ptr->flags2 = TR_ATTACK_SPD;
			if (t_ptr->weight <= 80)
			    t_ptr->p1 = randint(3);
			else if (t_ptr->weight <= 130)
			    t_ptr->p1 = randint(2);
			else
			    t_ptr->p1 = 1;
			t_ptr->name2 = SN_ATTACKS;
			t_ptr->cost += (t_ptr->p1 * 2000);
			t_ptr->cost *= 2;
			break;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    t_ptr->tohit = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->todam = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->flags |= TR_CURSED;
	    if (level > (20 + randint(15)) && randint(10) == 1) {
		t_ptr->name2 = SN_MORGUL;
		t_ptr->flags |= (TR_SEE_INVIS | TR_AGGRAVATE);
		t_ptr->tohit -= 15;
		t_ptr->todam -= 15;
		t_ptr->toac = -10;
		t_ptr->weight += 100;
	    }
	    t_ptr->cost = 0L;
	}
	break;
      case TV_BOW:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    t_ptr->tohit = randint(3) + m_bonus(0, 10, level);
	    t_ptr->todam = randint(3) + m_bonus(0, 10, level);
	    switch (randint(15)) {
	      case 1: case 2: case 3:
		if (((randint(3)==1)||(good==666)) && !not_unique &&
		    !stricmp(object_list[t_ptr->index].name, "& Long Bow") &&
		    (((i=randint(2))==1 && !BELEG) || (i==2 && !BARD))) {
		    switch (i) {
		    case 1:
			  if (BELEG)
			    break;
			if (wizard || peek)
			    msg_print("Belthronding");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = SN_BELEG;
			t_ptr->ident |= ID_NOSHOW_TYPE;
			t_ptr->subval = 4; /* make do x5 damage!! -CFT */
			t_ptr->tohit = 20;
			t_ptr->todam = 22;
			t_ptr->p1 = 3;
			t_ptr->flags |= (TR_STEALTH | TR_DEX);
			t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_DISENCHANT);
			t_ptr->cost = 35000L;
			BELEG = 1;
			break;
		      case 2:
			if (BARD)
			    break;
			if (wizard || peek)
			    msg_print("Bard");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = SN_BARD;
			t_ptr->subval = 3; /* make do x4 damage!! -CFT */
			t_ptr->tohit = 17;
			t_ptr->todam = 19;
			t_ptr->p1 = 3;
			t_ptr->flags |= (TR_FREE_ACT | TR_DEX);
			t_ptr->flags2 |= (TR_ARTIFACT);
			t_ptr->cost = 20000L;
			BARD = 1;
			break;
		    }
		    break;
		}
		if (((randint(5) == 1) || (good == 666)) && !not_unique &&
		    !stricmp(object_list[t_ptr->index].name, "& Light Crossbow")
		    && !CUBRAGOL) {
		    if (CUBRAGOL)
			break;
		    if (wizard || peek)
			msg_print("Cubragol");
		    t_ptr->name2 = SN_CUBRAGOL;
		    t_ptr->subval = 11;
		    t_ptr->tohit = 10;
		    t_ptr->todam = 14;
		    t_ptr->p1 = 1;
		    t_ptr->flags |= (TR_SPEED | TR_RES_FIRE);
		    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
		    t_ptr->cost = 38000L;
		    CUBRAGOL = 1;
		    break;
		}
		t_ptr->name2 = SN_MIGHT;
		if (peek)
		    msg_print("Bow of Might");
		rating += 15;
		t_ptr->subval++; /* make it do an extra multiple of damage */
		t_ptr->tohit += 5;
		t_ptr->todam += 10;
		break;
	      case 4: case 5: case 6: case 7: case 8:
		t_ptr->name2 = SN_MIGHT;
		if (peek) msg_print("Bow of Might");
		rating += 11;
		t_ptr->tohit += 5;
		t_ptr->todam += 12;
		break;

	      case 9: case 10: case 11: case 12:
	      case 13: case 14: case 15:
		t_ptr->name2 = SN_ACCURACY;
		rating += 11;
		if (peek)
		    msg_print("Accuracy");
		t_ptr->tohit += 12;
		t_ptr->todam += 5;
		break;
	    }
	} else if (magik(cursed)) {
	    t_ptr->tohit = (-m_bonus(5, 30, level));
	    t_ptr->todam = (-m_bonus(5, 20, level));	/* add damage. -CJS- */
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0L;
	}
	break;

      case TV_DIGGING:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || (good == 666)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->p1 += m_bonus(0, 5, level);
	    }
	    if (tmp == 2)	/* do not give additional plusses -CWS */
		;
	    else {
	    /* a cursed digging tool */
		t_ptr->p1 = (-m_bonus(1, 15, level));
		t_ptr->cost = 0L;
		t_ptr->flags |= TR_CURSED;
	    }
	}
	break;

      case TV_GLOVES:
	if (magik(chance) || good) {
	    t_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if ((((randint(2) == 1) && magik(5 * special / 2)) || (good == 666)) &&
		!stricmp(object_list[t_ptr->index].name,
			 "& Set of Leather Gloves") &&
		!not_unique && unique_armour(t_ptr));
	    else if ((((randint(4) == 1) && magik(special)) || (good == 666))
		     && !stricmp(object_list[t_ptr->index].name,
				 "& Set of Gauntlets") &&
		     !not_unique && unique_armour(t_ptr));
	    else if ((((randint(5) == 1) && magik(special)) || (good == 666))
		     && !stricmp(object_list[t_ptr->index].name,
				 "& Set of Cesti") &&
		     !not_unique && unique_armour(t_ptr));

	/* don't forget cesti -CFT */
	    else if (magik(special) || (good == 666)) {
		switch (randint(10)) {

		  case 1: case 2: case 3:
		    if (peek)	msg_print("Free action");
		    rating += 11;
		    t_ptr->flags |= TR_FREE_ACT;
		    t_ptr->name2 = SN_FREE_ACTION;
		    t_ptr->cost += 1000L;
		    break;

		  case 4: case 5: case 6:
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    rating += 17;
		    if (peek)
			msg_print("Slaying");
		    t_ptr->tohit += 1 + randint(4);
		    t_ptr->todam += 1 + randint(4);
		    t_ptr->name2 = SN_SLAYING;
		    t_ptr->cost += (t_ptr->tohit + t_ptr->todam) * 250;
		    break;

		  case 7: case 8: case 9:
		    t_ptr->name2 = SN_AGILITY;
		    if (peek)
			msg_print("Agility");
		    rating += 14;
		    t_ptr->p1 = 2 + randint(2);
		    t_ptr->flags |= TR_DEX;
		    t_ptr->cost += (t_ptr->p1) * 400;
		    break;

		  case 10:
		    if (((randint(3) == 1) || (good == 666)) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Power");
		    rating += 22;
		    t_ptr->name2 = SN_POWER;
		    t_ptr->p1 = 1 + randint(4);
		    t_ptr->tohit += 1 + randint(4);
		    t_ptr->todam += 1 + randint(4);
		    t_ptr->flags |= TR_STR;
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    t_ptr->ident |= ID_NOSHOW_TYPE;
		    t_ptr->cost += (t_ptr->tohit + t_ptr->todam + t_ptr->p1) * 300;
		    break;
		}
	    }
	}

	else if (magik(cursed)) {
	    if (magik(special)) {
		if (randint(2) == 1) {
		    t_ptr->flags |= TR_DEX;
		    t_ptr->name2 = SN_CLUMSINESS;
		}
		else {
		    t_ptr->flags |= TR_STR;
		    t_ptr->name2 = SN_WEAKNESS;
		}
		t_ptr->p1 = (randint(3) - m_bonus(0, 10, level));
	    }
	    t_ptr->toac = (-m_bonus(1, 20, level));
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	}

	break;


      case TV_BOOTS:
	if (magik(chance) || good) {

	    t_ptr->toac = randint(3) + m_bonus(1, 10, level);

	    if (magik(special) || (good == 666)) {
		tmp = randint(12);
		if (tmp == 1) {
		    if (!((randint(2) == 1) && !not_unique
			  && unique_armour(t_ptr))) {
			t_ptr->flags |= TR_SPEED;
			if (wizard || peek)
			    msg_print("Boots of Speed");
			t_ptr->name2 = SN_SPEED;
			rating += 30;
			t_ptr->p1 = 1;
			t_ptr->cost += 300000L;
		    }
		} else if (stricmp("& Pair of Metal Shod Boots",
				   object_list[t_ptr->index].name))	/* not metal */
		    if (tmp > 6) {
			t_ptr->flags |= TR_FFALL;
			rating += 7;
			t_ptr->name2 = SN_SLOW_DESCENT;
			t_ptr->cost += 250;
		    } else if (tmp < 5) {
			t_ptr->flags |= TR_STEALTH;
			rating += 16;
			t_ptr->p1 = randint(3);
			t_ptr->name2 = SN_STEALTH;
			t_ptr->cost += 500;
		    } else {	   /* 5,6 */
			t_ptr->flags |= TR_FREE_ACT;
			rating += 15;
			t_ptr->name2 = SN_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    }
		else
		 /* is metal boots, different odds since no stealth */
		    if (tmp < 5) {
			t_ptr->flags |= TR_FREE_ACT;
			rating += 15;
			t_ptr->name2 = SN_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    } else {	   /* tmp > 4 */
			t_ptr->flags |= TR_FFALL;
			rating += 7;
			t_ptr->name2 = SN_SLOW_DESCENT;
			t_ptr->cost += 250;
		    }
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags |= TR_SPEED;
		t_ptr->name2 = SN_SLOWNESS;
		t_ptr->p1 = -1;
	    } else if (tmp == 2) {
		t_ptr->flags |= TR_AGGRAVATE;
		t_ptr->name2 = SN_NOISE;
	    } else {
		t_ptr->name2 = SN_GREAT_MASS;
		t_ptr->weight = t_ptr->weight * 5;
	    }
	    t_ptr->cost = 0;
	    t_ptr->toac = (-m_bonus(2, 20, level));
	    t_ptr->flags |= TR_CURSED;
	}
	break;

      case TV_HELM:		   /* Helms */

	if ((t_ptr->subval >= 6) && (t_ptr->subval <= 8)) {
	/* give crowns a higher chance for magic */
	    chance += t_ptr->cost / 100;
	    special += special;
	}
	if (magik(chance) || good) {
	    t_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if (magik(special) || (good == 666)) {
		if (t_ptr->subval < 6) {
		    tmp = randint(14);
		    if (tmp < 3) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Intelligence");
			    t_ptr->p1 = randint(2);
			    rating += 13;
			    t_ptr->flags |= TR_INT;
			    t_ptr->name2 = SN_INTELLIGENCE;
			    t_ptr->cost += t_ptr->p1 * 500;
			}
		    } else if (tmp < 6) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Wisdom");
			    rating += 13;
			    t_ptr->p1 = randint(2);
			    t_ptr->flags |= TR_WIS;
			    t_ptr->name2 = SN_WISDOM;
			    t_ptr->cost += t_ptr->p1 * 500;
			}
		    } else if (tmp < 10) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    t_ptr->p1 = 1 + randint(4);
			    rating += 11;
			    t_ptr->flags |= TR_INFRA;
			    t_ptr->name2 = SN_INFRAVISION;
			    t_ptr->cost += t_ptr->p1 * 250;
			}
		    } else if (tmp < 12) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Light");
			    t_ptr->flags2 |= (TR_RES_LT | TR_LIGHT);
			    rating += 6;
			    t_ptr->name2 = SN_LIGHT;
			    t_ptr->cost += 500;
			}
		    } else if (tmp < 14) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Helm of Seeing");
			    t_ptr->flags |= TR_SEE_INVIS;
			    t_ptr->flags2 |= TR_RES_BLIND;
			    rating += 8;
			    t_ptr->name2 = SN_SEEING;
			    t_ptr->cost += 1000;
			}
		    } else {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Telepathy");
			    rating += 20;
			    t_ptr->flags2 |= TR_TELEPATHY;
			    t_ptr->name2 = SN_TELEPATHY;
			    t_ptr->cost += 50000L;
			}
		    }
		} else {
		    switch (randint(6)) {
		      case 1:
			if (!(((randint(2) == 1) || (good == 666)) &&
			      !not_unique && unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Crown of Might");
			    rating += 19;
			    t_ptr->p1 = randint(3);
			    t_ptr->flags |= (TR_FREE_ACT | TR_CON |
					     TR_DEX | TR_STR);
			    t_ptr->name2 = SN_MIGHT;
			    t_ptr->cost += 1000 + t_ptr->p1 * 500;
			}
			break;
		      case 2:
			if (peek)
			    msg_print("Lordliness");
			t_ptr->p1 = randint(3);
			rating += 17;
			t_ptr->flags |= (TR_CHR | TR_WIS);
			t_ptr->name2 = SN_LORDLINESS;
			t_ptr->cost += 1000 + t_ptr->p1 * 500;
			break;
		      case 3:
			if (peek)
			    msg_print("Crown of the Magi");
			rating += 15;
			t_ptr->p1 = randint(3);
			t_ptr->flags |= (TR_RES_LIGHT | TR_RES_COLD
				      | TR_RES_ACID | TR_RES_FIRE | TR_INT);
			t_ptr->name2 = SN_MAGI;
			t_ptr->cost += 3000 + t_ptr->p1 * 500;
			break;
		      case 4:
			rating += 8;
			if (peek)
			    msg_print("Beauty");
			t_ptr->p1 = randint(4);
			t_ptr->flags |= TR_CHR;
			t_ptr->name2 = SN_BEAUTY;
			t_ptr->cost += 750;
			break;
		      case 5:
			if (peek)
			    msg_print("Seeing");
			rating += 8;
			t_ptr->p1 = 5 * (1 + randint(4));
			t_ptr->flags |= (TR_SEE_INVIS | TR_SEARCH);
			t_ptr->name2 = SN_SEEING;
			t_ptr->cost += 1000 + t_ptr->p1 * 100;
			break;
		      case 6:
			t_ptr->flags |= TR_REGEN;
			rating += 10;
			if (peek)
			    msg_print("Regeneration");
			t_ptr->name2 = SN_REGENERATION;
			t_ptr->cost += 1500;
			break;

		    }
		}
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {
	    t_ptr->toac -= m_bonus(1, 20, level);
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	    if (magik(special))
		switch (randint(7)) {
		  case 1:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_INT;
		    t_ptr->name2 = SN_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_WIS;
		    t_ptr->name2 = SN_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_STR;
		    t_ptr->name2 = SN_WEAKNESS;
		    break;
		  case 6:
		    t_ptr->flags |= TR_TELEPORT;
		    t_ptr->name2 = SN_TELEPORTATION;
		    break;
		  case 7:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_CHR;
		    t_ptr->name2 = SN_UGLINESS;
		    break;
		}
	}
	break;

      case TV_RING:		   /* Rings	      */
	if (!((randint(10) == 1) && !not_unique && unique_armour(t_ptr))) {
	    switch (t_ptr->subval) {
	      case 0:
	      case 1:
	      case 2:
	      case 3:		   /* 132-135 */
		if (magik(cursed)) {
		    t_ptr->p1 = -m_bonus(1, 10, level);
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    t_ptr->p1 = m_bonus(1, 6, level);
		    t_ptr->cost += t_ptr->p1 * 100;
		}
		break;
	      case 4:		   /* 136 */
		if (magik(cursed)) {
		    t_ptr->p1 = -randint(3);
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    if (peek)
			msg_print("Ring of Speed");
		    rating += 35;
		    if (randint(888) == 1)
			t_ptr->p1 = 2;
		    else
			t_ptr->p1 = 1;
		}
		break;
	      case 5:
		t_ptr->p1 = 5 * m_bonus(1, 10, level);
		t_ptr->cost += t_ptr->p1 * 30;
		if (magik(cursed)) {
		    t_ptr->p1 = -t_ptr->p1;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 14:
	      case 15:
	      case 16:		   /* Flames, Acid, Ice */
		t_ptr->toac = m_bonus(1, 10, level);
		t_ptr->toac += 5 + randint(7);
		t_ptr->cost += t_ptr->toac * 100;
		break;
              case 17:
              case 18:		   /* WOE, Stupidity */
		t_ptr->toac = (-5) - m_bonus(1,10,level);
		t_ptr->p1 = (-randint(4));
		break;
	      case 19:		   /* Increase damage	      */
		t_ptr->todam = m_bonus(1, 10, level);
		t_ptr->todam += 3 + randint(10);
		t_ptr->cost += t_ptr->todam * 100;
		if (magik(cursed)) {
		    t_ptr->todam = -t_ptr->todam;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 20:		   /* Increase To-Hit	      */
		t_ptr->tohit = m_bonus(1, 10, level);
		t_ptr->tohit += 3 + randint(10);
		t_ptr->cost += t_ptr->tohit * 100;
		if (magik(cursed)) {
		    t_ptr->tohit = -t_ptr->tohit;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 21:		   /* Protection	      */
		t_ptr->toac = m_bonus(0, 10, level);
		t_ptr->toac += 4 + randint(5);
		t_ptr->cost += t_ptr->toac * 100;
		if (magik(cursed)) {
		    t_ptr->toac = -t_ptr->toac;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 24:
	      case 25:
	      case 26:
	      case 27:
	      case 28:
	      case 29:
		t_ptr->ident |= ID_NOSHOW_P1;
		break;
	      case 30:		   /* Slaying	      */
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->todam = m_bonus(1, 10, level);
		t_ptr->todam += 2 + randint(3);
		t_ptr->tohit = m_bonus(1, 10, level);
		t_ptr->tohit += 2 + randint(3);
		t_ptr->cost += (t_ptr->tohit + t_ptr->todam) * 100;
		if (magik(cursed)) {
		    t_ptr->tohit = -t_ptr->tohit;
		    t_ptr->todam = -t_ptr->todam;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      default:
		break;
	    }
	}
	break;

      case TV_AMULET:		   /* Amulets	      */
	if (t_ptr->subval < 2) {
	    if (magik(cursed)) {
		t_ptr->p1 = -m_bonus(1, 5, level);
		t_ptr->flags |= TR_CURSED;
		t_ptr->cost = -t_ptr->cost;
	    } else {
		t_ptr->p1 = m_bonus(1, 5, level);
		t_ptr->cost += t_ptr->p1 * 100;
	    }
	} else if (t_ptr->subval == 2) { /* searching */
	    t_ptr->p1 = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		t_ptr->p1 = -t_ptr->p1;
		t_ptr->cost = -t_ptr->cost;
		t_ptr->flags |= TR_CURSED;
	    } else
		t_ptr->cost += 20 * t_ptr->p1;
	} else if (t_ptr->subval == 8) {
	    rating += 25;
	    t_ptr->p1 = 5 * (randint(2) + m_bonus(0, 10, level));
	    t_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    t_ptr->cost += 20 * t_ptr->p1 + 50 * t_ptr->toac;
	    if (t_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		t_ptr->flags |= TR_CURSED;
	} else if (t_ptr->subval == 9) {
	/* amulet of DOOM */
	    t_ptr->p1 = (-randint(5) - m_bonus(2, 10, level));
	    t_ptr->toac = (-randint(3) - m_bonus(0, 6, level));
	    t_ptr->flags |= TR_CURSED;
	}
	break;

    /* Subval should be even for store, odd for dungeon */
    /* Dungeon found ones will be partially charged	 */
      case TV_LIGHT:
	if ((t_ptr->subval % 2) == 1) {
	    t_ptr->p1 = randint(t_ptr->p1);
	    t_ptr->subval -= 1;
	}
	break;

      case TV_WAND:
	switch (t_ptr->subval) {
	  case 0:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 1:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 4:
	    t_ptr->p1 = randint(4) + 3;
	    break;
	  case 5:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 6:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 7:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 8:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 9:
	    t_ptr->p1 = randint(12) + 6;
	    break;
	  case 10:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->p1 = randint(3) + 3;
	    break;
	  case 12:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 13:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 14:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 15:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 16:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->p1 = randint(5) + 4;
	    break;
	  case 18:
	    t_ptr->p1 = randint(8) + 4;
	    break;
	  case 19:
	    t_ptr->p1 = randint(6) + 2;
	    break;
	  case 20:
	    t_ptr->p1 = randint(4) + 2;
	    break;
	  case 21:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 22:
	    t_ptr->p1 = randint(5) + 2;
	    break;
	  case 23:
	    t_ptr->p1 = randint(12) + 12;
	    break;
	  case 24:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 26:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 27:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 28:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  default:
	    break;
	}
	break;

      case TV_STAFF:
	switch (t_ptr->subval) {
	  case 0:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 1:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 4:
	    t_ptr->p1 = randint(15) + 6;
	    break;
	  case 5:
	    t_ptr->p1 = randint(4) + 5;
	    break;
	  case 6:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 7:
	    t_ptr->p1 = randint(3) + 1;
	    t_ptr->level = 10;
	    break;
	  case 8:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 9:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 10:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 12:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 13:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 14:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 15:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 16:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 18:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 19:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 20:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 21:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 22:
	    t_ptr->p1 = randint(10) + 6;
	    t_ptr->level = 5;
	    break;
	  case 23:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 24:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->p1 = randint(2) + 2;
	    break;
	  case 26:
	    t_ptr->p1 = randint(15) + 5;
	    break;
	  case 27:
	    t_ptr->p1 = randint(2) + 2;
	    break;
	  case 28:
	    t_ptr->p1 = randint(5) + 5;
	    break;
	  case 29:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 30:
	    t_ptr->p1 = randint(6) + 2;
	    break;
	  default:
	    break;
	}
	break;

      case TV_CLOAK:
	if (magik(chance) || good) {
	    int                 made_art_cloak;

	    made_art_cloak = 0;
	    t_ptr->toac += 1 + m_bonus(0, 20, level);
	    if (magik(special) || (good == 666)) {
		if (!not_unique &&
		    !stricmp(object_list[t_ptr->index].name, "& Cloak")
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
			t_ptr->name2 = SN_COLLUIN;
			t_ptr->toac = 15;
			t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_POISON |
					 TR_RES_LIGHT | TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->cost = 10000L;
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
			t_ptr->name2 = SN_HOLCOLLETH;
			t_ptr->toac = 4;
			t_ptr->p1 = 2;
			t_ptr->flags |= (TR_INT | TR_WIS | TR_STEALTH |
					 TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->ident |= ID_NOSHOW_TYPE;
			t_ptr->cost = 13000L;
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
			t_ptr->name2 = SN_THINGOL;
			t_ptr->toac = 18;
			t_ptr->flags = (TR_DEX | TR_CHR | TR_RES_FIRE |
				   TR_RES_ACID | TR_RES_COLD | TR_FREE_ACT);
			t_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 3;
			t_ptr->cost = 35000L;
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
			t_ptr->name2 = SN_THORONGIL;
			t_ptr->toac = 10;
			t_ptr->flags = (TR_SEE_INVIS | TR_FREE_ACT |
					TR_RES_ACID);
			t_ptr->flags2 |= (TR_ARTIFACT);
			t_ptr->cost = 8000L;
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
			t_ptr->name2 = SN_COLANNON;
			t_ptr->toac = 15;
			t_ptr->flags |= (TR_STEALTH | TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 3;
			t_ptr->cost = 11000L;
			made_art_cloak = 1;
			COLANNON = 1;
			break;
		    }

		} else if (!not_unique &&
			   !stricmp(object_list[t_ptr->index].name,
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
			t_ptr->name2 = SN_LUTHIEN;
			t_ptr->toac = 20;
			t_ptr->flags = (TR_RES_FIRE | TR_RES_COLD |
				    TR_INT | TR_WIS | TR_CHR | TR_RES_ACID);
			t_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 2;
			t_ptr->cost = 45000L;
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
			t_ptr->name2 = SN_TUOR;
			t_ptr->toac = 12;
			t_ptr->flags = (TR_STEALTH |
				  TR_FREE_ACT | TR_SEE_INVIS | TR_RES_ACID);
			t_ptr->flags2 |= (TR_IM_ACID | TR_ARTIFACT);
			t_ptr->p1 = 4;
			t_ptr->cost = 35000L;
			made_art_cloak = 1;
			TUOR = 1;
			break;
		    }
		}
		if (!made_art_cloak) {
		    if (randint(2) == 1) {
			t_ptr->name2 = SN_PROTECTION;
			t_ptr->toac += m_bonus(0, 10, level) + (5 + randint(3));
			t_ptr->cost += 250L;
			rating += 8;
		    } else if (randint(10) < 10) {
			t_ptr->toac += m_bonus(3, 10, level);
			t_ptr->p1 = randint(3);
			t_ptr->flags |= TR_STEALTH;
			t_ptr->name2 = SN_STEALTH;
			t_ptr->cost += 500 + (50 * t_ptr->p1);
			rating += 9;
		    } else {
			t_ptr->toac += 10 + randint(10);
			t_ptr->p1 = randint(3);
			t_ptr->flags |= (TR_STEALTH | TR_RES_ACID);
			t_ptr->name2 = SN_AMAN;
			t_ptr->cost += 4000 + (100 * t_ptr->toac);
			rating += 16;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags |= TR_AGGRAVATE;
		t_ptr->name2 = SN_IRRITATION;
		t_ptr->toac -= m_bonus(1, 10, level);
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->tohit -= m_bonus(1, 10, level);
		t_ptr->todam -= m_bonus(1, 10, level);
		t_ptr->cost = 0;
	    } else if (tmp == 2) {
		t_ptr->name2 = SN_VULNERABILITY;
		t_ptr->toac -= m_bonus(10, 20, level + 50);
		t_ptr->cost = 0;
	    } else {
		t_ptr->name2 = SN_ENVELOPING;
		t_ptr->toac -= m_bonus(1, 10, level);
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->tohit -= m_bonus(2, 15, level + 10);
		t_ptr->todam -= m_bonus(2, 15, level + 10);
		t_ptr->cost = 0;
	    }
	    t_ptr->flags |= TR_CURSED;
	}
	break;

      case TV_CHEST:
	switch (randint(level + 4)) {
	  case 1:
	    t_ptr->flags = 0;
	    t_ptr->name2 = SN_EMPTY;
	    break;
	  case 2:
	    t_ptr->flags |= CH_LOCKED;
	    t_ptr->name2 = SN_LOCKED;
	    break;
	  case 3:
	  case 4:
	    t_ptr->flags |= (CH_LOSE_STR | CH_LOCKED);
	    t_ptr->name2 = SN_POISON_NEEDLE;
	    break;
	  case 5:
	  case 6:
	    t_ptr->flags |= (CH_POISON | CH_LOCKED);
	    t_ptr->name2 = SN_POISON_NEEDLE;
	    break;
	  case 7:
	  case 8:
	  case 9:
	    t_ptr->flags |= (CH_PARALYSED | CH_LOCKED);
	    t_ptr->name2 = SN_GAS_TRAP;
	    break;
	  case 10:
	  case 11:
	    t_ptr->flags |= (CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = SN_EXPLOSION_DEVICE;
	    break;
	  case 12:
	  case 13:
	  case 14:
	    t_ptr->flags |= (CH_SUMMON | CH_LOCKED);
	    t_ptr->name2 = SN_SUMMONING_RUNES;
	    break;
	  case 15:
	  case 16:
	  case 17:
	    t_ptr->flags |= (CH_PARALYSED | CH_POISON | CH_LOSE_STR |
			     CH_LOCKED);
	    t_ptr->name2 = SN_MULTIPLE_TRAPS;
	    break;
	  default:
	    t_ptr->flags |= (CH_SUMMON | CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = SN_MULTIPLE_TRAPS;
	    break;
	}
	if (not_unique)		/* if bought from store - dbd */
	    t_ptr->p1 = randint(t_ptr->level);
	else			/* store the level chest's found on - dbd */
	    t_ptr->p1 = dun_level;
	break;

      case TV_SPIKE:
	t_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    t_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	t_ptr->p1 = missile_ctr;
	break;

    case TV_BOLT: case TV_ARROW: case TV_SLING_AMMO:
     /* this fn makes ammo for player's missile weapon more common -CFT */
      magic_ammo(t_ptr, good, chance, special, cursed, level);
      break;

      case TV_FOOD:
    /* make sure all food rations have the same level */
	if (t_ptr->subval == 90)
	    t_ptr->level = 0;
    /* give all elvish waybread the same level */
	else if (t_ptr->subval == 92)
	    t_ptr->level = 6;
	break;

      case TV_SCROLL1:
    /* give all identify scrolls the same level */
	if (t_ptr->subval == 67)
	    t_ptr->level = 1;
    /* scroll of light */
	else if (t_ptr->subval == 69)
	    t_ptr->level = 0;
    /* scroll of trap detection */
	else if (t_ptr->subval == 80)
	    t_ptr->level = 5;
    /* scroll of door/stair location */
	else if (t_ptr->subval == 81)
	    t_ptr->level = 5;
	break;

      case TV_POTION1:		   /* potions */
    /* cure light */
	if (t_ptr->subval == 76)
	    t_ptr->level = 0;
	break;

      default:
	break;
    }
}


static struct opt_desc {
    cptr                o_prompt;
    int                *o_var;
} options[] = {

    { "Running: cut known corners",	 	&find_cut },
    { "Running: examine potential corners",	&find_examine },
    { "Running: print self during run", 	&find_prself },
    { "Running: stop when map sector changes",  &find_bound},
    { "Running: run through open doors", 	&find_ignore_doors},
    { "(g)et-key to pickup objects", 		&prompt_carry_flag},
    { "Prompt before pickup", 			&carry_query_flag},
    { "Rogue like commands", 			&rogue_like_commands},
    { "Show weights in inventory", 		&show_weight_flag},
    { "Show weights in equipment list",		&show_equip_weight_flag},
    { "Highlight and notice mineral seams", 	&highlight_seams},
    { "Disable haggling in stores",		&no_haggle_flag},
    { "Plain object descriptions",		&plain_descriptions},
    { "Quick messages",		                &quick_messages},
    { "Equippy chars",		                &equippy_chars},
    { "Low hitpoint warning",			&hitpoint_warn},
    { "Delay speed", 				&delay_spd},
    { (char *)0, 				(int *)0}
};


/*
 * Set or unset various boolean options.		-CJS-
 */
void set_options()
{
    register int i, max, ch;
    vtype        string;

    prt("  ESC when finished, y/n or 0-9 to set options, <return> or - to move cursor",
	0, 0);
    for (max = 0; options[max].o_prompt != 0; max++) {
	if (options[max].o_var == &hitpoint_warn)
	    (void)sprintf(string, "%-38s: %d0%% ", options[max].o_prompt,
			  *options[max].o_var);
	else if (options[max].o_var == &delay_spd)
	    (void)sprintf(string, "%-38s: %d ", options[max].o_prompt,
			  *options[max].o_var);
	else
	    (void)sprintf(string, "%-38s: %s ", options[max].o_prompt,
			  (*options[max].o_var ? "yes" : "no "));
	prt(string, max + 1, 0);
    }
    erase_line(max + 1, 0);
    i = 0;
    for (;;) {
	move_cursor(i + 1, 40);
	ch = inkey();
	switch (ch) {
	  case ESCAPE:	  
	    draw_cave();
	    creatures(FALSE);	 /* draw monsters */
	    prt_equippy_chars(); /* redraw equippy chars */
	    return;
	  case '-':
	    if (i > 0)
		i--;
	    else
		i = max - 1;
	    break;
	  case ' ':
	  case '\n':
	  case '\r':
	    if (i + 1 < max)
		i++;
	    else
		i = 0;
	    break;
	  case 'y':
	  case 'Y':
	    if ((options[i].o_var == &hitpoint_warn) ||
		(options[i].o_var == &delay_spd))
		bell();
	    else {
		put_buffer("yes ", i + 1, 40);
		*options[i].o_var = TRUE;
		if (i + 1 < max)
		    i++;
		else
		    i = 0;
	    }
	    break;
	  case 'n':
	  case 'N':
	    if (options[i].o_var == &delay_spd)
		bell();
	    else if (options[i].o_var == &hitpoint_warn) {
		put_buffer("00%", i + 1, 40);
		*options[i].o_var = 0;
	    } else {
		put_buffer("no  ", i + 1, 40);
		*options[i].o_var = FALSE;
		if (i + 1 < max)
		    i++;
		else
		    i = 0;
	    }
	    break;
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
	  case '0':
	    if ((options[i].o_var != &delay_spd) &&
		(options[i].o_var != &hitpoint_warn))
		bell();
	    else {
		ch = ch - '0';
		*options[i].o_var = ch;
		if (options[i].o_var == &hitpoint_warn)
		    sprintf(string, "%d0%%  ", ch);
		else
		    sprintf(string, "%d   ", ch);
		put_buffer(string, i + 1, 40);
		if (i + 1 < max)
		    i++;
		else
		    i = 0;
	    }
	    break;
	  default:
	    bell();
	    break;
	}
    }
}

static void magic_ammo(inven_type *t_ptr, int good, int chance, int special, int cursed, int level)
{
    register inven_type *i_ptr = NULL;
    register int         i;

    /* if wielding a bow as main/aux weapon, then ammo will be "right" ammo
     * more often than not of the time -CFT */
    if (inventory[INVEN_WIELD].tval == TV_BOW) i_ptr=&inventory[INVEN_WIELD];
    else if (inventory[INVEN_AUX].tval == TV_BOW) i_ptr=&inventory[INVEN_AUX];

    if (i_ptr && (randint(2)==1)){
	if ((t_ptr->tval == TV_SLING_AMMO) &&
	    (i_ptr->subval >= 20) && (i_ptr->subval <= 21));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_ARROW) &&
		 (i_ptr->subval >= 1) && (i_ptr->subval <= 4));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_BOLT) &&
		 (i_ptr->subval >= 10) && (i_ptr->subval <= 12));
	/* right type, do nothing */
	else if ((i_ptr->subval >= 20) && (i_ptr->subval <= 21))
	    invcopy(t_ptr, 83); /* this should be treasure list index of shots -CFT */
	else if ((i_ptr->subval >= 1) && (i_ptr->subval <= 4))
	    invcopy(t_ptr, 78); /* this should be index of arrows -CFT */
	else			/* xbow */
	    invcopy(t_ptr, 80); /* this should be index of bolts -CFT */
    }

    t_ptr->number = 0;
    for (i = 0; i < 7; i++)
	t_ptr->number += randint(6);
    if (missile_ctr == MAX_SHORT)
	missile_ctr = -MAX_SHORT - 1;
    else
	missile_ctr++;
    t_ptr->p1 = missile_ctr;

    /* always show tohit/todam values if identified */
    t_ptr->ident |= ID_SHOW_HITDAM;
    if (magik(chance)||good) {
	t_ptr->tohit = randint(5) + m_bonus(1, 15, level);
	t_ptr->todam = randint(5) + m_bonus(1, 15, level);
	/* see comment for weapons */
	if (magik(5*special/2)||(good==666))
	    switch(randint(11)) {
	      case 1: case 2: case 3:
		t_ptr->name2 = SN_WOUNDING; /* swapped with slaying -CFT */
		t_ptr->tohit += 5;
		t_ptr->todam += 5;
		t_ptr->damage[0] ++; /* added -CFT */
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 4: case 5:
		t_ptr->flags |= (TR_FLAME_TONGUE|TR_RES_FIRE); /* RF so won't burn */
		t_ptr->tohit += 2;
		t_ptr->todam += 4;
		t_ptr->name2 = SN_FIRE;
		t_ptr->cost += 25;
		rating += 6;
		break;
	      case 6: case 7:
		t_ptr->flags |= TR_SLAY_EVIL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_SLAY_EVIL;
		t_ptr->cost += 25;
		rating += 7;
		break;
	      case 8: case 9:
		t_ptr->flags |= TR_SLAY_ANIMAL;
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		t_ptr->name2 = SN_SLAY_ANIMAL;
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 10:
		t_ptr->flags |= TR_SLAY_DRAGON;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_DRAGON_SLAYING;
		t_ptr->cost += 35;
		rating += 9;
		break;
	      case 11:
		t_ptr->tohit += 10; /* reduced because of dice bonus -CFT */
		t_ptr->todam += 10;
		t_ptr->name2 = SN_SLAYING; /* swapped w/ wounding -CFT */
		t_ptr->damage[0] += 2; /* added -CFT */
		t_ptr->cost += 45;
		rating += 10;
		break;
	    }
	while (magik(special)) { /* added -CFT */
	    t_ptr->damage[0]++;
	    t_ptr->cost += t_ptr->damage[0]*5;
	}
    }
    else if (magik(cursed)) {
	t_ptr->tohit = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->todam = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->flags |= TR_CURSED;
	t_ptr->cost = 0;
	if (randint(5)==1) {
	    t_ptr->name2 = SN_BACKBITING;
	    t_ptr->tohit -= 20;
	    t_ptr->todam -= 20;
	}
    }
}









#define BLANK_LENGTH	24
static char blank_string[] = "                        ";

extern int rating;

/*
 * Places a particular trap at location y, x		-RAK-	
 */
void place_trap(int y, int x, int subval)
{
    register int cur_pos;

    if (!in_bounds(y, x))
	return;	       /* abort! -CFT */
    if (cave[y][x].cptr >= MIN_MONIX)
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
    cur_pos = popt();
    cave[y][x].tptr = cur_pos;
    invcopy(&t_list[cur_pos], OBJ_TRAP_LIST + subval);
}


/*
 * Places rubble at location y, x			-RAK-	 */
void place_rubble(int y, int x)
{
    register int        cur_pos;
    register cave_type *cave_ptr;

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
    cur_pos = popt();
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    cave_ptr->fval = BLOCKED_FLOOR;
    invcopy(&t_list[cur_pos], OBJ_RUBBLE);
}

/*
 * if killed a 'Creeping _xxx_ coins'... -CWS */
void get_coin_type(creature_type *c_ptr)
{
    if (!stricmp(c_ptr->name, "Creeping copper coins")) {
	coin_type = 2;
    }

    if (!stricmp(c_ptr->name, "Creeping silver coins")) {
	coin_type = 5;
    }

    if (!stricmp(c_ptr->name, "Creeping gold coins")) {
	coin_type = 10;
    }

    if (!stricmp(c_ptr->name, "Creeping mithril coins")) {
	coin_type = 16;
    }

    if (!stricmp(c_ptr->name, "Creeping adamantite coins")) {
	coin_type = 17;
    }
}

/*
 * Places a treasure (Gold or Gems) at given row, column -RAK-	 */
void place_gold(int y, int x)
{
    register int        i, cur_pos;
    register inven_type *t_ptr;

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
    cur_pos = popt();
    i = ((randint(object_level + 2) + 2) / 2) - 1;
    if (randint(OBJ_GREAT) == 1)
	i += randint(object_level + 1);
    if (i >= MAX_GOLD)
	i = MAX_GOLD - 1;
    if (coin_type) {			/* if killed a Creeping _xxx_ coins... */
	if (coin_type > MAX_GOLD - 1)
	    coin_type = 0;		/* safety check -CWS */
	i = coin_type;
    }
    cave[y][x].tptr = cur_pos;
    invcopy(&t_list[cur_pos], OBJ_GOLD_LIST + i);
    t_ptr = &t_list[cur_pos];
    t_ptr->cost += (8L * (long)randint((int)t_ptr->cost)) + randint(8);

/*
 * average the values to make Creeping _xxx_ coins not give too great treasure drops */
    if (coin_type)
	t_ptr->cost = ((8L * (long)randint((int)object_list[OBJ_GOLD_LIST + i].cost))
		       + (t_ptr->cost)) >> 1;
					  
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
}


/*
 * Returns the array number of a random object		-RAK-	 */
int get_obj_num(int level, int good)
{
    register int i, j;

    do {
	if (level == 0)
	    i = randint(t_level[0]) - 1;
	else {
	    if (level >= MAX_OBJ_LEVEL)
		level = MAX_OBJ_LEVEL;
	    else if (randint(OBJ_GREAT) == 1) {
		level = level * MAX_OBJ_LEVEL / randint(MAX_OBJ_LEVEL) + 1;
		if (level > MAX_OBJ_LEVEL)
		    level = MAX_OBJ_LEVEL;
	    }
	/*
	 * This code has been added to make it slightly more likely to get
	 * the higher level objects.	Originally a uniform distribution
	 * over all objects less than or equal to the dungeon level.  This
	 * distribution makes a level n objects occur approx 2/n% of the time
	 * on level n, and 1/2n are 0th level. 
	 */

	    if (randint(2) == 1)
		i = randint(t_level[level]) - 1;
	    else {		   /* Choose three objects, pick the highest level. */
		i = randint(t_level[level]) - 1;
		j = randint(t_level[level]) - 1;
		if (i < j)
		    i = j;
		j = randint(t_level[level]) - 1;
		if (i < j)
		    i = j;
		j = object_list[sorted_objects[i]].level;
		if (j == 0)
		    i = randint(t_level[0]) - 1;
		else
		    i = randint(t_level[j] - t_level[j - 1]) - 1 + t_level[j - 1];
	    }
	}
    } while (((object_list[sorted_objects[i]].rare ?
	       (randint(object_list[sorted_objects[i]].rare) - 1) : 0) && !good)
	     || (object_list[sorted_objects[i]].rare == 255));
    return (i);
}



int special_place_object(int y, int x)
{
    register int cur_pos, tmp;
    char         str[100];
    int          done = 0;

    if (!in_bounds(y, x))
	return 0;		   /* abort! -CFT */
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
    cur_pos = popt();
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
    cur_pos = popt();
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
 * Places a GOOD-object at given row, column co-ordinate ~Ludwig */
void place_special(int y, int x, u32b good)
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
    cur_pos = popt();
    cave[y][x].tptr = cur_pos;
    do {
	tmp = get_obj_num((object_level + 10), TRUE);
	tv = object_list[sorted_objects[tmp]].tval;
	if ((tv == TV_HELM) || (tv == TV_SHIELD) ||
	    (tv == TV_CLOAK) || (tv == TV_HAFTED) || (tv == TV_POLEARM) ||
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
	    (object_list[sorted_objects[tmp]].subval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
	if ((tv == TV_PRAYER_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Wrath of
					 * God */
	    (object_list[sorted_objects[tmp]].subval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
    } while (!is_good);
    invcopy(&t_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, object_level, (good & SPECIAL) ? 666 : 1, 0);
    if (peek) {
	if (object_list[sorted_objects[tmp]].level > object_level) {
	    char                buf[200];
	    byte               t;

	    t = t_list[cur_pos].ident;
	    t_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &t_list[cur_pos], TRUE);
	    t_list[cur_pos].ident = t;
	    msg_print(buf);
	}
    }
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
}


/* 
 * Allocates an object for tunnels and rooms		-RAK-	
 */
void alloc_object(int (*alloc_set) (), int typ, int num)
{
    register int i, j, k;

    for (k = 0; k < num; k++) {
	do {
	    i = randint(cur_height) - 1;
	    j = randint(cur_width) - 1;
	}
    /*
     * don't put an object beneath the player, this could cause problems if
     * player is standing under rubble, or on a trap 
     */
	while ((!(*alloc_set) (cave[i][j].fval)) ||
	       (cave[i][j].tptr != 0) || (i == char_row && j == char_col));
	if (typ < 4) {		   /* typ == 2 not used, used to be visible
				    * traps */
	    if (typ == 1)
		place_trap(i, j, randint(MAX_TRAP) - 1);	/* typ == 1 */
	    else
		place_rubble(i, j);/* typ == 3 */
	} else {
	    object_level = dun_level;
	    if (typ == 4)
		place_gold(i, j);  /* typ == 4 */
	    else
		place_object(i, j);/* typ == 5 */
	}
    }
}


/* 
 * Creates objects nearby the coordinates given		-RAK-	 
 */
void random_object(int y, int x, int num)
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    do {
	i = 0;
	do {
	    do {
		j = y - 3 + randint(5);
		k = x - 4 + randint(7);
	    } while (!in_bounds(j, k));
	    cave_ptr = &cave[j][k];
	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		object_level = dun_level;
		if (randint(100) < 75)
		    place_object(j, k);
		else
		    place_gold(j, k);
		i = 9;
	    }
	    i++;
	}
	while (i <= 10);
	num--;
    }
    while (num != 0);
}

void special_random_object(int y, int x, int num)
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    object_level = dun_level;
    do {
	i = 0;
	do {
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = &cave[j][k];
	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		if (randint(5) == 1) {
		    if (!special_place_object(j, k))
			place_special(j, k, SPECIAL);
		} else {
		    place_special(j, k, SPECIAL);
		}
		i = 9;
	    }
	    i++;
	}
	while (i <= 10);
	num--;
    }
    while (num != 0);
}












/*
 * Boost a stat artificially (by wearing something). If the display argument
 * is TRUE, then increase is shown on the screen. 
 */
void bst_stat(int stat, int amount)
{
    py.stats.mod_stat[stat] += amount;

    set_use_stat(stat);
/* can not call prt_stat() here, may be in store, may be in inven_command */
    py.flags.status |= (PY_STR << stat);
}



/*
 * Destroy an item in the inventory			-RAK-	 */
void inven_destroy(int item_val)
{
    register int         j;
    register inven_type *i_ptr;

    i_ptr = &inventory[item_val];
    if ((i_ptr->number > 1) && (i_ptr->subval <= ITEM_SINGLE_STACK_MAX)) {
	i_ptr->number--;
	inven_weight -= i_ptr->weight;
    } else {
	inven_weight -= i_ptr->weight * i_ptr->number;
	for (j = item_val; j < inven_ctr - 1; j++)
	    inventory[j] = inventory[j + 1];
	invcopy(&inventory[inven_ctr - 1], OBJ_NOTHING);
	inven_ctr--;
    }
    py.flags.status |= PY_STR_WGT;
}


/*
 * Copies the object in the second argument over the first argument. However,
 * the second always gets a number of one except for ammo etc. 
 */
void take_one_item(register inven_type *s_ptr, register inven_type *i_ptr)
{
    *s_ptr = *i_ptr;
    if ((s_ptr->number > 1) && (s_ptr->subval >= ITEM_SINGLE_STACK_MIN)
	&& (s_ptr->subval <= ITEM_SINGLE_STACK_MAX))
	s_ptr->number = 1;
}


/*
 * Drops an item from inventory to given location	-RAK-	 */
void inven_drop(register int item_val, register int drop_all)
{
    int                  i;
    register inven_type *i_ptr;
    vtype                prt2;
    bigvtype             prt1;

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


/*
 * Destroys a type of item on a given percent chance	-RAK-	 */
int 
inven_damage(typ, perc)
#ifdef MSDOS
int (*typ) (inven_type *);

#else
int (*typ) ();

#endif
register int perc;

{
    register int index, i, j, offset;
    vtype        tmp_str, out_val;

    j = 0;
    offset = randint(inven_ctr);
    for (index = 0; index < inven_ctr; index++) {
	i = (index + offset) % inven_ctr; /* be clever and not destroy the first item */
	if ((*typ) (&inventory[i]) && (randint(100) < perc)) {
	    objdes(tmp_str, &inventory[i], FALSE);
	    sprintf(out_val, "%sour %s (%c) %s destroyed!",
		    ((inventory[i].subval <= ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked single items */
		    ? "One of y" : "Y",
		    tmp_str, i + 'a',
		    ((inventory[i].subval > ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked group items */
		    ? "were" : "was");
	    msg_print(out_val);
	    inven_destroy(i);
	    j++;
	}
    }
    return (j);
}



/*
 * return FALSE if picking up an object would change the players speed */
int inven_check_weight(register inven_type *i_ptr)
{
    register int i, new_inven_weight;

    i = weight_limit();
    new_inven_weight = i_ptr->number * i_ptr->weight + inven_weight;
    if (i < new_inven_weight)
	i = new_inven_weight / (i + 1);
    else
	i = 0;

    if (pack_heavy != i)
	return FALSE;
    else
	return TRUE;
}


/*
 * Are we strong enough for the current pack and weapon?  -CJS-	 */
void check_strength()
{
    register int         i;
    register inven_type *i_ptr;
    static int           notlike = FALSE;

    i_ptr = &inventory[INVEN_WIELD];
    if (i_ptr->tval != TV_NOTHING
	&& (py.stats.use_stat[A_STR] * 15 < i_ptr->weight)) {
	if (weapon_heavy == FALSE) {
	    msg_print("You have trouble wielding such a heavy weapon.");
	    weapon_heavy = TRUE;
	    calc_bonuses();
	}
    } else if (weapon_heavy == TRUE) {
	weapon_heavy = FALSE;
	if (i_ptr->tval != TV_NOTHING)
	    msg_print("You are strong enough to wield your weapon.");
	else
	    msg_print("You feel relieved to put down your heavy weapon.");
	calc_bonuses();
    }
    i = weight_limit();
    if (i < inven_weight)
	i = inven_weight / (i + 1);
    else
	i = 0;
    if (pack_heavy != i) {
	if (pack_heavy < i)
	    msg_print("Your pack is so heavy that it slows you down.");
	else
	    msg_print("You move more easily under the weight of your pack.");
	change_speed(i - pack_heavy);
	pack_heavy = i;
    }
    py.flags.status &= ~PY_STR_WGT;

    if (py.misc.pclass == 2 && !notlike) {
        if ((i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
            && ((i_ptr->flags2 & TR_BLESS_BLADE) == 0)) {
            notlike = TRUE;
            msg_print("You do not feel comfortable with your weapon.");
        }
    } else if (py.misc.pclass == 2 && notlike) {
        if (i_ptr->tval == TV_NOTHING) {
            notlike = FALSE;
            msg_print("You feel comfortable again after removing that weapon.");
        } else if (!(i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
		   || !((i_ptr->flags2 & TR_BLESS_BLADE) == 0)) {
            notlike = FALSE;
            msg_print("You feel comfortable with your weapon once more.");
        }
    }
}


