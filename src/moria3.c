/* File: moria3.c */ 

/* Purpose: high level command processing */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * An enhanced look, with peripheral vision. Looking all 8	-CJS-
 * directions will see everything which ought to be visible.
 *
 * Can specify direction 5, which looks in all directions. 
 *
 * For the purpose of hindering vision, each place is regarded as a diamond just
 * touching its four immediate neighbours. A diamond is opaque if it is a
 * wall, or shut door, or something like that. A place is visible if any part
 * of its diamond is visible: i.e. there is a line from the view point to
 * part of the diamond which does not pass through any opaque diamonds. 
 *
 * Consider the following situation: 
 *
 * @....			    X	X   X	X   X .##..			   /
 * \ / \ / \ / \ / \ .....			  X @ X . X . X 1 X . X \ / \
 * / \ / \ / \ / X	X   X	X   X Expanded view, with	   / \ / \ /
 * \ / \ / \ diamonds inscribed	  X . X # X # X 2 X . X about each point,	  
 * \ / \ / \ / \ / \ / and some locations	    X	X   X	X   X
 * numbered.		   / \ / \ / \ / \ / \ X . X . X . X 3 X 4 X \ / \ /
 * \ / \ / \ / X	X   X	X   X - Location 1 is fully visible. -
 * Location 2 is visible, even though partially obscured. - Location 3 is
 * invisible, but if either # were transparent, it would be visible. -
 * Location 4 is completely obscured by a single #. 
 *
 * The function which does the work is look_ray. It sets up its own co-ordinate
 * frame (global variables map back to the dungeon frame) and looks for
 * everything between two angles specified from a central line. It is
 * recursive, and each call looks at stuff visible along a line parallel to
 * the center line, and a set distance away from it. A diagonal look uses
 * more extreme peripheral vision from the closest horizontal and vertical
 * directions; horizontal or vertical looks take a call for each side of the
 * central line. 
 */


/*
 * Globally accessed variables: gl_nseen counts the number of places where
 * something is seen. gl_rock indicates a look for rock or objects. 
 *
 * The others map co-ords in the ray frame to dungeon co-ords. 
 *
 * dungeon y = char_row	 + gl_fyx * (ray x)  + gl_fyy * (ray y)
 * dungeon x = char_col	 + gl_fxx * (ray x)  + gl_fxy * (ray y) 
 */
static int gl_fxx, gl_fxy, gl_fyx, gl_fyy;
static int gl_nseen, gl_noquery;
static int gl_rock;

/*
 * Intended to be indexed by dir/2, since is only relevant to horizontal or
 * vertical directions. 
 */
static int set_fxy[] = {0, 1, 0, 0, -1};
static int set_fxx[] = {0, 0, -1, 1, 0};
static int set_fyy[] = {0, 0, 1, -1, 0};
static int set_fyx[] = {0, 1, 0, 0, -1};

/*
 * Map diagonal-dir/2 to a normal-dir/2. 
 */
static int map_diag1[] = {1, 3, 0, 2, 4};
static int map_diag2[] = {2, 1, 0, 4, 3};

/*
 * Any sufficiently big number will do
 */
#define GRADF	10000



/*
 * Look at a monster
 */
static cptr look_mon_desc(int mnum)
{
    monster_type *m = &m_list[mnum];

    byte         living;
    s32b         thp, tmax, perc;

    living = !((c_list[m->mptr].cdefense & (UNDEAD|DEMON)) ||
	       ((c_list[m->mptr].cchar == 'E') ||
		(c_list[m->mptr].cchar == 'g') ||
		(c_list[m->mptr].cchar == 'v') ||
		(c_list[m->mptr].cchar == 'X')));
    
    if (m->maxhp == 0) {	   /* then we're just going to fix it! -CFT */
	if ((c_list[m->mptr].cdefense & MAX_HP) || be_nasty)
	    m->maxhp = max_hp(c_list[m->mptr].hd);
	else
	    m->maxhp = pdamroll(c_list[m->mptr].hd);
    }
    if (m->hp > m->maxhp)
	m->hp = m->maxhp;

    if ((m->maxhp == 0) || (m->hp >= m->maxhp))	/* shouldn't ever need > -CFT */
	return (living ? "unhurt" : "undamaged");
    thp = (s32b) m->hp;
    tmax = (s32b) m->maxhp;

    perc = (s32b) (thp * 100L) / tmax;

    if (perc > 60) {
	return (living ? "somewhat wounded" : "somewhat damaged");
    }

    if (perc > 25) {
	return (living ? "wounded" : "damaged");
    }

    if (perc > 10) {
	return (living ? "badly wounded" : "badly damaged");
    }

    return (living ? "almost dead" : "almost destroyed");
}




static int look_see(int x, int y, int *transparent)
{
    const char         *dstring,*string;
    char               query = 'a';
    register cave_type *c_ptr;
    register int        j;
    bigvtype            out_val, tmp_str;

    /* Paranoia */    
    if (x < 0 || y < 0 || y > x) {
	(void)sprintf(tmp_str, "Illegal call to look_see(%d, %d)", x, y);
	msg_print(tmp_str);
    }

    if (x == 0 && y == 0) dstring = "You are on";
    else dstring = "You see";

    /* Something */
    j = char_col + gl_fxx * x + gl_fxy * y;
    y = char_row + gl_fyx * x + gl_fyy * y;
    x = j;

    /* Off screen, stop looking, nothing to see */
    if (!panel_contains(y, x)) {
	*transparent = FALSE;
	return FALSE;
    }

    /* Get the cave */
    c_ptr = &cave[y][x];

    *transparent = c_ptr->fval <= MAX_OPEN_SPACE;

    /* Hack -- Don't look at a direct line of sight. */
    if (gl_noquery) return FALSE;

    /* Start the description */        
    out_val[0] = 0;

    /* Examine visible monsters */
    if (gl_rock == 0 && c_ptr->cptr > 1 && m_list[c_ptr->cptr].ml) {

	j = m_list[c_ptr->cptr].mptr;

	if (c_list[j].cdefense & UNIQUE) {
	    (void)sprintf(out_val, "%s %s (%s).  [(r)ecall]",
			  dstring,
			  c_list[j].name,
			  look_mon_desc((int)c_ptr->cptr));
	}
	else {
	    (void)sprintf(out_val, "%s %s %s (%s).  [(r)ecall]",
			  dstring,
			  (is_a_vowel(c_list[j].name[0]) ? "an" : "a"),
			  c_list[j].name,
			  look_mon_desc((int)c_ptr->cptr));
	}
	dstring = "It is on";

	prt(out_val, 0, 0);
	move_cursor_relative(y, x);
	query = inkey();

	if (query == 'r' || query == 'R') {
	save_screen();
	query = roff_recall(j);
	restore_screen();
	}
    }

    if (c_ptr->tl || c_ptr->pl || c_ptr->fm) {

	/* Is there an object there? */
	if (c_ptr->tptr) {

	    if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR) goto granite;

	    if (!gl_rock &&
		(t_list[c_ptr->tptr].tval != TV_INVIS_TRAP)) {
		
		objdes(tmp_str, &t_list[c_ptr->tptr], TRUE);
		(void)sprintf(out_val, "%s %s.  ---pause---", dstring, tmp_str);
		dstring = "It is in";
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();
	    }
	}
	if ((gl_rock || out_val[0]) && c_ptr->fval >= MIN_CLOSED_SPACE) {

	    switch (c_ptr->fval) {
	      case BOUNDARY_WALL:
	      case GRANITE_WALL:
	granite:
	    /* Granite is only interesting if it contains something. */
		if (out_val[0])
		    string = "a granite wall";
		else
		    string = NULL; /* In case we jump here */
		break;
	      case MAGMA_WALL:
		string = "some dark rock";
		break;
	      case QUARTZ_WALL:
		string = "a quartz vein";
		break;
	      default:
		string = NULL;
		break;
	    }

	    if (string) {
		(void)sprintf(out_val, "%s %s.  ---pause---", dstring, string);
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();
	    }
	}
    }

    if (out_val[0]) {
	gl_nseen++;
	if (query == ESCAPE) return TRUE;
    }

    return FALSE;
}


/*
 * Look at everything within a cone of vision between two ray
 * lines emanating from the player, and y or more places away
 * from the direct line of view. This is recursive.
 *
 * Rays are specified by gradients, y over x, multiplied by
 * 2*GRADF. This is ONLY called with gradients between 2*GRADF
 * (45 degrees) and 1 (almost horizontal).
 *
 * (y axis)/ angle from
 *   ^	  /	    ___ angle to
 *   |	 /	 ___
 *...|../.....___.................... parameter y (look at things in the
 *   | /   ___			      cone, and on or above this line)
 *   |/ ___
 *   @-------------------->   direction in which you are looking. (x axis)
 *   |
 *   | 
 */
static int look_ray(int y, int from, int to)
{
    register int        max_x, x;
    int                 transparent;

/*
 * from is the larger angle of the ray (larger than "to") since
 * we scan towards the center line.  If "from" is smaller than "to",
 * the ray does not exist
 */

    if (from <= to || y > MAX_SIGHT) return FALSE;

/*
 * Find first visible location along this line. Minimum x such that (2x-1)/x
 * < from/GRADF <=> x > GRADF(2x-1)/from. This may be called with y=0 whence
 * x will be set to 0. Thus we need a special fix. 
 */

    x = (long)GRADF * (2 * y - 1) / from + 1;

    if (x <= 0) x = 1;

/*
 * Find last visible location along this line. Maximum x such that (2x+1)/x >
 * to/GRADF <=> x < GRADF(2x+1)/to 
 */

    max_x = ((long)GRADF * (2 * y + 1) - 1) / to;
    if (max_x > MAX_SIGHT) max_x = MAX_SIGHT;
    if (max_x < x) return FALSE;

/*
 * Hack -- gl_noquery prevents doubling up on direct lines of sight. If
 * 'to' is greater than 1, we do not really look at stuff along the
 * direct line of sight, but we do have to see what is opaque for the
 * purposes of obscuring other objects. 
 */

    if ((y == 0 && to > 1) || (y == x && from < GRADF * 2)) {
	gl_noquery = TRUE;
    }
    else {
	gl_noquery = FALSE;
    }

    if (look_see(x, y, &transparent)) {
	return TRUE;
    }

    if (y == x) {
	gl_noquery = FALSE;
    }

    /* Hack */    
    if (transparent) goto init_transparent;

    /* Go until done */
    for (;;) {

	/* Look down the window we've found, allow abort */
	if (look_ray(y + 1, from, (int)((2 * y + 1) * (long)GRADF / x))) {
	    return TRUE;
	}

	/* Find the start of next window. */
	do {

	    /* All done (?) */
	    if (x == max_x) return FALSE;

	    /* See if this seals off the scan. (If y is zero, then it will.) */
	    from = (2 * y - 1) * (long)GRADF / x;
	    if (from <= to) return FALSE;

	    x++;

	    if (look_see(x, y, &transparent)) {
		return TRUE;
	    }
	}
	while (!transparent);

init_transparent:

	/* Find the end of this window of visibility. */
	do {
	    if (x == max_x) {
	    /* The window is trimmed by an earlier limit. */
		return look_ray(y + 1, from, to);
	    }

	    x++;

	    if (look_see(x, y, &transparent)) {
		return TRUE;
	    }
	}
	while (transparent);
    }
}



/*
 * Look at what we can see. This is a free move. 
 *
 * Prompts for a direction, and then looks at every object in turn within a cone
 * of vision in that direction. For each object, the cursor is moved over the
 * object, a description is given, and we wait for the user to type
 * something. Typing ESCAPE will abort the entire look. 
 *
 * Looks first at real objects and monsters, and looks at rock types only after
 * all other things have been seen.  Only looks at rock types if the
 * notice_seams option is set. 
 */
void look()
{
    register int        i, abort_look;
    int                 dir, dummy;

    /* Blind */
    if (py.flags.blind > 0) {
	msg_print("You can't see a damn thing!");
    }

    /* Hallucinating */
    else if (py.flags.image > 0) {
	msg_print("You can't believe what you are seeing! It's like a dream!");
    }

    else if (get_alldir("Look which direction? ", &dir)) {

	abort_look = FALSE;
	gl_nseen = 0;
	gl_rock = 0;
	gl_noquery = FALSE;	   /* Have to set this up for the look_see */

	if (look_see(0, 0, &dummy)) {
	    abort_look = TRUE;
	}
	else {
	    do {
		abort_look = FALSE;

		/* Look at everything */
		if (dir == 5) {

		    for (i = 1; i <= 4; i++) {
			gl_fxx = set_fxx[i];
			gl_fyx = set_fyx[i];
			gl_fxy = set_fxy[i];
			gl_fyy = set_fyy[i];
			if (look_ray(0, 2 * GRADF - 1, 1)) {
			    abort_look = TRUE;
			    break;
			}
			gl_fxy = (-gl_fxy);
			gl_fyy = (-gl_fyy);
			if (look_ray(0, 2 * GRADF, 2)) {
			    abort_look = TRUE;
			    break;
			}
		    }
		}

		/* Straight directions */
		else if ((dir & 1) == 0) {
		    i = dir >> 1;
		    gl_fxx = set_fxx[i];
		    gl_fyx = set_fyx[i];
		    gl_fxy = set_fxy[i];
		    gl_fyy = set_fyy[i];
		    if (look_ray(0, GRADF, 1)) {
			abort_look = TRUE;
		    }
		    else {
			gl_fxy = (-gl_fxy);
			gl_fyy = (-gl_fyy);
			abort_look = look_ray(0, GRADF, 2);
		    }
		}

		/* Diagonals */
		else {
		    i = map_diag1[dir >> 1];
		    gl_fxx = set_fxx[i];
		    gl_fyx = set_fyx[i];
		    gl_fxy = (-set_fxy[i]);
		    gl_fyy = (-set_fyy[i]);
		    if (look_ray(1, 2 * GRADF, GRADF)) {
			abort_look = TRUE;
		    }
		    else {
			i = map_diag2[dir >> 1];
			gl_fxx = set_fxx[i];
			gl_fyx = set_fyx[i];
			gl_fxy = set_fxy[i];
			gl_fyy = set_fyy[i];
			abort_look = look_ray(1, 2 * GRADF - 1, GRADF);
		    }
		}
	    }
	    while (abort_look == FALSE && notice_seams && (++gl_rock < 2));

	    if (abort_look) {
		msg_print("Aborting look.");
	    }
	    else {
		if (gl_nseen) {
		    if (dir == 5) {
			msg_print("That's all you see.");
		    }
		    else {
			msg_print("That's all you see in that direction.");
		    }
		}
		else if (dir == 5) {
		    msg_print("You see nothing of interest.");
		}
		else {
		    msg_print("You see nothing of interest in that direction.");
		}
	    }
	}
    }
}





/*
 * Chests have traps too.				-RAK-	 
 * Note: Chest traps are based on the FLAGS value		
 */
static void chest_trap(int y, int x)
{
    register int        i, j, k;

    register inven_type *t_ptr;

    t_ptr = &t_list[cave[y][x].tptr];
    if (CH_LOSE_STR & t_ptr->flags) {
	msg_print("A small needle has pricked you!");
	if (!py.flags.sustain_str) {
	    (void)dec_stat(A_STR);
	    take_hit(damroll(1, 4), "a poison needle");
	    msg_print("You feel weakened!");
	}
	else {
	    msg_print("You are unaffected.");
	}
    }

    if (CH_POISON & t_ptr->flags) {
	msg_print("A small needle has pricked you!");
	take_hit(damroll(1, 6), "a poison needle");
	if (!(py.flags.poison_resist ||
	      py.flags.resist_poison ||
	      py.flags.poison_im)) {
	    py.flags.poisoned += 10 + randint(20);
	}
    }

    if (CH_PARALYSED & t_ptr->flags) {
	msg_print("A puff of yellow gas surrounds you!");
	if (py.flags.free_act) {
	    msg_print("You are unaffected.");
	}
	else {
	    msg_print("You choke and pass out.");
	    py.flags.paralysis = 10 + randint(20);
	}
    }

    if (CH_SUMMON & t_ptr->flags) {
	for (i = 0; i < 3; i++) {
	    j = y;
	    k = x;
	    (void)summon_monster(&j, &k, FALSE);
	}
    }

    if (CH_EXPLODE & t_ptr->flags) {
	msg_print("There is a sudden explosion!");
	(void)delete_object(y, x);
	take_hit(damroll(5, 8), "an exploding chest");
    }
}





/*
 * Opens a closed door or closed chest.		-RAK-
 */
void openobject()
{
    int				y, x, i, dir;
    int				flag, no_object;
    register cave_type		*c_ptr;
    register inven_type		*t_ptr;
    register struct misc	*p_ptr;
    register monster_type	*m_ptr;
    vtype			m_name, out_val;
#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir, so we save
			       target_mode, then turn it off -CFT */
#endif

    y = char_row;
    x = char_col;
#ifdef TARGET
    target_mode = FALSE;
#endif
    if (get_dir(NULL, &dir)) {
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];
	no_object = FALSE;
	if (c_ptr->cptr > 1 && c_ptr->tptr != 0 &&
	    (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR
	     || t_list[c_ptr->tptr].tval == TV_CHEST)) {
	    m_ptr = &m_list[c_ptr->cptr];
	    if (m_ptr->ml) {
		if (c_list[m_ptr->mptr].cdefense & UNIQUE)
		    (void)sprintf(m_name, "%s", c_list[m_ptr->mptr].name);
		else
		    (void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
	    } else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);
	} else if (c_ptr->tptr != 0)
	/* Closed door		 */
	    if (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR) {
		t_ptr = &t_list[c_ptr->tptr];
		if (t_ptr->p1 > 0) {
		    p_ptr = &py.misc;
		    i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
			+ (class_level_adj[p_ptr->pclass][CLA_DISARM]
			   * p_ptr->lev / 3);
	/* give a 1/50 chance of opening anything, anyway -CWS */
		    if ((i - t_ptr->p1) < 2)
			i = t_ptr->p1 + 2;
		    if (py.flags.confused > 0)
			msg_print("You are too confused to pick the lock.");
		    else if ((i - t_ptr->p1) > randint(100)) {
			msg_print("You have picked the lock.");
			py.misc.exp++;
			prt_experience();
			t_ptr->p1 = 0;
		    } else
			count_msg_print("You failed to pick the lock.");
		} else if (t_ptr->p1 < 0)	/* It's stuck	  */
		    msg_print("It appears to be stuck.");
		if (t_ptr->p1 == 0) {
		    invcopy(&t_list[c_ptr->tptr], OBJ_OPEN_DOOR);
		    c_ptr->fval = CORR_FLOOR;
		    lite_spot(y, x);
		    check_view();
		    command_count = 0;
		}
	    }
    /* Open a closed chest.		     */
	    else if (t_list[c_ptr->tptr].tval == TV_CHEST) {
		p_ptr = &py.misc;
		i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
		    + (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
		t_ptr = &t_list[c_ptr->tptr];
		flag = FALSE;
		if (CH_LOCKED & t_ptr->flags)
		    if (py.flags.confused > 0)
			msg_print("You are too confused to pick the lock.");
		    else if ((i - (int)t_ptr->level) > randint(100)) {
			msg_print("You have picked the lock.");
			flag = TRUE;
			py.misc.exp += t_ptr->level;
			prt_experience();
		    } else
			count_msg_print("You failed to pick the lock.");
		else
		    flag = TRUE;
		if (flag) {
		    t_ptr->flags &= ~CH_LOCKED;
		    t_ptr->name2 = SN_EMPTY;
		    known2(t_ptr);
		    t_ptr->cost = 0;
		}
		flag = FALSE;
	    /* Was chest still trapped?	 (Snicker)   */
		if ((CH_LOCKED & t_ptr->flags) == 0) {
		    chest_trap(y, x);
		    if (c_ptr->tptr != 0)
			flag = TRUE;
		}
	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed.				   */
		if (flag) {
		/*
		 * clear the cursed chest/monster win flag, so that people
		 * can not win by opening a cursed chest 
		 */
		    t_ptr->flags &= ~TR_CURSED;

		/* generate based on level chest was found on - dbd */
		    object_level = t_ptr->p1;

	        /* but let's not get too crazy with storebought chests -CWS */
		    if (t_ptr->ident & ID_STOREBOUGHT) {
			if (object_level > 20)
			    object_level = 20;
		    }

		    if (object_level < 0) /* perform some sanity checking -CWS */
			object_level = 0;
		    if (object_level > MAX_OBJ_LEVEL)
			object_level = MAX_OBJ_LEVEL;

		    coin_type = 0;
		    opening_chest = TRUE; /* don't generate another chest -CWS */
		    (void)monster_death(y, x, t_list[c_ptr->tptr].flags, 0, 0);
		    t_list[c_ptr->tptr].flags = 0;
		    opening_chest = FALSE;
		}
	    } else
		no_object = TRUE;
	else
	    no_object = TRUE;

	if (no_object) {
	    msg_print("I do not see anything you can open there.");
	    free_turn_flag = TRUE;
	}
    }
#ifdef TARGET
    target_mode = temp;
#endif
}


/*
 * Close an open door.
 */
void closeobject()
{
    int                    y, x, dir, no_object;
    vtype                  out_val, m_name;
    register cave_type    *c_ptr;
    register monster_type *m_ptr;
#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir, so we save
			       target_mode, then turn it off -CFT */
#endif

    y = char_row;
    x = char_col;
#ifdef TARGET
    target_mode = FALSE;
#endif
    if (get_dir(NULL, &dir)) {
	(void)mmove(dir, &y, &x);

	c_ptr = &cave[y][x];
	no_object = FALSE;
	if (c_ptr->tptr != 0)
	    if (t_list[c_ptr->tptr].tval == TV_OPEN_DOOR)
		if (c_ptr->cptr == 0)
		    if (t_list[c_ptr->tptr].p1 == 0) {
			invcopy(&t_list[c_ptr->tptr], OBJ_CLOSED_DOOR);
			c_ptr->fval = BLOCKED_FLOOR;
			lite_spot(y, x);
		    } else
			msg_print("The door appears to be broken.");
		else {
		    m_ptr = &m_list[c_ptr->cptr];
		    if (m_ptr->ml) {
			if (c_list[m_ptr->mptr].cdefense & UNIQUE)
			    (void)sprintf(m_name, "%s", c_list[m_ptr->mptr].name);
			else
			    (void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
		    } else
			(void)strcpy(m_name, "Something");
		    (void)sprintf(out_val, "%s is in your way!", m_name);
		    msg_print(out_val);
		}
	    else
		no_object = TRUE;
	else
	    no_object = TRUE;

	if (no_object) {
	    msg_print("I do not see anything you can close there.");
	    free_turn_flag = TRUE;
	}
    }
#ifdef TARGET
    target_mode = temp;
#endif
}




/*
 * Tunneling through real wall: 10, 11, 12		-RAK-
 * Used by TUNNEL and WALL_TO_MUD				
 */
int twall(int y, int x, int t1, int t2)
{
    int		i, j;
    int		res, found;
    cave_type	*c_ptr;

    res = FALSE;

    /* Allow chaining of "probability" calls */
    if (t1 > t2) {

	c_ptr = &cave[y][x];
	if (c_ptr->tptr) { /* secret door or rubble or gold -CFT */
	    if (t_list[c_ptr->tptr].tval == TV_RUBBLE) {
		delete_object(y,x); /* blow it away... */
		if (randint(10)==1){
		    place_object(y,x); /* and drop a goodie! */
		}
	    }
	    else if (t_list[c_ptr->tptr].tval >= TV_MIN_DOORS)
		delete_object(y,x); /* no more door... */
	} /* if object there.... */
	
	c_ptr->fm = FALSE;
	if (panel_contains(y, x))
	    if ((c_ptr->tl || c_ptr->pl) && c_ptr->tptr != 0) {
		msg_print("You have found something!");
		c_ptr->fm = TRUE;
	    }		
	
/* should become a room space, check to see whether it should be
 * LIGHT_FLOOR or DARK_FLOOR
 */
	if (c_ptr->lr) {
	    found = FALSE;
	    for (i = y - 1; i <= y + 1; i++)
		for (j = x - 1; j <= x + 1; j++)
		    if (cave[i][j].fval <= MAX_CAVE_ROOM) {
			c_ptr->fval = cave[i][j].fval;
			c_ptr->pl = cave[i][j].pl;
			found = TRUE;
			break;
		    }
	    if (!found) {
		c_ptr->fval = CORR_FLOOR;
		c_ptr->pl = FALSE;
	    }
	} else {
	    /* should become a corridor space */
	    c_ptr->fval = CORR_FLOOR;
	    c_ptr->pl = FALSE;
	}


	/* Redisplay the grid */
	lite_spot(y, x);

	/* Worked */
	res = TRUE;
    }

    return (res);
}


/*
 * Tunnels through rubble and walls			-RAK-
 * Must take into account: secret doors,  special tools
 */
void tunnel(int dir)
{
    register int        i, tabil;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    int                 y, x;
    monster_type       *m_ptr;
    vtype               out_val, m_name;

    if ((py.flags.confused > 0) && /* Confused?	     */
	(randint(4) > 1))	   /* 75% random movement   */
	dir = randint(9);
    y = char_row;
    x = char_col;
    (void)mmove(dir, &y, &x);

    c_ptr = &cave[y][x];
/* Compute the digging ability of player; based on	   */
/* strength, and type of tool used			   */
    tabil = py.stats.use_stat[A_STR];
    i_ptr = &inventory[INVEN_WIELD];

/* Don't let the player tunnel somewhere illegal, this is necessary to
 * prevent the player from getting a free attack by trying to tunnel
 * somewhere where it has no effect.  
 */
    if (c_ptr->fval < MIN_CAVE_WALL
	&& (c_ptr->tptr == 0 || (t_list[c_ptr->tptr].tval != TV_RUBBLE
			  && t_list[c_ptr->tptr].tval != TV_SECRET_DOOR))) {
	if (c_ptr->tptr == 0) {
	    msg_print("Tunnel through what?  Empty air?!?");
	    free_turn_flag = TRUE;
	}

    else {
	    msg_print("You can't tunnel through that.");
	    free_turn_flag = TRUE;
	}
	return;
    }
    if (c_ptr->cptr > 1) {
	m_ptr = &m_list[c_ptr->cptr];
	if (m_ptr->ml) {
	    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
		(void)sprintf(m_name, "%s", c_list[m_ptr->mptr].name);
	    else
		(void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
	} else
	    (void)strcpy(m_name, "Something");
	(void)sprintf(out_val, "%s is in your way!", m_name);
	msg_print(out_val);

    /* let the player attack the creature */
	if (py.flags.afraid < 1)
	    py_attack(y, x);
	else
	    msg_print("You are too afraid!");
    } else if (i_ptr->tval != TV_NOTHING) {
	if (TR_TUNNEL & i_ptr->flags)
	    tabil += 25 + i_ptr->p1 * 50;
	else {
	    tabil += (i_ptr->damage[0] * i_ptr->damage[1]) + i_ptr->tohit
		+ i_ptr->todam;
	/* divide by two so that digging without shovel isn't too easy */
	    tabil >>= 1;
	}

	if (weapon_heavy) {
	    tabil += (py.stats.use_stat[A_STR] * 15) - i_ptr->weight;
	    if (tabil < 0)
		tabil = 0;
	}
    /* Regular walls; Granite, magma intrusion, quartz vein  */
    /* Don't forget the boundary walls, made of titanium (255) */
	switch (c_ptr->fval) {
	  case GRANITE_WALL:
	    i = randint(1200) + 80;
	    if (twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
		check_view();
	    } else
		count_msg_print("You tunnel into the granite wall.");
	    break;
	  case MAGMA_WALL:
	    i = randint(600) + 10;
	    if (twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
		check_view();
	    } else
		count_msg_print("You tunnel into the magma intrusion.");
	    break;
	  case QUARTZ_WALL:
	    i = randint(400) + 10;
	    if (twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
		check_view();
	    } else
		count_msg_print("You tunnel into the quartz vein.");
	    break;
	  case BOUNDARY_WALL:
	    msg_print("This seems to be permanent rock.");
	    break;
	  default:
	/* Is there an object in the way?  (Rubble and secret doors) */
	    if (c_ptr->tptr != 0) {
	    /* Rubble.     */
		if (t_list[c_ptr->tptr].tval == TV_RUBBLE) {
		    if (tabil > randint(180)) {
			(void)delete_object(y, x);
			msg_print("You have removed the rubble.");
			if (randint(10) == 1) {
			    place_object(y, x);
			    if (test_light(y, x))
				msg_print("You have found something!");
			}
			lite_spot(y, x);
			check_view();
		    } else
			count_msg_print("You dig in the rubble.");
		}
	    /* Secret doors. */
		else if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
		    count_msg_print("You tunnel into the granite wall.");
		    search(char_row, char_col, py.misc.srh);
		}
		else {
		    msg_print("You can't tunnel through that.");
		    free_turn_flag = TRUE;
		}
	    }

	    else {
		msg_print("Tunnel through what?  Empty air?!?");
		free_turn_flag = TRUE;
	    }
	    break;
	}
    } else
	msg_print("You dig with your hands, making no progress.");
}


/*
 * Disarms a trap	-RAK-	
 */
void disarm_trap()
{
    int                 y, x, level, tmp, dir, no_disarm;
    register int        tot, i;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    monster_type       *m_ptr;
    vtype               m_name, out_val;
#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir, so we save
			       target_mode, then turn it off -CFT */
#endif

    y = char_row;
    x = char_col;
#ifdef TARGET
    target_mode = FALSE;
#endif
    if (get_dir(NULL, &dir)) {
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];
	no_disarm = FALSE;
	if (c_ptr->cptr > 1 && c_ptr->tptr != 0 &&
	    (t_list[c_ptr->tptr].tval == TV_VIS_TRAP
	     || t_list[c_ptr->tptr].tval == TV_CHEST)) {
	    m_ptr = &m_list[c_ptr->cptr];
	    if (m_ptr->ml)
		(void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
	    else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);
	} else if (c_ptr->tptr != 0) {
	    tot = py.misc.disarm + 2 * todis_adj() + stat_adj(A_INT)
		+ (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
	    if ((py.flags.blind > 0) || (no_light()))
		tot = tot / 10;
	    if (py.flags.confused > 0)
		tot = tot / 10;
	    if (py.flags.image > 0)
		tot = tot / 10;
	    i_ptr = &t_list[c_ptr->tptr];
	    i = i_ptr->tval;
	    level = i_ptr->level;
	    if (i == TV_VIS_TRAP) {/* Floor trap    */
		if ((tot + 100 - level) > randint(100)) {
		    msg_print("You have disarmed the trap.");
		    py.misc.exp += i_ptr->p1;
		    (void)delete_object(y, x);
		/* make sure we move onto the trap even if confused */
		    tmp = py.flags.confused;
		    py.flags.confused = 0;
		    move_char(dir, FALSE);
		    py.flags.confused = tmp;
		    prt_experience();
		}
	    /* avoid randint(0) call */
		else if ((tot > 5) && (randint(tot) > 5))
		    count_msg_print("You failed to disarm the trap.");
		else {
		    msg_print("You set the trap off!");
		/* make sure we move onto the trap even if confused */
		    tmp = py.flags.confused;
		    py.flags.confused = 0;
		    move_char(dir, FALSE);
		    py.flags.confused += tmp;
		}
	    } else if (i == TV_CHEST) {
		if (!known2_p(i_ptr)) {
		    msg_print("I don't see a trap.");
		    free_turn_flag = TRUE;
		} else if (CH_TRAPPED & i_ptr->flags) {
		    if ((tot - level) > randint(100)) {
			i_ptr->flags &= ~CH_TRAPPED;
			if (CH_LOCKED & i_ptr->flags)
			    i_ptr->name2 = SN_LOCKED;
			else
			    i_ptr->name2 = SN_DISARMED;
			msg_print("You have disarmed the chest.");
			known2(i_ptr);
			py.misc.exp += level;
			prt_experience();
		    } else if ((tot > 5) && (randint(tot) > 5))
			count_msg_print("You failed to disarm the chest.");
		    else {
			msg_print("You set a trap off!");
			known2(i_ptr);
			chest_trap(y, x);
		    }
		} else {
		    msg_print("The chest was not trapped.");
		    free_turn_flag = TRUE;
		}
	    } else
		no_disarm = TRUE;
	} else
	    no_disarm = TRUE;

	if (no_disarm) {
	    msg_print("I do not see anything to disarm there.");
	    free_turn_flag = TRUE;
	}
    }
#ifdef TARGET
    target_mode = temp;
#endif
}


/*
 * Bash open a door or chest				-RAK-
 *
 * Note: Affected by strength and weight of character 
 *
 * For a closed door, p1 is positive if locked; negative if stuck. A disarm
 * spell unlocks and unjams doors! 
 *
 * For an open door, p1 is positive for a broken door. 
 *
 * A closed door can be opened - harder if locked. Any door might be bashed open
 * (and thereby broken). Bashing a door is (potentially) faster! You move
 * into the door way. To open a stuck door, it must be bashed. A closed door
 * can be jammed (which makes it stuck if previously locked). 
 *
 * Creatures can also open doors. A creature with open door ability will (if not
 * in the line of sight) move though a closed or secret door with no changes.
 * If in the line of sight, closed door are openned, & secret door revealed.
 * Whether in the line of sight or not, such a creature may unlock or unstick
 * a door. 
 *
 * A creature with no such ability will attempt to bash a non-secret door. 
 */
void bash()
{
    int                  y, x, dir, tmp;
    register cave_type  *c_ptr;
    register inven_type *t_ptr;
#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir, so we save
			       target_mode, then turn it off -CFT */
#endif

    y = char_row;
    x = char_col;
#ifdef TARGET
    target_mode = FALSE;
#endif
    if (get_dir(NULL, &dir)) {
	if (py.flags.confused > 0) {
	    msg_print("You are confused.");
	    do {
		dir = randint(9);
	    }
	    while (dir == 5);
	}
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];
	if (c_ptr->cptr > 1) {
	    if (py.flags.afraid > 0)
		msg_print("You are too afraid!");
	    else
		py_bash(y, x);
	} else if (c_ptr->tptr != 0) {
	    t_ptr = &t_list[c_ptr->tptr];
	    if (t_ptr->tval == TV_CLOSED_DOOR) {
		count_msg_print("You smash into the door!");
		tmp = py.stats.use_stat[A_STR] + py.misc.wt / 2;
	    /* Use (roughly) similar method as for monsters. */
		if (randint(tmp * (20 + MY_ABS(t_ptr->p1))) <
			10 * (tmp - MY_ABS(t_ptr->p1))) {
		    msg_print("The door crashes open!");
		    invcopy(&t_list[c_ptr->tptr], OBJ_OPEN_DOOR);
		    t_ptr->p1 = 1 - randint(2);	/* 50% chance of breaking door */
		    c_ptr->fval = CORR_FLOOR;
		    if (py.flags.confused == 0)
			move_char(dir, FALSE);
		    else
			lite_spot(y, x);
		    check_view();
		} else if (randint(150) > py.stats.use_stat[A_DEX]) {
		    msg_print("You are off-balance.");
		    py.flags.paralysis = 1 + randint(2);
		} else if (command_count == 0)
		    msg_print("The door holds firm.");
	    } else if (t_ptr->tval == TV_CHEST) {
		if (randint(10) == 1) {
		    msg_print("You have destroyed the chest and its contents!");
		    t_ptr->index = OBJ_RUINED_CHEST;
		    t_ptr->flags = 0;
		} else if ((CH_LOCKED & t_ptr->flags) && (randint(10) == 1)) {
		    msg_print("The lock breaks open!");
		    t_ptr->flags &= ~CH_LOCKED;
		} else
		    count_msg_print("The chest holds firm.");
	    } else
	    /*
	     * Can't give free turn, or else player could try directions
	     * until he found invisible creature 
	     */
		msg_print("You bash it, but nothing interesting happens.");
	} else {
	    if (c_ptr->fval < MIN_CAVE_WALL)
		msg_print("You bash at empty space.");
	    else
	    /* same message for wall as for secret door */
		msg_print("You bash it, but nothing interesting happens.");
	}
    }
#ifdef TARGET
    target_mode = temp;
#endif
}


/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
void rest(void)
{
    int   rest_num;
    vtype rest_str;

    if (command_count > 0) {
	rest_num = command_count;
	command_count = 0;
    } else {
	char                ch;

	/* Ask the question (perhaps a "prompt" routine would be good) */
	prt("Rest for how long? ('*' for HP/mana; '&' as needed) : ", 0, 0);
	rest_num = 0;
	if (get_string(rest_str, 0, 54, 5)) {
	    if (sscanf(rest_str, "%c", &ch) == 1) {
		if (ch == '*') {
		    rest_num = (-1);
		}
		else if (ch == '&') {
		    rest_num = (-2);
		}
		else {
		    if (atoi(rest_str) > 30000) rest_num = 30000;
		    else rest_num = atoi(rest_str);
		}
	    }
	}
    }
    if (rest_num != 0) {
	if (py.flags.status & PY_SEARCH)
	    search_off();
	py.flags.rest = rest_num;
	py.flags.status |= PY_REST;
	prt_state();
	py.flags.food_digested--;
	prt("Press any key to stop resting...", 0, 0);
	put_qio();
    } else {
	erase_line(MSG_LINE, 0);
	free_turn_flag = TRUE;
    }
}



