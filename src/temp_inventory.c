/*
 * All inventory commands (wear, exchange, take off, drop, inventory and
 * equipment) are handled in an alternative command input mode, which accepts
 * any of the inventory commands. 
 *
 * It is intended that this function be called several times in succession, as
 * some commands take up a turn, and the rest of moria must proceed in the
 * interim. A global variable is provided, doing_inven, which is normally
 * zero; however if on return from inven_command it is expected that
 * inven_command should be called *again*, (being still in inventory command
 * input mode), then doing_inven is set to the inventory command character
 * which should be used in the next call to inven_command. 
 *
 * On return, the screen is restored, but not flushed. Provided no flush of the
 * screen takes place before the next call to inven_command, the inventory
 * command screen is silently redisplayed, and no actual output takes place
 * at all. If the screen is flushed before a subsequent call, then the player
 * is prompted to see if we should continue. This allows the player to see
 * any changes that take place on the screen during inventory command input. 
 *
 * The global variable, screen_change, is cleared by inven_command, and set when
 * the screen is flushed. This is the means by which inven_command tell if
 * the screen has been flushed. 
 *
 * The display of inventory items is kept to the right of the screen to minimize
 * the work done to restore the screen afterwards.		-CJS- 
 */

/* Inventory command screen states. */
#define BLANK_SCR	0
#define EQUIP_SCR	1
#define INVEN_SCR	2
#define WEAR_SCR	3
#define HELP_SCR	4
#define WRONG_SCR	5

/* Keep track of the state of the inventory screen. */
static int scr_state, scr_left, scr_base;
static int wear_low, wear_high;

/* Draw the inventory screen. */
static void inven_screen(int new_scr)
{
    register int line = 0;

    if (new_scr != scr_state) {
	scr_state = new_scr;
	switch (new_scr) {
	  case BLANK_SCR:
	    line = 0;
	    break;
	  case HELP_SCR:
	    if (scr_left > 52)
		scr_left = 52;
	    prt("  e  : list used equipment", 1, scr_left);
	    prt("  i  : inventory of pack", 2, scr_left);
	    prt("  t  : take off item", 3, scr_left);
	    prt("  w  : wear or wield object", 4, scr_left);
	    prt("  x  : exchange weapons", 5, scr_left);
	    prt("  d  : drop object", 6, scr_left);
	    prt("  ESC: exit", 7, scr_left);
	    line = 7;
	    break;
	  case INVEN_SCR:
	    scr_left = show_inven(0, inven_ctr - 1, show_inven_weight, scr_left, 0);
	    line = inven_ctr;
	    break;
	  case WEAR_SCR:
	    scr_left = show_inven(wear_low, wear_high, show_inven_weight, scr_left, 0);
	    line = wear_high - wear_low + 1;
	    break;
	  case EQUIP_SCR:
	    scr_left = show_equip(show_equip_weight, scr_left);
	    line = equip_ctr;
	    break;
	}
	if (line >= scr_base) {
	    scr_base = line + 1;
	    erase_line(scr_base, scr_left);
	} else {
	    while (++line <= scr_base)
		erase_line(line, scr_left);
	}
    }
}


/* 
 * This does all the work. */
void inven_command(int command)
{
    register int         slot = 0, item;
    int                  tmp, tmp2, selecting, from, to, light_chg = FALSE;
    const char          *prompt, *swap, *disp, *string;
    char                 which, query;
    bigvtype             prt1, prt2;
    register inven_type *i_ptr;
    inven_type           tmp_obj;

    free_turn_flag = TRUE;
    save_screen();
/* Take up where we left off after a previous inventory command. -CJS- */
    if (doing_inven) {
    /* If the screen has been flushed, we need to redraw. If the command is a
     * simple ' ' to recover the screen, just quit. Otherwise, check and see
     * what the user wants. 
     */
	if (screen_change) {
	    if (command == ' ' || !get_check("Continuing with inventory command?")) {
		doing_inven = FALSE;
		return;
	    }
	    scr_left = 50;
	    scr_base = 0;
	}
	tmp = scr_state;
	scr_state = WRONG_SCR;
	inven_screen(tmp);
    } else {
	scr_left = 50;
	scr_base = 0;
    /* this forces exit of inven_command() if selecting is not set true */
	scr_state = BLANK_SCR;
    }
    do {
	if (isupper((int)command))
	    command = tolower((int)command);

    /* Simple command getting and screen selection. */
	selecting = FALSE;
	switch (command) {
	  case 'i':		   /* Inventory	    */
	    if (inven_ctr == 0)
		msg_print("You are not carrying anything.");
	    else
		inven_screen(INVEN_SCR);
	    break;
	  case 'e':		   /* Equipment	   */
	    if (equip_ctr == 0)
		msg_print("You are not using any equipment.");
	    else
		inven_screen(EQUIP_SCR);
	    break;
	  case 't':		   /* Take off	   */
	    if (equip_ctr == 0)
		msg_print("You are not using any equipment.");
	/* don't print message restarting inven command after taking off
	 * something, it is confusing 
	 */
	    else if (inven_ctr >= INVEN_WIELD && !doing_inven)
		msg_print("You will have to drop something first.");
	    else {
		if (scr_state != BLANK_SCR)
		    inven_screen(EQUIP_SCR);
		selecting = TRUE;
	    }
	    break;
	  case 'd':		   /* Drop */
	    if (inven_ctr == 0 && equip_ctr == 0)
		msg_print("But you're not carrying anything.");
	    else if (cave[char_row][char_col].tptr != 0)
		msg_print("There's no room to drop anything here.");
	    else {
		selecting = TRUE;
		if ((scr_state == EQUIP_SCR && equip_ctr > 0) || inven_ctr == 0) {
		    if (scr_state != BLANK_SCR)
			inven_screen(EQUIP_SCR);
		    command = 'r'; /* Remove - or take off and drop. */
		} else if (scr_state != BLANK_SCR)
		    inven_screen(INVEN_SCR);
	    }
	    break;
	  case 'w':		   /* Wear/wield	   */
	    for (wear_low = 0;
	     wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR;
		 wear_low++);
	    for (wear_high = wear_low;
		 wear_high < inven_ctr && inventory[wear_high].tval >= TV_MIN_WEAR;
		 wear_high++);
	    wear_high--;
	    if (wear_low > wear_high)
		msg_print("You have nothing to wear or wield.");
	    else {
		if (scr_state != BLANK_SCR && scr_state != INVEN_SCR)
		    inven_screen(WEAR_SCR);
		selecting = TRUE;
	    }
	    break;
	  case 'x':
	    if (inventory[INVEN_WIELD].tval == TV_NOTHING &&
		inventory[INVEN_AUX].tval == TV_NOTHING)
		msg_print("But you are wielding no weapons.");
	    else if (TR_CURSED & inventory[INVEN_WIELD].flags) {
		objdes(prt1, &inventory[INVEN_WIELD], FALSE);
		(void)sprintf(prt2,
		     "The %s you are wielding appears to be cursed.", prt1);
		msg_print(prt2);
	    } else {
		free_turn_flag = FALSE;
		tmp_obj = inventory[INVEN_AUX];
		inventory[INVEN_AUX] = inventory[INVEN_WIELD];
		inventory[INVEN_WIELD] = tmp_obj;
		if (scr_state == EQUIP_SCR)
		    scr_left = show_equip(show_inven_weight, scr_left);
		py_bonuses(&inventory[INVEN_AUX], -1);	/* Subtract bonuses */
		py_bonuses(&inventory[INVEN_WIELD], 1);	/* Add bonuses    */

		if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
		    (void)strcpy(prt1, "Primary weapon   : ");
		    objdes(prt2, &inventory[INVEN_WIELD], TRUE);
		    msg_print(strcat(prt1, prt2));
		} else
		    msg_print("No primary weapon.");

	    /* this is a new weapon, so clear the heavy flag */
/* no, don't; the check_strength will clear it if it needs to be cleared
		weapon_heavy = FALSE; */
		check_strength();
	    }
	    break;
	  case ' ':		   /* Dummy command to return again to main
				    * prompt. */
	    break;
	  case '?':
	    inven_screen(HELP_SCR);
	    break;
	  default:
	/* Nonsense command					   */
	    bell();
	    break;
	}

    /* Clear the doing_inven flag here, instead of at beginning, so that can
     * use it to control when messages above appear. 
     */
	doing_inven = 0;

    /* Keep looking for objects to drop/wear/take off/throw off */
	which = 'z';
	while (selecting && free_turn_flag) {
	    swap = "";
	    if (command == 'w') {
		from = wear_low;
		to = wear_high;
		prompt = "Wear/Wield";
	    } else {
		from = 0;
		if (command == 'd') {
		    to = inven_ctr - 1;
		    prompt = "Drop";
		    if (equip_ctr > 0)
			swap = ", / for Equip";
		} else {
		    to = equip_ctr - 1;
		    if (command == 't')
			prompt = "Take off";
		    else {	   /* command == 'r' */
			prompt = "Throw off";
			if (inven_ctr > 0)
			    swap = ", / for Inven";
		    }
		}
	    }
	    if (from > to)
		selecting = FALSE;
	    else {
		if (scr_state == BLANK_SCR)
		    disp = ", * to list";
		else
		    disp = "";
		(void)sprintf(prt1,
		   "(%c-%c%s%s, space to break, ESC to exit) %s which one?",
			      from + 'a', to + 'a', disp, swap, prompt);

	    /* Abort everything. */
		if (!get_com(prt1, &which)) {
		    selecting = FALSE;
		    which = ESCAPE;
		}
	    /* Draw the screen and maybe exit to main prompt. */
		else if (which == ' ' || which == '*') {
		    if (command == 't' || command == 'r')
			inven_screen(EQUIP_SCR);
		    else if (command == 'w' && scr_state != INVEN_SCR)
			inven_screen(WEAR_SCR);
		    else
			inven_screen(INVEN_SCR);
		    if (which == ' ')
			selecting = FALSE;
		}
	    /* Swap screens (for drop) */
		else if (which == '/' && swap[0]) {
		    if (command == 'd')
			command = 'r';
		    else
			command = 'd';
		    if (scr_state == EQUIP_SCR)
			inven_screen(INVEN_SCR);
		    else if (scr_state == INVEN_SCR)
			inven_screen(EQUIP_SCR);
		} else if (((which < from + 'a') || (which > to + 'a'))
			   && ((which < from + 'A') || (which > to + 'A')))
		    bell();
		else {		   /* Found an item! */
		    if (isupper((int)which))
			item = which - 'A';
		    else
			item = which - 'a';
		    if (command == 'r' || command == 't') {
		    /* Get its place in the equipment list. */
			tmp = item;
			item = 21;
			do {
			    item++;
			    if (inventory[item].tval != TV_NOTHING)
				tmp--;
			}
			while (tmp >= 0);
			if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else if (TR_CURSED & inventory[item].flags) {
			    msg_print("Hmmm, it seems to be cursed.");
			    item = (-1);
			} else if (command == 't' &&
				   !inven_check_num(&inventory[item])) {
			    if (cave[char_row][char_col].tptr != 0) {
				msg_print("You can't carry it.");
				item = (-1);
			    } else if (get_check("You can't carry it.  Drop it?"))
				command = 'r';
			    else
				item = (-1);
			}
			if (item >= 0)
			    if (item == INVEN_LITE)
				light_chg = TRUE;
			if (command == 'r') {
			    inven_drop(item, TRUE);

			    /* As a safety measure, set the player's inven weight to 0,
			     * when the last object is dropped */
			    if (inven_ctr == 0 && equip_ctr == 0)
				inven_weight = 0;
			} else if (item >= 0) {
			    slot = inven_carry(&inventory[item]);
			    takeoff(item, slot);
			}
			check_strength();
			free_turn_flag = FALSE;
			if (command == 'r')
			    selecting = FALSE;
		    } else if (command == 'w') {

    /* Wearing. Go to a bit of trouble over replacing existing equipment. */
			if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else
			    switch (inventory[item].tval) { /* Slot for equipment */
			      case TV_SHOT:
			      case TV_BOLT:
			      case TV_ARROW:
			      case TV_BOW:
			      case TV_HAFTED:
			      case TV_POLEARM:
			      case TV_SWORD:
			      case TV_DIGGING:
				slot = INVEN_WIELD;
				break;
			      case TV_LITE:
				slot = INVEN_LITE;
				break;
			      case TV_BOOTS:
				slot = INVEN_FEET;
				break;
			      case TV_GLOVES:
				slot = INVEN_HANDS;
				break;
			      case TV_CLOAK:
				slot = INVEN_OUTER;
				break;
			      case TV_HELM:
				slot = INVEN_HEAD;
				break;
			      case TV_SHIELD:
				slot = INVEN_ARM;
				break;
			      case TV_HARD_ARMOR:
			      case TV_SOFT_ARMOR:
				slot = INVEN_BODY;
				break;
			      case TV_AMULET:
				slot = INVEN_NECK;
				break;
			      case TV_RING:
				if (inventory[INVEN_RIGHT].tval == TV_NOTHING)
				    slot = INVEN_RIGHT;
				else if (inventory[INVEN_LEFT].tval == TV_NOTHING)
				    slot = INVEN_LEFT;
				else {
				    slot = 0;
				/*
				 * Rings. Give some choice over where they
				 * go. 
				 */
				    do {
					if (!get_com(
				 "Put ring on which hand (l/r/L/R)?", &query)) {
					    item = (-1);
					    slot = (-1);
					} else if (query == 'l')
					    slot = INVEN_LEFT;
					else if (query == 'r')
					    slot = INVEN_RIGHT;
					else {
					    if (query == 'L')
						slot = INVEN_LEFT;
					    else if (query == 'R')
						slot = INVEN_RIGHT;
					    else
						bell();
					    if (slot && !verify("Replace", slot))
						slot = 0;
					}
				    }
				    while (slot == 0);
				}
				break;
			      default:
		msg_print("IMPOSSIBLE: I don't see how you can use that.");
				item = (-1);
				break;
			    }
			if (item >= 0 && inventory[slot].tval != TV_NOTHING) {
			    if (TR_CURSED & inventory[slot].flags) {
				objdes(prt1, &inventory[slot], FALSE);
				(void)sprintf(prt2, "The %s you are ", prt1);
				if (slot == INVEN_WIELD)	/* changed from
								 * INVEN_HEAD -CFT */
				    (void)strcat(prt2, "wielding ");
				else
				    (void)strcat(prt2, "wearing ");
				msg_print(strcat(prt2, "appears to be cursed."));
				item = (-1);
			    } else if (inventory[item].sval == ITEM_GROUP_MIN &&
				       inventory[item].number > 1 &&
				       !inven_check_num(&inventory[slot])) {
 /* this can happen if try to wield a torch, and have more than one in your inventory */
				msg_print("You will have to drop something first.");
				item = (-1);
			    }
			}
			if (item >= 0) {
			/* OK. Wear it. */
			    free_turn_flag = FALSE;

			    if (slot == INVEN_LITE)
				light_chg = TRUE;

			/* first remove new item from inventory */
			    tmp_obj = inventory[item];
			    i_ptr = &tmp_obj;

			    wear_high--;
			/* Fix for torches	   */
			    if (i_ptr->number > 1
				&& i_ptr->sval <= ITEM_SINGLE_STACK_MAX) {
				i_ptr->number = 1;
				wear_high++;
			    }
			    inven_weight += i_ptr->weight * i_ptr->number;
			    inven_destroy(item);	/* Subtracts weight */

/* second, add old item to inv and remove from equipment list, if necessary */
			    i_ptr = &inventory[slot];
			    if (i_ptr->tval != TV_NOTHING) {
				tmp2 = inven_ctr;
				tmp = inven_carry(i_ptr);
/* if item removed did not stack with anything in inventory, then increment wear_high */
				if (inven_ctr != tmp2)
				    wear_high++;
				takeoff(slot, tmp);
			    }
			/* third, wear new item */
			    *i_ptr = tmp_obj;
			    equip_ctr++;
			    py_bonuses(i_ptr, 1);
			    if (slot == INVEN_WIELD)
				string = "You are wielding";
			    else if (slot == INVEN_LITE)
				string = "Your light source is";
			    else
				string = "You are wearing";
			    objdes(prt2, i_ptr, TRUE);
			/* Get the right equipment letter. */
			    tmp = INVEN_WIELD;
			    item = 0;
			    while (tmp != slot)
				if (inventory[tmp++].tval != TV_NOTHING)
				    item++;

			    (void)sprintf(prt1, "%s %s. (%c)", string, prt2,
					  'a' + item);
			    msg_print(prt1);
			/* check_str will clear the heavy flag if necessary */
			    check_strength();
			    if (i_ptr->flags & TR_CURSED) {
				msg_print("Oops! It feels deathly cold!");
				add_inscribe(i_ptr, ID_DAMD);
			    /* To force a cost of 0, even if unidentified. */
			    /* i_ptr->cost = (-1); Not... */
			    }
			}
		    } else {	   /* command == 'd' */
			if (inventory[item].number > 1) {
			    objdes(prt1, &inventory[item], TRUE);
			    (void)sprintf(prt2, "Drop all %s?  [y/n]", prt1);
			    prt(prt2, 0, 0);
			    query = inkey();
			    if (query != 'y' && query != 'n') {
				if (query != ESCAPE)
				    bell();
				erase_line(MSG_LINE, 0);
				item = (-1);
			    }
			} else if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else
			    query = 'y';
			if (item >= 0) {
			    free_turn_flag = FALSE;	/* Player turn   */
			    inven_drop(item, query == 'y');
			    check_strength();
			}
			selecting = FALSE;
			/* As a safety measure, set the player's inven weight to 0,
			 * when the last object is dropped. */
			if (inven_ctr == 0 && equip_ctr == 0)
			    inven_weight = 0;
		    }
		    if (free_turn_flag == FALSE && scr_state == BLANK_SCR)
			selecting = FALSE;
		}
	    }
	}
	if (which == ESCAPE || scr_state == BLANK_SCR)
	    command = ESCAPE;
	else if (!free_turn_flag) {
	/* Save state for recovery if they want to call us again next turn. */
	    if (selecting)
		doing_inven = command;
	    else
		doing_inven = ' '; /* A dummy command to recover screen. */
	/* flush last message before clearing screen_change and exiting */
	    msg_print(NULL);
	    screen_change = FALSE; /* This lets us know if the world changes */
	    command = ESCAPE;
	} else {
	/* Put an appropriate header. */
	    if (scr_state == INVEN_SCR) {
		if (!show_inven_weight || inven_ctr == 0)
		    (void)sprintf(prt1,
			  "You are carrying %d.%d pounds. In your pack there is %s",
				  inven_weight / 10, inven_weight % 10,
				  (inven_ctr == 0 ? "nothing." : "-"));
		else
		    (void)sprintf(prt1,
		  "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. %s",
				  inven_weight / 10, inven_weight % 10,
				  weight_limit() / 10, weight_limit() % 10,
				  "In your pack is -");
		prt(prt1, 0, 0);
	    } else if (scr_state == WEAR_SCR) {
		if (wear_high < wear_low)
		    prt("You have nothing you could wield.", 0, 0);
		else
		    prt("You could wield -", 0, 0);
	    } else if (scr_state == EQUIP_SCR) {
		if (equip_ctr == 0)
		    prt("You are not using anything.", 0, 0);
		else
		    prt("You are using -", 0, 0);
	    } else
		prt("Allowed commands:", 0, 0);
	    erase_line(scr_base, scr_left);
	    put_str("e/i/t/w/x/d/?/ESC:", scr_base, 60);
	    command = inkey();
	    erase_line(scr_base, scr_left);
	}
    }
    while (command != ESCAPE);
    if (scr_state != BLANK_SCR)
	restore_screen();

/* Update screen for changed light radius. -DGK */
/* Fix redisplay of lighting when in stores. -DGK */
    if (light_chg && !in_store_flag) {
	register int        i, j;
	int                 min_i, max_i, min_j, max_j;

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
	min_i = MY_MAX(0, (char_row - cur_lite));
	max_i = MY_MIN(cur_height, (char_row + cur_lite));
	min_j = MY_MAX(0, (char_col - cur_lite));
	max_j = MY_MIN(cur_width, (char_col + cur_lite));

	for (i = min_i; i <= max_i; i++)
	    for (j = min_j; j <= max_j; j++)
		cave[i][j].tl = FALSE;

#ifdef TC_COLOR
	textcolor(LIGHTGRAY);
#endif
	tmp2 = cur_lite;
	print('@', char_row, char_col);
	if (inventory[INVEN_LITE].tval == TV_LITE)
	    tmp = inventory[INVEN_LITE].sval;
	else
	    tmp = 195;
	cur_lite = 1 + (tmp < 190) + (tmp == 4 || tmp == 6);
	if (tmp2 < cur_lite)
	    tmp2 = cur_lite;

	if (!p_ptr->flags.blind) {
	    min_i = MY_MAX(0, (char_row - cur_lite));
	    max_i = MY_MIN(cur_height, (char_row + cur_lite));
	    min_j = MY_MAX(0, (char_col - cur_lite));
	    max_j = MY_MIN(cur_width, (char_col + cur_lite));

	    for (i = min_i; i <= max_i; i++)
		for (j = min_j; j <= max_j; j++)
		    if (los(char_row, char_col, i, j) &&
			distance(char_row, char_col, i, j) <= cur_lite)
			cave[i][j].tl = TRUE;
	}

	min_i = MY_MAX(0, (char_row - tmp2));
	max_i = MY_MIN(cur_height, (char_row + tmp2));
	min_j = MY_MAX(0, (char_col - tmp2));
	max_j = MY_MIN(cur_width, (char_col + tmp2));
	for (i = min_i; i <= max_i; i++)
	    for (j = min_j; j <= max_j; j++)
		lite_spot(i, j);
    }
    calc_bonuses();

    /* If we ain't in a store, do the equippy chars -DGK*/
    if (!in_store_flag) 
	prt_equippy_chars();
}


/*
 * Destroy an item in the inventory			-RAK-	 */
void inven_destroy(int item_val)
{
    register int         j;
    register inven_type *i_ptr;

    i_ptr = &inventory[item_val];
    if ((i_ptr->number > 1) && (i_ptr->sval <= ITEM_SINGLE_STACK_MAX)) {
	i_ptr->number--;
	inven_weight -= i_ptr->weight;
    } else {
	inven_weight -= i_ptr->weight * i_ptr->number;
	for (j = item_val; j < inven_ctr - 1; j++)
	    inventory[j] = inventory[j + 1];
	invcopy(&inventory[inven_ctr - 1], OBJ_NOTHING);
	inven_ctr--;
    }
    p_ptr->flags.status |= PY_STR_WGT;
}


/*
 * Copies the object in the second argument over the first argument. However,
 * the second always gets a number of one except for ammo etc. 
 */
void take_one_item(register inven_type *s_ptr, register inven_type *i_ptr)
{
    *s_ptr = *i_ptr;
    if ((s_ptr->number > 1) && (s_ptr->sval >= ITEM_SINGLE_STACK_MIN)
	&& (s_ptr->sval <= ITEM_SINGLE_STACK_MAX))
	s_ptr->number = 1;
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

