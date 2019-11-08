/* File: store.c */

/* Purpose: store code, updating store inventory, pricing objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"
int is_home = FALSE;		/* Are we in our home? */





static cptr comment1[14] = {
    "Okay. ", "Fine. ", "Accepted! ", "Agreed! ", "Done! ", "Taken! ",
    "You drive a hard bargain, but taken. ",
    "You'll force me bankrupt, but it's a deal. ", "Sigh.  I'll take it. ",
    "My poor sick children may starve, but done! ", "Finally!  I accept. ",
    "Robbed again. ", "A pleasure to do business with you! ",
    "My spouse will skin me, but accepted. "
};

static cptr comment2a[3] = {
    "%A2 is my final offer; take it or leave it.",
    "I'll give you no more than %A2.",
    "My patience grows thin.  %A2 is final."
};

static cptr comment2b[16] = {
    "%A1 for such a fine item?  HA!  No less than %A2.",
    "%A1 is an insult!  Try %A2 gold pieces.",
    "%A1?!?  Go try Londis instead!",
    "Why, I'll take no less than %A2 gold pieces.",
    "Ha!  No less than %A2 gold pieces.",
    "Thou knave!  No less than %A2 gold pieces.",
    "%A1 is far too little, how about %A2?",
    "I paid more than %A1 for it myself, try %A2.",
    "%A1 my arse!  How about %A2 gold pieces?",
    "As scrap this would bring %A1.  Try %A2 in gold.",
    "May the fleas of 1000 orcs molest you.  I want %A2.",
    "My mother you can get for %A1, this costs %A2.",
    "May your most favourite parts go mouldy!  I want %A2 in gold!",
    "Sell this for such a pittance?  Give me %A2 gold.",
    "May Morgoth find you tasty!  %A2 gold pieces?",
    "Your mother was a Troll/Orc/Elf!  %A2 or I'll tell."
};

static cptr comment3a[3] = {
    "I'll pay no more than %A1; take it or leave it.",
    "You'll get no more than %A1 from me.",
    "%A1 and that's final."
};

static cptr comment3b[15] = {
    "%A2 for that piece of junk?  No more than %A1.",
    "For %A2 I could own ten of those.  Try %A1.",
    "%A2?  NEVER!  %A1 is more like it.",
    "Let's be reasonable...NOT! How about %A1 gold pieces?",
    "%A1 gold for that! That's you, that is!.",
    "%A1 gold pieces and be thankful for it!",
    "%A1 gold pieces and not a copper more.",
    "%A2 gold?  HA!  %A1 is more like it.", "Try about %A1 gold.",
    "I wouldn't pay %A2 for your bottom, try %A1.",
    "*CHOKE* For that!?  Let's say %A1.", "How about %A1?",
    "That looks war surplus!  Say %A1 gold.",
    "I'll buy it as scrap for %A1.",
    "%A2 is too much, let us say %A1 gold."
};

static cptr comment4a[5] = {
    "ENOUGH!  You have abused me once too often!",
    "THAT DOES IT!  You shall waste my time no more!",
    "This is getting nowhere.  I'm going to Londis!",
    "BAHAHAHAHAHA!  No more shall you insult me!",
    "Begone!  I have had enough abuse for one day."
};

static cptr comment4b[5] = {
    "Out of my place!", "out... Out... OUT!!!", "Come back tomorrow.",
    "Leave my place.  Begone!", "Come back when thou art richer."
};

static cptr comment5[10] = {
    "You will have to do better than that!", "That's an insult!",
    "Do you wish to do business or not?", "Hah!  Try again.",
    "Ridiculous!", "You've got to be kidding!", "You'd better be kidding!",
    "You try my patience.", "I don't hear you.",
    "Hmmm, nice weather we're having."
};

static cptr comment6[5] = {
    "I must have heard you wrong.", "What was that?",
    "I'm sorry, say that again.", "What did you say?",
    "Sorry, what was that again?"
};



/*
 * Comments vary, and can take parameters.	-RAK-
 *
 * %A1 is replaced by offer, %A2 by asking price.
 */


/*
 * Given a buffer, replace the first occurance of the string "target"
 * with the textual form of the long integer "number"
 */
static void insert_lnum(char *object_str,  cptr mtc_str, s32b number, int  show_sign)
{
    int            mlen;
    vtype          str1, str2;
    register char *string, *tmp_str;
    int            flag;

    flag = 1;
    mlen = strlen(mtc_str);
    tmp_str = object_str;
    do {
	string = (char *) strcmp(tmp_str, mtc_str[0]);
	if (string == 0)
	    flag = 0;
	else {
	    flag = strncmp(string, mtc_str, mlen);
	    if (flag)
		tmp_str = string + 1;
	}
    }
    while (flag);
    if (string) {
	(void)strncpy(str1, object_str, (int)(string - object_str));
	str1[(int)(string - object_str)] = '\0';
	(void)strcpy(str2, string + mlen);
	if ((number >= 0) && (show_sign))
	    (void)sprintf(object_str, "%s+%ld%s", str1, (long)number, str2);
	else
	    (void)sprintf(object_str, "%s%ld%s", str1, (long)number, str2);
    }
}


/*
 * Successful haggle.
 */
static void prt_comment1(void)
{
    msg_print(comment1[randint(14) - 1]);
}


/*
 * Continue haggling (player is buying)
 */
static void prt_comment2(s32b offer, s32b asking, int final)
{
    vtype comment;

    if (final > 0) {
	(void)strcpy(comment, comment2a[randint(3) - 1]);
    }
    else {
	(void)strcpy(comment, comment2b[randint(16) - 1]);
    }

    insert_lnum(comment, "%A1", offer, FALSE);
    insert_lnum(comment, "%A2", asking, FALSE);

    msg_print(comment);
}


/*
 * Continue haggling (player is selling)
 */
static void prt_comment3(s32b offer, s32b asking, int final)
{
    vtype comment;

    if (final > 0) {
	(void)strcpy(comment, comment3a[rand_int(3)]);
    }
    else {
	(void)strcpy(comment, comment3b[rand_int(15)]);
    }

    insert_lnum(comment, "%A1", offer, FALSE);
    insert_lnum(comment, "%A2", asking, FALSE);

    msg_print(comment);
}


/*
 * Kick 'da bum out.					-RAK-	 
 */
static void prt_comment4(void)
{
    msg_print(comment4a[rand_int(5)]);
    msg_print(comment4b[rand_int(5)]);
}


/*
 * You are insulting me
 */
static void prt_comment5(void)
{
    msg_print(comment5[rand_int(10)]);
}


/*
 * That makes no sense.
 */
static void prt_comment6(void)
{
    msg_print(comment6[rand_int(5)]);
}



store_type store[MAX_STORES];

/*
 * Store owners have different characteristics for pricing and haggling
 * Note: Store owners should be added in groups, one for each store    
 */

static owner_type owners[MAX_OWNERS] = {

{"Rincewind the Chicken  (Human)      General Store",
	  450,	175,  108,    4,  0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armoury",
	32000,	200,  112,    4,  5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith",
	10000,	185,  110,    5,  1,  8},
{"Ludwig the Humble      (Human)      Temple",
	 5000,	175,  109,    6,  0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist",
	12000,	220,  115,    4,  4,  9},
{"Luthien Starshine      (Elf)        Magic Shop",
	32000,	175,  110,    5,  2, 11},
{"Durwin the Shifty      (Human)      Black Market",
	32000,	250,  155,   10,  0,  5},
{"Your home",
	    1,    1,    1,    1, 1, 1},

{"Bilbo the Friendly     (Hobbit)     General Store",
	  300,	170,  108,    5,  3, 15},
{"Darg-Low the Grim      (Human)      Armoury",
	10000,	190,  111,    4,  0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith",
	32000,	195,  112,    4,  5,  8},
{"Gunnar the Paladin     (Human)      Temple",
	12000,	185,  110,    5,  0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist",
	10000,	190,  111,    5,  1,  8},
{"Buggerby the Great!    (Gnome)      Magic Shop",
	20000,	215,  113,    6,  4, 10},
{"Histor the Goblin      (Orc)        Black Market",
	32000,	250,  160,   10,  6,  5},
{"Your sweet abode",
	    1,    1,    1,    1, 1, 1},

{"Lyar-el the Comely     (Elf)        General Store",
	  600,	165,  107,    6,  2, 18},
{"Decado the Handsome    (Human)      Armoury",
	25000,  200,  112,    4,  5, 10},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith",
	 6000,	210,  115,    6,  7,  8},
{"Delilah the Pure       (Half-Elf)   Temple",
	25000,	180,  107,    6,  1, 20},
{"Wizzle the Chaotic     (Hobbit)     Alchemist",
	10000,	190,  110,    6,  3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000,	200,  110,    7,  0, 10},
{"Drago the Fair?        (Elf)        Black Market",
	32000,	250,  150,   10,  2,  5},
{"Your house",
	    1,    1,    1,    1, 1, 1}
};


/*
 * Buying and selling adjustments for character race VS store
 * owner race							
 */

static byte rgold_adj[MAX_RACES][MAX_RACES] = {

			/*Hum, HfE, Elf,  Hal, Gno, Dwa, HfO, HfT, Dun, HiE*/

/*Human		 */	 { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105},
/*Half-Elf	 */	 { 110, 100, 100, 105, 110, 120, 125, 130, 110, 100},
/*Elf		 */	 { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100},
/*Halfling	 */	 { 115, 110, 105,  95, 105, 110, 115, 130, 115, 105},
/*Gnome		 */	 { 115, 115, 110, 105,  95, 110, 115, 130, 115, 110},
/*Dwarf		 */	 { 115, 120, 120, 110, 110,  95, 125, 135, 115, 120},
/*Half-Orc	 */	 { 115, 120, 125, 115, 115, 130, 110, 115, 115, 125},
/*Half-Troll	 */	 { 110, 115, 115, 110, 110, 130, 110, 110, 110, 115},
/*Dunedain 	 */	 { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105},
/*High_Elf	 */	 { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100}

};



/*
 * Return the value of an item. -RAK-
 */
s32b item_value(inven_type *i_ptr)
{
    s32b value;

    /* Start with the item's known base cost */
    value = i_ptr->cost;

    /* don't purchase known cursed items */
    if (i_ptr->ident & ID_DAMD) value = 0;

	/* Weapons and armor	 */
    else if (((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) ||
	     ((i_ptr->tval >= TV_BOOTS) && (i_ptr->tval <= TV_SOFT_ARMOR))) {

	if (!known2_p(i_ptr)) value = k_list[i_ptr->k_idx].cost;

	else if ((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) {
	    if (i_ptr->tohit < 0) value = 0;
	    else if (i_ptr->todam < 0) value = 0;
	    else if (i_ptr->toac < 0) value = 0;
	    else value = i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 100;
	}

	else {
	    if (i_ptr->toac < 0) value = 0;
	    else value = i_ptr->cost + i_ptr->toac * 100;
	}
    }

	else if (((i_ptr->tval >= TV_SHOT) && (i_ptr->tval <= TV_ARROW))
	       || (i_ptr->tval == TV_SPIKE)) {	/* Ammo			 */
	if (!known2_p(i_ptr)) value = k_list[i_ptr->k_idx].cost;

	else {
	    if (i_ptr->tohit < 0) value = 0;
	    else if (i_ptr->todam < 0) value = 0;
	    else if (i_ptr->toac < 0) value = 0;
	    else

	    /* use 5, because missiles generally appear in groups of 20, so
	     * 20 * 5 == 100, which is comparable to weapon bonus above */
		value = i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 5;
	}

				/* Potions, Scrolls, and Food */
    } else if ((i_ptr->tval == TV_SCROLL1) || (i_ptr->tval == TV_SCROLL2) ||
	       (i_ptr->tval == TV_POTION1) || (i_ptr->tval == TV_POTION2)) {
	if (!inven_aware_p(i_ptr)) value = 20;

    } else if (i_ptr->tval == TV_FOOD) {
	if ((i_ptr->sval < (ITEM_SINGLE_STACK_MIN + MAX_SHROOM))
	    && !inven_aware_p(i_ptr)) value = 1;

				/* Rings and amulets */
    } else if ((i_ptr->tval == TV_AMULET) || (i_ptr->tval == TV_RING)) {
	if (!inven_aware_p(i_ptr))
	/* player does not know what type of ring/amulet this is */
	    value = 45;

	else if (!known2_p(i_ptr))
	/* player knows what type of ring, but does not know whether it is
	 * cursed or not, if refuse to buy cursed objects here, then player
	 * can use this to 'identify' cursed objects  */
	    value = k_list[i_ptr->k_idx].cost;

				/* Wands and staffs */
    } else if ((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND)) {
	if (!inven_aware_p(i_ptr)) {

	    if (i_ptr->tval == TV_WAND) value = 50;
	    else value = 70;
	} else if (known2_p(i_ptr))
	    value = i_ptr->cost + (i_ptr->cost / 20) * i_ptr->pval;
    }

				/* picks and shovels */
    else if (i_ptr->tval == TV_DIGGING) {
	if (!known2_p(i_ptr))
	    value = k_list[i_ptr->k_idx].cost;

	else {
	    if (i_ptr->pval < 0) value = 0;
	    else {

	    /* some digging tools start with non-zero pval values, so only
	     * multiply the plusses by 100, make sure result is positive 
	     * no longer; have adjusted costs in treasure.c -CWS
	     */
		value = i_ptr->cost + i_ptr->pval;
		if (value < 0)
		    value = 0;
	    }
	}
    }
/* multiply value by number of items if it is a group stack item */
    if (i_ptr->sval > ITEM_GROUP_MIN)	/* do not include torches here */
	value = value * i_ptr->number;
    return (value);
}



/*
 * Asking price for an item			-RAK-
 */
static s32b sell_price(int snum, s32b *max_sell, s32b *min_sell, inven_type *i_ptr)
{
    register s32b      i;
    register store_type *s_ptr;

    s_ptr = &store[snum];
    i = item_value(i_ptr);

    /* check i_ptr->cost in case it is cursed, check i in case it is damaged */
    if ((i_ptr->cost <= 0) || (i <= 0)) return (0);

    /* Get the "basic value" */
    i = i * rgold_adj[owners[s_ptr->owner].owner_race][p_ptr->prace] / 100L;

    /* Nothing becomes free */
    if (i < 1) i = 1;

    /* Extract min/max sell values */
    *max_sell = i * owners[s_ptr->owner].max_inflate / 100L;
    *min_sell = i * owners[s_ptr->owner].min_inflate / 100L;

    /* Black market is always over-priced */
    if (snum == 6) {
	(*max_sell) *= 2;
	(*min_sell) *= 2;
    }

    /* Paranoia */
    if (*min_sell > *max_sell) *min_sell = *max_sell;

    /* Return the price */
    return (i);
}




/*
 * Check to see if the shop will be carrying too many objects	-RAK-	 
 */
static int store_check_num(inven_type *t_ptr, int store_num)
{
    register int        store_check, i;
    register store_type *st_ptr;
    register inven_type *j_ptr;

    store_check = FALSE;
    st_ptr = &store[store_num];

    /* Free space is always usable */
    if (st_ptr->store_ctr < STORE_INVEN_MAX) return TRUE;

    /* Check all the items */
    else if (t_ptr->sval >= ITEM_SINGLE_STACK_MIN)
    for (i = 0; i < st_ptr->store_ctr; i++) {

	/* Get the existing item */
	j_ptr = &st_ptr->store_item[i];

	/* note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match */
	    if (j_ptr->tval == t_ptr->tval && j_ptr->sval == t_ptr->subval
		&& ((int)j_ptr->number + (int)t_ptr->number < 256)
		&& (t_ptr->sval < ITEM_GROUP_MIN
		    || (j_ptr->pval == t_ptr->pval)))
		store_check = TRUE;
	}

/* But, wait.  If at home, don't let player drop 25th item, or he will lose it. -CFT */
    if (is_home && (t_ptr->sval >= ITEM_SINGLE_STACK_MIN))
	for (i = 0; i < st_ptr->store_ctr; i++) {
	    j_ptr = &st_ptr->store_item[i];
	/* note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match */
	    if (j_ptr->tval == t_ptr->tval && j_ptr->sval == t_ptr->subval
		&& ((int)j_ptr->number + (int)t_ptr->number > 24)
		&& (t_ptr->sval < ITEM_GROUP_MIN
		    || (j_ptr->pval == t_ptr->pval)))
		store_check = FALSE;
	}
    return (store_check);
}


/*
 * Add the item in INVEN_MAX to stores inventory.	-RAK-	 
 */
static void store_carry(int store_num, int *ipos, inven_type *t_ptr)
{
    int                 item_num, item_val, flag;
    register int        typ, subt;
    s32b               icost, dummy;
    register inven_type *i_ptr;
    register store_type *s_ptr;
    int stacked = FALSE; /* from inven_carry() -CFT */

    *ipos = -1;
    if (sell_price(store_num, &icost, &dummy, t_ptr) > 0 || is_home)
    {
	s_ptr = &store[store_num];
	item_val = 0;
	item_num = t_ptr->number;
	flag = FALSE;
	typ  = t_ptr->tval;
	subt = t_ptr->sval;
	if (subt >= ITEM_SINGLE_STACK_MIN) { /* try to stack in store's inven */
	    do {
		i_ptr = &s_ptr->store_item[item_val];
		if (typ == i_ptr->tval)
		{
		    if (subt == i_ptr->sval && /* Adds to other item        */
			subt >= ITEM_SINGLE_STACK_MIN
			&& (subt < ITEM_GROUP_MIN || i_ptr->pval == t_ptr->pval))
		    {
			stacked = TRUE; /* remember that we did stack it... -CFT */
			*ipos = item_val;
			i_ptr->number += item_num;
			/* must set new scost for group items, do this only for items
			   strictly greater than group_min, not for torches, this
			   must be recalculated for entire group */
			if (subt > ITEM_GROUP_MIN)
			{
			    (void) sell_price (store_num, &icost, &dummy, i_ptr);
			    s_ptr->store_item[item_val].scost = -icost;
			}
			/* must let group objects (except torches) stack over 24
			   since there may be more than 24 in the group */
			else if (i_ptr->number > 24)
			    i_ptr->number = 24;
			flag = TRUE;
		    }
		}
		item_val ++;
	    } while (!stacked && (item_val < s_ptr->store_ctr));
	} /* if might stack... -CFT */
	if (!stacked) {		/* either never stacks, or didn't find a place to stack */
	    item_val = 0;
	    do {
		i_ptr = &s_ptr->store_item[item_val];
		if ((typ > i_ptr->tval) || /* sort by desc tval, */
		    ((typ == i_ptr->tval) &&
		     ((t_ptr->level < i_ptr->level) || /* then by inc level, */
		      ((t_ptr->level == i_ptr->level) &&
		       (subt < i_ptr->sval))))) /* and finally by inc subval -CFT */
		{		/* Insert into list             */
		    insert_store(store_num, item_val, icost, t_ptr);
		    flag = TRUE;
		    *ipos = item_val;
		}
		item_val++;
	    } while ((item_val < s_ptr->store_ctr) && (!flag));
	} /* if didn't already stack it... */
	if (!flag)		/* Becomes last item in list    */
	{
	    insert_store(store_num, (int)s_ptr->store_ctr, icost, t_ptr);
	    *ipos = s_ptr->store_ctr - 1;
	}
    }
}



/*
 * Destroy an item in the stores inventory.  Note that if	 
 * "one_of" is false, an entire slot is destroyed	-RAK-	 
 */
static void store_destroy(int store_num, int item_val, int one_of)
{
    register int         j, number;
    register store_type *s_ptr;
    register inven_type *i_ptr;

    s_ptr = &store[store_num];
    i_ptr = &s_ptr->store_item[item_val];

/* for single stackable objects, only destroy one half on average, this will
 * help ensure that general store and alchemist have reasonable selection of
 * objects 
 */
    if ((i_ptr->sval >= ITEM_SINGLE_STACK_MIN) &&
	(i_ptr->sval <= ITEM_SINGLE_STACK_MAX)) {
	if (one_of)
	    number = 1;
	else
	    number = randint((int)i_ptr->number);
    } else
	number = i_ptr->number;

    if (number != i_ptr->number)
	i_ptr->number -= number;
    else {
	for (j = item_val; j < s_ptr->store_ctr - 1; j++)
	    s_ptr->store_item[j] = s_ptr->store_item[j + 1];
	invcopy(&s_ptr->store_item[s_ptr->store_ctr - 1], OBJ_NOTHING);
	s_ptr->store_item[s_ptr->store_ctr - 1].scost = 0;
	s_ptr->store_ctr--;
    }
}


/* 
 * Creates an item and inserts it into store's inven	-RAK-	
 */
static void store_create(int store_num)
{
    register int         i, tries;
    int                  cur_pos, dummy;
    register store_type *s_ptr;
    register inven_type *t_ptr;

    tries = 0;
    cur_pos = i_pop();
    s_ptr = &store[store_num];
    object_level = OBJ_TOWN_LEVEL;
    do {
	if (store_num != 6) {
	    i = store_choice[store_num][randint(STORE_CHOICES) - 1];
	    invcopy(&i_list[cur_pos], i);
	    magic_treasure(cur_pos, OBJ_TOWN_LEVEL, FALSE, TRUE);
	    t_ptr = &i_list[cur_pos];
	    if (store_check_num(t_ptr, store_num)) {
		if ((t_ptr->cost > 0) &&	/* Item must be good	 */
		    (t_ptr->cost < owners[s_ptr->owner].max_cost)) {
		    store_bought(t_ptr);
		    special_offer(t_ptr);
		    store_carry(store_num, &dummy, t_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	} else {
	    i = get_obj_num(40, FALSE);
	    invcopy(&i_list[cur_pos], i);
	    magic_treasure(cur_pos, 40, FALSE, TRUE);
	    t_ptr = &i_list[cur_pos];
	    if (store_check_num(t_ptr, store_num)) {
		if (t_ptr->cost > 0) {	/* Item must be good	 */
		    store_bought(t_ptr);
		    special_offer(t_ptr);
		    store_carry(store_num, &dummy, t_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	}
    }
    while (tries <= 3);
    pusht(cur_pos);
}



/*
 * eliminate need to bargain if player has haggled well in the past   -DJB-
 */
static int noneedtobargain(int store_num, s32b minprice)
{
    register int         flagnoneed;
    register store_type *s_ptr;

    /* Allow haggling to be turned off */
    if (no_haggle_flag) return (TRUE);

    s_ptr = &store[store_num];

    /* XXX Prevent "unsigned" bizarreness */
    good = st_ptr->good_buy;
    bad = st_ptr->bad_buy;

    flagnoneed = ((good == MAX_SHORT)
		  || ((good - 3 * bad) > (5 + (minprice/50))));

    /* Return the flag */
    return (flagnoneed);
}


/*
 * update the bargain info					-DJB- 
 */
void updatebargain(int store_num, s32b price, s32b minprice)
{
    register store_type *st_ptr;

    st_ptr = &store[store_num];

    /* Ignore cheap items */
    if (minprice < 10) return;

    /* Count the successful haggles */
    if (price == minprice) {
	if (st_ptr->good_buy < MAX_SHORT) {
	    st_ptr->good_buy++;
	}
    }

    /* Count the failed haggles */
    else {
	if (st_ptr->bad_buy < MAX_SHORT) {
	    st_ptr->bad_buy++;
	}
    }
}



/*
 * Displays the set of commands -RAK-
 */
static void display_commands(void)
{
    /* Display the legal commands */
    prt("You may:", 20, 0);
    if (is_home) {
	prt(" g) Get an item.               b) Browse through your home.", 21, 0);
	prt(" d) Drop an item.              i/e/t/w/x) Inventory/Equipment Lists.",
	    22, 0);
	prt("ESC) Exit from your home.      ^R) Redraw the screen.", 23, 0);
    } else {
	prt(" p) Purchase an item.          b) Browse store's inventory.", 21, 0);
	prt(" s) Sell an item.              i/e/t/w/x) Inventory/Equipment Lists.",
	    22, 0);
	prt("ESC) Exit from Building.       ^R) Redraw the screen.", 23, 0);
    }
}


/*
 * Displays the set of commands				-RAK-	 
 */
static void haggle_commands(int typ)
{
    if (typ == -1) {
	prt("Specify an asking-price in gold pieces.", 21, 0);
    }
    else {
	prt("Specify an offer in gold pieces.", 21, 0);
    }

    prt("ESC) Quit Haggling.", 22, 0);
    erase_line(23, 0);		   /* clear last line */
}



/*
 * Displays a store's inventory				-RAK-
 */
static void display_inventory(int store_num, int start)
{
    register store_type *s_ptr;
    register inven_type *i_ptr;
    register int         i, j, stop;
    bigvtype             out_val1, out_val2;
    s32b                x;

    s_ptr = &store[store_num];
    i = (start % 12);
    stop = ((start / 12) + 1) * 12;
    if (stop > s_ptr->store_ctr)
	stop = s_ptr->store_ctr;
    while (start < stop) {
	i_ptr = &s_ptr->store_item[start];
	x = i_ptr->number;
	if (!is_home) {
	    if ((i_ptr->sval >= ITEM_SINGLE_STACK_MIN)
		&& (i_ptr->sval <= ITEM_SINGLE_STACK_MAX))
		i_ptr->number = 1;
	}
	objdes(out_val1, i_ptr, TRUE);
	i_ptr->number = x;
	(void)sprintf(out_val2, "%c) %s", 'a' + i, out_val1);
	prt(out_val2, i + 5, 0);
	if (!is_home) {
	    x = s_ptr->store_item[start].scost;
	    if (x < 0) {
		s32b               value = (s32b)(-x);
		
		value = value * chr_adj() / 100;
		if (value <= 0)
		    value = 1;
		(void)sprintf(out_val2, "%9ld", (long) value);
	    } else {
		(void)sprintf(out_val2, "%9ld [Fixed]", (long) x);
	    }
	    prt(out_val2, i + 5, 59);
	}
	i++;
	start++;
    }
    if (i < 12)
	for (j = 0; j < (11 - i + 1); j++)
	    erase_line(j + i + 5, 0);	/* clear remaining lines */
    if (s_ptr->store_ctr > 12)
	put_str("- cont. -", 17, 60);
    else
	erase_line(17, 60);
}


/*
 * Re-displays only a single cost			-RAK-	
 */
static void display_cost(int store_num, int pos)
{
    register int         i;
    register s32b       j;
    vtype                out_val;
    register store_type *s_ptr;

    s_ptr = &store[store_num];
    i = (pos % 12);
    if (s_ptr->store_item[pos].scost < 0) {
	j = (- (s_ptr->store_item[pos]).scost);
	j = j * chr_adj() / 100;
	(void)sprintf(out_val, "%ld", (long) j);
    } else
	(void)sprintf(out_val, "%9ld [Fixed]",
		      (long) (s_ptr->store_item[pos]).scost);
    prt(out_val, i + 5, 59);
}


/*
 * Displays players gold					-RAK-	 
 */
static void store_prt_gold(void)
{
    vtype out_val;

    (void)sprintf(out_val, "Gold Remaining : %ld", (long)p_ptr->au);
    prt(out_val, 18, 17);
}


/*
 * Displays store (after clearing screen)		-RAK-	 
 */
static void display_store(int store_num, int cur_top)
{
    register store_type *s_ptr = &store[store_num];

    /* Erase the screen */
    clear_screen();

    put_str(owners[s_ptr->owner].owner_name, 3, 9);
    put_str("Item", 4, 3);
    if (!is_home) {
	put_str("Asking Price", 4, 60);
	store_prt_gold();
    }
    display_commands();
    display_inventory(store_num, cur_top);
}



/*
 * Get the ID of a store item and return it's value	-RAK-	 
 */
static int get_store_item(int *com_val, cptr pmt, int i, int j)
{
    char         command;
    vtype        out_val;

    *com_val = (-1);

    /* Build the prompt */
    (void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s",
		  i + 'a', j + 'a', pmt);

    /* Ask until done */
    while (TRUE) {

	/* Escape */
	if (!get_com(out_val, &command)) break;

	/* Legal responses */
	if (command >= i+'a' && command <= j+'a') {
	    *com_val = command - 'a';
	    break;
	}

	/* Oops */
	bell();
    }

    erase_line(MSG_LINE, 0);

    return (command != ESCAPE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-	 
 */
static int increase_insults(int store_num)
{
    store_type *st_ptr;
    st_ptr = &store[store_num];

    st_ptr->insult_cur++;

    if (st_ptr->insult_cur > owners[st_ptr->owner].insult_max) {
	prt_comment4();
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;
	st_ptr->store_open = turn + 2500 + randint(2500);
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Decrease insults					-RAK-	 
 */
static void decrease_insults(int store_num)
{
    register store_type *st_ptr;
    st_ptr = &store[store_num];

    if (st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling				-RAK-	 
 */
static int haggle_insults(int store_num)
{
    /* Increase insults */
    if (increase_insults(store_num)) return (TRUE);

    /* Display and flush insult */
    prt_comment5();
    msg_print(NULL);

    /* Still okay */
    return (FALSE);
}


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer,
		      int num, s32b price, int final)
{
    static s32b        last_inc = 0L;

    register s32b      i;
    register int        flag, clen;
    register char      *p;
    int                 inc = FALSE;
    vtype               out_val;
    char                buf[128];

    flag = TRUE;
    if (last_inc && !final) {
	(void)sprintf(buf, "%s [%c%ld] ", pmt, (last_inc < 0) ? '-' : '+',
			(last_inc < 0) ? (long)-last_inc : (long)last_inc);
    }
    else {
	(void)sprintf(buf, "%s [accept] ", pmt);
    }

    clen = strlen(buf);
    /* Keep asking until player quits, or answers */
    for (i = 0; flag && !i; ) {

	/* Prompt for a string, handle abort */
	prt(buf, 0, 0);
	if (!get_string(out_val, 0, clen, 40)) {
	    flag = FALSE, last_inc = FALSE;
	}

	i = atol(out_val);
    /*
     * don't allow incremental haggling, if player has not made an offer yet 
     */

	/* Skip leading spaces */
	for (p = out_val; *p == ' '; p++);

	/* Check for "incremental" */
	/* incremental haggling must be initiated */
	if (flag && num == 0 && (*p == '+' || *p == '-')) {
	    msg_print("You haven't even made your first offer yet!");
	    i = 0;
	}

	if (last_inc && !i && !final) i = last_inc, inc = TRUE;

	else if (!i) i = price;
    }

    if (flag) {
	for (p = out_val; *p == ' '; p++);
	if (*p == '+' || *p == '-')  *poffer += i, last_inc = i;
	else if (inc) *poffer += i, last_inc = i;
	else *poffer = i, last_inc = 0;
    } else erase_line(0, 0);
    return (flag);
}


/*
 * Receive an offer (from the player)
 */
static int receive_offer(int store_num, cptr pmt, s32b *poffer,
			 s32b last_offer, int num, int factor,
			 s32b price, int final)
{
    register int flag, receive;

    receive = 0;

    for (flag = FALSE; !flag; ) {
	if (get_haggle(pmt, poffer, num, price, final)) {
	    if ((*poffer) * factor >= last_offer * factor) {
		flag = TRUE;
	    }
	    else if (haggle_insults(store_num)) {
		receive = 2;
		flag = TRUE;
	    }
	    else {
		/* offer rejected, reset offer so that */
		/* incremental haggling works correctly */
		(*poffer) = last_offer;
	    }
	}
	else {
	    receive = 1;
	    flag = TRUE;
	}
    }

    return (receive);
}


/*
 * Haggling routine					-RAK-	 
 */
static int purchase_haggle(int store_num, s32b *price, inven_type *i_ptr)
{
    s32b               max_sell, min_sell, max_buy;
    s32b               cost, cur_ask, final_ask, min_offer;
    s32b               last_offer, offer;
    s32b               x1, x2, x3;
    s32b               min_per, max_per;
    register int        flag, loop_flag;
    const char         *pmt;
    vtype               out_val;
    int                 purchase, num, final_flag, final = FALSE;
    register store_type *s_ptr;
    register owner_type *o_ptr;

    purchase = 0;
    *price = 0;
    final_flag = 0;
    s_ptr = &store[store_num];
    o_ptr = &owners[s_ptr->owner];

    /* Determine the cost of the group of items */
    cost = sell_price(store_num, &max_sell, &min_sell, i_ptr);

    max_sell = max_sell * chr_adj() / 100;
    if (max_sell <= 0) max_sell = 1;

    min_sell = min_sell * chr_adj() / 100;
    if (min_sell <= 0) min_sell = 1;

    /* XXX This appears to be a hack.  See sell_price(). */
    /* cast max_inflate to signed so that subtraction works correctly */
    max_buy = cost * (200 - (int)o_ptr->max_inflate) / 100;
    if (max_buy <= 0) max_buy = 1;

    min_per = o_ptr->haggle_per;
    max_per = min_per * 3;

    haggle_commands(1);

    cur_ask = max_sell;
    final_ask = min_sell;
    min_offer = max_buy;
    last_offer = min_offer;
    offer = 0;

    /* this prevents incremental haggling on first try */
    num = 0;
    pmt = "Asking";

    /* Go right to final price if player has bargained well */
    if (noneedtobargain(store_num, final_ask)) {

	msg_print("After a long bargaining session, you agree upon the price.");
	msg_print(NULL);

	if (no_haggle_flag && min_sell > 10) {
	    min_sell *= 1.10;	/* not haggling penalty -CWS */
	}

	cur_ask = min_sell;

    }

    for (flag = FALSE; !flag; ) {

	do {
	    loop_flag = TRUE;
	    (void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
	    put_str(out_val, 1, 0);
	    purchase = receive_offer(store_num, "What do you offer? ",
				     &offer, last_offer, num,
				     1, cur_ask, final);
	    if (purchase != 0) {
		flag = TRUE;
	    }
	    else {
		if (offer > cur_ask) {
		    prt_comment6();
		    /* rejected, reset offer for incremental haggling */
		    offer = last_offer;
		}
		else if (offer == cur_ask) {
		    flag = TRUE;
		    *price = offer;
		}
		else {
		    loop_flag = FALSE;
		}
	    }
	}
	while (!flag && loop_flag);

	if (!flag) {
	    x1 = (offer - last_offer) * 100 / (cur_ask - last_offer);
	    if (x1 < min_per) {
		flag = haggle_insults(store_num);
		if (flag) purchase = 2;
	    }
	    else if (x1 > max_per) {
		x1 = x1 * 3 / 4;
		if (x1 < max_per) x1 = max_per;
	    }
	    x2 = x1 + randint(5) - 3;
	    x3 = ((cur_ask - offer) * x2 / 100L) + 1;
	    /* don't let the price go up */
	    if (x3 < 0) x3 = 0;
	    cur_ask -= x3;

	    if (cur_ask < final_ask) {
		final = TRUE;
		cur_ask = final_ask;
		pmt = "Final Offer";
		final_flag++;
		if (final_flag > 3) {
		    if (increase_insults(store_num)) purchase = 2;
		    else purchase = 1;
		    flag = TRUE;
		}
	    }
	    else if (offer >= cur_ask) {
		flag = TRUE;
		*price = offer;
	    }
	    if (!flag) {
		last_offer = offer;
		num++;	   /* enable incremental haggling */
		erase_line(1, 0);
		(void)sprintf(out_val, "Your last offer: %ld",
			      (long)last_offer);
		put_str(out_val, 1, 39);
		prt_comment2(last_offer, cur_ask, final_flag);
	    }
	}
    }

    /* update bargaining info */
    if (purchase == 0) updatebargain(store_num, *price, final_ask);

    return (purchase);
}


/*
 * Haggling routine					-RAK-	 
 */
static int sell_haggle(int store_num, s32b price, inven_type *i_ptr)
{
    s32b               max_sell = 0, max_buy = 0, min_buy = 0;
    s32b               cost = 0, cur_ask = 0, final_ask = 0, min_offer = 0;
    s32b               last_offer = 0, offer = 0;
    s32b               max_gold = 0;
    s32b               x1, x2, x3, tmp;
    s32b               min_per, max_per;
    register int        flag, loop_flag;
    const char          *pmt;
    int                 sell, num, final_flag, final = FALSE;
    vtype               out_val;

    register store_type *st_ptr;
    register owner_type *ot_ptr;

    st_ptr = &store[store_num];
	ot_ptr = &owners[st_ptr->owner];

    /* Get the value of the group of items */
    cost = item_value(i_ptr);

    sell = 0;
    *price = 0;
    final_flag = 0;

    /* Instantly react to worthless items */
    if (cost <= 0) return (3);

    /* XXX See "sell_price()" */

    cost = cost * (200L - chr_adj()) / 100L;

    tmp = (200L - rgold_adj[ot_ptr->owner_race][p_ptr->prace]);
    cost = cost * tmp / 100L;

    if (cost < 1) cost = 1;
    max_sell = cost * ot_ptr->max_inflate / 100L;

    /* cast max_inflate to signed so that subtraction works correctly */
    max_buy = cost * (200L - (s32b)(ot_ptr->max_inflate)) / 100L;
    if (max_buy < 1) max_buy = 1;

    min_buy = cost * (200L - (s32b)(ot_ptr->min_inflate)) / 100L;
    if (min_buy < 1) min_buy = 1;

    if (min_buy < max_buy) min_buy = max_buy;
    min_per = ot_ptr->haggle_per;
    max_per = min_per * 3;
    max_gold = ot_ptr->max_cost;

    haggle_commands(-1);

    if (max_buy > max_gold) {
	final_flag = 1;
	final = TRUE;
	pmt = "Final Offer";
	cur_ask = max_gold;
	final_ask = max_gold;
	msg_print("I am sorry, but I have not the money to afford such a fine item.");
    }
    else {
	cur_ask = max_buy;
	final_ask = min_buy;
	if (final_ask > max_gold) final_ask = max_gold;
	pmt = "Offer";

	/* go right to final price if player has bargained well */
	if (noneedtobargain(store_num, final_ask)) {

	    msg_print("After a long bargaining session, you agree upon the price.");
	    msg_print(NULL);

	    /* Sales tax.  10% if you have cancelled haggling */
	    if (no_haggle_flag && final_ask > 10)
		final_ask *= .90; /* not haggling penalty -CWS */

		cur_ask = final_ask;
	    }
	}

    min_offer = max_sell;
    last_offer = min_offer;
    offer = 0;

    /* this prevents incremental haggling on first try */
    num = 0;

    if (cur_ask < 1) cur_ask = 1;

    for (flag = FALSE; !flag; ) {

	do {
	    loop_flag = TRUE;
	    (void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
	    put_str(out_val, 1, 0);
	    sell = receive_offer(store_num, "What price do you ask? ",
				 &offer, last_offer, num,
				 -1, cur_ask, final);
	    if (sell != 0) {
		flag = TRUE;
	    }
	    else {
		if (offer < cur_ask) {
		    prt_comment6();
		    /* rejected, reset offer for incremental haggling */
		    offer = last_offer;
		}
		else if (offer == cur_ask) {
		    flag = TRUE;
		    *price = offer;
		}
		else {
		    loop_flag = FALSE;
		}
	    }
	}
	while (!flag && loop_flag);

	if (!flag) {
	    x1 = (last_offer - offer) * 100 / (last_offer - cur_ask);
	    if (x1 < min_per) {
		flag = haggle_insults(store_num);
		if (flag) sell = 2;
	    }
	    else if (x1 > max_per) {
		x1 = x1 * 3 / 4;
		if (x1 < max_per) x1 = max_per;
	    }
	    x2 = x1 + randint(5) - 3;
	    x3 = ((offer - cur_ask) * x2 / 100L) + 1;
	    /* don't let the price go down */
	    if (x3 < 0) x3 = 0;
	    cur_ask += x3;
	    if (cur_ask > final_ask) {
		cur_ask = final_ask;
		final = TRUE;
		pmt = "Final Offer";
		final_flag++;
		if (final_flag > 3) {
		    flag = TRUE;
		    sell = 1;
		    if (increase_insults(store_num)) sell = 2;
		}
	    }
	    else if (offer <= cur_ask) {
		flag = TRUE;
		*price = offer;
	    }
	    if (!flag) {
		last_offer = offer;
		num++;   /* enable incremental haggling */
		erase_line(1, 0);
		(void)sprintf(out_val,
			     "Your last bid %ld", (long)last_offer);
		put_str(out_val, 1, 39);
		prt_comment3(cur_ask, last_offer, final_flag);
	    }
	}
    }

    /* update bargaining info */
    if (sell == 0) updatebargain(store_num, *price, final_ask);

    return (sell);
}





/*
 * Buy an item from a store				-RAK-	 
 */
static int store_purchase(int store_num, int *cur_top)
{
    s32b               price;
    register int        i, choice;
    bigvtype            out_val, tmp_str;
    register store_type *st_ptr;
    inven_type          sell_obj;
    register inven_type *i_ptr;
    int                 item_val, item_new, purchase;

    purchase = FALSE;
    st_ptr = &store[store_num];

    /* i == number of objects shown on screen	 */
    if (*cur_top == 12)
	i = st_ptr->store_ctr - 1 - 12;
    else if (st_ptr->store_ctr > 11)
	i = 11;
    else
	i = st_ptr->store_ctr - 1;

    /* Empty? */
    if (st_ptr->store_ctr <= 0) {
	if (is_home) msg_print("Your home is empty.");
	else msg_print("I am currently out of stock.");
	return (FALSE);
    }

    /* Prompt */
    /* Get the item number to be bought */
    else if (!get_store_item(&item_val,
			    is_home ? "Which item do you want to take? " :
			    "Which item are you interested in? ", 0, i)) return (FALSE);

    /* Get the actual index */
    item_val = item_val + *cur_top;

	take_one_item(&sell_obj, &st_ptr->store_item[item_val]);

    /* Hack -- require room in pack */
    if (!inven_check_num(&sell_obj)) {
	prt("You cannot carry that many different items.", 0, 0);
	return (FALSE);
    }

	    if (!is_home) {
		if (st_ptr->store_item[item_val].scost > 0) {
		    price = st_ptr->store_item[item_val].scost;
		    choice = 0;
		} else
		    choice = purchase_haggle(store_num, &price, &sell_obj);
		if (choice == 0) {
		    if (p_ptr->au >= price) {
			prt_comment1();
			decrease_insults(store_num);
			p_ptr->au -= price;
			item_new = inven_carry(&sell_obj);
			i = st_ptr->store_ctr;
			store_destroy(store_num, item_val, TRUE);
			inventory[item_new].inscrip[0] = 0;
			objdes(tmp_str, &inventory[item_new], TRUE);
			(void)sprintf(out_val, "You have %s (%c)",
				      tmp_str, item_new + 'a');
			prt(out_val, 0, 0);
			check_strength();
			if (*cur_top >= st_ptr->store_ctr) {
			    *cur_top = 0;
			    display_inventory(store_num, *cur_top);
			} else {
			    i_ptr = &st_ptr->store_item[item_val];
			    if (i == st_ptr->store_ctr) {
				if (i_ptr->scost < 0) {
				    i_ptr->scost = price;
				    display_cost(store_num, item_val);
				}
			    } else
				display_inventory(store_num, item_val);
			    store_prt_gold();
			}
		    } else {
			if (increase_insults(store_num))
			    purchase = TRUE;
			else {
			    prt_comment1();
			    msg_print("Liar!  You have not the gold!");
			}
		    }
		} else if (choice == 2)
		    purchase = TRUE;
	    } else {		   /* is_home... */
		item_new = inven_carry(&sell_obj);
		i = st_ptr->store_ctr;
		store_destroy(store_num, item_val, TRUE);
		objdes(tmp_str, &inventory[item_new], TRUE);
		(void)sprintf(out_val, "You have %s (%c)",
			      tmp_str, item_new + 'a');
		prt(out_val, 0, 0);

		check_strength();
		if (*cur_top >= st_ptr->store_ctr) {
		    *cur_top = 0;
		    display_inventory(store_num, *cur_top);
		} else {
		    i_ptr = &st
		    display_inventory(store_num, item_val);
		}
	    }
	/* Less intuitive, but looks better here than in purchase_haggle. */
	    display_commands();
	    erase_line(1, 0);
	}

    /* Return the result */
    return (purchase);
}


/*
 * Sell an item to the store	-RAK-	 
 */
static int store_sell(int store_num, int *cur_top)
{

    int			sell, choice;
    int                 item_val, item_pos;
    int			test;
    s32b               price;

    inven_type          sold_obj;
    bigvtype            out_val, tmp_str;

    sell = FALSE;

    for (item_val = 0, test = FALSE; (!test && (item_val < inven_ctr)); item_val++)
	test = (*store_buy[store_num]) (inventory[item_val].tval);

    /* Check for stuff */
    if (inven_ctr < 1) {
	msg_print("You aren't carrying anything.");
	return (FALSE);
    }

    /* Check for stuff */
    if (!test)
	msg_print("You have nothing that I want.");
	return (FALSE);
    }

    if (get_item(&item_val, "Which one? ", 0,
		      inven_ctr - 1, (store_buy[store_num]))) {
	take_one_item(&sold_obj, &inventory[item_val]);
	objdes(tmp_str, &sold_obj, TRUE);
	if (!is_home) {
	    (void)sprintf(out_val, "Selling %s (%c)", tmp_str, item_val + 'a');
	    msg_print(out_val);
	}
	if ((*store_buy[store_num]) (sold_obj.tval))
	    if (store_check_num(&sold_obj, store_num)) {
		if (!is_home) {
		    choice = sell_haggle(store_num, &price, &sold_obj);
		    if (choice == 0) {
			s32b               cost;
			s32b               real;

			prt_comment1();
			decrease_insults(store_num);
			p_ptr->au += price;

			cost = item_value(&sold_obj);

		    /* identify object in inventory to set aware */
			identify(&item_val);
		    /* retake sold_obj so that it will be identified */
			take_one_item(&sold_obj, &inventory[item_val]);
		    /* call known2 for store item, so charges/pluses are known */
			known2(&sold_obj);

			real = item_value(&sold_obj);

			inven_destroy(item_val);
			objdes(tmp_str, &sold_obj, TRUE);
			(void)sprintf(out_val, "You've sold %s. ", tmp_str);
			msg_print(out_val);

			if (real == 0) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear a shriek!");
				break;
			      case 2:
				msg_print("You bastard!");
				break;
			      case 3:
				msg_print("You hear sobs coming from the back of the store...");
				break;
			      case 4:
				msg_print("Arrgghh!!!!");
				break;
			    }
			} else if (real < cost) {
			    switch (randint(3)) {
			      case 1:
				msg_print("You hear someone swearing...");
				break;
			      case 2:
				msg_print("You hear mumbled curses...");
				break;
			      case 3:
				msg_print("The shopkeeper glares at you.");
				break;
			    }
			} else if (real > (4 * cost)) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear someone jumping for joy!");
				break;
			      case 2:
				msg_print("Yipee!");
				break;
			      case 3:
				msg_print("I think I'll retire!");
				break;
			      case 4:
				msg_print("The shopkeeper smiles gleefully!");
				break;
			    }
			} else if (real > cost) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear someone giggling");
				break;
			      case 2:
				msg_print("You've made my day!");
				break;
			      case 3:
				msg_print("What a fool!");
				break;
			      case 4:
				msg_print("The shopkeeper laughs loudly!");
				break;
			    }
			}
			store_carry(store_num, &item_pos, &sold_obj);
			check_strength();
			if (item_pos >= 0) {
			    if (item_pos < 12)
				if (*cur_top < 12)
				    display_inventory(store_num, item_pos);
				else {
				    *cur_top = 0;
				    display_inventory(store_num, *cur_top);
				}
			    else if (*cur_top > 11)
				display_inventory(store_num, item_pos);
			    else {
				*cur_top = 12;
				display_inventory(store_num, *cur_top);
			    }
			}
			store_prt_gold();
		    } else if (choice == 2)
			sell = TRUE;
		    else if (choice == 3) {
			msg_print("How dare you!");
			msg_print("I will not buy that!");
			sell = increase_insults(store_num);
		    }
		/* Less intuitive, but looks better here than in sell_haggle. */
		    erase_line(1, 0);
		    display_commands();
		} else {	   /* is_home... */
		/* retake sold_obj so that it will be identified */
		    take_one_item(&sold_obj, &inventory[item_val]);
		    inven_destroy(item_val);
		    objdes(tmp_str, &sold_obj, TRUE);
		    (void)sprintf(out_val, "You drop %s", tmp_str);
		    msg_print(out_val);
		    store_carry(store_num, &item_pos, &sold_obj);
		    check_strength();
		    if (item_pos >= 0) {
			if (item_pos < 12)
			    if (*cur_top < 12)
				display_inventory(store_num, item_pos);
			    else {
				*cur_top = 0;
				display_inventory(store_num, *cur_top);
			    }
			else if (*cur_top > 11)
			    display_inventory(store_num, item_pos);
			else {
			    *cur_top = 12;
			    display_inventory(store_num, *cur_top);
			}
		    }
		/* Less intuitive, but looks better here than in sell_haggle. */
		    erase_line(1, 0);
		    display_commands();
		}
	    } else {
		if (is_home)
		    msg_print("Your home is full.");
		else
		    msg_print("I have not the room in my store to keep it.");
	    }
	else {
	    if (is_home)
		msg_print("Whoops! Cock up.");
	    else
		msg_print("I do not buy such items.");
	}
    }
    return (sell);
}


/*
 * Entering a store
 */
void enter_store(int which)
{
    int                  cur_top, tmp_chr;
    char                 command;
    register int         exit_flag;
    register store_type *st_ptr;

    st_ptr = &store[which];
    if (which == 7)
	is_home = TRUE;
    else
	is_home = FALSE;
    /* Check the "locked doors" */
    if (store[which].store_open >= turn) {
	msg_print("The doors are locked.");
	return;
    }


    /* Start at the beginning */
    cur_top = 0;

    /* Display the store */
    display_store(which, cur_top);

    /* Interact with player */
    for (exit_flag = FALSE; !exit_flag; ) {

	    move_cursor(20, 9);
	/* clear the msg flag just like we do in dungeon.c */
	    msg_flag = FALSE;
	    if (get_com(NULL, &command)) {

	/* Process the command */
		switch (command) {
		  case 'b':
		    if (cur_top == 0)
			if (st_ptr->store_ctr > 12) {
			    cur_top = 12;
			    display_inventory(which, cur_top);
			} else
			    msg_print("Entire inventory is shown.");
		    else {
			cur_top = 0;
			display_inventory(which, cur_top);
		    }
		    break;
		  case 'E':
		  case 'e':	   /* Equipment List	 */
		  case 'I':
		  case 'i':	   /* Inventory		 */
		  case 'T':
		  case 't':	   /* Take off		 */
		  case 'W':
		  case 'w':	   /* Wear			 */
		  case 'X':
		  case 'x':	   /* Switch weapon		 */
		    tmp_chr = p_ptr->use_stat[A_CHR];
		    do {
			in_store_flag = TRUE;
			inven_command(command);
			command = doing_inven;
			in_store_flag = FALSE;
		    }
		    while (command);
		/* redisplay store prices if charisma changes */
		    if (tmp_chr != p_ptr->use_stat[A_CHR])
			display_inventory(which, cur_top);
		    free_turn_flag = FALSE;	/* No free moves here. -CJS- */
		    break;
		  case 'g':
		    if (!is_home)
			bell();
		    else
			exit_flag = store_purchase(which, &cur_top);
		    break;
		  case 'p':
		    if (is_home)
			bell();
		    else
			exit_flag = store_purchase(which, &cur_top);
		    break;
		  case 's':
		    if (is_home)
			bell();
		    else
			exit_flag = store_sell(which, &cur_top);
		    break;
		  case 'd':
		    if (!is_home)
			bell();
		    else
			exit_flag = store_sell(which, &cur_top);
		    break;
		  default:
		    bell();
		    break;
		}
	    } else
		exit_flag = TRUE;
	}
	
    /* Can't save and restore the screen, because inven_command does that. */
	draw_cave();
    }

}



/* Insert INVEN_MAX at given location	 */
static void insert_store(int store_num, register int pos, s32b icost, inven_type *i_ptr)
{
    register int        i;
    register store_type *s_ptr;

    s_ptr = &store[store_num];
    for (i = s_ptr->store_ctr - 1; i >= pos; i--)
	s_ptr->store_item[i + 1] = s_ptr->store_item[i];
    s_ptr->store_item[pos] = *i_ptr;
    s_ptr->store_item[pos].scost = (-icost);
    s_ptr->store_ctr++;
}



static void special_offer(inven_type *i_ptr)
{
    s32b orig_cost = i_ptr->cost;

    if (randint(30) == 1) {
	i_ptr->cost = (i_ptr->cost * 3) / 4;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "25% discount");
    } else if (randint(150) == 1) {
	i_ptr->cost /= 2;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "50% discount");
    } else if (randint(300) == 1) {
	i_ptr->cost /= 4;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "75% discount");
    } else if (randint(500) == 1) {
	i_ptr->cost /= 10;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "to clear");
    }
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(void)
{
    register int         i, j;
    register store_type *st_ptr;


    /* Maintain every store (except the home) */
    for (i = 0; i < (MAX_STORES - 1); i++) {

	/* Activate that store */
	st_ptr = &store[i];

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;


	/* Choose the number of slots to keep */
	j = st_ptr->store_ctr;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->store_ctr > j) store_destroy(i, randint((int)st_ptr->store_ctr) - 1, FALSE);

	/* Choose the number of slots to fill */
	j = st_ptr->store_ctr;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= STORE_INVEN_MAX) j = STORE_INVEN_MAX - 1;

	/* Acquire some new items */
	while (st_ptr->store_ctr < j) store_create(i);
    }
}


/*
 * Initialize the stores with owners			-RAK-
 */
void store_init(void)
{
    register int         j, k;
    register store_type *st_ptr;

    /* Build each store */
    for (j = 0; j < MAX_STORES; j++) {

	st_ptr = &store[j];

	/* Pick an owner */
	st_ptr->owner = MAX_STORES * rand_int(MAX_OWNERS / MAX_STORES) + j;

	/* Initialize the store */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->store_ctr = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* No items yet */
	for (k = 0; k < STORE_INVEN_MAX; k++) {
	    invcopy(&st_ptr->store_item[k], OBJ_NOTHING);
	    st_ptr->store_item[k].scost = 0;
	}
    }
}

