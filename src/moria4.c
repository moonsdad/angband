/* File: moria4.c */

/* Purpose: more low level code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"





/*** Targetting Code ***/




/*
 * This targetting code stolen from Morgul -CFT
 * Returns TRUE if the given position is the target. -CDW 
 */
int target_at(int row,int col)
{

#ifdef TARGET


    /* Compare the locations */
    if (target_mode && (row==target_row) && (col==target_col)) {
	return (TRUE);
    }

#endif

    /* Assume target is not at (row,col) */
    return (FALSE);
}





/*
 * This targetting code stolen from Morgul -CFT
 * Targetting routine 					CDW
 */
void target()
{

#ifdef TARGET

    int monptr,exit,exit2;
    char query;
    vtype desc;

    exit = FALSE;
    exit2 = FALSE;
    if (p_ptr->flags.blind > 0)
	msg_print("You can't see anything to target!");
    /* Check monsters first */
    else {
	target_mode = FALSE;
	for (monptr = 0; (monptr<mfptr) && (!exit); monptr++) {
	    if (m_list[monptr].cdis<MAX_SIGHT) {
		if ((m_list[monptr].ml)&&
		    (los(char_row,char_col,m_list[monptr].fy,m_list[monptr].fx))) {
		    move_cursor_relative(m_list[monptr].fy,m_list[monptr].fx);
		    (void) sprintf(desc, "%s [(r)ecall] [(t)arget] [(l)ocation] [ESC quits]",
				   r_list[m_list[monptr].mptr].name);
		    prt(desc,0,0);
		    move_cursor_relative(m_list[monptr].fy,m_list[monptr].fx);
		    query = inkey();
		    while ((query == 'r')||(query == 'R')) {
			save_screen();
			query = roff_recall(m_list[monptr].mptr);
			restore_screen();
			move_cursor_relative(m_list[monptr].fy,m_list[monptr].fx);
			query = inkey();
		    }
		    switch (query) {
		    case ESCAPE:
			exit = TRUE;
			exit2 = TRUE;
			break;
		    case '.':	/* for NetHack players, '.' is used to select a target,
				   so I'm changing this... -CFT */
		    case 't': case 'T':
			target_mode = TRUE;
			target_mon  = monptr;
			target_row  = m_list[monptr].fy;
			target_col  = m_list[monptr].fx;
			exit2 = TRUE;
		    case 'l': case'L':
			exit = TRUE;
		    default:
			break;
		    }
		}
	    }
	}
	if (exit2 == FALSE) {
	    prt("Use cursor to designate target. [(t)arget]",0,0);
	    target_row = char_row;
	    target_col = char_col;
	    for (exit = FALSE; exit==FALSE ;) {
		move_cursor_relative(target_row, target_col);
		query=inkey();
		if (rogue_like_commands==FALSE) {
		    switch (query) {
		    case '1':
			query = 'b';
			break;
		    case '2':
			query = 'j';
			break;
		    case '3':
			query = 'n';
			break;
		    case '4':
			query = 'h';
			break;
		    case '5':
			query = '.';
		    case '6':
			query = 'l';
			break;
		    case '7':
			query = 'y';
			break;
		    case '8':
			query = 'k';
			break;
		    case '9':
			query = 'u';
			break;
		    default:
			break;
		    }
		}
		switch (query) {
		case ESCAPE:
		    case'q':
		case 'Q':
		    exit = TRUE;
		    break;
		case '.':	/* for NetHack players, '.' is used to select a target,
				   so I'm changing this... -CFT */
		case 't':
		case 'T':
		    if (distance(char_row,char_col,target_row,target_col)>MAX_SIGHT)
			prt(
			    "Target beyond range. Use cursor to designate target. [(t)arget].",
			    0,0);
		    else if (cave[target_row][target_col].fval>CORR_FLOOR)
			prt(
			    "Invalid target. Use cursor to designate target. [(t)arget].",
			    0,0);
		    else {
			target_mode = TRUE;
			target_mon  = MAX_M_IDX;
			exit = TRUE;
		    }
		    break;
		case 'b':
		    target_col--;
		case 'j':
		    target_row++;
		    break;
		case 'n':
		    target_row++;
		case 'l':
		    target_col++;
		    break;
		case 'y':
		    target_row--;
		case 'h':
		    target_col--;
		    break;
		case 'u':
		    target_col++;
		case 'k':
		    target_row--;
		    break;
		default:
		    break;
		}
		if ((target_col>MAX_WIDTH-2)||(target_col>panel_col_max))
		    target_col--;
		else if ((target_col<1)||(target_col<panel_col_min))
		    target_col++;
		if ((target_row>MAX_HEIGHT-2)||(target_row>panel_row_max))
		    target_row--;
		else if ((target_row<1)||(target_row<panel_row_min))
		    target_row++;
		
	    }
	}
	if (target_mode==TRUE)
	    msg_print("Target selected.");
	else
	    msg_print("Aborting Target.");
    }

#endif /* TARGET */

}



/*
 * A more "correct" movement calculator.
 */
void mmove2(int *y, int *x, int sourcey, int sourcex, int desty, int destx)
{
    int d_y, d_x, k, dist, max_dist, min_dist, shift;

    d_y = (*y < sourcey) ? sourcey - *y : *y - sourcey;
    d_x = (*x < sourcex) ? sourcex - *x : *x - sourcex;

    dist = (d_y > d_x) ? d_y : d_x;
    dist++;

    d_y = (desty < sourcey) ? sourcey - desty : desty - sourcey;
    d_x = (destx < sourcex) ? sourcex - destx : destx - sourcex;

    if (d_y > d_x) {
	max_dist = d_y;
	min_dist = d_x;
    }
    else {
	max_dist = d_x;
	min_dist = d_y;
    }

    for (k = 0, shift = max_dist >> 1; k < dist; k++, shift -= min_dist) {
	shift = (shift > 0) ? shift : shift + max_dist;
    }

    if (shift < 0) shift = 0;

    if (d_y > d_x) {
	d_y = (desty < sourcey) ? *y - 1 : *y + 1;
	if (shift)
	    d_x = *x;
	else
	    d_x = (destx < sourcex) ? *x - 1 : *x + 1;
    }
    else {
	d_x = (destx < sourcex) ? *x - 1 : *x + 1;
	if (shift)
	    d_y = *y;
	else
	    d_y = (desty < sourcey) ? *y - 1 : *y + 1;
    }

    *y = d_y;
    *x = d_x;
}



/*
 * Given direction "dir", returns new row, column location -RAK-
 * targeting code stolen from Morgul -CFT
 * 'dir=0' moves toward target				    CDW  
 */
int mmove(int dir, int *y, int *x)
{
    register int new_row = 0, new_col = 0;
    int          boolflag;

    switch (dir) {

#ifdef TARGET
      case 0:			/* targetting code stolen from Morgul -CFT */
	new_row = *y;
	new_col = *x;
	mmove2(&new_row, &new_col,
	       char_row, char_col,
	       target_row, target_col);

	break;

#endif /* TARGET */

      case 1:
	new_row = *y + 1;
	new_col = *x - 1;
	break;
      case 2:
	new_row = *y + 1;
	new_col = *x;
	break;
      case 3:
	new_row = *y + 1;
	new_col = *x + 1;
	break;
      case 4:
	new_row = *y;
	new_col = *x - 1;
	break;
      case 5:
	new_row = *y;
	new_col = *x;
	break;
      case 6:
	new_row = *y;
	new_col = *x + 1;
	break;
      case 7:
	new_row = *y - 1;
	new_col = *x - 1;
	break;
      case 8:
	new_row = *y - 1;
	new_col = *x;
	break;
      case 9:
	new_row = *y - 1;
	new_col = *x + 1;
	break;
    }
    boolflag = FALSE;
    if ((new_row >= 0) && (new_row < cur_height)
	&& (new_col >= 0) && (new_col < cur_width)) {
	*y = new_row;
	*x = new_col;
	boolflag = TRUE;
    }
    return (boolflag);
}





/*
 * Similar to get_dir, except that no memory exists, and it is		-CJS-
 * allowed to enter the null direction. 
 */
int get_alldir(cptr prompt, int *dir)
{
    char command;

    for (;;) {
#ifdef MACINTOSH
	if (!get_comdir(prompt, &command))
#else
	if (!get_com(prompt, &command))
#endif
	{
	    free_turn_flag = TRUE;
	    return FALSE;
	}
	if (rogue_like_commands)
	    command = map_roguedir(command);
	if (command >= '1' && command <= '9') {
	    *dir = command - '0';
	    return TRUE;
	}
	bell();
    }
}


/*
 * Prompts for a direction				-RAK-
 * Direction memory added, for repeated commands.  -CJS
 */
int get_dir(cptr prompt, int *dir)
{
    char        command;
    int         save;
    static char prev_dir;  /* Direction memory. -CJS- */

    if (default_dir) {		   /* used in counted commands. -CJS- */
	*dir = prev_dir;
	return TRUE;
    }
#ifdef TARGET
/* This targetting code stolen from Morgul -CFT */
/* Aggle.  Gotta be target mode and either (valid monster and line of sight*/
/* to monster) or (not valid monster and line of sight to position).  CDW */
/* Also, for monster targetting, monster must be lit!  Otherwise player can
   "lock phasers" on an invis monster while a potion of see inv lasts,
   and then continue to hit it when the see inv goes away.  Also,
   targetting mode shouldn't help the player shoot a monster in a
   dark room.  If he can't see it, he shouldn't be able to aim... -CFT */
    if ((target_mode)&&
	(((target_mon<MAX_M_IDX)&& m_list[target_mon].ml &&
	  (los(char_row,char_col,m_list[target_mon].fy,m_list[target_mon].fx))||
	  ((target_mon>=MAX_M_IDX) &&
	   (los(char_row,char_col,target_row,target_col)))))) {
      /* It don't get no better than this */
	*dir=0;
	return TRUE;
    }
    else {
#endif
    if (prompt == NULL)
	prompt = "Which direction?";
    for (;;) {
	save = command_count;	   /* Don't end a counted command. -CJS- */
#ifdef MACINTOSH
	if (!get_comdir(prompt, &command))
#else
	if (!get_com(prompt, &command))
#endif
	{
	    free_turn_flag = TRUE;
	    return FALSE;
	}
	command_count = save;
	if (rogue_like_commands)
	    command = map_roguedir(command);
	if (command >= '1' && command <= '9' && command != '5') {
	    prev_dir = command - '0';
	    *dir = prev_dir;
	    return TRUE;
	}
	bell();
    }
#ifdef TARGET
    }
#endif
}



/* 
 * Search Mode enhancement				-RAK-
 */
void search_on()
{
    change_speed(1);
    
    /* Visual feedback */
    p_ptr->flags.status |= PY_SEARCH;
    prt_state();
    prt_speed();
    p_ptr->flags.food_digested++;
}

void search_off(void)
{
    check_view();

    change_speed(-1);
    
    /* Visual feedback */
    p_ptr->flags.status &= ~PY_SEARCH;
    prt_state();
    prt_speed();
    p_ptr->flags.food_digested--;
}


/*
 * Something happens to disturb the player.		-CJS-
 * The first arg indicates a major disturbance, which affects search.
 *
 */
void disturb(int stop_search, int light_change)
{
    command_count = 0;

    if (stop_search && (p_ptr->flags.status & PY_SEARCH)) search_off();

    if (p_ptr->flags.rest != 0) rest_off();

    if (light_change || find_flag) {
	find_flag = FALSE;
	check_view();
    }

    /* Flush the output */
    flush();
}


/*
 * Searches for hidden things.			-RAK-	
 */
void search(int y, int x, int chance)
{
    register int           i, j;
    register cave_type    *c_ptr;
    register inven_type   *i_ptr;
    bigvtype               tmp_str, tmp_str2;

    if ((p_ptr->flag.blind > 0) || no_lite()) chance = chance / 10;
    if (p_ptr->flag.confused > 0) chance = chance / 10;
    if (p_ptr->flag.image > 0) chance = chance / 10;

    /* Search the nearby grids, which are always in bounds */
    for (i = (y - 1); i <= (y + 1); i++) {
	for (j = (x - 1); j <= (x + 1); j++) {

	    /* Sometimes, notice things */
	    if (randint(100) < chance) {

		c_ptr = &cave[i][j];
		i_ptr = &t_list[c_ptr->tptr];

	    /* Search for hidden objects */
		if (c_ptr->tptr == 0) {
		    /* Nothing */
		}

		/* Trap on floor? */
		else if (i_ptr->tval == TV_INVIS_TRAP) {
		    objdes(tmp_str2, i_ptr, TRUE);
		    (void)sprintf(tmp_str, "You have found %s.", tmp_str2);
		    msg_print(tmp_str);
		    change_trap(i, j);
		    end_find();
		}

		/* Secret door?	*/
		else if (i_ptr->tval == TV_SECRET_DOOR) {
		    msg_print("You have found a secret door.");

			change_trap(i, j);
			end_find();
		}

		/* Chest is trapped? */
		else if (i_ptr->tval == TV_CHEST) {
		    /* mask out the treasure bits */
			if ((i_ptr->flags & CH2_TRAP_MASK) > 1)
			if (!known2_p(i_ptr)) {
			known2(i_ptr);
			msg_print("You have discovered a trap on the chest!");
			}
			else msg_print("The chest is trapped!");
		    }
		}
	    }
	}
    }
}



void rest_off()
{
    p_ptr->flags.rest = 0;

    /* Hack -- update the state */
    p_ptr->flags.status &= ~PY_REST;
    prt_state();

    /* flush last message, or delete "press any key" message */
    msg_print(NULL);

    p_ptr->flags.food_digested++;
}





/*
 * Player wants to pick up an object or gold.
 * Player is on an object.  Many things can happen based -RAK-	 
 * on the TVAL of the object.  Traps are set off, money and most 
 * objects are picked up.  Some objects, such as open doors, just 
 * sit there.
 */
void carry(int y, int x, int pickup)
{
    register int         locn, i;
    bigvtype             out_val, tmp_str;
    register cave_type  *c_ptr;
    register inven_type *i_ptr;

    c_ptr = &cave[y][x];
    i_ptr = &t_list[c_ptr->tptr];
    i = t_list[c_ptr->tptr].tval;

    if (i <= TV_MAX_PICK_UP) {
	end_find();

    /* There's GOLD in them thar hills!      */
    if (i == TV_GOLD) {
	p_ptr->misc.au += i_ptr->cost;
	objdes(tmp_str, i_ptr, TRUE);
	(void)sprintf(out_val,
		      "You have found %ld gold pieces worth of %s.",
		      (long)i_ptr->cost, tmp_str);
	prt_gold();
	delete_object(y, x);
	msg_print(out_val);
    }

    else {
	    if (pickup && inven_check_num(i_ptr)) { /* Too many objects? */
		if (carry_query_flag) {	/* Okay,  pick it up  */
		    objdes(tmp_str, i_ptr, TRUE);
		    (void)sprintf(out_val, "Pick up %s? ", tmp_str);
		    pickup = get_check(out_val);
		}
	    /* Check to see if it will change the players speed. */
		if (pickup && !inven_check_weight(i_ptr)) {
		    objdes(tmp_str, i_ptr, TRUE);
		    (void)sprintf(out_val,
				  "Exceed your weight limit to pick up %s? ",
				  tmp_str);
		    pickup = get_check(out_val);
		}
	    /* Attempt to pick up an object.	       */
		if (pickup) {
		    locn = inven_carry(i_ptr);
		    objdes(tmp_str, &inventory[locn], TRUE);
		    (void)sprintf(out_val, "You have %s. (%c)", tmp_str, locn + 'a');
		    msg_print(out_val);
		    (void)delete_object(y, x);
		}
	    }
		else if (pickup) {   /* only if was trying to pick it up... -CFT */
		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val, "You can't carry %s.", tmp_str);
		msg_print(out_val);
	    }
	}
    }
/* OOPS!				   */
    else if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP || i == TV_STORE_DOOR)
	hit_trap(y, x);
}




/*
 * The cycle lists the directions in anticlockwise order, for over  -CJS- 
 * two complete cycles. The chome array maps a direction on to its position
 * in the cycle. 
 */
static int cycle[] = {1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1};
static int chome[] = {-1, 8, 9, 10, 7, -1, 11, 6, 5, 4};
static int find_openarea, find_breakright, find_breakleft, find_prevdir;
static int command_dir;/* Keep a record of which way we are going. */


/*
 * Hack -- Do we see a wall?  Used in running.		-CJS- 
 */
static int see_wall(int dir, int y, int x)
{
    char c;

   /* check to see if movement there possible */
    if (!mmove(dir, &y, &x)) return TRUE;

#ifdef MSDOS
    if ((c = loc_symbol(y, x)) == wallsym || c == '%')
#else
#ifdef ATARIST_MWC
    if ((c = loc_symbol(y, x)) == (unsigned char)240 || c == '%')
#else
    if ((c = loc_symbol(y, x)) == '#' || c == '%')
#endif
#endif
	return (TRUE);
    
    /* Default */
    return (FALSE);
}

/*
 * Hack -- Do we see anything? Used in running.		-CJS- 
 */
static int see_nothing(int dir, int y, int x)
{
    /* check to see if movement there possible */
    if (!mmove(dir, &y, &x)) return FALSE;

    if (loc_symbol(y, x) == ' ') return (TRUE);
    
    /* Default */
    return (FALSE);
}



/*
 * Determine the next direction for a run, or if we should stop.  -CJS- 
 */
static void area_affect(int dir, int y, int x)
{
    int                  newdir = 0, t, inv, check_dir = 0, row, col;
    register int         i, max, option, option2;

    register cave_type *c_ptr;

    /* We must be able to see... */
    if (p_ptr->flags.blind < 1) {

	option = 0;
	option2 = 0;
	dir = find_prevdir;
	max = (dir & 1) + 1;


	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++) {

	    newdir = cycle[chome[dir] + i];
	    row = y;
	    col = x;
	    if (mmove(newdir, &row, &col)) {

	    /* Objects player can see (Including doors?) cause a stop. */
		c_ptr = &cave[row][col];


		if (player_light || c_ptr->tl || c_ptr->pl || c_ptr->fm) {

		if (c_ptr->tptr != 0) {

		    t = t_list[c_ptr->tptr].tval;

		    if (t != TV_INVIS_TRAP && t != TV_SECRET_DOOR
		    && (t != TV_OPEN_DOOR || !find_ignore_doors)) {

			end_find();
			return;
		}
		}


		/* Also Creatures		 */
		/* the monster should be visible since update_mon() checks
		 * for the special case of being in find mode  */
		if (c_ptr->cptr > 1 && m_list[c_ptr->cptr].ml) {
			end_find();
			return;
		}

		/* The grid is "visible" */
		inv = FALSE;
	    }

	    else inv = TRUE;		/* Square unseen. Treat as open. */
	    if (inv || c_ptr->fval <= MAX_OPEN_SPACE) {

		/* Certain somethings */
		if (find_openarea) {
		    if (i < 0) {
			if (find_breakright) {
			    end_find();
			    return;
			}
		    }
		    else if (i > 0) {
			if (find_breakleft) {
			    end_find();
			    return;
			}
		    }
		}

		/* The first new direction. */
		else if (option == 0) {
		    option = newdir;
		}

		/* Three new directions. Stop running. */
		else if (option2 != 0) {
		    end_find();             
		    return;
		}

		/* If not adjacent to prev, STOP */
		else if (option != cycle[chome[dir] + i - 1]) {
		    end_find();
		    return;
		}

		/* Two adjacent choices. Make option2 the diagonal, */
		/* and remember the other diagonal adjacent to the  */
		/* first option. */
		else {
		    if ((newdir & 1) == 1) {
			check_dir = cycle[chome[dir] + i - 2];
			option2 = newdir;
		    }
		    else {
			check_dir = cycle[chome[dir] + i + 1];
			option2 = option;
			option = newdir;
		    }
		}
	    }

	    /* We see an obstacle.  Break to one side. */
	    /* In open area, STOP if on a side previously open. */
	    else if (find_openarea) {
		if (i < 0) {
		    if (find_breakleft) {
			end_find();
			return;
		    }
		    find_breakright = TRUE;
		}
		else if (i > 0) {
		    if (find_breakright) {
			end_find();
			return;
		    }
		    find_breakleft = TRUE;
		}
	    }
	}
	}


	/* choose a direction. */
	if (find_openarea == FALSE) {

	    /* There is only one option, or if two, then we always examine */
	    /* potential corners and never cut known corners, so you step */
	    /* into the straight option. */
	    if (option2 == 0 || (find_examine && !find_cut)) {
		if (option != 0) command_dir = option;
		if (option2 == 0) find_prevdir = option;
		else find_prevdir = option2;
	    }

	    /* Two options! */
	    else {

		row = y;
		col = x;
		(void)mmove(option, &row, &col);

		/* Don't see that it is closed off.  This could be a */
		/* potential corner or an intersection.  */
		if (!see_wall(option, row, col) ||
		    !see_wall(check_dir, row, col)) {

		    /* Can not see anything ahead and in the direction we */
		    /* are  turning, assume that it is a potential corner. */
		    if (find_examine && see_nothing(option, row, col) &&
			see_nothing(option2, row, col)) {
			command_dir = option;
			find_prevdir = option2;
		    }

		    /* STOP: we are next to an intersection or a room */
		    else {
			end_find();
		    }
		}

		/* This corner is seen to be enclosed; we cut the corner. */
		else if (find_cut) {
		    command_dir = option2;
		    find_prevdir = option2;
		}

		/* This corner is seen to be enclosed, and we */
		/* deliberately go the long way. */
		else {
		    command_dir = option;
		    find_prevdir = option2;
		}
	    }
	}
    }
}




/*
 * Moves player from one space to another. -RAK-
 *
 * Note: This routine has been pre-declared; see that for argument
 */
void move_player(int dir, int do_pickup)
{
    int                 old_row, old_col, old_find_flag;
    int                 y, x;
    register int        i, j;
    register cave_type *c_ptr, *d_ptr;

    if (((p_ptr->flags.confused > 0) || (p_ptr->flags.stun > 0)) &&	/* Confused/Stunned?  */
	(randint(4) > 1) &&	   /* 75% random movement */
	(dir != 5)) {		   /* Never random if sitting */
	dir = randint(9);
	end_find();
    }

    /* Find the result of moving */
    y = char_row;
    x = char_col;
    if (mmove(dir, &y, &x)) {	   /* Legal move?	      */

    /* Examine the destination */    
    c_ptr = &cave[y][x];

    /* if there is no creature, or an unlit creature in the walls then... */
    /* disallow attacks against unlit creatures in walls because moving into
     * a wall is a free turn normally, hence don't give player free turns
     * attacking each wall in an attempt to locate the invisible creature,
     * instead force player to tunnel into walls which always takes a turn 
     */
	if ((c_ptr->cptr < 2)
	  || (!m_list[c_ptr->cptr].ml && c_ptr->fval >= MIN_CLOSED_SPACE)) {
	    if (c_ptr->fval <= MAX_OPEN_SPACE) {	/* Open floor spot	 */
	    /* Make final assignments of char co-ords */
		old_row = char_row;
		old_col = char_col;
		char_row = y;
		char_col = x;
	    /* Move character record (-1)	       */
		move_rec(old_row, old_col, char_row, char_col);
	    /* Check for new panel		       */
		if (get_panel(char_row, char_col, FALSE))
		    prt_map();
	    /* Check to see if he should stop	       */
		if (find_flag)
		    area_affect(dir, char_row, char_col);
	    /* Check to see if he notices something  */
	    /* fos may be negative if have good rings of searching */
		if ((p_ptr->misc.fos <= 1) || (randint(p_ptr->misc.fos) == 1) ||
		    (p_ptr->flags.status & PY_SEARCH))
		    search(char_row, char_col, p_ptr->misc.srh);
	    /* A room of light should be lit.	     */
		if ((c_ptr->fval == LIGHT_FLOOR) ||
		    (c_ptr->fval == NT_LIGHT_FLOOR)) {
		    if (!c_ptr->pl && !p_ptr->flags.blind)
			lite_room(char_row, char_col);
		}
	    /* In doorway of light-room?	       */
		else if (c_ptr->lr && (p_ptr->flags.blind < 1)) {
		    byte lit = FALSE;	/* only call lite_room once... -CFT */

		    for (i = (char_row - 1); !lit && i <= (char_row + 1); i++)
			for (j = (char_col - 1); !lit && j <= (char_col + 1); j++) {
			    d_ptr = &cave[i][j];
			    if (((d_ptr->fval == LIGHT_FLOOR) ||
				 (d_ptr->fval == NT_LIGHT_FLOOR)) &&
				(!d_ptr->pl)) {
			    /* move light 1st, or corridor may be perm lit */
				move_light(old_row, old_col, char_row, char_col);
				lite_room(char_row, char_col);
				lit = TRUE;	/* okay, we can stop now... -CFT */
			    }
			}
		}
	    /* Move the light source		       */
		move_light(old_row, old_col, char_row, char_col);
	    /* An object is beneath him.	     */
		if (c_ptr->tptr != 0) {
		    i = t_list[c_ptr->tptr].tval;
		    if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP
			|| i == TV_STORE_DOOR || !prompt_carry_flag
			|| i == TV_GOLD)
			carry(char_row, char_col, do_pickup);
		    else if (prompt_carry_flag && i != TV_OPEN_DOOR
			     && i != TV_UP_STAIR && i != TV_DOWN_STAIR) {
			inven_type         *i_ptr;
			bigvtype            tmp_str, tmp2_str;

			i_ptr = &t_list[cave[char_row][char_col].tptr];
			objdes(tmp_str, i_ptr, TRUE);
			sprintf(tmp2_str, "You see %s.", tmp_str);
			msg_print(tmp2_str);
		    }
		/* if stepped on falling rock trap, and space contains
		 * rubble, then step back into a clear area 
		 */
		    if (t_list[c_ptr->tptr].tval == TV_RUBBLE) {
			move_rec(char_row, char_col, old_row, old_col);
			move_light(char_row, char_col, old_row, old_col);
			char_row = old_row;
			char_col = old_col;
	/* check to see if we have stepped back onto another trap, if so, set it off */
			c_ptr = &cave[char_row][char_col];
			if (c_ptr->tptr != 0) {
			    i = t_list[c_ptr->tptr].tval;
			    if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP
				|| i == TV_STORE_DOOR)
				hit_trap(char_row, char_col);
			}
		    }
		}
	    } else {		   /* Can't move onto floor space */
		if (!find_flag && (c_ptr->tptr != 0)) {
		    if (t_list[c_ptr->tptr].tval == TV_RUBBLE)
			msg_print("There is rubble blocking your way.");
		    else if (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR)
			msg_print("There is a closed door blocking your way.");
		} else
		    end_find();
		free_turn_flag = TRUE;
	    }
	} else {		   /* Attacking a creature! */
	    old_find_flag = find_flag;
	    end_find();
	/* if player can see monster, and was in find mode, then nothing */
	    if (m_list[c_ptr->cptr].ml && old_find_flag) {
	    /* did not do anything this turn */
		free_turn_flag = TRUE;
	    } else {
		if (p_ptr->flags.afraid < 1)	/* Coward?	 */
		    py_attack(y, x);
		else		   /* Coward!	 */
		    msg_print("You are too afraid!");
	    }
	}
    }
}






/* The running algorithm:			-CJS- */


/*
   Overview: You keep moving until something interesting happens.
   If you are in an enclosed space, you follow corners. This is
   the usual corridor scheme. If you are in an open space, you go
   straight, but stop before entering enclosed space. This is
   analogous to reaching doorways. If you have enclosed space on
   one side only (that is, running along side a wall) stop if
   your wall opens out, or your open space closes in. Either case
   corresponds to a doorway.

   What happens depends on what you can really SEE. (i.e. if you
   have no light, then running along a dark corridor is JUST like
   running in a dark room.) The algorithm works equally well in
   corridors, rooms, mine tailings, earthquake rubble, etc, etc.

   These conditions are kept in static memory:
	find_openarea	 You are in the open on at least one
			 side.
	find_breakleft	 You have a wall on the left, and will
			 stop if it opens
	find_breakright	 You have a wall on the right, and will
			 stop if it opens

   To initialize these conditions is the task of find_init. If
   moving from the square marked @ to the square marked . (in the
   two diagrams below), then two adjacent sqares on the left and
   the right (L and R) are considered. If either one is seen to
   be closed, then that side is considered to be closed. If both
   sides are closed, then it is an enclosed (corridor) run.

	 LL		L
	@.	       L.R
	 RR	       @R

   Looking at more than just the immediate squares is
   significant. Consider the following case. A run along the
   corridor will stop just before entering the center point,
   because a choice is clearly established. Running in any of
   three available directions will be defined as a corridor run.
   Note that a minor hack is inserted to make the angled corridor
   entry (with one side blocked near and the other side blocked
   further away from the runner) work correctly. The runner moves
   diagonally, but then saves the previous direction as being
   straight into the gap. Otherwise, the tail end of the other
   entry would be perceived as an alternative on the next move.

	   #.#
	  ##.##
	  .@...
	  ##.##
	   #.#

   Likewise, a run along a wall, and then into a doorway (two
   runs) will work correctly. A single run rightwards from @ will
   stop at 1. Another run right and down will enter the corridor
   and make the corner, stopping at the 2.

	#@	  1
	########### ######
	2	    #
	#############
	#

   After any move, the function area_affect is called to
   determine the new surroundings, and the direction of
   subsequent moves. It takes a location (at which the runner has
   just arrived) and the previous direction (from which the
   runner is considered to have come). Moving one square in some
   direction places you adjacent to three or five new squares
   (for straight and diagonal moves) to which you were not
   previously adjacent.

       ...!	  ...	       EG Moving from 1 to 2.
       .12!	  .1.!		  . means previously adjacent
       ...!	  ..2!		  ! means newly adjacent
		   !!!

   You STOP if you can't even make the move in the chosen
   direction. You STOP if any of the new squares are interesting
   in any way: usually containing monsters or treasure. You STOP
   if any of the newly adjacent squares seem to be open, and you
   are also looking for a break on that side. (i.e. find_openarea
   AND find_break) You STOP if any of the newly adjacent squares
   do NOT seem to be open and you are in an open area, and that
   side was previously entirely open.

   Corners: If you are not in the open (i.e. you are in a
   corridor) and there is only one way to go in the new squares,
   then turn in that direction. If there are more than two new
   ways to go, STOP. If there are two ways to go, and those ways
   are separated by a square which does not seem to be open, then
   STOP.

   Otherwise, we have a potential corner. There are two new open
   squares, which are also adjacent. One of the new squares is
   diagonally located, the other is straight on (as in the
   diagram). We consider two more squares further out (marked
   below as ?).
	  .X
	 @.?
	  #?
   If they are both seen to be closed, then it is seen that no
   benefit is gained from moving straight. It is a known corner.
   To cut the corner, go diagonally, otherwise go straight, but
   pretend you stepped diagonally into that next location for a
   full view next time. Conversely, if one of the ? squares is
   not seen to be closed, then there is a potential choice. We check
   to see whether it is a potential corner or an intersection/room entrance.
   If the square two spaces straight ahead, and the space marked with 'X'
   are both blank, then it is a potential corner and enter if find_examine
   is set, otherwise must stop because it is not a corner.
*/



/*
 * Note that move_player() may modify command_dir via "area_affect"
 * Note that the "running" routines now use "command_dir" instead
 * of "find_direction"
 */
void find_run(void)
{
    /* prevent infinite loops in find mode, will stop after moving 100 times */
    if (find_flag++ > 100) {
	msg_print("You stop running to catch your breath.");
	end_find();
    }

     else move_player(command_dir, TRUE);
}


/*
 * Initialize the running algorithm, do NOT take any steps.
 *
 */
void find_init(int dir)
{
    int          row, col, deepleft, deepright;
    register int i, shortleft, shortright;

    darken_player(char_row, char_col);
    old_lite = cur_lite;
    if (cur_lite >= 0) cur_lite = 1;

    /* Find the destination grid */
    row = char_row;
    col = char_col;
    if (!mmove(dir, &row, &col)) find_flag = FALSE;

    else {
	command_dir = dir;
	find_flag = 1;
	find_breakright = find_breakleft = FALSE;
	find_prevdir = dir;
	if (p_ptr->flags.blind < 1) {
	    i = chome[dir];
	    deepleft = deepright = FALSE;
	    shortright = shortleft = FALSE;
	    if (see_wall(cycle[i + 1], char_row, char_col)) {
		find_breakleft = TRUE;
		shortleft = TRUE;
	    }
	    else if (see_wall(cycle[i + 1], row, col)) {
		find_breakleft = TRUE;
		deepleft = TRUE;
	    }
	    if (see_wall(cycle[i - 1], char_row, char_col)) {
		find_breakright = TRUE;
		shortright = TRUE;
	    }
	    else if (see_wall(cycle[i - 1], row, col)) {
		find_breakright = TRUE;
		deepright = TRUE;
	    }
	    if (find_breakleft && find_breakright) {
		find_openarea = FALSE;
		/* a hack to allow angled corridor entry */
		if (dir & 1) {
		    if (deepleft && !deepright)
			find_prevdir = cycle[i - 1];
		    else if (deepright && !deepleft)
			find_prevdir = cycle[i + 1];
		}
	    /* else if there is a wall two spaces ahead and seem to be in a
	     * corridor, then force a turn into the side corridor, must be
	     * moving straight into a corridor here 
	     */
		else if (see_wall(cycle[i], row, col)) {
		    if (shortleft && !shortright)
			find_prevdir = cycle[i - 2];
		    else if (shortright && !shortleft)
			find_prevdir = cycle[i + 2];
		}
	    }
	    else {
		find_openarea = TRUE;
	    }
	}
    }

/*
 * We must erase the player symbol '@' here, because sub3_move_light() does
 * not erase the previous location of the player when in find mode and when
 * find_prself is FALSE.  The player symbol is not draw at all in this case
 * while moving, so the only problem is on the first turn of find mode, when
 * the initial position of the character must be erased. Hence we must do the
 * erasure here.  
 */
    if (!light_flag && !find_prself)
#ifdef TC_COLOR
	lite_spot(char_row, char_col);
#else
	print(loc_symbol(char_row, char_col), char_row, char_col);
#endif

    move_player(dir, TRUE);
    if (find_flag == FALSE)
	command_count = 0;
}


/*
 * Switch off the run flag - and get the light correct. -CJS-
 */
void end_find()
{
    /* Were we running? */
    if (find_flag) {

	/* Cancel the running */
	find_flag = 0;

	cur_lite = old_lite;
	move_light(char_row, char_col, char_row, char_col);
    }
}




#ifdef MACINTOSH
/* Same as get_com(), but translates direction keys from keypad */
int get_comdir(char *prompt, char *command)
{
    int res;

    if (prompt)
	prt(prompt, 0, 0);
    *command = inkeydir();
    if (*command == 0 || *command == ESCAPE)
	res = FALSE;
    else
	res = TRUE;
    erase_line(MSG_LINE, 0);
    return (res);
}

#endif
