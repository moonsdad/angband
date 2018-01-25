/* File: wizard.c */ 

/* Purpose: Version history and info, and wizard mode debugging aids. */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"






/*
 * Wizard routine for gaining on stats                  -RAK-    
 */
static void change_character()
{
    register int          tmp_val;
    register s32b        tmp_lval;

    vtype                 tmp_str;

    prt("(3 - 118) Strength     = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_STR] = tmp_val;
	(void)res_stat(A_STR);
    }

    prt("(3 - 118) Intelligence = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_INT] = tmp_val;
	(void)res_stat(A_INT);
    }

    prt("(3 - 118) Wisdom       = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_WIS] = tmp_val;
	(void)res_stat(A_WIS);
    }

    prt("(3 - 118) Dexterity    = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_DEX] = tmp_val;
	(void)res_stat(A_DEX);
    }

    prt("(3 - 118) Constitution = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_CON] = tmp_val;
	(void)res_stat(A_CON);
    }

    prt("(3 - 118) Charisma     = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 3)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 2) && (tmp_val < 119)) {
	p_ptr->max_stat[A_CHR] = tmp_val;
	(void)res_stat(A_CHR);
    }

    prt("(1 - 32767) Hit points = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 5)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > 0) && (tmp_val <= MAX_SHORT)) {
	p_ptr->mhp = tmp_val;
	p_ptr->chp = tmp_val;
	p_ptr->chp_frac = 0;
	prt_mhp();
	prt_chp();
    }

    prt("(0 - 32767) Mana       = ", 0, 0);
    if (!get_string(tmp_str, 0, 25, 5)) return;

    tmp_val = atoi(tmp_str);
    if ((tmp_val > -1) && (tmp_val <= MAX_SHORT) && (*tmp_str != '\0')) {
	p_ptr->mana = tmp_val;
	p_ptr->cmana = tmp_val;
	p_ptr->cmana_frac = 0;
	prt_cmana();
    }

    (void)sprintf(tmp_str, "Current=%ld  Gold = ", (long)p_ptr->au);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 7)) return;

    tmp_lval = atol(tmp_str);
    if (tmp_lval > -1 && (*tmp_str != '\0')) {
	p_ptr->au = tmp_lval;
	prt_gold();
    }

    (void)sprintf(tmp_str, "Current=%ld  Max Exp = ", (long)p_ptr->max_exp);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 7)) return;

    tmp_lval = atol(tmp_str);
    if (tmp_lval > -1 && (*tmp_str != '\0')) {
	p_ptr->max_exp = tmp_lval;
	prt_experience();
    }

    (void)sprintf(tmp_str, "Current=%d  (0-200) Searching = ", p_ptr->srh);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;

    tmp_val = atoi(tmp_str);
	if ((tmp_val > -1) && (tmp_val < 201) && (*tmp_str != '\0')) {
	    p_ptr->srh = tmp_val;
    }


    (void)sprintf(tmp_str, "Current=%d  (-1-18) Stealth = ", p_ptr->stl);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if ((tmp_val > -2) && (tmp_val < 19) && (*tmp_str != '\0'))
	    p_ptr->stl = tmp_val;

    (void)sprintf(tmp_str, "Current=%d  (0-200) Disarming = ", p_ptr->disarm);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if ((tmp_val > -1) && (tmp_val < 201) && (*tmp_str != '\0'))
	    p_ptr->disarm = tmp_val;

    (void)sprintf(tmp_str, "Current=%d  (0-100) Save = ", p_ptr->save);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if ((tmp_val > -1) && (tmp_val < 201) && (*tmp_str != '\0'))
	    p_ptr->save = tmp_val;

    (void)sprintf(tmp_str, "Current=%d  (0-200) Base to hit = ", p_ptr->bth);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if ((tmp_val > -1) && (tmp_val < 201) && (*tmp_str != '\0'))
	    p_ptr->bth = tmp_val;

    (void)sprintf(tmp_str, "Current=%d  (0-200) Bows/Throwing = ", p_ptr->bthb);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if ((tmp_val > -1) && (tmp_val < 201) && (*tmp_str != '\0'))
	    p_ptr->bthb = tmp_val;

    (void)sprintf(tmp_str, "Current=%d  Weight = ", p_ptr->wt);
    tmp_val = strlen(tmp_str);
    prt(tmp_str, 0, 0);
    if (!get_string(tmp_str, 0, tmp_val, 3)) return;
	tmp_val = atoi(tmp_str);
	if (tmp_val > -1 && (*tmp_str != '\0'))
	    p_ptr->wt = tmp_val;

    while (get_com("Alter speed? (+/-)", tmp_str)) {
	if (*tmp_str == '+')
	    change_speed(-1);
	else if (*tmp_str == '-')
	    change_speed(1);
	else
	    break;
	prt_speed();
    }
}


/*
 * Wizard routine for creating objects		-RAK-	 
 */
void wizard_create()
{
    int         tmp_val;
    int                  i, j, k;
    s32b                tmp_lval;
    char                 tmp_str[100];
    inven_type *i_ptr;
    inven_kind        t_type, *t_ptr;
    inven_type           forge;
    cave_type  *c_ptr;
    char                 ch;
    int                  more = FALSE;

    t_ptr = &t_type;
    i_ptr = &forge;
    i_ptr->name2 = 0;
    i_ptr->ident = ID_KNOWN2 | ID_STOREBOUGHT;

    save_screen();
    prt("What type of item?    : ", 0, 0);
    prt("[W]eapon, [A]rmour, [O]thers.", 1, 0);
    if (!get_com(NULL, &ch)) {
	restore_screen();
	return;
    }

    switch (ch) {

      case 'W': case 'w':
	prt("What type of Weapon?    : ", 0, 0);
	prt("[S]word, [H]afted, [P]olearm, [B]ow, [A]mmo.", 1, 0);
	if (!get_com((char *)0, &ch)) {
	    restore_screen();
	    return;
	}

	switch (ch) {
	  case 'S': case 's':
	    i_ptr->tval = TV_SWORD;
	    break;

	  case 'P': case 'p':
	    i_ptr->tval = TV_POLEARM;
	    break;

	  case 'H': case 'h':
	    i_ptr->tval = TV_HAFTED;
	    break;

	  case 'B': case 'b':
	    i_ptr->tval = TV_BOW;
	    break;

	  case 'A': case 'a':
	    prt("What type of Ammo?    : ", 0, 0);
	    prt("[A]rrow, [B]olt, [P]ebble.", 1, 0);
	    if (!get_com(NULL, &ch)) {
		restore_screen();
		return;
	    }
	    switch (ch) {
	      case 'A': case 'a':
		i_ptr->tval = TV_ARROW;
		break;
	      case 'B': case 'b':
		i_ptr->tval = TV_BOLT;
		break;
	      case 'P': case 'p':
		i_ptr->tval = TV_SHOT;
		break;
	      default:
		break;
	    }
	    break;

	  default:
	    restore_screen();
	    return;
	}
	break;

      case 'A': case 'a':
	prt("What type of Armour?    : ", 0, 0);
	prt("[A]rmour, [G]loves, [B]oots, [S]hields, [H]elms, [C]loaks.", 1, 0);
	if (!get_com(NULL, &ch)) {
	    restore_screen();
	    return;
	}

	switch (ch) {

	  case 'S': case 's':
	    i_ptr->tval = TV_SHIELD;
	    break;

	  case 'H': case 'h':
	    i_ptr->tval = TV_HELM;
	    break;

	  case 'G': case 'g':
	    i_ptr->tval = TV_GLOVES;
	    break;

	  case 'B': case 'b':
	    i_ptr->tval = TV_BOOTS;
	    break;

	  case 'C': case 'c':
	    i_ptr->tval = TV_CLOAK;
	    break;

	  case 'A': case 'a':
	    prt("What type of Armour?    : ", 0, 0);
	    prt("[H]ard armour, [S]oft armour.", 1, 0);
	    if (!get_com(NULL, &ch)) {
		restore_screen();
		return;
	    }

	    switch (ch) {
	      case 'H': case 'h':
		i_ptr->tval = TV_HARD_ARMOR;
		break;
	      case 'S': case 's':
		i_ptr->tval = TV_SOFT_ARMOR;
		break;
	      default:
		break;
	    }
	    break;

	  default:
	    restore_screen();
	    return;
	}
	break;

      case 'O': case 'o':
	prt("What type of Object?    : ", 0, 0);
	prt("[R]ing, [P]otion, [W]and/staff, [S]croll, [M]agicbook, [A]mulet, [T]ool.", 1, 0);
	if (!get_com(NULL, &ch)) {
	    restore_screen();
	    return;
	}

	switch (ch) {

	  case 'R': case 'r':
	    i_ptr->tval = TV_RING;
	    break;

	  case 'P': case 'p':
	    i_ptr->tval = TV_POTION1;
	    break;

	  case 'S': case 's':
	    i_ptr->tval = TV_SCROLL1;
	    break;

	  case 'A': case 'a':
	    i_ptr->tval = TV_AMULET;
	    break;

	  case 'W': case 'w':
	    prt("Wand, Staff or Rod?    : ", 0, 0);
	    prt("[W]and, [S]taff, [R]od.", 1, 0);
	    if (!get_com(NULL, &ch)) {
		restore_screen();
		return;
	    }
	    switch (ch) {
	      case 'W': case 'w':
		i_ptr->tval = TV_WAND;
		break;
	      case 'S': case 's':
		i_ptr->tval = TV_STAFF;
		break;
	      case 'R':
	      case 'r':
		i_ptr->tval = TV_ROD;
		break;
	      default:
		restore_screen();
		return;
	    }
	    break;
	  case 'M': case 'm':
	    prt("Spellbook or Prayerbook?    : ", 0, 0);
	    prt("[S]pellbook, [P]rayerbook.", 1, 0);
	    if (!get_com(NULL, &ch)) {
		restore_screen();
		return;
	    }

	    switch (ch) {
	      case 'P': case 'p':
		i_ptr->tval = TV_PRAYER_BOOK;
		break;
	      case 'S': case 's':
		i_ptr->tval = TV_MAGIC_BOOK;
		break;
	      default:
		restore_screen();
		return;
	    }
	    break;

	  case 'T': case 't':
	    prt("Which Tool etc...?  : ", 0, 0);
	    prt("[S]pike, [D]igger, [C]hest, [L]ight, [F]ood, [O]il.", 1, 0);
	    if (!get_com(NULL, &ch)) {
		restore_screen();
		return;
	    }
	    switch (ch) {
	      case 'S': case 's':
		i_ptr->tval = TV_SPIKE;
		break;
	      case 'D':
		i_ptr->tval = TV_DIGGING;
		break;
	      case 'C': case 'c':
		i_ptr->tval = TV_CHEST;
		break;
	      case 'L': case 'l':
		i_ptr->tval = TV_LITE;
		break;
	      case 'F': case 'f':
		i_ptr->tval = TV_FOOD;
		break;
	      case 'O':
	      case 'o':
		i_ptr->tval = TV_FLASK;
		break;
	      default:
		restore_screen();
		return;
	    }
	    break;

	  default:
	    restore_screen();
	    return;
	}
	break;

      default:
	restore_screen();
	return;
    }

    j = 0;
    i = 0;
    k = 0;
again:
    restore_screen();
    save_screen();
    prt("Which Item?  : ", 0, 0);
    for (; i < MAX_DUNGEON_OBJ; i++) {
	switch (i_ptr->tval) {
	  case TV_POTION1:
	    if ((k_list[i].tval == TV_POTION1) ||
		(k_list[i].tval == TV_POTION2)) {
		sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		prt(tmp_str, 1 + j, 0);
		j++;
	    }
	    break;
	  case TV_SCROLL1:
	    if ((k_list[i].tval == TV_SCROLL1) ||
		(k_list[i].tval == TV_SCROLL2)) {
		sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		prt(tmp_str, 1 + j, 0);
		j++;
	    }
	    break;
	  default:
	    if (k_list[i].tval == i_ptr->tval) {
		sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		prt(tmp_str, 1 + j, 0);
		j++;
	    }
	    break;
	}
	if (j == 21) {
	    more = TRUE;
	    break;
	}
    }
    if (j < 21) {
	for (i = (i - (MAX_DUNGEON_OBJ - 1)) + (SPECIAL_OBJ - 1); i < MAX_OBJECTS; i++) {
	    switch (i_ptr->tval) {
	      case TV_POTION1:
		if ((k_list[i].tval == TV_POTION1) ||
		    (k_list[i].tval == TV_POTION2)) {
		    sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		    prt(tmp_str, 1 + j, 0);
		    j++;
		}
		break;
	      case TV_SCROLL1:
		if ((k_list[i].tval == TV_SCROLL1) ||
		    (k_list[i].tval == TV_SCROLL2)) {
		    sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		    prt(tmp_str, 1 + j, 0);
		    j++;
		}
		break;
	      default:
		if (k_list[i].tval == i_ptr->tval) {
		    sprintf(tmp_str, "%c) %s", 'a' + j, k_list[i].name);
		    prt(tmp_str, 1 + j, 0);
		    j++;
		}
		break;
	    }
	    if (j == 21) {
		more = TRUE;
		break;
	    }
	}
    }
    if (more)
	prt("v) NEXT PAGE", 22, 0);

    do {
	if (!get_com((char *)0, &ch)) {
	    restore_screen();
	    return;
	}
    } while ((ch < 'a' && ch > ('a' + j)) || (more && ch < 'a' && ch > ('a' + j + 1)));

    if ((ch == 'v') && more) {
	more = FALSE;
	k += (j - 1);
	j = 0;
	goto again;
    }
    k += (ch - 'a' + 1);

    j = 0;
    for (i = 0; i < MAX_DUNGEON_OBJ; i++) {
	switch (i_ptr->tval) {
	  case TV_POTION1:
	    if ((k_list[i].tval == TV_POTION1) ||
		(k_list[i].tval == TV_POTION2)) {
		j++;
	    }
	    break;
	  case TV_SCROLL1:
	    if ((k_list[i].tval == TV_SCROLL1) ||
		(k_list[i].tval == TV_SCROLL2)) {
		j++;
	    }
	    break;
	  default:
	    if (k_list[i].tval == i_ptr->tval) {
		j++;
	    }
	    break;
	}
	if (j == k)
	    break;
    }
    if (j != k) {
	for (i = (SPECIAL_OBJ - 1); i < MAX_OBJECTS; i++) {
	    switch (i_ptr->tval) {
	      case TV_POTION1:
		if ((k_list[i].tval == TV_POTION1) ||
		    (k_list[i].tval == TV_POTION2)) {
		    j++;
		}
		break;
	      case TV_SCROLL1:
		if ((k_list[i].tval == TV_SCROLL1) ||
		    (k_list[i].tval == TV_SCROLL2)) {
		    j++;
		}
		break;
	      default:
		if (k_list[i].tval == i_ptr->tval) {
		    j++;
		}
		break;
	    }
	    if (j == k)
		break;
	}
    }
    if (j != k) {
	restore_screen();
	return;
    }
    invcopy(i_ptr, i);
    i_ptr->timeout = 0;
    restore_screen();
    save_screen();

    prt("Number of items? [return=1]: ", 0, 0);
    if (!get_string(tmp_str, 0, 33, 5)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val) i_ptr->number = tmp_val;

    prt("Weight of item? [return=default]: ", 0, 0);
    if (!get_string(tmp_str, 0, 35, 5)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val) i_ptr->weight = tmp_val;

    if ((i_ptr->tval == TV_SWORD) ||
	(i_ptr->tval == TV_HAFTED) ||
	(i_ptr->tval == TV_POLEARM) ||
	(i_ptr->tval == TV_ARROW) ||
	(i_ptr->tval == TV_BOLT) ||
	(i_ptr->tval == TV_SHOT) ||
	(i_ptr->tval == TV_DIGGING)) {

	i_ptr->flags3 |= TR3_SHOW_MODS;
	prt("Damage (dice): ", 0, 0);
	if (!get_string(tmp_str, 0, 15, 3)) goto end;
	tmp_val = atoi(tmp_str);
	if (tmp_val != 0) i_ptr->dd = tmp_val;

	prt("Damage (sides): ", 0, 0);
	if (!get_string(tmp_str, 0, 16, 3)) goto end;
	tmp_val = atoi(tmp_str);
	if (tmp_val != 0) i_ptr->ds = tmp_val;
    }

    prt("+To hit: ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val != 0) i_ptr->tohit = tmp_val;

    prt("+To dam: ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val != 0) i_ptr->todam = tmp_val;

    /* Extra Armor Info */
    if ((i_ptr->tval == TV_SOFT_ARMOR) ||
	(i_ptr->tval == TV_HARD_ARMOR) ||
	(i_ptr->tval == TV_HELM) ||
	(i_ptr->tval == TV_CLOAK) ||
	(i_ptr->tval == TV_BOOTS) ||
	(i_ptr->tval == TV_GLOVES) ||
	(i_ptr->tval == TV_SHIELD)) {

	prt("Base AC : ", 0, 0);
	if (!get_string(tmp_str, 0, 10, 3)) goto end;
	tmp_val = atoi(tmp_str);
	if (tmp_val != 0) i_ptr->ac = tmp_val;
    }

    prt("+To AC : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val != 0) i_ptr->toac = tmp_val;

    prt("Magic Plus Flag  : ", 0, 0);
    if (!get_string(tmp_str, 0, 20, 5)) goto end;
    tmp_val = atoi(tmp_str);
    if (tmp_val != 0) i_ptr->pval = tmp_val;

    save_screen();

    /* Only do TRN_* flags for wearable objects */

    if ((i_ptr->tval <= TV_MAX_WEAR) && (i_ptr->tval >= TV_MIN_WEAR)) {

	if ((i_ptr->tval == TV_SWORD) ||
	    (i_ptr->tval == TV_HAFTED) ||
	    (i_ptr->tval == TV_POLEARM) ||
	    (i_ptr->tval == TV_ARROW) ||
	    (i_ptr->tval == TV_BOLT) ||
	    (i_ptr->tval == TV_SHOT) ||
	    (i_ptr->tval == TV_DIGGING)) {

	    if (get_com("Slay Evil? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_EVIL;
	    } else if (ch == '\033')goto end;

	    if (get_com("Slay Animal? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_ANIMAL;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Undead? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_UNDEAD;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Giant? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_GIANT;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Demon? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_DEMON;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Troll? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_TROLL;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Orc? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_ORC;
	    } else if (ch == '\033') goto end;

	    if (get_com("Slay Dragon? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SLAY_DRAGON;
	    } else if (ch == '\033') goto end;

	    if (get_com("Execute Dragon? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_KILL_DRAGON;
	    } else if (ch == '\033') goto end;

	    if (get_com("Frost Brand? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_BRAND_COLD;
	    } else if (ch == '\033') goto end;

	    if (get_com("Fire Brand? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_BRAND_FIRE;
	    } else if (ch == '\033') goto end;

	    if (get_com("Lightning Brand? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR_LIGHTNING;
	    } else if (ch == '\033') goto end;

	    if (get_com("Earthquake Brand? [yn]: ", &ch)) {
		if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_IMPACT;
	    } else if (ch == '\033') goto end;
	}
	if (get_com("Affect Strength? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_STR;
	} else if (ch == '\033')
	    goto end;

	if (get_com("Affect Intelligence? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_INT;
	} else if (ch == '\033') goto end;

	if (get_com("Affect Wisdom? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_WIS;
	} else if (ch == '\033') goto end;

	if (get_com("Affect Dexterity? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_DEX;
	} else if (ch == '\033') goto end;

	if (get_com("Affect Constitution? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_CON;
	} else if (ch == '\033') goto end;

	if (get_com("Affect Charisma? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_CHR;
	} else if (ch == '\033') goto end;

	if (get_com("Automatic Searching? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SEARCH;
	} else if (ch == '\033')  goto end;

	if (get_com("Slow Digestion? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_SLOW_DIGEST;
	} else if (ch == '\033') goto end;

	if (get_com("Stealth? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_STEALTH;
	} else if (ch == '\033') goto end;

	if (get_com("Aggravate Monsters? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_AGGRAVATE;
	} else if (ch == '\033') goto end;

	if (get_com("Regeneration? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_REGEN;
	} else if (ch == '\033') goto end;

	if (get_com("Speed? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_SPEED;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Fire? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_FIRE;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Cold? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_COLD;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Acid? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_ACID;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Lightning? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_ELEC;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Poison? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_POIS;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Confusion? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_CONF;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Sound? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_SOUND;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Light? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_LITE;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Dark? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_DARK;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Chaos? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_CHAOS;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Disenchantment? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_DISEN;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Shards? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_SHARDS;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Nexus? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_NEXUS;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Nether? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_NETHER;
	} else if (ch == '\033') goto end;

	if (get_com("Resist Blindness? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_RES_BLIND;
	} else if (ch == '\033') goto end;

	if (get_com("Sustain a stat (Magic value 10 for all stats)? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR_SUST_STAT;
	} else if (ch == '\033') goto end;

	if (get_com("See invisible? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_SEE_INVIS;
	} else if (ch == '\033') goto end;

	if (get_com("Free Action? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_FREE_ACT;
	} else if (ch == '\033') goto end;

	if (get_com("Feather Falling? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_FEATHER;
	} else if (ch == '\033') goto end;

	if (get_com("Tunneling? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_TUNNEL;
	} else if (ch == '\033') goto end;

	if (get_com("Infra-vision? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags1 |= TR1_INFRA;
	} else if (ch == '\033') goto end;

	if (get_com("Resist life level loss? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_HOLD_LIFE;
	} else if (ch == '\033') goto end;

	if (get_com("Telepathy? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_TELEPATHY;
	} else if (ch == '\033') goto end;

	if (get_com("Immune to Fire? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_IM_FIRE;
	} else if (ch == '\033') goto end;

	if (get_com("Immune to Cold? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_IM_COLD;
	} else if (ch == '\033') goto end;

	if (get_com("Immune to Acid? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_IM_ACID;
	} else if (ch == '\033') goto end;

	if (get_com("Immune to Lightning? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_IM_ELEC;
	} else if (ch == '\033') goto end;

	if (get_com("Immune to Poison? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR2_IM_POIS;
	} else if (ch == '\033') goto end;

	if (get_com("Give off Light? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_LITE;
	} else if (ch == '\033') goto end;

	if (get_com("Activatable Item? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_ACTIVATE;
	} else if (ch == '\033') goto end;

	if (get_com("Is it an Artifact? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags2 |= TR_ARTIFACT;
	} else if (ch == '\033') goto end;

	if (get_com("Cursed? [yn]: ", &ch)) {
	    if (ch == 'y' || ch == 'Y') i_ptr->flags3 |= TR3_CURSED;
	} else if (ch == '\033') goto end;

    } /* end if TV_MAX_WEAR >= i_ptr->tval >= TV_MIN_WEAR -CFT */

    prt("Cost : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 8)) {
	restore_screen();
	return;
    }
    tmp_lval = atol(tmp_str);
    if (tmp_val) i_ptr->cost = tmp_lval;

    prt("Dungeon Level on which it is found : ", 0, 0);
    if (!get_string(tmp_str, 0, 39, 3)) {
	restore_screen();
	return;
    }
    tmp_val = atoi(tmp_str);
    if (tmp_val != 0) i_ptr->level = tmp_val;

    j = 0;
    i = 0;
    k = 0;
    more = FALSE;
SNagain:
    restore_screen();
    save_screen();
    for (; i < SN_ARRAY_SIZE; i++) {
	sprintf(tmp_str, "%c) %s", 'a' + j, special_names[i]);
	prt(tmp_str, 1 + j, 0);
	j++;
	if (j == 21) {
	    more = TRUE;
	    break;
	}
    }
    if (more)
	prt("v) NEXT PAGE", 22, 0);

    do {
	if (!get_com("Please choose a secondary name for the item : ", &ch)) {
	    restore_screen();
	    return;
	}
    } while ((ch < 'a' && ch > ('a' + j)) || (more && ch < 'a' && ch > ('a' + j + 1)));

    if ((ch == 'v') && more) {
	more = FALSE;
	k += (j - 1);
	j = 0;
	goto SNagain;
    } else {
	i_ptr->name2 = k + (ch - 'a');
    }
    restore_screen();
    save_screen();

end:
    if (get_check("Allocate?")) {
    /* delete object first if any, before call i_pop */
	c_ptr = &cave[char_row][char_col];
	if (c_ptr->i_idx != 0)
	    (void)delete_object(char_row, char_col);

	store_bought(i_ptr);
	tmp_val = i_pop();
	i_list[tmp_val] = forge;
	c_ptr->i_idx = tmp_val;
	msg_print("Allocated.");
    } else
	msg_print("Aborted.");
    restore_screen();
}


/* pause if screen fills up while printint up artifacts - cba */

void artifact_screen_full(int *i, int j)
{
    int t;

/* is screen full? */
    if (*i == 22) {
	prt("-- more --", *i, j);
	inkey();
	for (t = 2; t < 23; t++)
	    erase_line(t, j);	   /* don't forget to erase extra */
	prt("Artifacts seen: (continued)", 1, j + 5);
	*i = 2;
    }
}

/* Print out the artifacts seen without using a file - cba */

void artifact_check_no_file()
{
    int i, j;

    save_screen();
    j = 15;

    for (i = 1; i < 23; i++)
	erase_line(i, j - 2);

    i = 1;
    prt("Artifacts Seen:", i++, j + 5);

/* weapons */

    if (AEGLIN) {
	prt("The Broadsword 'Aeglin'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (AEGLOS) {
	prt("The Spear 'Aeglos'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (AGLARANG) {
	prt("The Katana 'Aglarang'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ANDURIL) {
	prt("The Longsword 'Anduril'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ANGRIST) {
	prt("The Dagger 'Angrist'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ANGUIREL) {
	prt("The Longsword 'Anguirel'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ARUNRUTH) {
	prt("The Broadsword 'Arunruth'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (AULE) {
	prt("The War Hammer of Aule", i++, j);
	artifact_screen_full(&i, j);
    }
    if (AVAVIR) {
	prt("The Scythe 'Avavir'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BALLI) {
	prt("The Battle Axe of Balli Stonehand", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BARUKKHELED) {
	prt("The Broad Axe 'Barukkheled'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BELANGIL) {
	prt("The Dagger 'Belangil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BLOODSPIKE) {
	prt("The Morningstar 'Bloodspike'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CALRIS) {
	prt("The Bastard Sword 'Calris'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CARETH) {
	prt("The Saber 'Careth Asdriag'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CRISDURIAN) {
	prt("The Executioner's Sword 'Crisdurian'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DEATHWREAKER) {
	prt("The Mace of Disruption 'Deathwreaker'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DETHANC) {
	prt("The Dagger 'Dethanc'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DOOMCALLER) {
	prt("The Blade of Chaos 'Doomcaller'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DURIN) {
	prt("The Great Axe of Durin", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ELVAGIL) {
	prt("The Longsword 'Elvagil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (EONWE) {
	prt("The Great Axe of Eonwe", i++, j);
	artifact_screen_full(&i, j);
    }
    if (EORLINGAS) {
	prt("The Lance of Eorlingas", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ERIRIL) {
	prt("The Quarterstaff 'Eriril'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (FIRESTAR) {
	prt("The Morningstar 'Firestar'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (FORASGIL) {
	prt("The Rapier 'Forasgil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GILETTAR) {
	prt("The Short Sword 'Gilettar'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GLAMDRING) {
	prt("The Broadsword 'Glamdring'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GONDRICAM) {
	prt("The Cutlass 'Gondricam'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GURTHANG) {
	prt("The Two-Handed Sword 'Gurthang'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (HARADEKKET) {
	prt("The Scimitar 'Haradekket'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (LOTHARANG) {
	prt("The Battle Axe 'Lotharang'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (MAEDHROS) {
	prt("The Main Gauche of Maedhros", i++, j);
	artifact_screen_full(&i, j);
    }
    if (MORMEGIL) {
	prt("The Two-Handed Sword 'Mormegil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (MUNDWINE) {
	prt("The Lochaber Axe 'Mundwine'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NAR) {
	prt("The Quarterstaff 'Nar-i-vagil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NARTHANC) {
	prt("The Dagger 'Narthanc'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NIMLOTH) {
	prt("The Spear 'Nimloth'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NIMTHANC) {
	prt("The Dagger 'Nimthanc'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (OLORIN) {
	prt("The Quarterstraff of Olorin", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ORCRIST) {
	prt("The Broadsword 'Orcrist'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (OROME) {
	prt("The Spear 'Orome'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (OSONDIR) {
	prt("The Halberd 'Osondir'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (PAIN) {
	prt("The Glaive of Pain", i++, j);
	artifact_screen_full(&i, j);
    }
    if (RILIA) {
	prt("The Dagger of Rilia", i++, j);
	artifact_screen_full(&i, j);
    }
    if (RINGIL) {
	prt("The Longsword 'Ringil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (STING) {
	prt("The Short Sword 'Sting'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TARATOL) {
	prt("The Mace 'Taratol'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THEODEN) {
	prt("The Beaked Axe of Theoden", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THUNDERFIST) {
	prt("The Two-Handed Great Flail 'Thunderfist'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TIL) {
	prt("The Pike 'Til-i-arc'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TOTILA) {
	prt("The Flail 'Totila'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TURMIL) {
	prt("The Lucerne Hammer 'Turmil'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ULMO) {
	prt("The Trident of Ulmo", i++, j);
	artifact_screen_full(&i, j);
    }
    if (WRATH) {
	prt("The Trident of Wrath", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ZARCUTHRA) {
	prt("The Two-Handed Sword 'Zarcuthra'", i++, j);
	artifact_screen_full(&i, j);
    }
/* missle weapons */

    if (BARD) {
	prt("The Longbow of Bard", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BELEG) {
	prt("The Longbow of Beleg Cuthalion", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CUBRAGOL) {
	prt("The Light Crossbow 'Cubragol'", i++, j);
	artifact_screen_full(&i, j);
    }

/* cloaks */

    if (COLANNON) {
	prt("The Cloak 'Colannon'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (COLLUIN) {
	prt("The Cloak 'Colluin'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (HOLCOLLETH) {
	prt("The Cloak 'Holcolleth'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (LUTHIEN) {
	prt("The Shadow Cloak of Luthien", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THINGOL) {
	prt("The Cloak of Thingol", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THORONGIL) {
	prt("The Cloak of Thorongil", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TUOR) {
	prt("The Shadow Cloak of Tuor", i++, j);
	artifact_screen_full(&i, j);
    }

/* armor */

    if (ANARION) {
	prt("The Large Metal Shield of Anarion", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ARVEDUI) {
	prt("The Chain Mail of Arvedui", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BELEGENNON) {
	prt("The Mithril Chain Mail 'Belegennon'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BERUTHIEL) {
	prt("The Iron Crown of Beruthiel", i++, j);
	artifact_screen_full(&i, j);
    }
    if (BLADETURNER) {
	prt("The Power Dragon Scale Mail 'Bladeturner'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CAMBELEG) {
	prt("The Leather Gloves 'Cambeleg'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CAMLOST) {
	prt("The Gauntlets 'Camlost'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CAMMITHRIM) {
	prt("The Leather Gloves 'Cammithrin'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CASPANION) {
	prt("The Augmented Chain Mail of Caspanion", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CELEBORN) {
	prt("The Mithril Plate Mail of Celeborn", i++, j);
	artifact_screen_full(&i, j);
    }
    if (CELEGORM) {
	prt("The Large Leather Shield of Celegorm", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DAL) {
	prt("The Soft Leather Boots 'Dal-i-thalion'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (DOR_LOMIN) {
	prt("The Iron Helm of Dor-Lomin", i++, j);
	artifact_screen_full(&i, j);
    }
    if (FEANOR) {
	prt("The Hard Leather Boots of Feanor", i++, j);
	artifact_screen_full(&i, j);
    }
    if (FINGOLFIN) {
	prt("The Set of Cesti of Fingolfin", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GONDOR) {
	prt("The Golden Crown of Gondor", i++, j);
	artifact_screen_full(&i, j);
    }
    if (GORLIM) {
	prt("The Iron Helm of Gorlim", i++, j);
	artifact_screen_full(&i, j);
    }
    if (HAMMERHAND) {
	prt("The Steel Helm 'Hammerhand'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (HITHLOMIR) {
	prt("The Soft Leather Armour 'Hithlomir'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (HOLHENNETH) {
	prt("The Iron Helm 'Holhenneth'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ISILDUR) {
	prt("The Full Plate Armour of Isildur", i++, j);
	artifact_screen_full(&i, j);
    }
    if (PAURAEGEN) {
	prt("The Gauntlets 'Pauraegen'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (PAURHACH) {
	prt("The Gauntlets 'Paurhach'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (PAURNEN) {
	prt("The Gauntlets 'Paurnen'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (PAURNIMMEN) {
	prt("The Gauntlets 'Paurnimmen'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (RAZORBACK) {
	prt("The Multi-Hued Dragon Scale Mail 'Razorback'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ROHAN) {
	prt("The Metal Brigandine Armor of the Rohirrim", i++, j);
	artifact_screen_full(&i, j);
    }
    if (SOULKEEPER) {
	prt("The Adamantine Plate Mail 'Soulkeeper'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THALKETTOTH) {
	prt("The Leather Scale Mail 'Thalkettoth'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THENGEL) {
	prt("The Metal Cap of Thengel", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THORIN) {
	prt("The Small Metal Shield of Thorin", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THRANDUIL) {
	prt("The Hard Leather Cap of Thranduil", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THROR) {
	prt("The Pair of Metal Shod Boots of Thror", i++, j);
	artifact_screen_full(&i, j);
    }

/* amulets and necklaces */

    if (CARLAMMAS) {
	prt("The Amulet of Carlammas", i++, j);
	artifact_screen_full(&i, j);
    }
    if (INGWE) {
	prt("The Amulet of Ingwe", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NECKLACE) {
	prt("The Amulet of the Dwarves", i++, j);
	artifact_screen_full(&i, j);
    }

/* light sources */

    if (PHIAL) {
	prt("The Phial of Galadriel", i++, j);
	artifact_screen_full(&i, j);
    }
    if (ELENDIL) {
	prt("The Star of Elendil", i++, j);
	artifact_screen_full(&i, j);
    }
    if (THRAIN) {
	prt("The Arkenstone of Thrain", i++, j);
	artifact_screen_full(&i, j);
    }

/* rings */

    if (BARAHIR) {
	prt("The Ring of Barahir", i++, j);
	artifact_screen_full(&i, j);
    }
    if (TULKAS) {
	prt("The Ring of Tulkas", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NARYA) {
	prt("The Ring of Power 'Narya'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (NENYA) {
	prt("The Ring of Power 'Nenya'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (VILYA) {
	prt("The Ring of Power 'Vilya'", i++, j);
	artifact_screen_full(&i, j);
    }
    if (POWER) {
	prt("The One Ring", i++, j);
	artifact_screen_full(&i, j);
    }

/* morgoth's stuff */

    if (GROND) {
	prt("The Lead-Filled Mace 'Grond'", i++, j);
	artifact_screen_full(&i, j);
    }
    pause_line(i);
    restore_screen();
}


/* print out the status of uniques - cba */

void unique_screen_full(int *i, int j)
{
    int t;

/* is screen full? */
    if (*i == 22) {
	prt("-- more --", *i, j);
	inkey();
	for (t = 2; t < 23; t++)
	    erase_line(t, j);	   /* don't forget to erase extra */
	prt("Uniques: (continued)", 1, j + 5);
	*i = 2;
    }
}


