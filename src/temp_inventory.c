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
    p_ptr->status |= PY_STR_WGT;
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

