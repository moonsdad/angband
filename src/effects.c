/* File: effects.c */

/* Purpose: effects of various "objects" */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 */



/*
 * Lose a strength point.				-RAK-	 
 */
static void lose_str()
{
    if (!p_ptr->flags.sustain_str) {
	(void)dec_stat(A_STR);
	msg_print("You feel very weak.");
    }
    else {
	msg_print("You feel weak for a moment;  it passes.");
    }
}


/*
 * Lose an intelligence point.				-RAK-	 
 */
static void lose_int()
{
    if (!p_ptr->flags.sustain_int) {
	(void)dec_stat(A_INT);
	msg_print("You become very dizzy.");
    }
    else {
	msg_print("You become dizzy for a moment;  it passes.");
    }
}


/*
 * Lose a wisdom point.					-RAK-	 
 */
static void lose_wis()
{
    if (!p_ptr->flags.sustain_wis) {
	(void)dec_stat(A_WIS);
	msg_print("You feel very naive.");
    }
    else {
	msg_print("You feel naive for a moment;  it passes.");
    }
}


/*
 * Lose a dexterity point.				-RAK-	 
 */
static void lose_dex()
{
    if (!p_ptr->flags.sustain_dex) {
	(void)dec_stat(A_DEX);
	msg_print("You feel very sore.");
    }
    else {
	msg_print("You feel sore for a moment;  it passes.");
    }
}


/*
 * Lose a constitution point.				-RAK-	 
 */
static void lose_con()
{
    if (!p_ptr->flags.sustain_con) {
	(void)dec_stat(A_CON);
	msg_print("You feel very sick.");
    }
    else {
	msg_print("You feel sick for a moment;  it passes.");
    }
}


/*
 * Lose a charisma point.				-RAK-	 
 */
static void lose_chr()
{
    if (!p_ptr->flags.sustain_chr) {
	(void)dec_stat(A_CHR);
	msg_print("Your skin starts to itch.");
    }
    else {
	msg_print("Your skin starts to itch, but feels better now.");
    }
}



/*
 * Eat some food. -RAK-
 */
void eat(void)
{
    u32b                 i;
    int			   j, k, ident;
    int                    item_val;
    register inven_type   *i_ptr;

    /* Assume the turn is free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_FOOD, TV_NEVER, &j, &k)) {
	msg_print("You are not carrying any food.");
	return;
    }

    /* Get a food */
    if (get_item(&item_val, "Eat what?", j, k, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Identity not known yet */
    ident = FALSE;

    /* Apply all of the food flags */
    for (i = i_ptr->flags; i; ) {

	/* Extract the next "effect" bit */
	j = bit_pos(&i);

	/* Analyze the effect */
	switch (j + 1) {

	  case 1:
	    if (!p_ptr->flags.resist_pois) {
		p_ptr->flags.poisoned += randint(10) + i_ptr->level;
	    }
		ident = TRUE;
	    break;

	  case 2:
	    if (!p_ptr->flags.resist_blind) {
		p_ptr->flags.blind += randint(250) + 10 * i_ptr->level + 100;
		draw_cave();
		msg_print("A veil of darkness surrounds you.");
		ident = TRUE;
	    }
	    break;

	  case 3:
	    if (!p_ptr->flags.resist_fear) {
		p_ptr->flags.afraid += randint(10) + i_ptr->level;
		msg_print("You feel terrified!");
		ident = TRUE;
	    }
	    break;

	  case 4:
	    if ((!p_ptr->flags.resist_conf) && (!p_ptr->flags.resist_chaos)) {
		p_ptr->flags.confused += randint(10) + i_ptr->level;
		msg_print("You feel drugged.");
	    }
		ident = TRUE;
	    break;

	  case 5:
	    p_ptr->flags.image += randint(200) + 25 * i_ptr->level + 200;
	    msg_print("You feel drugged.");
	    ident = TRUE;
	    break;

	  case 6:
	    ident = cure_poison();
	    break;

	  case 7:
	    ident = cure_blindness();
	    break;

	  case 8:
	    if (p_ptr->flags.afraid > 1) {
		p_ptr->flags.afraid = 1;
		ident = TRUE;
	    }
	    break;

	  case 9:
	    ident = cure_confusion();
	    break;

	  case 10:
	    lose_str();
	    ident = TRUE;
	    break;

	  case 11:
	    lose_con();
	    ident = TRUE;
	    break;

#if 0				   /* 12 through 15 are not used */
	  case 12:
	    lose_int();
	    ident = TRUE;
	    break;

	  case 13:
	    lose_wis();
	    ident = TRUE;
	    break;

	  case 14:
	    lose_dex();
	    ident = TRUE;
	    break;

	  case 15:
	    lose_chr();
	    ident = TRUE;
	    break;
#endif

	  case 16:
	    if (res_stat(A_STR)) {
		msg_print("You feel your strength returning.");
		ident = TRUE;
	    }
	    break;

	  case 17:
	    if (res_stat(A_CON)) {
		msg_print("You feel your health returning.");
		ident = TRUE;
	    }
	    break;

	  case 18:
	    if (res_stat(A_INT)) {
		msg_print("Your head spins a moment.");
		ident = TRUE;
	    }
	    break;

	  case 19:
	    if (res_stat(A_WIS)) {
		msg_print("You feel your wisdom returning.");
		ident = TRUE;
	    }
	    break;

	  case 20:
	    if (res_stat(A_DEX)) {
		msg_print("You feel more dextrous.");
		ident = TRUE;
	    }
	    break;

	  case 21:
	    if (res_stat(A_CHR)) {
		msg_print("Your skin stops itching.");
		ident = TRUE;
	    }
	    break;

	  case 22:
	    ident = hp_player(randint(6));
	    break;

	  case 23:
	    ident = hp_player(randint(12));
	    break;

	  case 24:
	    ident = hp_player(randint(18));
	    break;

#if 0				   /* 25 is not used */
	  case 25:
	    ident = hp_player(damroll(3, 6));
	    break;
#endif

	  case 26:
	    ident = hp_player(damroll(3, 12));
	    break;

	  case 27:
	    take_hit(randint(18), "poisonous food.");
	    ident = TRUE;
	    break;

#if 0				   /* 28 through 30 are not used */
	  case 28:
	    take_hit(randint(8), "poisonous food.");
	    ident = TRUE;
	    break;

	  case 29:
	    take_hit(damroll(2, 8), "poisonous food.");
	    ident = TRUE;
	    break;

	  case 30:
	    take_hit(damroll(3, 8), "poisonous food.");
	    ident = TRUE;
	    break;
#endif
	      default:
		msg_print("Internal error in eat()");
	    break;
	    }
	}

	if (ident) {
	    if (!known1_p(i_ptr)) {

	    /* The player is now aware of the object */
	    /* round half-way case up */
		p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) / p_ptr->misc.lev;
		prt_experience();

		identify(&item_val);
		i_ptr = &inventory[item_val];
	    }
	} else if (!known1_p(i_ptr))
	    sample(i_ptr);

    /* Consume the food */
    add_food(i_ptr->p1);

    /* Hack -- note loss of hunger */
    p_ptr->flags.status &= ~(PY_WEAK | PY_HUNGRY);
    prt_hunger();

	desc_remain(item_val);
	inven_destroy(item_val);
    }
}





/*
 * Potions for the quaffing -RAK-
 */
void quaff(void)
{
    u32b i, l;
    int    j, k, item_val;
    int    ident;
    register inven_type   *i_ptr;

    /* Assume the turn will be free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_POTION1, TV_POTION2, &j, &k)) {
	msg_print("You are not carrying any potions.");
	return;
    }

    /* Get a potion */
    if (get_item(&item_val, "Quaff which potion?", j, k, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Not identified yet */
    ident = FALSE;

    /* Note potions with no effects */
    if (i_ptr->flags == 0) {
	msg_print("You feel less thirsty.");
	ident = TRUE;
    }

    /* Analyze the first set of effects */    
    for (i = i_ptr->flags1; i; ) {

	/* Extract the next effect bit */
	j = bit_pos(&i);
	if (i_ptr->tval == TV_POTION2)
	    j += 32;

	/* Analyze the effect */
	switch (j + 1) {

	  case 1:
	    if (inc_stat(A_STR)) {
		msg_print("Wow!  What bulging muscles! ");
		ident = TRUE;
	    }
	    break;

	  case 2:
	    ident = TRUE;
	    lose_str();
	    break;

	  case 3:
	    if (res_stat(A_STR)) {
		msg_print("You feel warm all over.");
		ident = TRUE;
	    }
	    break;

	  case 4:
	    if (inc_stat(A_INT)) {
		msg_print("Aren't you brilliant! ");
		ident = TRUE;
	    }
	    break;

	  case 5:
	    ident = TRUE;
	    lose_int();
	    break;

	  case 6:
	    if (res_stat(A_INT)) {
		msg_print("You have a warm feeling.");
		ident = TRUE;
	    }
	    break;

	  case 7:
	    if (inc_stat(A_WIS)) {
		msg_print("You suddenly have a profound thought! ");
		ident = TRUE;
	    }
	    break;

	  case 8:
	    ident = TRUE;
	    lose_wis();
	    break;

	  case 9:
	    if (res_stat(A_WIS)) {
		msg_print("You feel your wisdom returning.");
		ident = TRUE;
	    }
	    break;

	  case 10:
	    if (inc_stat(A_CHR)) {
		msg_print("Gee, ain't you cute! ");
		ident = TRUE;
	    }
	    break;

	  case 11:
	    ident = TRUE;
	    lose_chr();
	    break;

	  case 12:
	    if (res_stat(A_CHR)) {
		msg_print("You feel your looks returning.");
		ident = TRUE;
	    }
	    break;

	  case 13:
	    if (hp_player(damroll(2, 7))) ident = TRUE;
	    if (p_ptr->flags.cut > 0) {
		p_ptr->flags.cut -= 10;
		if (p_ptr->flags.cut < 0) p_ptr->flags.cut = 0;
		ident = TRUE;
		msg_print("Your wounds heal.");
	    }
	    break;

	  case 14:
	    if (hp_player(damroll(4, 7))) ident = TRUE;
	    if (p_ptr->flags.cut > 0) {
		p_ptr->flags.cut = (p_ptr->flags.cut / 2) - 50;
		if (p_ptr->flags.cut < 0) p_ptr->flags.cut = 0;
		ident = TRUE;
		msg_print("Your wounds heal.");
	    }
	    break;

	  case 15:
	    if (hp_player(damroll(6, 7))) ident = TRUE;
	    if (p_ptr->flags.cut > 0) {
		p_ptr->flags.cut = 0;
		ident = TRUE;
		msg_print("Your wounds heal.");
	    }
	    if (p_ptr->flags.stun > 0) {
		if (p_ptr->flags.stun > 50) {
		    p_ptr->misc.ptohit += 20;
		    p_ptr->misc.ptodam += 20;
		} else {
		    p_ptr->misc.ptohit += 5;
		    p_ptr->misc.ptodam += 5;
		}
		p_ptr->flags.stun = 0;
		ident = TRUE;
		msg_print("Your head stops stinging.");
	    }
	    break;

	  case 16:
	    if (hp_player(400)) ident = TRUE;
	    if (p_ptr->flags.stun > 0) {
		if (p_ptr->flags.stun > 50) {
		    p_ptr->misc.ptohit += 20;
		    p_ptr->misc.ptodam += 20;
		} else {
		    p_ptr->misc.ptohit += 5;
		    p_ptr->misc.ptodam += 5;
		}
		p_ptr->flags.stun = 0;
		ident = TRUE;
		msg_print("Your head stops stinging.");
		}
		   if (p_ptr->flags.cut > 0) {
		p_ptr->flags.cut = 0;
		ident = TRUE;
		msg_print("Your wounds heal.");
	    }
	    break;

	  case 17:
	    if (inc_stat(A_CON)) {
		msg_print("You feel tingly for a moment.");
		ident = TRUE;
	    }
	    break;

	  case 18:
	    if (p_ptr->misc.exp < MAX_EXP) {
		l = (p_ptr->misc.exp / 2) + 10;
		if (l > 100000L) l = 100000L;
		p_ptr->misc.exp += l;
		msg_print("You feel more experienced.");
		prt_experience();
		ident = TRUE;
	    }
	    break;

	  case 19:
	    if (!f_ptr->free_act) {
		/* paralysis must be zero, we are drinking */
		/* but what about multiple potion effects? */
		msg_print("You fall asleep.");
		p_ptr->flags.paralysis += randint(4) + 4;
		ident = TRUE;
	    }
	    break;

	  case 20:
	    if (!p_ptr->flags.resist_blind) {
		if (p_ptr->flags.blind == 0) {
		    msg_print("You are covered by a veil of darkness.");
		    ident = TRUE;
		}
		f_ptr->flags.blind += randint(100) + 100;
	    }
	    break;

	  case 21:
	    if (!p_ptr->flags.resist_conf) {
		if (p_ptr->flags.confused == 0) {
		    msg_print("Hey!  This is good stuff!  *Hick!*");
		    ident = TRUE;
		}
		p_ptr->flags.confused += randint(20) + 12;
	    }
	    break;

	  case 22:
	    if (!(p_ptr->flags.immune_pois ||
		  p_ptr->flags.resist_pois ||
		  p_ptr->flags.oppose_pois)) {
		msg_print("You feel very sick.");
		p_ptr->flags.poisoned += randint(15) + 10;
	    }
	    else {
		msg_print("The poison has no effect.");
	    }
	    if (!p_ptr->flags.resist_pois)
	    ident = TRUE;
	    break;

	  case 23:
	    if (p_ptr->flags.fast == 0) ident = TRUE;
	    if (p_ptr->flags.fast <= 0) {
		p_ptr->flags.fast += randint(25) + 15;
	    } else
		p_ptr->flags.fast += randint(5);
	    break;

	  case 24:
	    if (p_ptr->flags.slow == 0) ident = TRUE;
	    p_ptr->flags.slow += randint(25) + 15;
	    break;

	  case 26:
	    if (inc_stat(A_DEX)) {
		msg_print("You feel more limber! ");
		ident = TRUE;
	    }
	    break;

	  case 27:
	    if (res_stat(A_DEX)) {
		msg_print("You feel less clumsy.");
		ident = TRUE;
	    }
	    break;

	  case 28:
	    if (res_stat(A_CON)) {
		msg_print("You feel your health returning! ");
		ident = TRUE;
	    }
	    break;

	  case 29:
	    if (cure_blindness()) ident = TRUE;
	    break;

	  case 30:
	    if (cure_confusion()) ident = TRUE;
	    break;

	  case 31:
	    if (cure_poison()) ident = TRUE;
	    break;

	  case 34:
	    ident = TRUE;
	    if (!p_ptr->flags.hold_life && p_ptr->misc.exp > 0) {
		s32b               m, scale;

		msg_print("You feel your memories fade.");
		m = p_ptr->misc.exp / 5;
		if (p_ptr->misc.exp > MAX_SHORT) {
		    scale = MAX_LONG / p_ptr->misc.exp;
		    m += (randint((int)scale) * p_ptr->misc.exp) / (scale * 5);
		}
		else {
		    m += randint((int)p_ptr->misc.exp) / 5;
		}
		lose_exp(m);
	    }
	    else {
		msg_print ("You feel you memories fade for a moment, but quickly return.");
	    }
	    break;

	  case 35:
	    ident = cure_poison();
	    if (p_ptr->flags.food > 150) p_ptr->flags.food = 150;
	    p_ptr->flags.paralysis = 4;
	    msg_print("The potion makes you vomit! ");
	    ident = TRUE;
	    break;

	  case 37:
	    if (p_ptr->flags.hero == 0) ident = TRUE;
	    p_ptr->flags.hero += randint(25) + 25;
	    break;

	  case 38:
	    if (p_ptr->flags.shero == 0) ident = TRUE;
	    p_ptr->flags.shero += randint(25) + 25;
	    break;

	  case 39:
	    if (remove_fear()) ident = TRUE;
	    break;

	  case 40:
	    if (restore_level()) ident = TRUE;
	    break;

	  case 41:
	    if (!p_ptr->flags.oppose_fire) ident = TRUE;
	    p_ptr->flags.oppose_fire += randint(10) + 10;
	    break;

	  case 42:
	    if (!p_ptr->flags.oppose_cold) ident = TRUE;
	    p_ptr->flags.oppose_cold += randint(10) + 10;
	    break;

	  case 43:
	    if (!p_ptr->flags.detect_inv) ident = TRUE;
	    detect_inv2(randint(12) + 12);
	    break;

	  case 44:
	    if (slow_poison()) ident = TRUE;
	    break;

	  case 45:
	    if (cure_poison()) ident = TRUE;
	    break;

	  case 46:
	    if (p_ptr->misc.cmana < p_ptr->misc.mana) {
		p_ptr->misc.cmana = p_ptr->misc.mana;
		ident = TRUE;
		msg_print("Your feel your head clear.");
		prt_cmana();
	    }
	    break;

	  case 47:
	    if (p_ptr->flags.tim_infra == 0) {
		msg_print("Your eyes begin to tingle.");
		ident = TRUE;
	    }
	    p_ptr->flags.tim_infra += 100 + randint(100);
	    break;

	  case 48:
	    wiz_lite(TRUE);
	    if (!res_stat(A_WIS)) inc_stat(A_WIS);
	    if (!res_stat(A_INT)) inc_stat(A_INT);
	    msg_print("You feel more enlightened! ");
	    msg_print(NULL);
	    /* after all, what is the key to enlightenment? -CFT */
	    self_knowledge();
	    identify_pack();
	    (void)detect_treasure();
	    (void)detect_object();
	    (void)detect_sdoor();
	    (void)detect_trap();
	    ident = TRUE;
	    break;

	  case 49:
	    msg_print("Massive explosions rupture your body! ");
	    take_hit(damroll(50, 20), "a potion of Detonation");
	    cut_player(5000);
	    stun_player(75);
	    ident = TRUE;
	    break;

	  case 50:
	    msg_print("A feeling of Death flows through your body.");
	    take_hit(5000, "a potion of Death");
	    ident = TRUE;
	    break;

	  case 51:	   /* Life */
	    if (restore_level() |
	    res_stat(A_STR) |
	    res_stat(A_CON) |
	    res_stat(A_DEX) |
	    res_stat(A_WIS) |
	    res_stat(A_INT) |
	    res_stat(A_CHR) |
	    hp_player(5000) |
	    cure_poison() |
	    cure_blindness() |
	    cure_confusion() |
	    (p_ptr->flags.stun > 0) |
		(p_ptr->flags.cut > 0) |
	    (p_ptr->flags.image > 0) |
	    remove_fear()) {
		ident = TRUE;
		p_ptr->flags.cut = 0;
		p_ptr->flags.image = 0;
		if (p_ptr->flags.stun > 0) {
		    if (p_ptr->flags.stun > 50) {
			p_ptr->misc.ptohit += 20;
			p_ptr->misc.ptodam += 20;
		    } else {
			p_ptr->misc.ptohit += 5;
			p_ptr->misc.ptodam += 5;
		    }
		    p_ptr->flags.stun = 0;
		}
	    }
	    break;

	  case 52:	   /* Augm */
	    if (inc_stat(A_DEX) |
	    inc_stat(A_WIS) |
	    inc_stat(A_INT) |
	    inc_stat(A_STR) |
	    inc_stat(A_CHR) |
	    inc_stat(A_CON)) {
	    ident = TRUE;
		msg_print("You feel power flow through your body! ");
	    }
	    break;

	  case 53:	   /* Ruination */
	    take_hit(damroll(10, 10), "a potion of Ruination");
	    ruin_stat(A_DEX);
	    ruin_stat(A_WIS);
	    ruin_stat(A_CON);
	    ruin_stat(A_STR);
	    ruin_stat(A_CHR);
	    ruin_stat(A_INT);
	    ident = TRUE;
	    msg_print("Your nerves and muscles feel weak and lifeless! ");
	    break;

	  case 54:
	    msg_print("An image of your surroundings forms in your mind...");
	    wiz_lite(TRUE);
	    ident = TRUE;
	    break;

	  case 55:
	    msg_print("You feel you know yourself a little better...");
	    msg_print(NULL);
	    self_knowledge();
	    ident = TRUE;
	    break;

	  case 56:
	    if (hp_player(1200)) ident = TRUE;
	    if (p_ptr->flags.stun > 0) {
		if (p_ptr->flags.stun > 50) {
		    p_ptr->misc.ptohit += 20;
		    p_ptr->misc.ptodam += 20;
		} else {
		    p_ptr->misc.ptohit += 5;
		    p_ptr->misc.ptodam += 5;
		}
		p_ptr->flags.stun = 0;
		msg_print("Your head stops stinging.");
		ident = TRUE;
	    }
	    if (p_ptr->flags.cut > 0) {
		p_ptr->flags.cut = 0;
		msg_print("Your wounds heal.");
		ident = TRUE;
	    }
	    if (cure_blindness()) ident = TRUE;
	    if (cure_confusion()) ident = TRUE;
	    if (cure_poison()) ident = TRUE;
	    break;

	  default://REVERT BELOW
	    if (1) {
			char                tmp_str[100];

			msg_print("Internal error in potion()");
			sprintf(tmp_str, "Number %d...", j);
			msg_print(tmp_str);
		    }
		    break;//REVERT^
		}

	    }

    /* An identification was made */
    if (ident) {
	if (!known1_p(i_ptr)) {
	    /* round half-way case up */
		p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) / p_ptr->misc.lev;
		prt_experience();

		identify(&item_val);
		i_ptr = &inventory[item_val];
	    }
	} else if (!known1_p(i_ptr))
	    sample(i_ptr);

    /* Potions can feed the player */
    add_food(i_ptr->p1);

    /* Destroy the potion */
    desc_remain(item_val);
    inven_destroy(item_val);
    }
}




/*
 * Read a scroll -RAK-
 */
void read_scroll(void)
{
    u32b                i;
    int                   item_val, y, x;
    int			  j, k;
    int                   used_up, ident, l;
    int                   tmp[6];
    register inven_type  *i_ptr;
    bigvtype              out_val, tmp_str;

    free_turn_flag = TRUE;

    if (p_ptr->flags.blind > 0) {
	msg_print("You can't see to read the scroll.");
	return;
    }

    if (no_lite()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->flags.confused > 0) {
	msg_print("You are too confused to read a scroll.");
	return;
    }

    if (inven_ctr == 0) {
	msg_print("You are not carrying anything!");
	return;
    }

    if (!find_range(TV_SCROLL1, TV_SCROLL2, &j, &k)) {
	msg_print("You are not carrying any scrolls!");
	return;
    }
    
    /* Get a scroll */
    if (get_item(&item_val, "Read which scroll?", j, k, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Assume the scroll will get used up */
    used_up = TRUE;

    /* Not identified yet */
    ident = FALSE;

    /* Apply the first set of scroll effects */
    for (i = i_ptr->flags; i; ) {

	/* Extract the next effect bit-flag */
	    j = bit_pos(&i);
	    if (i_ptr->tval == TV_SCROLL2)
		j += 32;

	/* Scrolls. */
	switch (j+1) {

	  case 1:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];

	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, 1, ENCH_TOHIT)) {
		    msg_print("The enchantment fails. ");
		}
		ident = TRUE;
	    }
	    break;

	  case 2:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];

	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, 1, ENCH_TODAM)) {
		    msg_print("The enchantment fails. ");
		}
		ident = TRUE;
	    }
	    break;		

	  case 3:

	    /* Hack -- make a "list" of "armor" indexes, size is "k" */
	    k = 0;
		l = 0;
	    if (inventory[INVEN_BODY].tval != TV_NOTHING)  tmp[k++] = INVEN_BODY;
	    if (inventory[INVEN_ARM].tval != TV_NOTHING)   tmp[k++] = INVEN_ARM;
	    if (inventory[INVEN_OUTER].tval != TV_NOTHING) tmp[k++] = INVEN_OUTER;
	    if (inventory[INVEN_HANDS].tval != TV_NOTHING) tmp[k++] = INVEN_HANDS;
	    if (inventory[INVEN_HEAD].tval != TV_NOTHING)  tmp[k++] = INVEN_HEAD;
	    if (inventory[INVEN_FEET].tval != TV_NOTHING)  tmp[k++] = INVEN_FEET;

		/* Pick a random item */
		if (k > 0)
		    l = tmp[randint(k) - 1];
		if (TR_CURSED & inventory[INVEN_BODY].flags)
		    l = INVEN_BODY;
		else if (TR_CURSED & inventory[INVEN_ARM].flags)
		    l = INVEN_ARM;
		else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		    l = INVEN_OUTER;
		else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		    l = INVEN_HEAD;
		else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		    l = INVEN_HANDS;
		else if (TR_CURSED & inventory[INVEN_FEET].flags)
		    l = INVEN_FEET;
		if (l > 0) {
		    i_ptr = &inventory[l];

		    /* Visual effect (known) */
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		    msg_print(out_val);

		    /* Attempt to enchant */
		    if (!enchant(i_ptr, 1, ENCH_TOAC)) {
			msg_print("The enchantment fails. ");
		    }
		    ident = TRUE;
		}

	    break;

	  case 4:

	    /* Can be identified by "label" on scroll */
	    msg_print("This is an identify scroll.");
	    ident = TRUE;

		/* Identify an item, or preserve the scroll */
		used_up = ident_spell();

	    /* The identify may merge objects, causing the identify scroll to
	     * move to a different place.  Check for that here.  It can
	     * move arbitrarily far if an identify scroll was used on another
	     * identify scroll, but it always moves down. 
	     */
		while (i_ptr->tval != TV_SCROLL1 || i_ptr->flags != 0x00000008) {
		    item_val--;
		    i_ptr = &inventory[item_val];
		}

	    break;

	  case 5:
	    if (remove_curse()) {
		msg_print("You feel as if someone is watching over you.");
		ident = TRUE;
	    }
	    break;

	  case 6:
	    ident = lite_area(char_row, char_col, damroll(2, 12), 2);
	    break;

	  case 7:
	    for (k = 0; k < randint(3); k++) {
		y = char_row;
		x = char_col;
		ident |= summon_monster(&y, &x, FALSE);
	    }
	    break;

	  case 8:
	    teleport(10);
	    ident = TRUE;
	    break;

	  case 9:
	    teleport(100);
	    ident = TRUE;
	    break;

	  case 10:
	    (void)tele_level();
	    ident = TRUE;
	    break;

	  case 11:
	    if (p_ptr->flags.confuse_monster == 0) {
		msg_print("Your hands begin to glow.");
		p_ptr->flags.confuse_monster = TRUE;
		ident = TRUE;
	    }
	    break;

	  case 12:
	    map_area();
	    ident = TRUE;
	    break;

	  case 13:
	    ident = sleep_monsters1(char_row, char_col);
	    break;

	  case 14:
	    warding_glyph();
	    ident = TRUE;
	    break;

	  case 15:
	    ident = detect_treasure();
	    break;

	  case 16:
	    ident = detect_object();
	    break;

	  case 17:
	    ident = detect_trap();
	    break;

	  case 18:
	    ident = detect_sdoor();
	    break;

	  case 19:
	    msg_print("This is a mass genocide scroll.");
	    mass_genocide(TRUE);
	    ident = TRUE;
	    break;

	  case 20:
	    ident = detect_invisible();
	    break;

	  case 21:
	    if (aggravate_monster(20)) {
		msg_print("There is a high pitched humming noise.");
		ident = TRUE;
	    }
	    break;

	  case 22:
	    ident = trap_creation();
	    break;

	  case 23:
	    ident = td_destroy();
	    break;

	  /* Not Used, used to be door creation */
	  case 24:
	    break;

	  case 25:
	    msg_print("This is a Recharge-Item scroll.");
	    /* Hack -- Like identify, recharge can be cancelled */
	    used_up = recharge(60);
	    ident = TRUE;
	    break;

	  case 26:
	    msg_print("This is a genocide scroll.");
	    genocide(TRUE);
	    ident = TRUE;
	    break;

	  case 27:
	    ident = unlite_area(char_row, char_col);
	    if (!p_ptr->flags.resist_blind) {
		p_ptr->flags.blind += 3 + randint(5);
	    }
	    break;

	  case 28:
	    ident = protect_evil();
	    break;

	  case 29:
	    create_food();
	    ident = TRUE;
	    break;

	  case 30:
	    ident = dispel_creature(UNDEAD, 60);
	    break;

	  case 31:
	    remove_all_curse();
	    ident = TRUE;
	    break;

	  case 33:

	    /* Pick a weapon */
		i_ptr = &inventory[INVEN_WIELD];

	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows brightly!", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, randint(3), ENCH_TOHIT|ENCH_TODAM)) {
		    msg_print("The enchantment fails.");
		}
		ident = TRUE;
	    }
	    break;

	  case 34:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];
	    
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		if ((i_ptr->flags2 & TR_ARTIFACT) && (randint(7) < 4)) {
		    msg_print("A terrible black aura tries to surround your weapon,");
		    sprintf(out_val, "but your %s resists the effects!", tmp_str);
		    msg_print(out_val);
		}

		/* not artifact or failed save... */
		else {

		    /* Oops */
		    sprintf(out_val, "A terrible black aura blasts your %s!",
				   tmp_str);
			msg_print(out_val);

		    /* Shatter the weapon */
		    py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
		    i_ptr->name2 = SN_SHATTERED;
		    i_ptr->tohit = (-randint(5) - randint(5));
		    i_ptr->todam = (-randint(5) - randint(5));
		    i_ptr->flags = TR_CURSED;
		    i_ptr->flags2 = 0;
		    i_ptr->dd = i_ptr->ds = 1;
		    i_ptr->toac = 0;	/* in case defender... */
		    i_ptr->cost = (-1);
			py_bonuses(i_ptr, 1);	/* now apply new "bonuses"  -CFT */
		    
		    /* Recalculate bonuses */
		    calc_bonuses();
		}

		ident = TRUE;
	    }
	    break;

	  case 35:

	    /* Hack -- make a "list" of "armor" indexes, size is "k" */
	    k = 0;
	    l = 0;
	    if (inventory[INVEN_BODY].tval != TV_NOTHING)  tmp[k++] = INVEN_BODY;
	    if (inventory[INVEN_ARM].tval != TV_NOTHING)   tmp[k++] = INVEN_ARM;
	    if (inventory[INVEN_OUTER].tval != TV_NOTHING) tmp[k++] = INVEN_OUTER;
	    if (inventory[INVEN_HANDS].tval != TV_NOTHING) tmp[k++] = INVEN_HANDS;
	    if (inventory[INVEN_HEAD].tval != TV_NOTHING)  tmp[k++] = INVEN_HEAD;
	    if (inventory[INVEN_FEET].tval != TV_NOTHING)  tmp[k++] = INVEN_FEET;

		/* Pick a random item */
		if (k > 0)
		    l = tmp[randint(k) - 1];
		if (TR_CURSED & inventory[INVEN_BODY].flags)
		    l = INVEN_BODY;
		else if (TR_CURSED & inventory[INVEN_ARM].flags)
		    l = INVEN_ARM;
		else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		    l = INVEN_OUTER;
		else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		    l = INVEN_HEAD;
		else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		    l = INVEN_HANDS;
		else if (TR_CURSED & inventory[INVEN_FEET].flags)
		    l = INVEN_FEET;

		if (l > 0) {
		    i_ptr = &inventory[l];




	    /* Message (and knowledge) */
	    objdes(tmp_str, i_ptr, FALSE);
	    (void) sprintf(out_val,"Your %s glows brightly!", tmp_str);
	    msg_print(out_val);

	    /* Attempt to enchant */
	    if (!enchant(i_ptr, randint(3)+1, ENCH_TOAC)) {
		msg_print("The enchantment fails.");
	    }
	    ident = TRUE;
	    }

	    break;

	  case 36:

	    /* Hack -- make a "list" of "armor" indexes, size is "k" */
		if ((inventory[INVEN_BODY].tval != TV_NOTHING)
		    && (randint(4) == 1))  k = INVEN_BODY;
		else if ((inventory[INVEN_ARM].tval != TV_NOTHING)
			 && (randint(3) == 1))   k = INVEN_ARM;
		else if ((inventory[INVEN_OUTER].tval != TV_NOTHING)
			 && (randint(3) == 1)) k = INVEN_OUTER;
		else if ((inventory[INVEN_HEAD].tval != TV_NOTHING)
			 && (randint(3) == 1)) k = INVEN_HEAD;
		else if ((inventory[INVEN_HANDS].tval != TV_NOTHING)
			 && (randint(3) == 1)) k = INVEN_HANDS;
		else if ((inventory[INVEN_FEET].tval != TV_NOTHING)
			 && (randint(3) == 1)) k = INVEN_FEET;
		else if (inventory[INVEN_BODY].tval != TV_NOTHING) k = INVEN_BODY;
		else if (inventory[INVEN_ARM].tval != TV_NOTHING)  k = INVEN_ARM;
		else if (inventory[INVEN_OUTER].tval != TV_NOTHING) k = INVEN_OUTER;
		else if (inventory[INVEN_HEAD].tval != TV_NOTHING) k = INVEN_HEAD;
		else if (inventory[INVEN_HANDS].tval != TV_NOTHING) k = INVEN_HANDS;
		else if (inventory[INVEN_FEET].tval != TV_NOTHING) k = INVEN_FEET;
		else k = 0;

		if (k > 0) {

		/* Pick a random item */
	    i_ptr = &inventory[k];

	    /* Describe */
	    objdes(tmp_str, i_ptr, FALSE);

	    /* Attempt a saving throw for artifacts */
	    if ((i_ptr->flags2 & TR_ARTIFACT) && (randint(7) < 4)) {
		msg_print("A terrible black aura tries to surround your");
	    sprintf(out_val, "%s, but it resists the effects!", tmp_str);
		msg_print(out_val);
	    }

	    /* not artifact or failed save... */
	    else {

		/* Oops */
	    sprintf(out_val, "A terrible black aura blasts your %s!",
			       tmp_str);
		msg_print(out_val);

		/* Blast the armor */
		py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
		i_ptr->name2 = SN_BLASTED;
		i_ptr->flags = TR_CURSED;
		i_ptr->flags2 = 0;
		i_ptr->toac = (-randint(5) - randint(5));
		i_ptr->tohit = i_ptr->todam = 0; /* in case gaunlets of slaying... */
		i_ptr->ac = (i_ptr->ac > 9) ? 1 : 0;
		i_ptr->cost = (-1);

		/* Recalculate bonuses */
		py_bonuses(i_ptr, 1);	/* now apply new "bonuses"  -CFT */
		calc_bonuses();
	    }

		/* Well, you know all about it */
		ident = TRUE;
	    }
	    break;

	  case 37:
		ident = FALSE;
	    for (k = 0; k < randint(3); k++) {
		y = char_row;
		x = char_col;
		ident |= summon_undead(&y, &x);
	    }
	    break;

	  case 38:
	    bless(randint(12) + 6);
	    ident = TRUE;
	    break;

	  case 39:
	    bless(randint(24) + 12);
	    ident = TRUE;
	    break;

	  case 40:
	    bless(randint(48) + 24);
	    ident = TRUE;
	    break;

	  case 41:
	    if (p_ptr->flags.word_recall == 0) {
		p_ptr->flags.word_recall = 15 + randint(20);
		msg_print("The air about you becomes charged...");
	    }
	    else {
		p_ptr->flags.word_recall = 0;
		msg_print("A tension leaves the air around you...");
	    }
	    ident = TRUE;
	    break;

	  case 42:
	    destroy_area(char_row, char_col);
	    ident = TRUE;
	    break;

	  case 43:
	    place_good(char_row, char_col, SPECIAL);
	    prt_map();
	    ident = TRUE;
	    break;

	  case 44:
	    special_random_object(char_row, char_col, 1);
	    prt_map();
	    ident = TRUE;
	    break;

	  default:
		msg_print("Internal error in scroll()");
		break;
	}
    }

    /* An identification was made */
	i_ptr = &inventory[item_val];
    if (ident) {
	    if (!known1_p(i_ptr)) {
	    /* round half-way case up */
		p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) / p_ptr->misc.lev;
		prt_experience();

		identify(&item_val);
		i_ptr = &inventory[item_val];
	    }
	} else if (!known1_p(i_ptr))
	    sample(i_ptr);

    /* Allow certain scrolls to be "preserved" */
    if (used_up) {

    /* Destroy the scroll */
	    desc_remain(item_val);
	    inven_destroy(item_val);
	}
    }
}




/*
 * Aim a wand.
 */
void aim(void)
{
    u32b                i;
    int			ident, dir, l;
    int			item_val, done_effect, j, k, chance;
    inven_type		*i_ptr;

    free_turn_flag = TRUE;

    if (!inven_ctr) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_WAND, TV_NEVER, &j, &k)) {
	msg_print("You are not carrying any wands.");
	return;
    }

    /* Get a wand */
    if (get_item(&item_val, "Aim which wand?", j, k, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;
	if (get_dir(NULL, &dir)) {
	    if (p_ptr->flags.confused > 0) {
		msg_print("You are confused.");
		do {
		    dir = randint(9);
		} while (dir == 5);
	    }
	    ident = FALSE;

    /* Chance of success */
    chance = (p_ptr->misc.save + stat_adj(A_INT) - (int)(i_ptr->level>42?42:i_ptr->level) +
	      (class_level_adj[p_ptr->misc.pclass][CLA_DEVICE] * p_ptr->misc.lev / 3));

    if (p_ptr->flags.confused > 0) chance = chance / 2;

    /* Give everyone a slight chance */
    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;
    }

    if (chance <= 0) chance = 1;

    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the wand properly.");
	return;
    }

    if (i_ptr->p1 > 0) {
	i = i_ptr->flags;
	done_effect = 0;
	(i_ptr->p1)--;
	while (!done_effect) {

    /* Start at the player */
    k = char_row;
    l = char_col;

    /* Various effects */
    switch (i) {

	case WD_LT:
	    msg_print("A line of blue shimmering light appears.");
	    lite_line(dir, char_row, char_col);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_DRG_FIRE:
	    fire_ball(GF_FIRE, dir, k, l, 100, 3);
	    ident = TRUE;
		done_effect = 1;
	    break;

    case WD_DRG_FRST:
	    fire_ball(GF_COLD, dir, k, l, 80, 3);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_DRG_BREA:
		switch (randint(5)) {
		  case 1:
		    fire_ball(GF_FIRE, dir, k, l, 100, 3);
		    break;
		  case 2:
		    fire_ball(GF_COLD, dir, k, l, 80, 3);
		    break;
		  case 3:
		    fire_ball(GF_ACID, dir, k, l, 90, 3);
		    break;
		  case 4:
		    fire_ball(GF_ELEC, dir, k, l, 70, 3);
		    break;
		  default:
		    fire_ball(GF_POIS, dir, k, l, 70, 3);
		    break;
		}
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_AC_BLTS:	/* Acid , New */
		if (randint(5)==1)
		    line_spell(GF_ACID,dir,k,l,damroll(5,8));
		else
		    fire_bolt(GF_ACID,dir,k,l,damroll(5,8));
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_LT_BLTS:	/* Lightning */
		if (randint(6)==1)
		    line_spell(GF_ELEC,dir,k,l,damroll(3,8));
		else
		    fire_bolt(GF_ELEC, dir, k, l, damroll(3, 8));
		ident = TRUE;
		done_effect = 1;
	    break;

	case WD_FT_BLTS:	/* Frost */
		if (randint(6)==1)
		    line_spell(GF_ELEC,dir,k,l,damroll(3,8));
		else
		    fire_bolt(GF_ELEC, dir, k, l, damroll(3, 8));
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_FR_BLTS:	/* Fire */
		if (randint(4)==1)
		    line_spell(GF_FIRE,dir,k,l,damroll(6,8));
		else
		    fire_bolt(GF_FIRE, dir, k, l, damroll(6, 8));
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_ST_MUD:
	    ident = wall_to_mud(dir, k, l);
		done_effect = 1;
	    break;

	case WD_POLY:
	    ident = poly_monster(dir, k, l);
		done_effect = 1;
	    break;

	case WD_HEAL_MN:
	    ident = hp_monster(dir, k, l, -damroll(4, 6));
		done_effect = 1;
	    break;

	case WD_HAST_MN:
	    ident = speed_monster(dir, k, l, 1);
		done_effect = 1;
	    break;

	case WD_SLOW_MN:
	    ident = speed_monster(dir, k, l, -1);
		done_effect = 1;
	    break;

	case WD_CONF_MN:
	    ident = confuse_monster(dir, k, l, 10);
		done_effect = 1;
	    break;

	case WD_SLEE_MN:
	    ident = sleep_monster(dir, k, l);
		done_effect = 1;
	    break;

	case WD_DRAIN:
	    ident = drain_life(dir, k, l, 75);
		done_effect = 1;
	    break;

	case WD_ANHIL:
	    ident = drain_life(dir, k, l, 125);
		done_effect = 1;
	    break;

	case WD_TR_DEST:
	    ident = td_destroy2(dir, k, l);
		done_effect = 1;
	    break;

	case WD_MAG_MIS:
		if (randint(6)==1)
		    line_spell(GF_MAGIC_MISSILE,dir,k,l,damroll(2,6));
		else
		    fire_bolt(GF_MAGIC_MISSILE, dir, k, l, damroll(2, 6));
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_FEAR_MN:	/* Fear Monster */
	    ident = fear_monster(dir, k, l, 10);
		done_effect = 1;
	    break;

	case WD_CLONE:
	    ident = clone_monster(dir, k, l);
		done_effect = 1;
	    break;

	case WD_TELE:
	    ident = teleport_monster(dir, k, l);
		done_effect = 1;
	    break;

	case WD_DISARM:
	    ident = disarm_all(dir, k, l);
		done_effect = 1;
	    break;

	case WD_LT_BALL:
	    fire_ball(GF_ELEC, dir, k, l, 32, 2);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_CD_BALL:
	    fire_ball(GF_COLD, dir, k, l, 48, 2);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_FR_BALL:
	    fire_ball(GF_FIRE, dir, k, l, 72, 2);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_ST_CLD:
	    fire_ball(GF_POIS, dir, k, l, 12, 2);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_AC_BALL:
		fire_ball(GF_ACID, dir, k, l, 60, 2);
	    ident = TRUE;
		done_effect = 1;
	    break;

	case WD_WONDER:
		i = randint(23);
	    break;

	default:
	    msg_print("Oops.  Undefined wand effect.");
		done_effect = 1;
	    break;
    }
	}

    /* Apply identification */
	    if (ident) {
		    if (!known1_p(i_ptr)) {
		    /* round half-way case up */
			p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) / p_ptr->misc.lev;
			prt_experience();

			identify(&item_val);
			i_ptr = &inventory[item_val];
		    }
		} else if (!known1_p(i_ptr))
		    sample(i_ptr);

    /* Describe the remaining charges */
		desc_charges(item_val);
	    } else {
    /* The wand is already empty! */
		msg_print("The wand has no charges left.");
		if (!known2_p(i_ptr))
		    add_inscribe(i_ptr, ID_EMPTY);
	    }
	}
    }
}




/*
 * Use a staff.			-RAK-	
 */
void use(void)
{
    u32b                i;
    int			  ident, chance, k, j;
    int                   item_val, x, y;
    register inven_type  *i_ptr;

    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_STAFF, TV_NEVER, &j, &k)) {
	msg_print("You are not carrying any staffs.");
	return;
    }
    
    /* Get a staff */
    if (get_item(&item_val, "Use which staff?", j, k, 0)) {

    
    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Chance of success */
    chance = p_ptr->misc.save + stat_adj(A_INT) - (int)(i_ptr->level > 50 ? 50 : i_ptr->level) +
	      (class_level_adj[p_ptr->misc.pclass][CLA_DEVICE] * p_ptr->misc.lev / 3);

    if (p_ptr->flags.confused > 0) chance = chance / 2;

    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;   /* Give everyone a slight chance */
    }

    if (chance <= 0) chance = 1;

    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the staff properly.");
	return;
    }

    if (i_ptr->p1 > 0) {
    i = i_ptr->flags;

    ident = FALSE;
    (i_ptr->p1)--;

    switch (i) {

      case ST_HEALING:
	ident = hp_player(300);
	if (p_ptr->flags.stun > 0) {
		    if (p_ptr->flags.stun > 50) {
			p_ptr->misc.ptohit += 20;
			p_ptr->misc.ptodam += 20;
		    } else {
			p_ptr->misc.ptohit += 5;
			p_ptr->misc.ptodam += 5;
		    }
		    p_ptr->flags.stun = 0;
			ident = TRUE;
		    msg_print("Your head stops stinging.");
		}
		if (p_ptr->flags.cut > 0) {
		    p_ptr->flags.cut = 0;
		    ident = TRUE;
		    msg_print("You feel better.");
		}
		break;

      case ST_GENOCIDE:
	genocide(FALSE);
	ident = TRUE;
	break;

      case ST_PROBE:
	probing();
	ident = TRUE;
	break;

      case ST_IDENTIFY:

    ident_spell();
	ident = TRUE;
	break;

      case ST_HOLYNESS:
	dispel_creature(EVIL, 120);
	protect_evil();
	cure_poison();
	remove_fear();
	hp_player(50);
	if (p_ptr->flags.stun > 0) {
	    if (p_ptr->flags.stun > 50) {
		p_ptr->misc.ptohit += 20;
		p_ptr->misc.ptodam += 20;
	    } else {
		p_ptr->misc.ptohit += 5;
		p_ptr->misc.ptodam += 5;
	    }
	    p_ptr->flags.stun = 0;
	    ident = TRUE;
	    msg_print("Your head stops stinging.");
	}
	if (p_ptr->flags.cut > 0) {
	    p_ptr->flags.cut = 0;
	    ident = TRUE;
	    msg_print("You feel better.");
	}
	ident = TRUE;
	break;

      case ST_MAGI:
	if (res_stat(A_INT)) {
	    msg_print("You have a warm feeling.");
	    ident = TRUE;
	}
	if (p_ptr->misc.cmana < p_ptr->misc.mana) {
	    p_ptr->misc.cmana = p_ptr->misc.mana;
	    ident = TRUE;
	    msg_print("Your feel your head clear.");
	    prt_cmana();
	}
	break;

      case ST_POWER:
	dispel_creature(0xFFFFFFFFL, 120);
	break;

      case ST_SURROUND:
	map_area();
	ident = TRUE;
	break;

      case ST_LIGHT:
	ident = lite_area(char_row, char_col, damroll(2, 10), 2);
	break;

      case ST_DR_LC:
	ident = detect_sdoor();
	break;

      case ST_TRP_LC:
	ident = detect_trap();
	break;

      case ST_TRE_LC:
	ident = detect_treasure();
	break;

      case ST_OBJ_LC:
	ident = detect_object();
	break;

      case ST_TELE:
	teleport(100);
	ident = TRUE;
	break;

      case ST_EARTH:
	ident = TRUE;
	earthquake();
	break;

      case ST_SUMMON:
	ident = FALSE;
	for (k = 0; k < randint(4); k++) {
	    y = char_row;
	    x = char_col;
	    ident |= summon_monster(&y, &x, FALSE);
	}
	break;

      case ST_DEST:
	ident = TRUE;
	destroy_area(char_row, char_col);
	break;

      case ST_STAR:
	ident = TRUE;
	starlite(char_row, char_col);
	break;

      case ST_HAST_MN:
	ident = speed_monsters(1);
	break;

      case ST_SLOW_MN:
	ident = speed_monsters(-1);
	break;

      case ST_SLEE_MN:
	ident = sleep_monsters2();
	break;

      case ST_CURE_LT:
	ident = hp_player(randint(8));
	break;

      case ST_DET_INV:
	ident = detect_invisible();
	break;

      case ST_SPEED:
	if (p_ptr->flags.fast == 0) ident = TRUE;
	if (p_ptr->flags.fast <= 0)
	    p_ptr->flags.fast += randint(30) + 15;
	else
	    p_ptr->flags.fast += randint(5);
	break;

      case ST_SLOW:
	if (p_ptr->flags.slow == 0) ident = TRUE;
	p_ptr->flags.slow += randint(30) + 15;
	break;

      case ST_REMOVE:
	if (remove_curse()) {
	    if (p_ptr->flags.blind < 1)
		msg_print("The staff glows blue for a moment..");
	    ident = TRUE;
	}
	break;

      case ST_DET_EVI:
	ident = detect_evil();
	break;

      case ST_CURING:
	if ((cure_blindness())
	|| (cure_poison())
	|| (cure_confusion()) 
	|| (p_ptr->flags.stun > 0)
	|| (p_ptr->flags.cut > 0)) {
	    ident = TRUE;
	}
	if (p_ptr->flags.stun > 0) {
	    if (p_ptr->flags.stun > 50) {
		p_ptr->misc.ptohit += 20;
		p_ptr->misc.ptodam += 20;
	    } else {
		p_ptr->misc.ptohit += 5;
		p_ptr->misc.ptodam += 5;
	    }
	    p_ptr->flags.stun = 0;
	    msg_print("Your head stops stinging.");
	}
	else if (p_ptr->flags.cut > 0) {
	    p_ptr->flags.cut = 0;
	    msg_print("You feel better.");
	}
	break;

      case ST_DSP_EVI:
	ident = dispel_creature(EVIL, 60);
	break;

      case ST_DARK:
	ident = unlite_area(char_row, char_col);
	break;

      default:
	msg_print("Internal error in staffs()");
	break;
    }

    /* An identification was made */
	    if (ident) {
		if (!known1_p(i_ptr)) {
		/* round half-way case up */
		    p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) /
			p_ptr->misc.lev;
		    prt_experience();

		    identify(&item_val);
		    i_ptr = &inventory[item_val];
		}
	    } else if (!known1_p(i_ptr))
		sample(i_ptr);
	    desc_charges(item_val);
	} else {
	    msg_print("The staff has no charges left.");
	    if (!known2_p(i_ptr))
		add_inscribe(i_ptr, ID_EMPTY);
	}
    }
}

/*
 * rods.c : rod code 
 *
 * Copyright (c) 1989 Andrew Astrand 1991 
 *
 * Do what you like with it 
 *
 * I will sucker! ~Ludwig 
 */


static int direction(int *dir)
{
    if (get_dir(NULL, dir)) {
	if (p_ptr->flags.confused > 0) {
	    msg_print("You are confused.");
	    do {
		*dir = randint(9);
	    } while (*dir == 5);
	}
	return 1;
    }
    return 0;
}





/*
 * Activate (zap) a Rod
 */
void activate_rod(void)
{
    u32b              i;
    int                 ident, chance, dir, l;
    int                 item_val, j, k;
    inven_type		*i_ptr;

    /* Assume free turn */
    free_turn_flag = TRUE;

    if (!inven_ctr) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_ROD, TV_NEVER, &j, &k)) {
	msg_print("You are not carrying any rods.");
	return;
    }

    /* Get a rod */
    if (get_item(&item_val, "Activate which rod?", j, k, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;
	ident = FALSE;

    /* Calculate the chance */
    chance = (p_ptr->misc.save + (stat_adj(A_INT) * 2) - (int)((i_ptr->level > 70) ? 70 : i_ptr->level) +
	      (class_level_adj[p_ptr->misc.pclass][CLA_DEVICE] * p_ptr->misc.lev / 3));

    if (p_ptr->flags.confused > 0) chance = chance / 2;

    /* Give everyone a slight chance */
    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;
    }

    /* Prevent errors in "randint" */
    if (chance <= 0) chance = 1;

    /* Fail to use */
    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the rod properly.");
	return;
    }

    if (i_ptr->timeout <= 0) {
	    i = i_ptr->flags;

    /* Starting location */
    k = char_row;
    l = char_col;

    /* Activate it */
    switch (i) {

      case RD_LT:
	if (!direction(&dir)) goto no_charge;
	msg_print("A line of blue shimmering light appears.");
	lite_line(dir, char_row, char_col);
	ident = TRUE;
	i_ptr->timeout = 9;
	break;

      case RD_ILLUME:
	lite_area(k, l, damroll(2, 8), 2);
	ident = TRUE;
	i_ptr->timeout = 30;
	break;

      case RD_AC_BLTS:	   /* Acid , New */
	if (!direction(&dir)) goto no_charge;
	if (randint(10)==1)
	    line_spell(GF_ACID,dir,k,l,damroll(6,8));
	else
	    fire_bolt(GF_ACID,dir,k,l,damroll(6,8));
	ident = TRUE;
	i_ptr->timeout = 12;
	break;

      case RD_LT_BLTS:	   /* Lightning */
	if (!direction(&dir)) goto no_charge;
	if (randint(12)==1)
	    line_spell(GF_ELEC, dir, k, l, damroll(3, 8));
	else
	    fire_bolt(GF_ELEC, dir, k, l, damroll(3, 8));
	ident = TRUE;
	i_ptr->timeout = 11;
	break;

      case RD_FT_BLTS:	   /* Frost */
	if (!direction(&dir)) goto no_charge;
	if (randint(10)==1)
	    line_spell(GF_COLD, dir, k, l, damroll(5, 8));
	else
	    fire_bolt(GF_COLD, dir, k, l, damroll(5, 8));
	ident = TRUE;
	i_ptr->timeout = 13;
	break;

      case RD_FR_BLTS:	   /* Fire */
	if (!direction(&dir)) goto no_charge;
	if (randint(8)==1)
	    line_spell(GF_FIRE, dir, k, l, damroll(8, 8));
	else
	    fire_bolt(GF_FIRE, dir, k, l, damroll(8, 8));
	ident = TRUE;
	i_ptr->timeout = 15;
	break;

      case RD_POLY:
	if (!direction(&dir)) goto no_charge;
	ident = poly_monster(dir, k, l);
	i_ptr->timeout = 25;
	break;

      case RD_SLOW_MN:
	if (!direction(&dir)) goto no_charge;
	ident = speed_monster(dir, k, l, -1);
	i_ptr->timeout = 20;
	break;

      case RD_SLEE_MN:
	if (!direction(&dir)) goto no_charge;
	ident = sleep_monster(dir, k, l);
	i_ptr->timeout = 18;
	break;

      case RD_DRAIN:
	if (!direction(&dir)) goto no_charge;
	ident = drain_life(dir, k, l, 75);
	i_ptr->timeout = 23;
	break;

      case RD_TELE:
	if (!direction(&dir)) goto no_charge;
	ident = teleport_monster(dir, k, l);
	i_ptr->timeout = 25;
	break;

      case RD_DISARM:
	if (!direction(&dir)) goto no_charge;
	ident = disarm_all(dir, k, l);
	i_ptr->timeout = 30;
	break;

      case RD_LT_BALL:
	if (!direction(&dir)) goto no_charge;
	fire_ball(GF_ELEC, dir, k, l, 32, 2);
	ident = TRUE;
	i_ptr->timeout = 23;
	break;

      case RD_CD_BALL:
	if (!direction(&dir)) goto no_charge;
	fire_ball(GF_COLD, dir, k, l, 48, 2);
	ident = TRUE;
	i_ptr->timeout = 25;
	break;

      case RD_FR_BALL:
	if (!direction(&dir)) goto no_charge;
	fire_ball(GF_FIRE, dir, k, l, 72, 2);
	ident = TRUE;
	i_ptr->timeout = 30;
	break;

      case RD_AC_BALL:
	if (!direction(&dir)) goto no_charge;
	fire_ball(GF_ACID, dir, k, l, 60, 2);
	ident = TRUE;
	i_ptr->timeout = 27;
	break;

      case RD_MAPPING:
	map_area();
	ident = TRUE;
	i_ptr->timeout = 99;
	break;

      case RD_IDENT:
	ident_spell();

	/* We know what it is now */
	ident = TRUE;
	i_ptr->timeout = 10;
	break;

      case RD_CURE:
	if ((cure_blindness())
	|| (cure_poison())
	|| (cure_confusion())
	|| (p_ptr->flags.stun > 0) ||
	(p_ptr->flags.cut > 0)) {
	    ident = TRUE;
	}
	if (p_ptr->flags.stun > 0) {
	    if (p_ptr->flags.stun > 50) {
		p_ptr->misc.ptohit += 20;
		p_ptr->misc.ptodam += 20;
	    } else {
		p_ptr->misc.ptohit += 5;
		p_ptr->misc.ptodam += 5;
	    }
	    p_ptr->flags.stun = 0;
	    ident = TRUE;
	    msg_print("Your head stops stinging.");
	}
	else if (p_ptr->flags.cut > 0) {
	    msg_print("You feel better.");
	    p_ptr->flags.cut = 0;
	    ident = TRUE;
	}
	i_ptr->timeout = 888;
	break;

      case RD_HEAL:
	ident = hp_player(500);
	if (p_ptr->flags.stun > 0) {
	    if (p_ptr->flags.stun > 50) {
		p_ptr->misc.ptohit += 20;
		p_ptr->misc.ptodam += 20;
	    } else {
		p_ptr->misc.ptohit += 5;
		p_ptr->misc.ptodam += 5;
	    }
	    p_ptr->flags.stun = 0;
	    ident = TRUE;
	    msg_print("Your head stops stinging.");
	}
	if (p_ptr->flags.cut > 0) {
	    msg_print("You feel better.");
	    p_ptr->flags.cut = 0;
	    ident = TRUE;
	}
	i_ptr->timeout = 888;
	break;

      case RD_RECALL:
	if (p_ptr->flags.word_recall == 0) {
	    msg_print("The air about you becomes charged...");
	    p_ptr->flags.word_recall = 15 + randint(20);
	}
	else {
	    p_ptr->flags.word_recall = 0;
	    msg_print("A tension leaves the air around you...");
	}
	ident = TRUE;
	i_ptr->timeout = 60;
	break;

      case RD_PROBE:
	probing();
	ident = TRUE;
	i_ptr->timeout = 50;
	break;

      case RD_DETECT:
	detection();
	ident = TRUE;
	i_ptr->timeout = 99;
	break;

      case RD_RESTORE:
	if (restore_level()
	|| res_stat(A_STR)
	|| res_stat(A_INT)
	|| res_stat(A_WIS)
	|| res_stat(A_DEX)
	|| res_stat(A_CON)
	|| res_stat(A_CHR)) ident = TRUE;
	i_ptr->timeout = 999;
	break;

      case RD_SPEED:
	if (p_ptr->flags.fast == 0) ident = TRUE;
	p_ptr->flags.fast += randint(30) + 15;
	i_ptr->timeout = 99;
	break;

      case RD_TRAP_LOC:
	if (detect_trap()) ident = TRUE;
	i_ptr->timeout = 99;	/* fairly long timeout because rod so low lv -CFT */
	break;

#if 0
      case RD_MK_WALL:	   /* JLS */
	if (!direction(&dir)) goto no_charge;
	ident = build_wall(dir, k, l);
	/* don't want people to abuse this -JLS */
	i_ptr->timeout = 999;
	break;
#endif

      default:
	msg_print("Oops.  Undefined rod.");
	break;
    }

    /* Successfully determined the object function */
    if (ident) {
	if (!known1_p(i_ptr)) {
	/* round half-way case up */
	p_ptr->misc.exp += (i_ptr->level + (p_ptr->misc.lev >> 1)) / p_ptr->misc.lev;
	prt_experience();

	identify(&item_val);
	i_ptr = &inventory[item_val];
	}
    } else if (!known1_p(i_ptr)) {
	sample(i_ptr);
    }

    no_charge:;
	} else {
	    msg_print("The rod is currently exhausted.");
	}
    }
}


static void activate(void)
{
    int          i, a, flag, first, num, j, redraw, dir, test = FALSE;
    char         out_str[200], tmp[200], tmp2[200], choice;
    inven_type  *i_ptr;

    flag = FALSE;
    redraw = FALSE;
    num = 0;
    first = 0;
    for (i = 22; i < (INVEN_TOTAL - 1); i++) {
	if ((inventory[i].flags2 & TR_ACTIVATE) && (known2_p(&(inventory[i])))) {
	    num++;
	    if (!flag)
		first = i;
	    flag = TRUE;
	}
    }

    /* Nothing found */
    
    if (!flag) {
	msg_print("You are not wearing/wielding anything that can be activated.");
	free_turn_flag = TRUE;
	return;
    }
    sprintf(out_str, "Activate which item? (%c-%c, * to list, ESC to exit) ?",
	    'a', 'a' + (num - 1));
    flag = FALSE;
    while (!flag){
	if (!get_com(out_str, &choice))  /* on escape, get_com returns false: */
	    choice = '\033';             /* so it's set here to ESC.  Wierd huh? -CFT */

	if ((choice=='*') && !redraw) {  /* don't save screen again if it's already
					  * listed, OW it doesn't clear -CFT */
	    save_screen();
	    j=0;
	    if (!redraw) {
		for (i = first; i < (INVEN_TOTAL - 1); i++) {
		    if ((inventory[i].flags2 & TR_ACTIVATE) &&
			known2_p(&(inventory[i]))) {
			objdes(tmp2, &inventory[i], TRUE);
			sprintf(tmp, "%c) %-61s", 'a' + j, tmp2);
			erase_line(1 + j, 13);
/* we display at 15, but erase from 13 to give a couple of spaces,
 * so it looks tidy.  -CFT */

			sprintf(tmp, "%c) %-40s", 'a' + j, tmp2);
			prt(tmp, 1 + j, 15);
/* No need to check for bottom of screen, since only have 11 items in equip,
 * so will never reach bottom... -CFT */
			j++;
		    }
		}
		redraw = TRUE;
		continue;
	    }
	} else {
	    if (choice >= 'A' && choice <= ('A' + (num - 1))) {
		choice -= 'A';
		test = TRUE; /* test to see if he means it */
	    }
	    else if (choice >= 'a' && choice <= ('a' + (num - 1)))
		choice -= 'a';
	    else if (choice == '\033') {
		if (redraw) {
		    restore_screen();
		    redraw = FALSE;
		}
		free_turn_flag = TRUE;
		break;
	    } else {
		bell();
		continue;	   /* try another letter */
	    }

	    if (redraw) {
		restore_screen();
		redraw = FALSE;
	    }
	    if (choice > num)
		continue;
	    flag = TRUE;
	    j = 0;
	    for (i = first; i < (INVEN_TOTAL - 1); i++) {
		if ((inventory[i].flags2 & TR_ACTIVATE) && known2_p(&(inventory[i]))) {
		    if (j == choice)
			break;
		    j++;
		}
	    }

	    if ( (test && verify("Activate", i)) || !test)
		flag = TRUE;
	    else {
		flag = TRUE;           /* exit loop, he didn't want to try it... */
		free_turn_flag = TRUE; /* but he didn't do anything either */
		continue;
	    }

	    if (inventory[i].timeout > 0) {
		msg_print("It whines, glows and fades...");
		break;
	    }
	    if (p_ptr->stats.use_stat[A_INT] < randint(18) &&
	     randint(object_list[inventory[i].index].level) > p_ptr->misc.lev) {
		msg_print("You fail to activate it properly.");
		break;
	    }
	    msg_print("You activate it...");
	    switch (inventory[i].index) {
	      case (29):
	      case (395):
	      case (396):	   /* The dreaded daggers:-> */
	      case (397):
		if (inventory[i].name2 == SN_NARTHANC) {
		    msg_print("Your dagger is covered in fire...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_FIRE, dir, char_row, char_col, damroll(9, 8));
			inventory[i].timeout = 5 + randint(10);
		    }
		} else if (inventory[i].name2 == SN_NIMTHANC) {
		    msg_print("Your dagger is covered in frost...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(6, 8));
			inventory[i].timeout = 4 + randint(8);
		    }
		} else if (inventory[i].name2 == SN_DETHANC) {
		    msg_print("Your dagger is covered in sparks...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_ELEC, dir, char_row, char_col, damroll(4, 8));
			inventory[i].timeout = 3 + randint(7);
		    }
		} else if (inventory[i].name2 == SN_RILIA) {
		    msg_print("Your dagger throbs deep green...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_POIS, dir, char_row, char_col, 12, 3);
			inventory[i].timeout = 3 + randint(3);
		    }
		} else if (inventory[i].name2 == SN_BELANGIL) {
		    msg_print("Your dagger is covered in frost...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_COLD, dir, char_row, char_col, 48, 2);
			inventory[i].timeout = 3 + randint(7);
		    }
		}
		break;
	      case (91):
		if (inventory[i].name2 == SN_DAL) {
		    msg_print("You feel energy flow through your feet...");
		    remove_fear();
		    cure_poison();
		    inventory[i].timeout = 5;
		}
		break;
	      case (42):
	      case (43):
		if (inventory[i].name2 == SN_RINGIL) {
		    msg_print("Your sword glows an intense blue...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_COLD, dir, char_row, char_col, 100, 2);
			inventory[i].timeout = 300;
		    }
		} else if (inventory[i].name2 == SN_ANDURIL) {
		    msg_print("Your sword glows an intense red...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_FIRE, dir, char_row, char_col, 72, 2);
			inventory[i].timeout = 400;
		    }
		}
		break;
	      case (52):
		if (inventory[i].name2 == SN_FIRESTAR) {
		    msg_print("Your morningstar rages in fire...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_FIRE, dir, char_row, char_col, 72, 3);
			inventory[i].timeout = 100;
		    }
		}
		break;
	      case (92):
		if (inventory[i].name2 == SN_FEANOR) {
		    p_ptr->flags.fast += randint(25) + 15;
		    inventory[i].timeout = 200;
		}
		break;
	      case (59):
		if (inventory[i].name2 == SN_THEODEN) {
		    msg_print("The blade of your axe glows black...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			drain_life(dir, char_row, char_col, 120);
			inventory[i].timeout = 400;
		    }
		}
		break;
	      case (62):
		if (inventory[i].name2 == SN_TURMIL) {
		    msg_print("The head of your hammer glows white...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			drain_life(dir, char_row, char_col, 90);
			inventory[i].timeout = 70;
		    }
		}
		break;
	      case (111):
		if (inventory[i].name2 == SN_CASPANION) {
		    msg_print("Your mail magically disarms traps...");
		    td_destroy();
		    inventory[i].timeout = 10;
		}
		break;
	      case (71):
		if (inventory[i].name2 == SN_AVAVIR) {
		    if (p_ptr->flags.word_recall == 0) {
			p_ptr->flags.word_recall = 15 + randint(20);
			msg_print("The air about you becomes charged...");
		    } else {
			p_ptr->flags.word_recall = 0;
			msg_print("A tension leaves the air around you...");
		    }
		    inventory[i].timeout = 200;
		}
		break;
	      case (53):
		if (inventory[i].name2 == SN_TARATOL) {
		    if (p_ptr->flags.fast == 0)
			p_ptr->flags.fast += randint(30) + 15;
		    inventory[i].timeout = 166;
		}
		break;
	      case (54):
		if (inventory[i].name2 == SN_ERIRIL) {
		    ident_spell();
		    inventory[i].timeout = 10;
		} else if (inventory[i].name2 == SN_OLORIN) {
		    probing();
		    inventory[i].timeout = 20;
		}
		break;
	      case (67):
		if (inventory[i].name2 == SN_EONWE) {
		    msg_print("Your axe lets out a long, shrill note...");
		    mass_genocide(TRUE);
		    inventory[i].timeout = 1000;
		}
		break;
	      case (68):
		if (inventory[i].name2 == SN_LOTHARANG) {
		    msg_print("Your battle axe radiates deep purple...");
		    hp_player(damroll(4, 7));
		    if (p_ptr->flags.cut > 0) {
			p_ptr->flags.cut = (p_ptr->flags.cut / 2) - 50;
			if (p_ptr->flags.cut < 0)
			    p_ptr->flags.cut = 0;
			msg_print("You wounds heal.");
		    }
		    inventory[i].timeout = 2 + randint(2);
		}
		break;
	      case (75):
		if (inventory[i].name2 == SN_CUBRAGOL) {
		    for (a = 0; a < INVEN_WIELD; a++)
/* search for bolts that are not cursed and are not already named -CWS */
			if ((inventory[a].tval == TV_BOLT) &&
			    !(inventory[a].flags & TR_CURSED) &&
			    (inventory[a].name2 == SN_NULL))
			    break;
		    if (a < INVEN_WIELD) {
			i_ptr = &inventory[a];
			msg_print("Your bolts are covered in a fiery aura!");
			i_ptr->name2 = SN_FIRE;
			i_ptr->flags |= (TR1_BRAND_FIRE|TR2_RES_FIRE);
			i_ptr->cost += 25;
			enchant(i_ptr, 3+randint(3), ENCH_TOHIT|ENCH_TODAM);
			calc_bonuses();
		    } else {
			msg_print("The fiery enchantment fails.");
		    }
		    inventory[i].timeout = 999;
		}
		break;
	      case (34):
	      case (35):
		if (inventory[i].name2 == SN_ARUNRUTH) {
		    msg_print("Your sword glows a pale blue...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(12, 8));
			inventory[i].timeout = 500;
		    }
		}
		break;
	      case (64):
		if (inventory[i].name2 == SN_AEGLOS) {
		    msg_print("Your spear glows a bright white...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_COLD, dir, char_row, char_col, 100, 2);
			inventory[i].timeout = 500;
		    }
		} else if (inventory[i].name2 == SN_OROME) {
		    msg_print("Your spear pulsates...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			wall_to_mud(dir, char_row, char_col);
			inventory[i].timeout = 5;
		    }
		}
		break;
	      case (118):
		if (inventory[i].name2 == SN_SOULKEEPER) {
		    msg_print("Your armour glows a bright white...");
		    msg_print("You feel much better...");
		    hp_player(1000);
		    inventory[i].timeout = 888;
		}
		break;
	      case (120):
		if (inventory[i].name2 == SN_BELEGENNON) {
		    teleport(10);
		    inventory[i].timeout = 2;
		}
		break;
	      case (119):
		if (inventory[i].name2 == SN_CELEBORN) {
		    genocide(TRUE);
		    inventory[i].timeout = 500;
		}
		break;
	      case (124):
		if (inventory[i].name2 == SN_LUTHIEN) {
		    restore_level();
		    inventory[i].timeout = 450;
		}
		break;
	      case (65):
		if (inventory[i].name2 == SN_ULMO) {
		    msg_print("Your trident glows deep red...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			teleport_monster(dir, char_row, char_col);
			inventory[i].timeout = 150;
		    }
		}
		break;
	      case (123):	   /* Cloak */
	      case (411):
		if (inventory[i].name2 == SN_COLLUIN) {
		    msg_print("Your cloak glows many colours...");
		    msg_print("You feel you can resist anything.");
		    p_ptr->flags.oppose_fire += randint(20) + 20;
		    p_ptr->flags.oppose_cold += randint(20) + 20;
		    p_ptr->flags.oppose_elec += randint(20) + 20;
		    p_ptr->flags.oppose_pois += randint(20) + 20;
		    p_ptr->flags.oppose_acid += randint(20) + 20;
		    inventory[i].timeout = 111;
		} else if (inventory[i].name2 == SN_HOLCOLLETH) {
		    msg_print("You momentarily disappear...");
		    sleep_monsters1(char_row, char_col);
		    inventory[i].timeout = 55;
		} else if (inventory[i].name2 == SN_THINGOL) {
		    msg_print("You hear a low humming noise...");
		    recharge(60);
		    inventory[i].timeout = 70;
		} else if (inventory[i].name2 == SN_COLANNON) {
		    teleport(100);
		    inventory[i].timeout = 45;
		}
		break;
	      case (50):	   /* Flail */
		if (inventory[i].name2 == SN_TOTILA) {
		    msg_print("Your flail glows in scintillating colours...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			confuse_monster(dir, char_row, char_col, 20);
			inventory[i].timeout = 15;
		    }
		}
		break;
	      case (125):	   /* Gloves */
		if (inventory[i].name2 == SN_CAMMITHRIM) {
		    msg_print("Your gloves glow extremely brightly...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_MAGIC_MISSILE, dir, char_row, char_col,
				  damroll(2, 6));
			inventory[i].timeout = 2;
		    }
		}
		break;
	      case (126):	   /* Gauntlets */
		if (inventory[i].name2 == SN_PAURHACH) {
		    msg_print("Your gauntlets are covered in fire...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			if (randint(4)==1)
			    line_spell(GF_FIRE, dir, char_row, char_col, damroll(9,8));
			else
			    fire_bolt(GF_FIRE, dir, char_row, char_col, damroll(9,8));
			inventory[i].timeout = 5 + randint(10);
		    }
		} else if (inventory[i].name2 == SN_PAURNIMMEN) {
		    msg_print("Your gauntlets are covered in frost...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(6, 8));
			inventory[i].timeout = 4 + randint(8);
		    }
		} else if (inventory[i].name2 == SN_PAURAEGEN) {
		    msg_print("Your gauntlets are covered in sparks...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_ELEC, dir, char_row, char_col, damroll(4, 8));
			inventory[i].timeout = 3 + randint(7);
		    }
		} else if (inventory[i].name2 == SN_PAURNEN) {
		    msg_print("Your gauntlets look very acidic...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_ACID, dir, char_row, char_col, damroll(5, 8));
			inventory[i].timeout = 4 + randint(7);
		    }
		}
		break;
	      case (127):
		if (inventory[i].name2 == SN_FINGOLFIN) {
		    msg_print("Magical spikes appear on your cesti...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_ARROW, dir, char_row, char_col, 150);
			inventory[i].timeout = 88 + randint(88);
		    }
		}
		break;
	      case (96):
		if (inventory[i].name2 == SN_HOLHENNETH) {
		    msg_print("You close your eyes and an image forms in your mind...");
		    detection();
		    inventory[i].timeout = 55 + randint(55);
		}
		break;
	      case (99):
		if (inventory[i].name2 == SN_GONDOR) {
		    msg_print("You feel a warm tingling inside...");
		    hp_player(500);
		    inventory[i].timeout = 500;
		}
		break;
	      case (SPECIAL_OBJ - 1):	/* Narya */
		msg_print("The ring glows deep red...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_FIRE, dir, char_row, char_col, 120, 3);
		    inventory[i].timeout = 222 + randint(222);
		}
		break;
	      case (SPECIAL_OBJ): /* Nenya */
		msg_print("The ring glows bright white...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_COLD, dir, char_row, char_col, 200, 3);
		    inventory[i].timeout = 222 + randint(333);
		}
		break;
	      case (SPECIAL_OBJ + 1):	/* Vilya */
		msg_print("The ring glows deep blue...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_ELEC, dir, char_row, char_col, 250, 3);
		    inventory[i].timeout = 222 + randint(444);
		}
		break;
	      case (SPECIAL_OBJ + 2):	/* Power */
		msg_print("The ring glows intensely black...");
		switch (randint(17) + (8 - p_ptr->misc.lev / 10)) {
		  case 5:
		    dispel_creature(0xFFFFFFFL, 1000);
		    break;
		  case 6:
		  case 7:
		    msg_print("You are surrounded by a malignant aura");
		    p_ptr->misc.lev--;
		    p_ptr->misc.exp = player_exp[p_ptr->misc.lev - 2] * p_ptr->misc.expfact / 100 +
			randint((player_exp[p_ptr->misc.lev - 1] * p_ptr->misc.expfact / 100) -
		       (player_exp[p_ptr->misc.lev - 2] * p_ptr->misc.expfact / 100));
		    p_ptr->misc.max_exp = p_ptr->misc.exp;
		    prt_experience();
		    ruin_stat(A_STR);
		    ruin_stat(A_INT);
		    ruin_stat(A_WIS);
		    ruin_stat(A_DEX);
		    ruin_stat(A_CON);
		    ruin_stat(A_CHR);
		    calc_hitpoints();
		    if (class[p_ptr->misc.pclass].spell == MAGE) {
			calc_spells(A_INT);
			calc_mana(A_INT);
		    } else if (class[p_ptr->misc.pclass].spell == PRIEST) {
			calc_spells(A_WIS);
			calc_mana(A_WIS);
		    }
		    prt_level();
		    prt_title();
		    take_hit((p_ptr->misc.chp > 2) ? p_ptr->misc.chp / 2 : 0, "malignant aura");
		    break;
		  case 8:
		  case 9:
		  case 10:
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_MANA, dir, char_row, char_col, 300, 3);
		    }
		    break;
		  default:
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_bolt(GF_MANA, dir, char_row, char_col, 250);
		    }
		}
		inventory[i].timeout = 444 + randint(444);
		break;
	      case (389):	   /* Blue */
		msg_print("You breathe lightning...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_ELEC, dir, char_row, char_col, 100, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (390):	   /* White */
		msg_print("You breathe frost...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_COLD, dir, char_row, char_col, 110, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (391):	   /* Black */
		msg_print("You breathe acid...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_ACID, dir, char_row, char_col, 130, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (392):	   /* Gas */
		msg_print("You breathe poison gas...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_POIS, dir, char_row, char_col, 150, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (393):	   /* Fire */
		msg_print("You breathe fire...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_FIRE, dir, char_row, char_col, 200, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (394):	   /* Multi-hued */
		if (inventory[i].name2 == SN_RAZORBACK) {
		    msg_print("A storm of lightning spikes fires in all directions...");
		    starball(char_row, char_col);
		    inventory[i].timeout = 1000;
		} else {
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			choice = randint(5);
			sprintf(tmp2, "You breathe %s...",
				((choice == 1) ? "lightning" :
				 ((choice == 2) ? "frost" :
				  ((choice == 3) ? "acid" :
				((choice == 4) ? "poison gas" : "fire")))));
			msg_print(tmp2);
			fire_ball(((choice == 1) ? GF_ELEC :
				   ((choice == 2) ? GF_COLD :
				    ((choice == 3) ? GF_ACID :
			       ((choice == 4) ? GF_POIS : GF_FIRE)))),
				  dir, char_row, char_col, 250, 2);
			inventory[i].timeout = 222 + randint(222);
		    }
		}
		break;
	      case (408):	   /* Bronze */
		msg_print("You breathe confusion...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_CONFUSION, dir, char_row, char_col, 120, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (409):	   /* Gold */
		msg_print("You breathe sound...");
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    fire_ball(GF_SOUND, dir, char_row, char_col, 130, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;
	      case (415):	   /* Chaos */
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "chaos" : "disenchantment")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_CHAOS : GF_DISENCHANT), dir,
			      char_row, char_col, 220, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;
	      case (416):	   /* Law */
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "sound" : "shards")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_SOUND : GF_SHARDS), dir,
			      char_row, char_col, 230, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;
	      case (417):	   /* Balance */
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    choice = randint(4);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1) ? "chaos" :
			     ((choice == 2) ? "disenchantment" :
			      ((choice == 3) ? "sound" : "shards"))));
		    msg_print(tmp2);
		    fire_ball(((choice == 1) ? GF_CHAOS :
			       ((choice == 2) ? GF_DISENCHANT :
				((choice == 3) ? GF_SOUND : GF_SHARDS))),
			      dir, char_row, char_col, 250, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;
	      case (418):	   /* Shining */
		if (get_dir(NULL, &dir)) {
		    if (p_ptr->flags.confused > 0) {
			msg_print("You are confused.");
			do {
			    dir = randint(9);
			} while (dir == 5);
		    }
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "light" : "darkness")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_LIGHT : GF_DARK), dir,
			      char_row, char_col, 200, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;
	      case (419):	   /* Power Dragon Scale Mail */
		if (inventory[i].name2 == SN_BLADETURNER) {
		    msg_print("Your armour glows many colours...");
		    msg_print("You enter a berserk rage...");
		    p_ptr->flags.hero += randint(50) + 50;
		    p_ptr->flags.shero += randint(50) + 50;
		    bless(randint(50) + 50);
		    p_ptr->flags.oppose_fire += randint(50) + 50;
		    p_ptr->flags.oppose_cold += randint(50) + 50;
		    p_ptr->flags.oppose_elec += randint(50) + 50;
		    p_ptr->flags.oppose_acid += randint(50) + 50;
		    inventory[i].timeout = 400;
		} else {
		    msg_print("You breathe the elements...");
		    if (get_dir(NULL, &dir)) {
			if (p_ptr->flags.confused > 0) {
			    msg_print("You are confused.");
			    do {
				dir = randint(9);
			    } while (dir == 5);
			}
			fire_ball(GF_MAGIC_MISSILE, dir, char_row, char_col, 300, 2);
			inventory[i].timeout = 300 + randint(300);
		    }
		}
		break;
	      case (SPECIAL_OBJ + 3):
		msg_print("The phial wells with clear light...");
		lite_area(char_row, char_col, damroll(2, 15), 3);
		inventory[i].timeout = 10 + randint(10);
		break;
	      case (SPECIAL_OBJ + 4):
		msg_print("An aura of good floods the area...");
		dispel_creature(EVIL, (int)(5 * p_ptr->misc.lev));
		inventory[i].timeout = 444 + randint(222);
		break;
	      case (SPECIAL_OBJ + 5):
		msg_print("The amulet lets out a shrill wail...");
		msg_print("You feel somewhat safer...");
		protect_evil();
		inventory[i].timeout = 222 + randint(222);
		break;
	      case (SPECIAL_OBJ + 6):
		msg_print("The star shines brightly...");
		msg_print("And you sense your surroundings...");
		map_area();
		inventory[i].timeout = 50 + randint(50);
		break;
	      case (SPECIAL_OBJ + 7):
		msg_print("The stone glows a deep green");
		wiz_lite(TRUE);
		(void)detect_sdoor();
		(void)detect_trap();
		inventory[i].timeout = 100 + randint(100);
		break;
	      case (SPECIAL_OBJ + 8):
		msg_print("The ring glows brightly...");
		p_ptr->flags.fast += randint(100) + 50;
		inventory[i].timeout = 200;
		break;
	      default:
		(void)sprintf(tmp2, "Inventory num %d, index %d", i,
			      inventory[i].index);
		msg_print(tmp2);
	    }
	}
    }

    if (redraw) {
	restore_screen();
	redraw = FALSE;
    }
    if (!flag)			   /* if flag still false, then user aborted. 
				    * So we don't charge him a turn. -CFT */
	free_turn_flag = TRUE;
}


