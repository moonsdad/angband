/*
 * scrolls.c: scroll code 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"
#include "monster.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

/* Scrolls for the reading				-RAK-	 */
void 
read_scroll()
{
    u32b                i;
    int                   j, k, item_val, y, x;
    int                   tmp[6], used_up;
    bigvtype              out_val, tmp_str;
    register int          ident, l;
    register inven_type  *i_ptr;
    register struct misc *m_ptr;

    free_turn_flag = TRUE;
    if (py.flags.blind > 0)
	msg_print("You can't see to read the scroll.");
    else if (no_lite())
	msg_print("You have no light to read by.");
    else if (py.flags.confused > 0)
	msg_print("You are too confused to read a scroll.");
    else if (inven_ctr == 0)
	msg_print("You are not carrying anything!");
    else if (!find_range(TV_SCROLL1, TV_SCROLL2, &j, &k))
	msg_print("You are not carrying any scrolls!");
    else if (get_item(&item_val, "Read which scroll?", j, k, 0)) {
	i_ptr = &inventory[item_val];
	free_turn_flag = FALSE;
	used_up = TRUE;
	i = i_ptr->flags;
	ident = FALSE;

	while (i != 0) {
	    j = bit_pos(&i) + 1;
	    if (i_ptr->tval == TV_SCROLL2)
		j += 32;

	/* Scrolls.			 */
	    switch (j) {
	      case 1:
		i_ptr = &inventory[INVEN_WIELD];
		if (i_ptr->tval != TV_NOTHING) {
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		    msg_print(out_val);
		    if (!enchant(i_ptr, 1, ENCH_TOHIT))
			msg_print("The enchantment fails. ");
		    ident = TRUE;
		}
		break;
	      case 2:
		i_ptr = &inventory[INVEN_WIELD];
		if (i_ptr->tval != TV_NOTHING) {
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		    msg_print(out_val);
		    if (!enchant(i_ptr, 1, ENCH_TODAM))
			msg_print("The enchantment fails. ");
		    ident = TRUE;
		}
		break;		
	      case 3:
		k = 0;
		l = 0;
		if (inventory[INVEN_BODY].tval != TV_NOTHING)
		    tmp[k++] = INVEN_BODY;
		if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    tmp[k++] = INVEN_ARM;
		if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    tmp[k++] = INVEN_OUTER;
		if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HANDS;
		if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HEAD;
	    /* also enchant boots */
		if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    tmp[k++] = INVEN_FEET;

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
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		    msg_print(out_val);
		    if (!enchant(i_ptr, 1, ENCH_TOAC))
			msg_print("The enchantment fails. ");
		    ident = TRUE;
		}
		break;
		
	      case 4:
		msg_print("This is an identify scroll.");
		ident = TRUE;
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
		if (py.flags.confuse_monster == 0) {
		    msg_print("Your hands begin to glow.");
		    py.flags.confuse_monster = TRUE;
		    ident = TRUE;
		}
		break;
	      case 12:
		ident = TRUE;
		map_area();
		break;
	      case 13:
		ident = sleep_monsters1(char_row, char_col);
		break;
	      case 14:
		ident = TRUE;
		warding_glyph();
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
		ident = aggravate_monster(20);
		if (ident)
		    msg_print("There is a high pitched humming noise.");
		break;
	      case 22:
		ident = trap_creation();
		break;
	      case 23:
		ident = td_destroy();
		break;
	      case 24:		   /* Not Used , used to be door creation */
		break;
	      case 25:
		msg_print("This is a Recharge-Item scroll.");
		ident = TRUE;
		used_up = recharge(60);
		break;
	      case 26:
		msg_print("This is a genocide scroll.");
		genocide(TRUE);
		ident = TRUE;
		break;
	      case 27:
		ident = unlite_area(char_row, char_col);
		if (!py.flags.resist_blind)
		    py.flags.blind += 3 + randint(5);
		break;
	      case 28:
		ident = protect_evil();
		break;
	      case 29:
		ident = TRUE;
		create_food();
		break;
	      case 30:
		ident = dispel_creature(UNDEAD, 60);
		break;
	      case 31:
		remove_all_curse();
		ident = TRUE;
		break;
	      case 33:
		i_ptr = &inventory[INVEN_WIELD];
		if (i_ptr->tval != TV_NOTHING) {
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val, "Your %s glows brightly!", tmp_str);
		    msg_print(out_val);
		    if (!enchant(i_ptr, randint(3), ENCH_TOHIT|ENCH_TODAM))
			msg_print("The enchantment fails.");
		    ident = TRUE;
		}
		break;
		
	      case 34:
		i_ptr = &inventory[INVEN_WIELD];
		if (i_ptr->tval != TV_NOTHING) {
		    objdes(tmp_str, i_ptr, FALSE);
		    if ((i_ptr->flags2 & TR_ARTIFACT) && (randint(7) < 4)) {
		msg_print("A terrible black aura tries to surround your weapon,");
		(void)sprintf(out_val, "but your %s resists the effects!", tmp_str);
			msg_print(out_val);
		    } else {	   /* not artifact or failed save... */
		(void)sprintf(out_val, "A terrible black aura blasts your %s!", tmp_str);
			msg_print(out_val);
			py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
			i_ptr->name2 = EGO_SHATTERED;
			i_ptr->tohit = (-randint(5) - randint(5));
			i_ptr->todam = (-randint(5) - randint(5));
			i_ptr->flags = TR_CURSED;
			i_ptr->flags2 = 0;
			i_ptr->damage[0] = i_ptr->damage[1] = 1;
			i_ptr->toac = 0;	/* in case defender... */
			i_ptr->cost = (-1);
			py_bonuses(i_ptr, 1);	/* now apply new "bonuses"
						 * -CFT */
			calc_bonuses();
		    }
		    ident = TRUE;  /* even if artifact makes save... */
		}
		break;
	      case 35:
		k = 0;
		l = 0;
		if (inventory[INVEN_BODY].tval != TV_NOTHING)
		    tmp[k++] = INVEN_BODY;
		if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    tmp[k++] = INVEN_ARM;
		if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    tmp[k++] = INVEN_OUTER;
		if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HANDS;
		if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    tmp[k++] = INVEN_HEAD;
	    /* also enchant boots */
		if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    tmp[k++] = INVEN_FEET;

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
		    objdes(tmp_str, i_ptr, FALSE);
		    (void) sprintf(out_val,"Your %s glows brightly!", tmp_str);
		    msg_print(out_val);
		    if (!enchant(i_ptr, randint(3)+1, ENCH_TOAC))
			msg_print("The enchantment fails.");
		    ident = TRUE;
		}
		break;
	      case 36:
		if ((inventory[INVEN_BODY].tval != TV_NOTHING)
		    && (randint(4) == 1))
		    k = INVEN_BODY;
		else if ((inventory[INVEN_ARM].tval != TV_NOTHING)
			 && (randint(3) == 1))
		    k = INVEN_ARM;
		else if ((inventory[INVEN_OUTER].tval != TV_NOTHING)
			 && (randint(3) == 1))
		    k = INVEN_OUTER;
		else if ((inventory[INVEN_HEAD].tval != TV_NOTHING)
			 && (randint(3) == 1))
		    k = INVEN_HEAD;
		else if ((inventory[INVEN_HANDS].tval != TV_NOTHING)
			 && (randint(3) == 1))
		    k = INVEN_HANDS;
		else if ((inventory[INVEN_FEET].tval != TV_NOTHING)
			 && (randint(3) == 1))
		    k = INVEN_FEET;
		else if (inventory[INVEN_BODY].tval != TV_NOTHING)
		    k = INVEN_BODY;
		else if (inventory[INVEN_ARM].tval != TV_NOTHING)
		    k = INVEN_ARM;
		else if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		    k = INVEN_OUTER;
		else if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		    k = INVEN_HEAD;
		else if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		    k = INVEN_HANDS;
		else if (inventory[INVEN_FEET].tval != TV_NOTHING)
		    k = INVEN_FEET;
		else
		    k = 0;

		if (k > 0) {
		    i_ptr = &inventory[k];
		    objdes(tmp_str, i_ptr, FALSE);
		    if ((i_ptr->flags2 & TR_ARTIFACT) && (randint(7) < 4)) {
			msg_print("A terrible black aura tries to surround your");
		(void)sprintf(out_val, "%s, but it resists the effects!", tmp_str);
			msg_print(out_val);
		    } else {	   /* not artifact or failed save... */
		(void)sprintf(out_val, "A terrible black aura blasts your %s!", tmp_str);
			msg_print(out_val);
			py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
			i_ptr->name2 = EGO_BLASTED;
			i_ptr->flags = TR_CURSED;
			i_ptr->flags2 = 0;
			i_ptr->toac = (-randint(5) - randint(5));
			i_ptr->tohit = i_ptr->todam = 0; /* in case gaunlets of
							 * slaying... */
			i_ptr->ac = (i_ptr->ac > 9) ? 1 : 0;
			i_ptr->cost = (-1);
			py_bonuses(i_ptr, 1);	/* now apply new "bonuses"
						 * -CFT */
			calc_bonuses();
		    }
		    ident = TRUE;  /* even if artifact makes save... */
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
		ident = TRUE;
		bless(randint(12) + 6);
		break;
	      case 39:
		ident = TRUE;
		bless(randint(24) + 12);
		break;
	      case 40:
		ident = TRUE;
		bless(randint(48) + 24);
		break;
	      case 41:
		ident = TRUE;
		if (py.flags.word_recall == 0) {
		    py.flags.word_recall = 15 + randint(20);
		    msg_print("The air about you becomes charged...");
		} else {
		    py.flags.word_recall = 0;
		    msg_print("A tension leaves the air around you...");
		}
		break;
	      case 42:
		destroy_area(char_row, char_col);
		ident = TRUE;
		break;
	      case 43:
		place_good(char_row, char_col, SPECIAL);
		ident = TRUE;
		prt_map();
		break;
	      case 44:
		special_random_object(char_row, char_col, 1);
		ident = TRUE;
		prt_map();
		break;
	      default:
		msg_print("Internal error in scroll()");
		break;
	    }
	/* End of Scrolls.			       */
	}
	i_ptr = &inventory[item_val];
	if (ident) {
	    if (!known1_p(i_ptr)) {
		m_ptr = &py.misc;
	    /* round half-way case up */
		m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
		prt_experience();

		identify(&item_val);
		i_ptr = &inventory[item_val];
	    }
	} else if (!known1_p(i_ptr))
	    sample(i_ptr);
	if (used_up) {
	    inven_item_describe(item_val);
	    inven_destroy(item_val);
	}
    }
}
