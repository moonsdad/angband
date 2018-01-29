/* File: project.c */

/* Purpose: generic bolt/ball/beam code	(-BEN-) */

#include "angband.h"




/*
 * Apply disenchantment to the player's stuff
 *
 * The "mode" is currently unused.
 *
 * Return "TRUE" if the player notices anything
 */
bool apply_disenchant(int mode)
{
    int                t = 0;
    inven_type         *i_ptr;
    vtype               t1, t2;


    /* Pick a random slot */
    switch (randint(8)) {
	 case 1: t = INVEN_WIELD; break;
	 case 2: t = INVEN_AUX; break;
	 case 3: t = INVEN_BODY; break;
	 case 4: t = INVEN_OUTER; break;
	 case 5: t = INVEN_ARM; break;
	 case 6: t = INVEN_HEAD; break;
	 case 7: t = INVEN_HANDS; break;
	 case 8: t = INVEN_FEET; break;
    }

    /* Get the item */                                
    i_ptr = &inventory[t];

    /* No item, nothing happens */
    if (i_ptr->tval == TV_NOTHING) return (FALSE);


    /* Nothing to disenchant */
    if ((i_ptr->tohit <= 0) && (i_ptr->todam <= 0) && (i_ptr->toac <= 0)) {

	/* Nothing to notice */
	return (FALSE);
    }


    /* Describe the object */
    objdes(t1, i_ptr, FALSE);

    /* Artifacts have chance to resist */
    if (artifact_p(i_ptr) && !(randint(5) < 3)) {

	/* Message */
	sprintf(t2, "Your %s (%c) %s disenchantment!",
		t1, t+'a'-INVEN_WIELD,
		(i_ptr->number != 1) ? "resist" : "resists");
	msg_print(t2);

	/* Notice */
	return (TRUE);
    }


    /* Disenchant tohit */
    i_ptr->tohit -= randint(2);
    /* don't send it below zero */
    if (i_ptr->tohit < 0) i_ptr->tohit = 0;

    /* Disenchant todam */
    i_ptr->todam -= randint(2);
    /* don't send it below zero */
    if (i_ptr->todam < 0) i_ptr->todam = 0;

    /* Disenchant toac */
    i_ptr->toac  -= randint(2);
    /* don't send it below zero */
    if (i_ptr->toac < 0) i_ptr->toac = 0;


    sprintf(t2, "Your %s (%c) %s disenchanted!",
	    t1, t+'a'-INVEN_WIELD,
	    (i_ptr->number != 1) ? "were" : "was");
    msg_print(t2);

    /* Recalculate bonuses */
    calc_bonuses();

    /* Notice */
    return (TRUE);
}


/*
 * Apply Nexus
 */
static void apply_nexus(monster_type *m_ptr)
{
    int max1, cur1, max2, cur2, ii, jj;

    switch (randint(7)) {

	case 1: case 2: case 3:

	    teleport(200);
	    break;

	case 4: case 5:

	    teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	    break;

	case 6:

	    if (player_saves()) {
		msg_print("You resist the effects.");
		break;
	    } else {          

	    /* Teleport Level */
	    int k = dun_level;

	    if (dun_level == Q_PLANE) dun_level = 0;
	    else if (is_quest(dun_level)) dun_level -= 1;
	    else dun_level += (-3) + 2 * randint(2);
	    if (dun_level < 0) dun_level = 0;
	    if (k == Q_PLANE)
	        msg_print("You warp through a cross-dimension gate.");
	    else if (k < dun_level)
	        msg_print("You sink through the floor.");
	    else
	        msg_print("You rise up through the ceiling.");
	    new_level_flag = TRUE;
	    } break;

	case 7:

	    if (player_saves() && randint(2) == 1) {
		msg_print("You resist the effects.");
		break;
	    }

	    msg_print("Your body starts to scramble...");

	    /* Pick a pair of stats */
	    ii = randint(6) - 1;
	    for (jj = ii; jj == ii; jj = randint(6) - 1);

	    max1 = p_ptr->max_stat[ii];
	    cur1 = p_ptr->cur_stat[ii];
	    max2 = p_ptr->max_stat[jj];
	    cur2 = p_ptr->cur_stat[jj];

	    p_ptr->max_stat[ii] = max2;
	    p_ptr->cur_stat[ii] = cur2;
	    p_ptr->max_stat[jj] = max1;
	    p_ptr->cur_stat[jj] = cur1;

	    set_use_stat(ii);
	    set_use_stat(jj);
	    prt_stat(ii);
	    prt_stat(jj);

	    break;
    }
}





/*
 * Note that amulets, rods, and high-level spell books are immune
 * to "inventory damage" of any kind.  Also sling ammo and shovels.
 */


/*
 * Does a given class of objects (usually) hate acid?
 */
static bool hates_acid(inven_type *i)
{
    /* Analyze the type */
    switch (i_ptr->tval) {

      /* Wearable items */
      case TV_ARROW:
      case TV_BOLT:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_HELM:
      case TV_SHIELD:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
	return (TRUE);

      /* Staffs/Scrolls are wood/paper */
      case TV_STAFF:
      case TV_SCROLL1:
      case TV_SCROLL2:
	return (TRUE);

      /* Doors are wood */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
	return (TRUE);

      /* Ouch */
      case TV_FOOD:
      case TV_CHEST:
	return (TRUE);

      /* Junk is useless */
      case TV_MISC:
	return (TRUE);
    }

    return (FALSE);
}
static bool set_corrodes(inven_type *i)
{
    /* Analyze the type */
    switch (i_ptr->tval) {

      /* Wearable items */
      case TV_SWORD:
      case TV_HELM:
      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_WAND:
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate electricity?
 */
static bool hates_elec(inven_type *i_ptr)
{
    switch (i_ptr->tval) {

      case TV_RING:
	return (TRUE);
	
      case TV_WAND:
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate fire?
 * Hafted/Polearm weapons have wooden shafts.
 * Arrows/Bows are mostly wooden.
 */
static bool hates_fire(inven_type *i_ptr)
{
    /* Analyze the type */    
    switch (i_ptr->tval) {

      /* Wearable items */
      case TV_LITE:
	if (e->sval >= 192)	   /* only torches... -CFT */
	    return (TRUE);
	else
	    return (FALSE);
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
	return (TRUE);

      /* Hack -- Good Books are Powerful */
      case TV_MAGIC_BOOK:
      case TV_PRAYER_BOOK:
	if (i_ptr->sval < SV_BOOK + 4) return TRUE;
	return (FALSE);

      /* Staffs/Scrolls burn */
      case TV_STAFF:
      case TV_SCROLL1:
      case TV_SCROLL2:
	return (TRUE);

      case TV_POTION1:
      case TV_POTION2:
      case TV_FLASK:
      case TV_FOOD:
	return (TRUE);

      /* Doors are made of wood */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate cold?
 */
static bool hates_cold(inven_type *i_ptr)
{
    switch (i_ptr->tval) {
      case TV_POTION1:
      case TV_POTION2:
      case TV_FLASK:
	return (TRUE);
    }

    return (FALSE);
}









/*
 * Melt something
 */
static int set_acid_destroy(inven_type *i_ptr)
{
    if (!hates_acid(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && (i_ptr->flags3 & TR3_IGNORE_ACID)) return (FALSE);
    return (TRUE);
}


/*
 * Electrical damage
 */
static int set_elec_destroy(inven_type *i_ptr)
{
    if (!hates_elec(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && (i_ptr->flags3 & TR3_IGNORE_ELEC)) return (FALSE);
    return (TRUE);
}


/*
 * Burn something
 */
static int set_fire_destroy(inven_type *i_ptr)
{
    if (!hates_fire(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && (i_ptr->flags3 & TR3_IGNORE_FIRE)) return (FALSE);
    return (TRUE);
}


/*
 * Freeze things
 */
static int set_cold_destroy(inven_type *i_ptr)
{
    if (!hates_cold(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && (i_ptr->flags3 & TR3_IGNORE_COLD)) return (FALSE);
    return (TRUE);
}




/* This seems like a pretty standard "typedef" */
/* For some reason, it was not being used on Unix */
typedef int (*inven_func)(inven_type *);

/*
 * Destroys a type of item on a given percent chance	-RAK-	 
 */
static int inven_damage(inven_func typ, int perc)
{
    register int i, index, offset;
    int		j,
    vtype	tmp_str, out_val;

    j = 0;
    offset = randint(inven_ctr);
    for (index = 0; index < inven_ctr; index++) {
	i = (index + offset) % inven_ctr; /* be clever and not destroy the first item */
	if ((*typ) (&inventory[i]) && (randint(100) < perc)) {
	    objdes(tmp_str, &inventory[i], FALSE);
	    sprintf(out_val, "%sour %s (%c) %s destroyed!",
		    ((inventory[i].sval <= ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked single items */
		    ? "One of y" : "Y",
		    tmp_str, i + 'a',
		    ((inventory[i].sval > ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked group items */
		    ? "were" : "was");
	    msg_print(out_val);
	    inven_destroy(i);
	    j++;
	}
    }

    /* Return the casualty count */
    return (j);
}




/*
 * Acid has hit the player, attempt to affect some armor.
 *
 * AC gets worse					-RAK-
 *
 * Note: This routine affects magical AC bonuses so that stores
 * can detect the damage. But that the "base armor" of an object never changes.
 */
static int minus_ac(void)
{
    inven_type		*i_ptr;
    bigvtype		out_val, tmp_str;


    /* Pick an inventory slot */
    int tmp[6], i = 0;
    if (inventory[INVEN_BODY].tval  != TV_NOTHING) { tmp[i] = INVEN_BODY;  i++; }
    if (inventory[INVEN_ARM].tval   != TV_NOTHING) { tmp[i] = INVEN_ARM;   i++; }
    if (inventory[INVEN_OUTER].tval != TV_NOTHING) { tmp[i] = INVEN_OUTER; i++; }
    if (inventory[INVEN_HANDS].tval != TV_NOTHING) { tmp[i] = INVEN_HANDS; i++; }
    if (inventory[INVEN_HEAD].tval  != TV_NOTHING) { tmp[i] = INVEN_HEAD;  i++; }
    if (inventory[INVEN_FEET].tval  != TV_NOTHING) { tmp[i] = INVEN_FEET;  i++; }

    /* Nothing to damage */
    if (!(i > 0)) return (FALSE);

    i_ptr = &inventory[tmp[randint(i) - 1]];

    /* No damage left to be done */
    if (i_ptr->ac + i_ptr->toac <= 0) return (FALSE);


    /* Object resists? */
    if (i_ptr->flags3 & TR3_IGNORE_ACID) {
	objdes(tmp_str, i_ptr, FALSE);
	(void)sprintf(out_val, "Your %s resists damage!", tmp_str);
	msg_print(out_val);
	return (TRUE);
    }

    /* Describe the damage */
    objdes(tmp_str, i_ptr, FALSE);
    (void)sprintf(out_val, "Your %s is damaged!", tmp_str);
    msg_print(out_val);

    /* Damage the item */
    i_ptr->toac--;
    calc_bonuses();

    /* Item was damaged */
    return (TRUE);
}


/*
 * Throw acid on the hapless victim			-RAK-
 */
void acid_dam(int dam, cptr kb_str)
{
    register int flag;

    if (p_ptr->resist_acid > 0) dam = dam / 3;
    if (p_ptr->oppose_acid > 0) dam = dam / 3;
    if (p_ptr->immune_acid) dam = 1;

    flag = 0;

    if (!p_ptr->oppose_acid)
	if (minus_ac()) flag = 1;
    if (p_ptr->resist_acid) flag += 2;
    inven_damage(set_acid_destroy, 3);
}


/*
 * Lightning bolt the sucker away.			-RAK-
 */
void elec_dam(int dam, cptr kb_str)
{
    if (p_ptr->oppose_elec) dam = dam / 3;
    if (p_ptr->resist_elec) dam = dam / 3;
    if (p_ptr->immune_elec) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_lightning_destroy, 3);
}




/*
 * Burn the fool up.					-RAK-
 */
void fire_dam(int dam, cptr kb_str)
{
    if (p_ptr->resist_fire) dam = dam / 3;
    if (p_ptr->oppose_fire > 0) dam = dam / 3;
    if (p_ptr->immune_fire) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_flammable, 3);
}


/*
 * Freeze him to death.				-RAK-
 */
void cold_dam(int dam, cptr kb_str)
{
    if (p_ptr->resist_cold) dam = dam / 3;
    if (p_ptr->oppose_cold > 0) dam = dam / 3;
    if (p_ptr->immune_cold) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_cold_destroy, 5);
}



/*
 * Poison gas the idiot.				-RAK-
 */
void poison_gas(int dam, cptr kb_str)
{
    if (p_ptr->oppose_pois > 0) dam = 2 * dam / 3;
    if (p_ptr->resist_pois) dam = (dam * 3) / 5;
    if (p_ptr->immune_pois) dam = 1;
    take_hit(dam, kb_str);
    if (!(p_ptr->resist_pois || p_ptr->oppose_pois
	  || p_ptr->immune_pois)) p_ptr->poisoned += 12 + randint(dam);
}


/* 
 * Corrode the unsuspecting person's armor		 -RAK-	
 */
void corrode_gas(cptr kb_str)
{
    if (!p_ptr->immune_acid)
	if (!minus_ac()
	    take_hit(randint(8), kb_str);
    inven_damage(set_corrodes, 5);
}













/*
 * this assumes only 1 move apart -CFT
 */
static char bolt_char(int y, int x, int ny, int nx)
{
    if (ny == y) return '-';
    if (nx == x) return '|';
    if ((ny-y) == (nx-x)) return '\\';
    return '/';
}




