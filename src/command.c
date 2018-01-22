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
    u32b               j1, j2, tmp;
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

    if (no_lite()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->flags.confused > 0) {
	msg_print("You are too confused.");
	return;
    }

    
    /* Get a book or stop checking */
    if (!get_item(&item_val, "Which Book?", i, k, 0)) return;

    flag = FALSE;

    i_ptr = &inventory[item_val];

    /* Check the language */
    if (class[p_ptr->misc.pclass].spell == MAGE) {
	if (i_ptr->tval == TV_MAGIC_BOOK) flag = TRUE;
    }
    else if (class[p_ptr->misc.pclass].spell == PRIEST) {
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
	s_ptr = &magic_spell[p_ptr->misc.pclass - 1][k];
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

    c_ptr = &cave[char_row][char_col];

    /* Verify stairs */
    if ((c_ptr->tptr == 0)||(i_list[c_ptr->tptr].tval != TV_UP_STAIR)) {
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

    c_ptr = &cave[char_row][char_col];

    if ((c_ptr->tptr == 0)||(i_list[c_ptr->tptr].tval != TV_DOWN_STAIR)) {
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
 * Refill the players lamp	-RAK-
 */
static void do_cmd_refill_lamp()
{
    int                  i, j;
    register int         k;
    register inven_type *i_ptr;

    free_turn_flag = TRUE;
    k = inventory[INVEN_LITE].sval;
    if (k != 0) msg_print("But you are not using a lamp.");

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
 */
/*
 */
/*
 */
/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 */
void do_command(char com_val)
{
    int                    dir_val, do_pickup;
    int                    y, x, i, j = 0;
    vtype                  out_val, tmp_str;
    register struct flags *f_ptr;
    char                   prt1[80];

/* hack for move without pickup.  Map '-' to a movement command. */
    if (com_val == '-') {
	do_pickup = FALSE;
	i = command_count;
#ifdef TARGET
	{
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	int temp = target_mode;
	target_mode = FALSE;
#endif
	if (get_dir(NULL, &dir_val)) {
	    command_count = i;
	    switch (dir_val) {
	      case 1:
		com_val = 'b';
		break;
	      case 2:
		com_val = 'j';
		break;
	      case 3:
		com_val = 'n';
		break;
	      case 4:
		com_val = 'h';
		break;
	      case 6:
		com_val = 'l';
		break;
	      case 7:
		com_val = 'y';
		break;
	      case 8:
		com_val = 'k';
		break;
	      case 9:
		com_val = 'u';
		break;
	      default:
		com_val = '(';
		break;
	    }
	} else
	    com_val = ' ';
#ifdef TARGET
	target_mode = temp;
	}
#endif
    } else
	do_pickup = TRUE;

    switch (com_val) {
      case 'Q':			/* (Q)uit		(^K)ill */
	flush();
	if ((!total_winner) ? get_check("Do you really want to quit?")
	    : get_check("Do you want to retire?")) {
	    new_level_flag = TRUE;
	    death = TRUE;
	    (void)strcpy(died_from, "Quitting");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('P'):		/* (^P)revious message. */
	if (command_count > 0) {
	    i = command_count;
	    if (i > MAX_SAVE_MSG)
		i = MAX_SAVE_MSG;
	    command_count = 0;
	} else if (last_command != 16)
	    i = 1;
	else
	    i = MAX_SAVE_MSG;
	j = last_msg;
	if (i > 1) {
	    save_screen();
	    x = i;
	    while (i > 0) {
		i--;
		prt(old_msg[j], i, 0);
		if (j == 0)
		    j = MAX_SAVE_MSG - 1;
		else
		    j--;
	    }
	    erase_line(x, 0);
	    pause_line(x);
	    restore_screen();
	} else {
	/* Distinguish real and recovered messages with a '>'. -CJS- */
	    put_str(">", 0, 0);
	    prt(old_msg[j], 0, 1);
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('F'):		/* Repeat (^F)eeling */
	free_turn_flag = TRUE;
	do_cmd_feeling();
	break;
      case CTRL('W'):		/* (^W)izard mode */
	if (wizard) {
	    wizard = FALSE;
	    msg_print("Wizard mode off.");
	} else if (enter_wiz_mode())
	    msg_print("Wizard mode on.");
	prt_winner();
	free_turn_flag = TRUE;
	break;
      case CTRL('X'):		/* e(^X)it and save */
	if (total_winner) {
	    msg_print("You are a Total Winner,  your character must be retired.");
	    if (rogue_like_commands)
		msg_print("Use 'Q' to when you are ready to retire.");
	    else
		msg_print("Use <Control>-K when you are ready to retire.");
	} else {
	    (void)strcpy(died_from, "(saved)");
	    msg_print("Saving game...");
	    if (save_player())
		exit_game();
	    msg_print("Save failed...");
	    (void)strcpy(died_from, "(alive and well)");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('R'):
	if (p_ptr->flags.image > 0)
	    msg_print("You cannot be sure what is real and what is not!");
	else {
	    draw_cave();
	    update_monsters();	  /* draw monsters */
	    prt_equippy_chars();  /* redraw equippy chars */
	}
	free_turn_flag = TRUE;
	break;
#ifdef TARGET
/* select a target (sorry, no intuitive letter keys were left: a/A for aim,
 * t/T for target, f/F for focus, s/S for select, c/C for choose and p/P for pick
 *  were all already taken.  Wiz light command moved to '$', which was unused. -CFT
 */
      case '*':
	target();		/* target code taken from Morgul -CFT */
	free_turn_flag = TRUE;
	break;    			
#endif
      case '=':			/* (=) set options */
	save_screen();
	do_cmd_options();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case '{':			/* ({) inscribe an object    */
	scribe_object();
	free_turn_flag = TRUE;
	break;
      case '!':			/* (!) escape to the shell */
	if (!wizard)
#ifdef MSDOS			/* Let's be a little more accurate... */
	    msg_print("Sorry, Angband doesn't leave enough free memory for a subshell.");
#else
	    msg_print("Sorry, inferior shells are not allowed from ANGBAND.");
#endif
	else
	    rerate();
	free_turn_flag = TRUE;
	break;
      case ESCAPE:		/* (ESC)   do nothing. */
      case ' ':			/* (space) do nothing. */
	free_turn_flag = TRUE;
	break;
      case 'b':			/* (b) down, left	(1) */
	move_player(1, do_pickup);
	break;
      case 'j':			/* (j) down		(2) */
	move_player(2, do_pickup);
	break;
      case 'n':			/* (n) down, right	(3) */
	move_player(3, do_pickup);
	break;
      case 'h':			/* (h) left		(4) */
	move_player(4, do_pickup);
	break;
      case 'l':			/* (l) right		(6) */
	move_player(6, do_pickup);
	break;
      case 'y':			/* (y) up, left		(7) */
	move_player(7, do_pickup);
	break;
      case 'k':			/* (k) up		(8) */
	move_player(8, do_pickup);
	break;
      case 'u':			/* (u) up, right	(9) */
	move_player(9, do_pickup);
	break;
      case 'B':			/* (B) run down, left	(. 1) */
	find_init(1);
	break;
      case 'J':			/* (J) run down		(. 2) */
	find_init(2);
	break;
      case 'N':			/* (N) run down, right	(. 3) */
	find_init(3);
	break;
      case 'H':			/* (H) run left		(. 4) */
	find_init(4);
	break;
      case 'L':			/* (L) run right	(. 6) */
	find_init(6);
	break;
      case 'Y':			/* (Y) run up, left	(. 7) */
	find_init(7);
	break;
      case 'K':			/* (K) run up		(. 8) */
	find_init(8);
	break;
      case 'U':			/* (U) run up, right	(. 9) */
	find_init(9);
	break;
      case '/':			/* (/) identify a symbol */
	ident_char();
	free_turn_flag = TRUE;
	break;
      case '.':			/* (.) stay in one place (5) */
	move_player(5, do_pickup);
	if (command_count > 1) {
	    command_count--;
	    rest();
	}
	break;
      case '<':			/* (<) go down a staircase */
	do_cmd_go_up();
	break;
      case '>':			/* (>) go up a staircase */
	do_cmd_go_down();
	break;
      case '?':			/* (?) help with commands */
	if (rogue_like_commands)
	    helpfile(ANGBAND_R_HELP);
	else
	    helpfile(ANGBAND_O_HELP);
	free_turn_flag = TRUE;
	break;
#ifdef ALLOW_SCORE
      case 'v':   /* score patch originally by Mike Welsh mikewe@acacia.cs.pdx.edu */
	sprintf(prt1,"Your current score is: %ld", total_points());
	msg_print(prt1);
	break;
#endif
      case 'f':			/* (f)orce		(B)ash */
	bash();
	break;
      case 'A':			/* (A)ctivate		(A)ctivate */
	activate();
	break;
      case 'C':			/* (C)haracter description */
	save_screen();
	change_name();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case 'D':			/* (D)isarm trap */
	do_cmd_disarm();
	break;
      case 'E':			/* (E)at food */
	eat();
	break;
      case 'F':			/* (F)ill lamp */
	do_cmd_refill_lamp();
	break;
      case 'G':			/* (G)ain magic spells */
	gain_spells();
	break;
      case 'g':			/* (g)et an object... */
	if (prompt_carry_flag) {
	    if (cave[char_row][char_col].tptr != 0)	/* minor change -CFT */
		carry(char_row, char_col, TRUE);
	} else
	    free_turn_flag = TRUE;
	break;
      case 'W':			/* (W)here are we on the map	(L)ocate on map */
	if ((p_ptr->flags.blind > 0) || no_lite())
	    msg_print("You can't see your map.");
	else {
	    int                 cy, cx, p_y, p_x;
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	    int temp = target_mode;
	    target_mode = FALSE;
#endif

	    y = char_row;
	    x = char_col;
	    if (get_panel(y, x, TRUE))
		prt_map();
	    cy = panel_row;
	    cx = panel_col;
	    for (;;) {
		p_y = panel_row;
		p_x = panel_col;
		if (p_y == cy && p_x == cx)
		    tmp_str[0] = '\0';
		else
		    (void)sprintf(tmp_str, "%s%s of",
			     p_y < cy ? " North" : p_y > cy ? " South" : "",
			      p_x < cx ? " West" : p_x > cx ? " East" : "");
		(void)sprintf(out_val,
      "Map sector [%d,%d], which is%s your sector. Look which direction?",
			      p_y, p_x, tmp_str);
		if (!get_dir(out_val, &dir_val))
		    break;

/* -CJS- Should really use the move function, but what the hell. This is nicer,
 * as it moves exactly to the same place in another section. The direction
 * calculation is not intuitive. Sorry.
 */
		for (;;) {
		    x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
		    y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
		    if (x < 0 || y < 0 || x >= cur_width || y >= cur_width) {
			msg_print("You've gone past the end of your map.");
			x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
			y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
			break;
		    }
		    if (get_panel(y, x, TRUE)) {
			prt_map();
			break;
		    }
		}
	    }
	/* Move to a new panel - but only if really necessary. */
	    if (get_panel(char_row, char_col, FALSE))
		prt_map();
#ifdef TARGET
	    target_mode = temp; /* restore target mode... */
#endif
	}
	free_turn_flag = TRUE;
	break;
      case 'R':			/* (R)est a while */
	rest();
	break;
      case '#':			/* (#) search toggle	(S)earch toggle */
	if (p_ptr->flags.status & PY_SEARCH)
	    search_off();
	else
	    search_on();
	free_turn_flag = TRUE;
	break;
      case CTRL('B'):		/* (^B) tunnel down left	(T 1) */
	tunnel(1);
	break;
      case CTRL('M'):		/* cr must be treated same as lf. */
      case CTRL('J'):		/* (^J) tunnel down		(T 2) */
	tunnel(2);
	break;
      case CTRL('N'):		/* (^N) tunnel down right	(T 3) */
	tunnel(3);
	break;
      case CTRL('H'):		/* (^H) tunnel left		(T 4) */
	tunnel(4);
	break;
      case CTRL('L'):		/* (^L) tunnel right		(T 6) */
	tunnel(6);
	break;
      case CTRL('Y'):		/* (^Y) tunnel up left		(T 7) */
	tunnel(7);
	break;
      case CTRL('K'):		/* (^K) tunnel up		(T 8) */
	tunnel(8);
	break;
      case CTRL('U'):		/* (^U) tunnel up right		(T 9) */
	tunnel(9);
	break;
      case 'z':			/* (z)ap a wand		(a)im a wand */
	do_cmd_aim_wand();
	break;
      case 'a':			/* (a)ctivate a rod	(z)ap a rod */
	do_cmd_zap_rod();
	break;
      case 'M':
	screen_map();
	free_turn_flag = TRUE;
	break;
      case 'P':			/* (P)eruse a book	(B)rowse in a book */
	do_cmd_browse();
	free_turn_flag = TRUE;
	break;
      case 'c':			/* (c)lose an object */
	do_cmd_close();
	break;
      case 'd':			/* (d)rop something */
	inven_command('d');
	break;
      case 'e':			/* (e)quipment list */
	inven_command('e');
	break;
      case 't':			/* (t)hrow something	(f)ire something */
	do_cmd_fire();
	break;
      case 'i':			/* (i)nventory list */
	inven_command('i');
	break;
      case 'S':			/* (S)pike a door	(j)am a door */
	do_cmd_spike();
	break;
      case 'x':			/* e(x)amine surrounds	(l)ook about */
	look();
	free_turn_flag = TRUE;
	break;
      case 'm':			/* (m)agic spells */
	cast();
	break;
      case 'o':			/* (o)pen something */
	do_cmd_open();
	break;
      case 'p':			/* (p)ray */
	pray();
	break;
      case 'q':			/* (q)uaff */
	quaff();
	break;
      case 'r':			/* (r)ead */
	read_scroll();
	break;
      case 's':			/* (s)earch for a turn */
	search(char_row, char_col, p_ptr->misc.srh);
	break;
      case 'T':			/* (T)ake off something	(t)ake off */
	inven_command('t');
	break;
      case 'Z':			/* (Z)ap a staff	(u)se a staff */
	use();
	break;
      case 'V':			/* (V)ersion of game */
	helpfile(ANGBAND_VERSION);
	free_turn_flag = TRUE;
	break;
      case 'w':			/* (w)ear or wield */
	inven_command('w');
	break;
      case 'X':			/* e(X)change weapons	e(x)change */
	inven_command('x');
	break;
#ifdef ALLOW_CHECK_ARTIFACTS /* -CWS */
      case '~':
	if ((!wizard) && (dun_level != 0)) {
	    msg_print("You need to be on the town level to check artifacts!");
	    msg_print(NULL);		/* make sure can see the message -CWS */
	} else
	    artifact_check_no_file();
	break;
#endif
#ifdef ALLOW_CHECK_UNIQUES /* -CWS */
      case '|':
	do_cmd_check_uniques();
	break;
#endif
      default:
	if (wizard) {
	    free_turn_flag = TRUE; /* Wizard commands are free moves */
	    switch (com_val) {
	      case '\\':	   /* \ wizard help */
		helpfile(ANGBAND_W_HELP);
	      case CTRL('A'):	   /* ^A = Cure all */
		(void)remove_all_curse();
		(void)cure_blindness();
		(void)cure_confusion();
		(void)cure_poison();
		(void)remove_fear();
		(void)res_stat(A_STR);
		(void)res_stat(A_INT);
		(void)res_stat(A_WIS);
		(void)res_stat(A_CON);
		(void)res_stat(A_DEX);
		(void)res_stat(A_CHR);
		(void)restore_level();
		(void)hp_player(2000);
		p_ptr->flags.food = PLAYER_FOOD_MAX;
		f_ptr = &p_ptr->flags;
		if (f_ptr->slow > 1)
		    f_ptr->slow = 1;
		if (f_ptr->image > 1)
		    f_ptr->image = 1;
		if (f_ptr->cut > 1)
		    f_ptr->cut = 1;
		if (f_ptr->stun > 1)
		    f_ptr->stun = 1;
		break;
	      case CTRL('D'):	/* ^D = up/down */
		if (command_count > 0) {
		    if (command_count > 99)
			i = 0;
		    else
			i = command_count;
		    command_count = 0;
		} else {
		    prt("Go to which level (0-10000) ? ", 0, 0);
		    i = (-1);
		    if (get_string(tmp_str, 0, 27, 10))
			i = atoi(tmp_str);
		    if (i > 10000)
			i = 10000;
		}
		if (i > -1) {
		    dun_level = i;
		    if (dun_level > 10000)
			dun_level = 10000;
		    new_level_flag = TRUE;
		} else
		    erase_line(MSG_LINE, 0);
		break;
	      case CTRL('E'):	/* ^E = wizchar */
		change_character();
		erase_line(MSG_LINE, 0); /* from um55 -CFT */
		break;
	      case CTRL('G'):	/* ^G = treasure */
		if (command_count > 0) {
		    i = command_count;
		    command_count = 0;
		} else
		    i = 1;
		random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('I'):	/* ^I = identify */
		(void)ident_spell();
		break;
	      case CTRL('O'):	/* ^O = objects */
		print_objects();
		break;
	      case CTRL('T'):	/* ^T = teleport */
		teleport(100);
		break;
	      case CTRL('V'):	/* ^V special treasure */
		if (command_count > 0) {
		    i = command_count;
		    command_count = 0;
		} else
		    i = 1;
		special_random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('Z'):	/* ^Z = genocide */
		(void)mass_genocide(FALSE);
		break;
	      case ':':
		map_area();
		break;
	      case '~':
		artifact_check_no_file();
		break;
	      case '|':
		do_cmd_check_uniques();
		break;
	      case '@':
		wizard_create();
		break;
	      case '$':	   /* $ = wiz light */
		wiz_lite(TRUE);
		break;
	      case '%':	   /* self-knowledge */
		self_knowledge();
		break;
	      case '&':	   /* & = summon  */
		y = char_row;
		x = char_col;
		(void)summon_monster(&y, &x, TRUE);
		update_monsters();
		break;
	      case '*':		/* '*' = identify all up to a level */
		prt("Identify objects upto which level (0-200) ? ", 0, 0);
		i = (-1);
		if (get_string(tmp_str, 0, 47, 10))
		    i = atoi(tmp_str);
		if (i > 200)
		    i = 200;
		if (i > -1) {
		    int                 temp;
		    inven_type          inv;

		    for (temp = 0; temp < MAX_DUNGEON_OBJ; temp++) {
			if (k_list[temp].level <= i) {
			    invcopy(&inv, temp);
			    known1(&inv);
			}
		    }
		}
		erase_line(MSG_LINE, 0);
		break;
	      case '+':
		if (command_count > 0) {
		    p_ptr->misc.exp = command_count;
		    command_count = 0;
		} else if (p_ptr->misc.exp == 0)
		    p_ptr->misc.exp = 1;
		else
		    p_ptr->misc.exp = p_ptr->misc.exp * 2;
		prt_experience();
		break;
	      default:
		prt("Type '?' or '\\' for help.", 0, 0);
	    }
	} else {
	    prt("Type '?' for help.", 0, 0);
	    free_turn_flag = TRUE;
	}
    }
    last_command = com_val;
}

