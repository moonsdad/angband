/* File: command.c */ 

/* Purpose: process player commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * Hack -- make sure we have a good "ANSI" definition for "CTRL()"
 */
#undef CTRL
#define CTRL(C) ((C)&037)



/*
 * Examine a Book					-RAK-	
 */
static void do_cmd_browse(void)
{
    u32b               j1, j2;
    int                  i, k, item_val, flag;
    int                  spell_index[63];
    register inven_type *i_ptr;
    register spell_type *s_ptr;
    int                  first_spell;

    if (!find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k)) {
	msg_print("You are not carrying any books.");
	return;
    }

    if (p_ptr->flags.blind > 0) {
	msg_print("You can't see to read your spell book!");
	return;
    }

    if (no_light()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->flags.confused > 0) {
	msg_print("You are too confused.");
	return;
    }

    
    /* Get a book or stop checking */
    if (!get_item(&item_val, "Which Book?", i, k, 0)) return;

    flag = TRUE;

    i_ptr = &inventory[item_val];

    /* Check the language */
    if (class[p_ptr->misc.pclass].spell == MAGE) {
	if (i_ptr->tval != TV_MAGIC_BOOK) flag = FALSE;
    }
    else if (class[p_ptr->misc.pclass].spell == PRIEST) {
	if (i_ptr->tval != TV_PRAYER_BOOK) flag = FALSE;
    }
    else flag = FALSE;

    if (!flag) {
	msg_print("You do not understand the language.");
	return;
    }

    i = 0;

    j1 = (u32b) inventory[item_val].flags;

    /* check which spell was first */
    first_spell = bit_pos(&j1);
    j1 = (u32b) inventory[item_val].flags;	/* restore j1 value */

    while (j1) {
	k = bit_pos(&j1);
	s_ptr = &magic_spell[p_ptr->misc.pclass - 1][k];
	if (s_ptr->slevel < 99) {
	    spell_index[i] = k;
	    i++;
	}
    }

    j2 = (u32b) inventory[item_val].flags2;

    /* if none from other set of flags */
    if (first_spell == -1) {
	first_spell = 32 + bit_pos(&j2);	/* get 1st spell # */
	j2 = (u32b) inventory[item_val].flags2;	/* and restore j2 */
    }

    while (j2) {
	k = bit_pos(&j2);
	s_ptr = &magic_spell[p_ptr->misc.pclass - 1][k + 32];
	if (s_ptr->slevel < 99) {
	    spell_index[i] = (k + 32);
	    i++;
	}
    }

    /* Display the spells */
    save_screen();
    print_spells(spell_index, i, TRUE, first_spell);
    pause_line(0);
    restore_screen();
}




/*
 * Go up one level					-RAK-	
 */
static void do_cmd_go_up()
{
    cave_type *c_ptr;
    int        no_stairs = FALSE;

    c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr != 0)
	if (t_list[c_ptr->tptr].tval == TV_UP_STAIR) {
	    if (dun_level == Q_PLANE) {
		dun_level = 0;
		new_level_flag = TRUE;
		msg_print("You enter an inter-dimensional staircase. ");
	    } else {
		dun_level--;
		new_level_flag = TRUE;
		if (dun_level > 0)
		    create_down_stair = TRUE;
		msg_print("You enter a maze of up staircases. ");
	    }
	} else
	    no_stairs = TRUE;
    else
	no_stairs = TRUE;

    if (no_stairs) {
	msg_print("I see no up staircase here.");
	free_turn_flag = TRUE;
    }
}


/*
 * Go down one level -RAK-
 */
static void do_cmd_go_down()
{
    cave_type *c_ptr;
    int        no_stairs = FALSE;

    c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr != 0)
	if (t_list[c_ptr->tptr].tval == TV_DOWN_STAIR) {
	    if (dun_level == Q_PLANE) {
		dun_level = 0;
		new_level_flag = TRUE;
		msg_print("You enter an inter-dimensional staircase. ");
	    } else {
		dun_level++;
		new_level_flag = TRUE;
		create_up_stair = TRUE;
		msg_print("You enter a maze of down staircases. ");
	    }
	} else
	    no_stairs = TRUE;
    else
	no_stairs = TRUE;

    if (no_stairs) {
	msg_print("I see no down staircase here.");
	free_turn_flag = TRUE;
    }
}


/*
 * Refill the players lamp	-RAK-
 */
static void do_cmd_refill_lamp()
{
    int                  i, j;
    register int         k;
    register inven_type *i_ptr;

    free_turn_flag = TRUE;
    k = inventory[INVEN_LITE].subval;
    if (k != 0)
	msg_print("But you are not using a lamp.");
    else if (!find_range(TV_FLASK, TV_NEVER, &i, &j)) {
	msg_print("You have no oil.");
    }

    else {

	free_turn_flag = FALSE;
	i_ptr = &inventory[INVEN_LITE];

	i_ptr->p1 += inventory[i].p1;

	if (i_ptr->p1 > FUEL_LAMP) {
	    i_ptr->p1 = FUEL_LAMP;
	    msg_print("Your lamp overflows, spilling oil on the ground.");
	    msg_print("Your lamp is full.");
	}
	else if (i_ptr->p1 > FUEL_LAMP / 2) {
	    msg_print("Your lamp is more than half full.");
	}
	else if (i_ptr->p1 == FUEL_LAMP / 2) {
	    msg_print("Your lamp is half full.");
	}
	else {
	    msg_print("Your lamp is less than half full.");
	}
	desc_remain(i);
	inven_destroy(i);
    }
}


/*
 * A simple structure to hold some options
 */
typedef struct _opt_desc {
    cptr	o_prompt;
    int         *o_var;
} opt_desc;


/*
 * General User-Interface Options
 */
static opt_desc options[] = {

    { "Running: cut known corners",	 	&find_cut },
    { "Running: examine potential corners",	&find_examine },
    { "Running: print self during run", 	&find_prself },
    { "Running: stop when map sector changes",  &find_bound},
    { "Running: run through open doors", 	&find_ignore_doors},
    { "(g)et-key to pickup objects", 		&prompt_carry_flag},
    { "Prompt before pickup", 			&carry_query_flag},
    { "Rogue like commands", 			&rogue_like_commands},
    { "Show weights in inventory", 		&show_inven_weight},
    { "Show weights in equipment list",		&show_equip_weight},
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
 * Set or unset various boolean options. -CJS-
 */
void set_options()
{
    register int i, max, ch;
    vtype        string;

    /* Prompt */    
    prt("  ESC when finished, y/n or 0-9 to set options, <return> or - to move cursor",
	0, 0);

    /* Prepare the screen, Count the options. */
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

    /* Start at the first option */
    i = 0;

    /* Interact with the player */
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
	    if (i > 0) i--;
	    else i = max - 1;
	    break;
	  case ' ':
	  case '\n':
	  case '\r':
	    i = (i + 1) % max;
	    break;
	  case 'y':
	  case 'Y':
	    if ((options[i].o_var == &hitpoint_warn) ||
		(options[i].o_var == &delay_spd))
		bell();
	    else {
		put_buffer("yes ", i + 1, 40);
	    *options[i].o_var = TRUE;
	    i = (i + 1) % max;
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
		i = (i + 1) % max;
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
		i = (i + 1) % max;
	    }
	    break;
	  default:
	    bell();
	    break;
	}
    }
}
