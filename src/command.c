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
 * Give the player some help (files may be out of date)
 */
static void do_cmd_help(cptr fname)
{	    
    /* Help is always free */
    free_turn_flag = TRUE;

    /* Default help files */
    if (!fname) fname = rogue_like_commands ? ANGBAND_R_HELP : ANGBAND_O_HELP;

    /* Dump the help file file */
    helpfile(fname);
}




/*
 * Examine a Book					-RAK-	
 */
static void do_cmd_browse(void)
{
    u32b               j1, j2, tmp;
    int                  i, k, item_val, flag;
    int                  spell_index[63];
    register inven_type *i_ptr;
    register spell_type *s_ptr;
    int                  first_spell;

    /* This command is free */
    free_turn_flag = TRUE;

    if (!find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k)) {
	msg_print("You are not carrying any books.");
	return;
    }

    if (p_ptr->blind > 0) {
	msg_print("You can't see to read your spell book!");
	return;
    }

    if (no_lite()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->confused > 0) {
	msg_print("You are too confused.");
	return;
    }

    
    /* Get a book or stop checking */
    if (!get_item(&item_val, "Which Book?", i, k, 0)) return;

    flag = FALSE;

    i_ptr = &inventory[item_val];

    /* Check the language */
    if (class[p_ptr->pclass].spell == MAGE) {
	if (i_ptr->tval == TV_MAGIC_BOOK) flag = TRUE;
    }
    else if (class[p_ptr->pclass].spell == PRIEST) {
	if (i_ptr->tval == TV_PRAYER_BOOK) flag = TRUE;
    }

    if (!flag) {
	msg_print("You do not understand the language.");
	return;
    }

    i = 0;

    j1 = i_ptr->flags1;

    /* check which spell was first */
    tmp = j1;
    first_spell = bit_pos(&tmp);

    while (j1) {
	k = bit_pos(&j1);
	s_ptr = &magic_spell[p_ptr->pclass - 1][k];
	if (s_ptr->slevel < 99) {
	    spell_index[i] = k;
	    i++;
	}
    }

    j2 = i_ptr->flags2;

    /* if none from other set of flags */
    if (first_spell == -1) {
	tmp = j2;
	first_spell = 32 + bit_pos(&tmp);
    }

    while (j2) {
	k = bit_pos(&j2);
	s_ptr = &magic_spell[p_ptr->pclass - 1][k + 32];
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
    inven_type *i_ptr;

    c_ptr = &cave[char_row][char_col];
    i_ptr = &i_list[c_ptr->i_idx];

    /* Verify stairs */
    if ((c_ptr->i_idx == 0)||(i_ptr->tval != TV_UP_STAIR)) {
	msg_print("I see no up staircase here.");
	free_turn_flag = TRUE;
	return;
    }

    if (dun_level == Q_PLANE) {
	dun_level = 0;
	new_level_flag = TRUE;
	msg_print("You enter an inter-dimensional staircase. ");
	return;
    }

    /* Success */    
    msg_print("You enter a maze of up staircases. ");

    /* Go up the stairs */
    dun_level--;
    new_level_flag = TRUE;

    /* Create a way back */
    if (dun_level > 0) create_down_stair = TRUE;
}


/*
 * Go down one level -RAK-
 */
static void do_cmd_go_down()
{
    cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = &cave[char_row][char_col];
    i_ptr = &i_list[c_ptr->i_idx];

    if ((c_ptr->i_idx == 0)||(i_ptr->tval != TV_DOWN_STAIR)) {
	msg_print("I see no down staircase here.");
	free_turn_flag = TRUE;
	return;
    }

    if (dun_level == Q_PLANE) {
	dun_level = 0;
	new_level_flag = TRUE;
	msg_print("You enter an inter-dimensional staircase. ");
        return;
    }
    /* Success */
    msg_print("You enter a maze of down staircases. ");

    /* Go down */
    dun_level++;
    new_level_flag = TRUE;

    /* Create a way back */
    create_up_stair = TRUE;
}


/*
 * Hack -- commit suicide
 */
static void do_cmd_suicide(void)
{
    free_turn_flag = TRUE;

    flush();

    if (total_winner) {
	if (!get_check("Do you want to retire?")) return;
    }
    else {
	if (!get_check("Do you really want to quit?")) return;
    }

    new_level_flag = TRUE;
    death = TRUE;
    (void)strcpy(died_from, "Quitting");
}


/*
 * Hack -- redraw the screen
 */
static void do_cmd_redraw(void)
{
    free_turn_flag = TRUE;

    if (p_ptr->image > 0) {
	msg_print("You cannot be sure what is real and what is not!");
    }

    else {
    /* Redraw the screen */
    draw_cave();
    update_monsters();	  /* draw monsters */
    prt_equippy_chars();  /* redraw equippy chars */
    }
}


/*
 * Hack -- change name
 */
static void do_cmd_change_name(void)
{
    free_turn_flag = TRUE;

    save_screen();
    change_name();
    restore_screen();
}


/*
 * Hack -- toggle search mode
 */
static void do_cmd_toggle_search(void)
{
    free_turn_flag = TRUE;

    if (p_ptr->status & PY_SEARCH) {
	search_off();
    }
    else {
	search_on();
    }
}


/*
 * Hack -- pick up objects
 */
static void do_cmd_pick_up(void)
{
    free_turn_flag = TRUE;

    if (!prompt_carry_flag) return;
    
    if (cave[char_row][char_col].i_idx == 0) return; /* minor change -CFT */

    free_turn_flag = FALSE;
    carry(char_row, char_col, TRUE);
}


/*
 * Refill the players lamp	-RAK-
 */
static void do_cmd_refill_lamp()
{
    int                  i, j;

    register inven_type *i_ptr;

    free_turn_flag = TRUE;
    i = inventory[INVEN_LITE].sval;
    if (i != 0) msg_print("But you are not using a lamp.");

    /* Find some oil or complain */
    else if (!find_range(TV_FLASK, TV_NEVER, &i, &j)) {
	msg_print("You have no oil.");
    }

    else {

	free_turn_flag = FALSE;
	i_ptr = &inventory[INVEN_LITE];

	i_ptr->pval += inventory[i].pval;

	if (i_ptr->pval > FUEL_LAMP) {
	    i_ptr->pval = FUEL_LAMP;
	    msg_print("Your lamp overflows, spilling oil on the ground.");
	    msg_print("Your lamp is full.");
	}
	else if (i_ptr->pval > FUEL_LAMP / 2) {
	    msg_print("Your lamp is more than half full.");
	}
	else if (i_ptr->pval == FUEL_LAMP / 2) {
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
 * Support code for the "CTRL('P')" recall command
 */

static void do_cmd_messages(void)
{
    int i = 0, j = 0;

    /* Free move */
    free_turn_flag = TRUE;

    if (command_rep > 0) {
        i = command_rep;
        if (i > MAX_SAVE_MSG) i = MAX_SAVE_MSG;
        command_rep = 0;
    }
    else if (command_old != 16) i = 1;
    else i = MAX_SAVE_MSG;

    j = last_msg;

    if (i > 1) {

    /* Save the screen */
    save_screen();
    x = i;
    while (i > 0) {
	i--;
	prt(old_msg[j], i, 0);
	if (j == 0) j = MAX_SAVE_MSG - 1;
	else j--;
    }
    erase_line(x, 0);
    pause_line(x);

    /* Restore the screen */
    restore_screen();

    } else {
    /* Distinguish real and recovered messages with a '>'. -CJS- */
    put_str(">", 0, 0);
    prt(old_msg[j], 0, 1);
    }
}


/*
 * Target command
 */
static void do_cmd_target()
{
    /* Free move */
    free_turn_flag = TRUE;

#ifdef TARGET

    /* Be sure we can see */
    if (p_ptr->blind > 0) {
	msg_print("You can't see anything to target!");
    }
    else if (!target_set()) {
	msg_print("Target Aborted");
    }
    else {
	msg_print("Target Selected");
    }

#else

    msg_print("Target code not compiled in this version.");

#endif

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
static opt_desc options_interface[] = {

    { "Rogue-like commands", 			&rogue_like_commands },
    { "Quick messages",		                &quick_messages },
    { "(g)et-key to pickup objects", 		&prompt_carry_flag },
    { "Prompt before pickup", 			&carry_query_flag },
    { "Repeat obvious commands",		&always_repeat },

    { "Use new screen layout",                  &new_screen_layout },
    { "Display Equippy chars",		        &equippy_chars },
    { "Show dungeon level in feet",		&depth_in_feet },
    { "Highlight and notice mineral seams", 	&highlight_seams},

    { "Use color",				&use_color },

    { NULL,					NULL }
};


/*
 * Disturbance Options -- for running/resting
 */
static opt_desc options_disturb[] = {

    { "Cut known corners",	 		&find_cut },
    { "Examine potential corners",		&find_examine },
    { "Print self during run",			&find_prself },
    { "Stop when map sector changes",		&find_bound },
    { "Run through open doors", 		&find_ignore_doors },

    { NULL,					NULL }
};


/*
 * Gameplay Options -- these actually affect game-play
 */
static opt_desc options_gameplay[] = {

    { "Disable haggling in stores",		&no_haggle_flag },

    { "Show weights in inventory", 		&show_inven_weight },
    { "Show weights in equipment list",		&show_equip_weight },
    { "Plain object descriptions",		&plain_descriptions },

    { NULL,					NULL}
};


/*
 * Set or unset various boolean options. -CJS-
 */
static void do_cmd_options_aux(opt_desc *options, cptr info)
{
    register int i, max, ch;
    vtype        pmt, string;

    clear_screen();

    /* Prompt */    
    sprintf(pmt, "%s (RET to advance, y/n to set, ESC to accept) ", info);
    prt(pmt, 0, 0);

    /* Prepare the screen, Count the options. */
    for (max = 0; options[max].o_prompt; max++) {
	sprintf(string, "%-38s: %s ", options[max].o_prompt,
		(*options[max].o_var ? "yes" : "no "));
	prt(string, max + 2, 0);
    }

    /* Start at the first option */
    i = 0;

    /* Interact with the player */
    for (;;) {
	move_cursor(i + 2, 40);
	ch = inkey();
	switch (ch) {
	  case ESCAPE:	  
	    return;
	  case '-':
	    i = (max + i - 1) % max;
	    break;
	  case ' ':
	  case '\n':
	  case '\r':
	    i = (i + 1) % max;
	    break;
	  case 'y':
	  case 'Y':
	    put_str("yes ", i + 2, 40);
	    *options[i].o_var = TRUE;
	    i = (i + 1) % max;
	    break;
	  case 'n':
	  case 'N':
	    put_str("no  ", i + 2, 40);
	    *options[i].o_var = FALSE;
	    i = (i + 1) % max;
	    break;
	  default:
	    bell();
	    break;
	}
    }
}


/*
 * Set or unset various options.  Redraw screen when done.
 */
static void do_cmd_options()
{
    register int i;


    while (1) {

	clear_screen();

	/* Give some choices */
	prt("(1) User Interface Options", 2, 5);
	prt("(2) Disturbance Options", 3, 5);
	prt("(3) Game-Play Options", 4, 5);
	prt("(4) Hitpoint Warning", 5, 5);
	prt("(5) Delay Speed", 6, 5);

	/* Ask for a choice */        
	prt("Angband Options (1-8 or ESC to exit) ", 0, 0);
	i = inkey();
	if (i == ESCAPE) break;

	/* General Options */
	if (i == '1') {

	    /* Process the general options */
	    do_cmd_options_aux(options_interface, "User Interface Options");
	}

	/* Disturbance Options */
	else if (i == '2') {

	    /* Process the running options */
	    do_cmd_options_aux(options_disturb, "Disturbance Options");
	}

	/* Gameplay Options */
	else if (i == '3') {

	    /* Process the running options */
	    do_cmd_options_aux(options_gameplay, "Game-Play Options");
	}

	/* Hack -- hitpoint warning factor */
	else if (i == '4') {

	    clear_screen();

	    /* Get a new value */
	    while (1) {
		char buf[128];
		sprintf(buf, "Current hitpoint warning: %d0%%", hitpoint_warn);
		prt(buf, 5, 5);
		prt("Hitpoint Warning (0-9 or ESC to accept) ", 0, 0);                    
		i = inkey();
		if (i == ESCAPE) break;
		if (isdigit(i)) hitpoint_warn = (i - '0');
		else bell();
	    }
	}

	/* Hack -- Delay Speed */
	else if (i == '5') {

	    clear_screen();

	    /* Get a new value */
	    while (1) {
		char buf[128];
		sprintf(buf, "Current delay speed: %d milliseconds", delay_spd);
		prt(buf, 5, 5);
		prt("Delay Speed (0-9 or ESC to accept) ", 0, 0);                    
		i = inkey();
		if (i == ESCAPE) break;
		if (isdigit(i)) delay_spd = (i - '0');
		else bell();
	    }
	}
	
	/* Unknown option */
	else {
	    bell();
	}
    }


    /* Free turn */
    free_turn_flag = TRUE;

    /* Redraw the screen */
    draw_cave();
    update_monsters();	 /* draw monsters */
    prt_equippy_chars(); /* redraw equippy chars */
}





/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 */
void process_command(void)
{
    char prt1[80];

    /* Parse the command */
    switch (command_cmd) {

	/* (ESC) do nothing. */
	case ESCAPE:
	    free_turn_flag = TRUE; break;

	/* (SPACE) do nothing */
	case ' ':
	    free_turn_flag = TRUE; break;


	 
	/*** Wizard Commands ***/
	    
	/* Toggle Wizard Mode */
	case CTRL('W'):
	    free_turn_flag = TRUE;
	    if (wizard) {
		wizard = FALSE;
		msg_print("Wizard mode off.");
	    }
	    else if (enter_wiz_mode()) {
		msg_print("Wizard mode on.");
	    }
	    else {
		msg_print("You are not allowed to do that.");
	    }
	    prt_winner();
	    break;

	/* Special Wizard Command */
	case CTRL('A'):
	    if (wizard) {
		do_wiz_command();
	    }
	    else {
		msg_print("You are not allowed to do that.");
	    }
	    break;


	/*** Extra commands ***/

	case '!': /* XXX Some BS about a subshell RMHV */
	    free_turn_flag = TRUE;
	    if (wizard) rerate();
	    break;


	/*** Inventory Commands ***/

	/* Wear or wield something */
	case 'w':
	    inven_command('w'); break;

	/* Take something off */
	case 'T':
	    inven_command('t'); break;

	/* Exchange primary and aux weapons */
	case 'X':
	    inven_command('x'); break;

	/* Drop something */
	case 'd':
	    inven_command('d'); break;

	/* Equipment list */
	case 'e':
	    inven_command('e'); break;

	/* Inventory */
	case 'i':
	    inven_command('i'); break;


	/*** Standard "Movement" Commands ***/

	/* Dig a tunnel */
	case '+':
	    do_cmd_tunnel(); break;

	/* Move without picking anything up */
	case '-':
	    do_cmd_walk(FALSE); break;

	/* Move (picking up) */
	case ';':
	    do_cmd_walk(TRUE); break;


	/*** Commands that "re-interpret" the repeat count ***/

	/* Begin Running -- Arg is Max Distance */
	case '.':
	    do_cmd_run(); break;

	/* Stay still, and, if repeated, Begin Resting */
	case ',':
	    do_cmd_stay(TRUE);
	    if (command_rep > 1) {
	        command_rep--;
	        do_cmd_rest();
	    }
	    break;

	/* Rest a while */
	case 'R':
	    do_cmd_rest(); break;



	/*** Searching, Resting ***/

	/* Pick up an object */
	case 'g':
	    do_cmd_pick_up(); break;

	/* Toggle search status */
	case '#':
	    do_cmd_toggle_search(); break;

	/* Search the adjoining grids */
	case 's':
	    do_cmd_search(); break;


	/*** Stairs and Doors and Chests and Traps ***/

	/* Go up staircases */
	case '<':
	    do_cmd_go_up(); break;

	/* Go down staircases */
	case '>':
	    do_cmd_go_down(); break;

	/* Open something */
	case 'o':
	    do_cmd_open(); break;

	/* Close something */
	case 'c':
	    do_cmd_close(); break;

	/* Spike a door */
	case 'S':
	    do_cmd_spike(); break;

	/* Force a door or Bash a monster. */
	case 'f':
	    do_cmd_bash(); break;

	/* Disarm a trap */
	case 'D':
	    do_cmd_disarm(); break;


	/*** Magic and Prayers ***/

	/* Peruse a Book */
	case 'P':
	    do_cmd_browse(); break;

	/* Gain some spells */
	case 'G':
	    gain_spells(); break;

	/* Cast a magic spell */
	case 'm':
	    cast(); break;

	/* Pray a prayer */
	case 'p':
	    pray(); break;


	/*** Use various objects ***/

	/* Inscribe an object */
	case '{':
	    do_cmd_inscribe(); break;

	/* Activate an artifact */
	case 'A':
	    do_cmd_activate(); break;

	/* Eat some food */
	case 'E':
	    eat(); break;

	/* Fill the lamp */
	case 'F':
	    do_cmd_refill_lamp(); break;

	/* Throw something */
	case 't':
	    do_cmd_fire(); break;

	/* Zap a wand */
	case 'z':
	    do_cmd_aim_wand(); break;

	/* Activate a rod */
	case 'a':
	    do_cmd_zap_rod(); break;

	/* Quaff a potion */
	case 'q':
	    quaff(); break;

	/* Read a scroll */
	case 'r':
	    do_cmd_read_scroll(); break;

	/* Zap a staff */
	case 'Z':
	    use(); break;


	/*** Looking at Things (nearby or on map) ***/

	/* Full screen Map */
	case 'M':
	    do_cmd_view_map(); break;

	/* Locate player on the map */	
	case 'W':
	    do_cmd_locate(); break;

	/* Examine surroundings */
	case 'x':
	    do_cmd_look(); break;

	/* Attempt to select a new target, if compiled */
	/* select a target (sorry, no intuitive letter keys were left: a/A for aim,
	 * t/T for target, f/F for focus, s/S for select, c/C for choose and p/P for pick
	 *  were all already taken.  Wiz light command moved to '$', which was unused. -CFT */
	case '*':
	    do_cmd_target(); break;



	/*** Help and Such ***/

	/* Help */
	case '?':
	    do_cmd_help(NULL); break;

	/* Identify Symbol */
	case '/':
	    do_cmd_query_symbol(); break;

	/* Character Description */
	case 'C':
	    do_cmd_change_name(); break;


	/*** System Commands ***/

	/* Game Version */
	case 'V':
	    do_cmd_help(ANGBAND_VERSION); break;

	/* Repeat Feeling */
	case CTRL('F'):
	    do_cmd_feeling(); break;

	/* Previous message(s). */
	case CTRL('P'):
	    do_cmd_messages(); break;

	/* Commit Suicide and Quit */
	case 'Q':
	    do_cmd_suicide(); break;

	/* Save and Quit */
	case CTRL('X'):
	    if (total_winner) {
	        msg_print("You are a Total Winner,  your character must be retired.");
	        if (rogue_like_commands)
	            msg_print("Use 'Q' to when you are ready to retire.");
	        else
	            msg_print("Use <Control>-K when you are ready to retire.");
	    } else {
	        (void)strcpy(died_from, "(saved)");
	        msg_print("Saving game...");
	        if (save_player()) exit_game();
	        msg_print("Save failed...");
	        (void)strcpy(died_from, "(alive and well)");
	    }
	    free_turn_flag = TRUE;
	    break;

	/* Redraw the screen */
	case CTRL('R'):
	    do_cmd_redraw(); break;

	/* Set options */
	case '=':
	    save_screen();
	    do_cmd_options();
	    restore_screen();
	    free_turn_flag = TRUE;
	    break;

#ifdef ALLOW_CHECK_ARTIFACTS /* -CWS */
	/* Check artifacts */
	case '~':
	if ((!wizard) && (dun_level != 0)) {
	    msg_print("You need to be on the town level to check artifacts!");
	    msg_print(NULL);		/* make sure can see the message -CWS */
	} else artifact_check_no_file();
	break;
#endif

#ifdef ALLOW_CHECK_UNIQUES /* -CWS */
	/* Check uniques */
	case '|':
	    do_cmd_check_uniques(); break;
#endif

#ifdef ALLOW_SCORE
      case 'v':   /* score patch originally by Mike Welsh mikewe@acacia.cs.pdx.edu */
	sprintf(prt1,"Your current score is: %ld", total_points());
	msg_print(prt1);
	break;
#endif	

	/* Hack -- Unknown command */
	default:
	    free_turn_flag = TRUE;
	    prt("Type '?' for help.", 0, 0);
	    return;
    }


    /* Save the command */
    command_old = command_cmd;
}





/*
 * XXX An explanation of the "Angband Keypress" concept. XXX
 *
 * Inherently, many Angband commands consist not only of a basic action
 * (such as "tunnel"), but also a direction (such as "north-east"), and
 * even a "repeat" count
 *
 * These components are thus explicitly represented, with globals.
 *
 * The "base command" (see below) is stored in "command_cmd"
 * The "desired direction" is stored in "command_dir".
 * The "repeat count" is stored in "command_rep"
 *
 * Hack -- a "command_dir" of "zero" means "the current target".
 *
 * Note that, to correctly handle repeating commands, all commands that
 * wish to be repeatable AND to do so with a specific "direction" had
 * better set "command_dir" on their first call to the user's DESIRED
 * direction.  A local copy can then be used to check confusion, etc.
 * The easiest way to do this is to actually USE "command_dir" as the
 * argument holding the direction (see "do_cmd_walk").
 *
 * The last command successfully executed is stored in "command_old".
 *
 */



/*
 * Check whether this command will accept a count.     -CJS- 
 */
static int command_takes_rep(char c)
{
    /* Examine the command */
    switch (c) {
      case 'Q':
      case CTRL('W'):
      case CTRL('X'):
      case '=':
      case '{':
      case '/':
      case '<':
      case '>':
      case '?':
      case 'A':
      case 'C':
      case 'E':
      case 'F':
      case 'G':
      case '#':
      case 'z':
      case 'P':
      case 'c':
      case 'd':
      case 'e':
      case 't':
      case 'i':
      case 'x':
      case 'm':
      case 'p':
      case 'q':
      case 'r':
      case 'T':
      case 'Z':
      case 'V':
      case 'w':
      case 'W':
      case 'X':
      case CTRL('A'):
      case '\\':
      case CTRL('I'):
      case CTRL('^'):
      case '$':
      case '*':
      case ':':
      case CTRL('T'):
      case CTRL('E'):
      case CTRL('F'):
      case CTRL('Z'):
      case CTRL('S'):
      case CTRL('Q'):
      case CTRL('R'):
	return FALSE;
      case CTRL('P'):
      case ESCAPE:
      case ' ':
      case '-':
      case 'b':
      case 'f':
      case 'j':
      case 'n':
      case 'h':
      case 'l':
      case 'y':
      case 'k':
      case 'u':
      case '.':
      case 'B':
      case 'J':
      case 'N':
      case 'H':
      case 'L':
      case 'Y':
      case 'K':
      case 'U':
      case 'D':
      case 'R':
      case CTRL('Y'):
      case CTRL('K'):
      case CTRL('U'):
      case CTRL('L'):
      case CTRL('N'):
      case CTRL('J'):
      case CTRL('B'):
      case CTRL('H'):
      case 'S':
      case 'o':
      case 's':
      case CTRL('D'):
      case CTRL('G'):
      case '+':
	return TRUE;
      default:
	return FALSE;
    }
}






/*
 * Request a command from the user.
 *
 * Sets command_cmd, command_dir, command_rep
 *
 * Note that "caret" ("^") is treated special, and is used to
 * allow manual input of control characters.  This can be used
 * on many machines to request repeated tunneling (Ctrl-H) and
 * on the Macintosh to request "Control-Caret".
 */
void request_command(void)
{
    int i = 0;
    char cmd;

    /*  Hack -- Illuminate the players character */
    move_cursor_relative(char_row, char_col);


    /* Hack -- process "repeated" commands */
    if (command_rep > 0) {

	/* Count this execution */
	command_rep--;

	/* Hack -- Flush the output */
	Term_fresh();

	/* Hack -- Assume messages were seen */
	msg_flag = FALSE;

	/* All done */
	return;
    }


    /* No command yet */
    command_cmd = 0;

    /* Hack -- no direction yet */
    command_dir = -1;


#ifdef TARGET
    /* This bit of targetting code taken from Morgul -CFT */
    /* If we are in targetting mode, with a creature target, make the targetted */
    /* row and column match the creature's.  This optimizes a lot of code.  CDW */
    if ((target_mode)&&(target_mon<(unsigned) m_max)) {
        target_row = m_list[target_mon].fy;
        target_col = m_list[target_mon].fx;
    }
#endif


    /* Get a keypress in "command" mode */
	msg_flag = FALSE;
	cmd = inkey();

    /* Get a count for a command. */
    if ((rogue_like_commands
    && command_cmd >= '0' && command_cmd <= '9')
    || (!rogue_like_commands && command_cmd == '#')) {
    char                tmp[8];

    /* Analyze the keypress */
    command_cmd = cmd;
    if (command_cmd == '#') command_cmd = '0';

    /* Special command -- Get a "count" for another command */

	/* Begin the input */
	prt("Repeat count:", 0, 0);

	/* Get a command count */
	while (1) {

	    /* Simple editing */
	    if (cmd == DELETE || cmd == CTRL('H')) {
		i = i / 10;
		(void)sprintf(tmp, "%d", i);
		prt(tmp, 0, 14);
	    }

	    /* Actual numeric data */
	    else if (cmd >= '0' && cmd <= '9') {

		/* Allow counts up to 999 */
		if (i > 99) {
		    bell();
		}

		/* Incorporate that digit */
		else {
		    i = i * 10 + cmd - '0';
		    (void)sprintf(tmp, "%d", i);
		    prt(tmp, 0, 14);
		}
	    }

	    /* Exit on "unusable" input */
	    else {
		break;
	    }
	    cmd = inkey();
	}

	/* Let a "non-count" default to 99 repetitions */
	if (i == 0) {
	    i = 99;
	    (void)sprintf(tmp, "%d", i);
	    prt(tmp, 0, 14);
	}

	/* Hack -- white-space means "enter command now" */
	if (cmd == ' ') {
	    prt("Command:", 0, 20);
	    cmd = inkey();
	}

	/* Analyze the keypress */
	command_cmd = cmd;
    }

    /* Another way of typing control codes -CJS- */
    if (command_cmd == '^') {

	if (command_rep > 0) prt_state();

	/* Get a char to "cast" into a control char */
	if (get_com("Control-", &cmd)) {

	    if (cmd >= 'A' && cmd <= 'Z') cmd -= 'A' - 1;
	    else if (cmd >= 'a' && cmd <= 'z') cmd -= 'a' - 1;
	    else {
		msg_print("Type ^ <letter> for a control char");
		cmd = ' ';
	    }
	}
	else cmd = ' ';

	/* Analyze the keypress */
	command_cmd = cmd;
    }


    /* Move cursor to player char again, in case it moved */
    move_cursor_relative(char_row, char_col);

    /* Hack -- let some commands get repeated by default */
    if (always_repeat && (i <= 0)) {

	/* Hack -- Tunnel gets 99 tries */
	if (command_cmd == '+') i = 99;
	
	/* Bash, Disarm, Open, Search get 99 tries */
	else if (strchr("BDos", command_cmd)) i = 99;
    }

    /* Commands are always converted to rogue form. -CJS- */
    if (rogue_like_commands == FALSE)
    command_cmd = original_commands(command_cmd);

    /* Make sure a "Count" is legal for this command */
    if (i > 0) {

	/* Commands that can be repeated */
	if (command_takes_rep(command_cmd)) {

	    /* Save and display the count */
	    command_rep = i;
	    prt_state();

	    /* This execution gets counted */
	    command_rep--;
	}

	/* Invalid combination */
	else {

	    /* Abort gracefully */
	    msg_print("Invalid command with a count.");

	    /* Forget the command */
	    command_cmd = ' ';
	}
    }
    
    /* Flash the message line. */
    erase_line(MSG_LINE, 0);

    /* Hack -- Hilite the player */
    move_cursor_relative(char_row, char_col);
}



