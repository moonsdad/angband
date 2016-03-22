/* File: cave.c */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
 * 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu. 
 *
 * Returns TRUE if a line of sight can be traced from x0, y0 to x1, y1. 
 *
 * The LOS begins at the center of the tile [x0, y0] and ends at the center of
 * the tile [x1, y1].  If los() is to return TRUE, all of the tiles this line
 * passes through must be transparent, WITH THE EXCEPTIONS of the starting
 * and ending tiles. 
 *
 * We don't consider the line to be "passing through" a tile if it only passes
 * across one corner of that tile. 
 */

/*
 * Because this function uses (short) ints for all calculations, overflow may
 * occur if d_x and d_y exceed 90.
 */
int los(int fromY, int fromX, int toY, int toX)
{
    register int p_x, p_y, d_x, d_y, a_x, a_y;


    /* Extract the offset */    
    d_y = toY - fromY;
    d_x = toX - fromX;

    /* Extract the absolute offset */
    a_y = MY_ABS(d_y);
    a_x = MY_ABS(d_x);


    /* Handle adjacent (or identical) grids */
    if ((a_x < 2) && (a_y < 2)) return (TRUE);


    /* Directly South/North: Handle the cases where d_x or d_y == 0. */
    if (!d_x) {

	register int p_y;   /* y position -- loop variable	 */

	/* South -- check for walls */
	if (d_y > 0) {
	    for (p_y = fromY + 1; p_y < toY; p_y++) {
		if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
	
	/* North -- check for walls */
	else {
	    for (p_y = fromY - 1; p_y > toY; p_y--) {
		if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
	
	/* Assume los */
	return TRUE;
    }

    /* Directly East/West */
    if (!d_y) {
    
	register int p_x;	   /* x position -- loop variable	 */

	/* East -- check for walls */
	if (d_x > 0) {
	    for (p_x = fromX + 1; p_x < toX; p_x++) {
		if (cave[fromY][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}

	/* West -- check for walls */
	else {
	    for (p_x = fromX - 1; p_x > toX; p_x--) {
		if (cave[fromY][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
		
	/* Assume los */
	return TRUE;
    }


    /* Handle Knightlike shapes -CWS */
    if (a_x == 1) {
	if (d_y == 2) {
	    if (cave[fromY + 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (d_y == (-2)) {
	    if (cave[fromY - 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }
    else if (a_y == 1) {
	if (d_x == 2) {
	    if (cave[fromY][fromX + 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (d_x == (-2)) {
	    if (cave[fromY][fromX - 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }


/*
 * Now, we've eliminated all the degenerate cases. In the computations below,
 * dy (or dx) and m are multiplied by a scale factor, scale = abs(d_x *
 * d_y * 2), so that we can use integer arithmetic. 
 */

    {
	register int        scale,	/* a scale factor		 */
			    scale2;	/* above scale factor / 2	 */

	int		    xSign,	/* sign of d_x		 */
			    ySign,	/* sign of d_y		 */
			    m;		/* slope or 1/slope of LOS	 */

	scale2 = (a_x * a_y);
	scale = scale2 << 1;

	xSign = (d_x < 0) ? -1 : 1;
	ySign = (d_y < 0) ? -1 : 1;


	/* Travel from one end of the line to the other, */
	/* oriented along the longer axis. */

	if (a_x >= a_y) {

	    register int        dy;  /* "fractional" y position	 */

	/*
	 * We start at the border between the first and second tiles, where
	 * the y offset = .5 * slope.  Remember the scale factor.  We have: 
	 *
	 * m = d_y / d_x * 2 * (d_y * d_x) = 2 * d_y * d_y. 
	 */

	    dy = d_y * d_y;
	    m = dy << 1;
	    p_x = fromX + xSign;

	    /* Consider the special case where slope == 1. */
	    if (dy == scale2) {
		p_y = fromY + ySign;
		dy -= scale;
	    }
	    else {
		p_y = fromY;
	    }

	    /* Note (below) the case (dy == scale2), where */
	    /* the LOS exactly meets the corner of a tile. */
	    while (toX - p_x) {
		if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		dy += m;
		if (dy < scale2) {
		    p_x += xSign;
		}
		else if (dy > scale2) {
		    p_y += ySign;
		    if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dy -= scale;
		    p_x += xSign;
		}
		else {
		/* Note: This is the case, (dy == scale2), where */
		/* the LOS exactly meets the corner of a tile. */
		    p_y += ySign;
		    dy -= scale;
		    p_x += xSign;
		}
	    }
	    return TRUE;
	}

	else {
	
	    register int        dx;	/* "fractional" x position	 */

	    dx = d_x * d_x;
	    m = dx << 1;

	    p_y = fromY + ySign;
	    if (dx == scale2) {
		p_x = fromX + xSign;
		dx -= scale;
	    }
	    else {
		p_x = fromX;
	    }

	    /* Note (below) the case (dx == scale2), where */
	    /* the LOS exactly meets the corner of a tile. */
	    while (toY - p_y) {
		if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		dx += m;
		if (dx < scale2) {
		    p_y += ySign;
		}
		else if (dx > scale2) {
		    p_x += xSign;
		    if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dx -= scale;
		    p_y += ySign;
		}
		else {
		    p_x += xSign;
		    dx -= scale;
		    p_y += ySign;
		}
	    }
	}
    }

    /* Assume los */
    return TRUE;
}









/*
 * Tests a spot for light or field mark status		-RAK-	
 */
int test_light(int y, int x)
{
    register cave_type *cave_ptr= &cave[y][x];

    if (cave_ptr->pl || cave_ptr->tl || cave_ptr->fm) return (TRUE);
    else return (FALSE);
}


/*
 * Returns true if player has no light -RAK-
 */
int no_light(void)
{
    register cave_type *c_ptr = &cave[char_row][char_col];
    if (!c_ptr->tl && !c_ptr->pl) return TRUE;
    return FALSE;
}









/*
 * Moves the cursor to a given MAP (y,x) location -RAK-
 */

void move_cursor_relative(int row, int col)
{
    /* Real co-ords convert to screen positions */
    row -= panel_row_prt;
    col -= panel_col_prt;

#ifdef MACINTOSH

    DSetScreenCursor(col, row);

#else
	{

    vtype tmp_str;


    if (move(row, col) == ERR) {
	abort();
    /* clear msg_flag to avoid problems with unflushed messages */
	msg_flag = 0;
	(void)sprintf(tmp_str,
		      "error in move_cursor_relative, row = %d col = %d\n",
		      row, col);
	prt(tmp_str, 0, 0);
	bell();
    /* wait so user can see error */
	(void)sleep(2);
    }

	}
#endif

}


/*
 * Redraw (on the screen) a given MAP location -RAK-
 */
void lite_spot(int y, int x)
{
    /* Redraw if on screen */
    if (panel_contains(y, x)) {
	print(loc_symbol(y, x), y, x);
    }
}


/*
 * Prints the map of the dungeon -RAK-
 */
void prt_map(void)
{
    register int x, y, k;
    register unsigned char tmp_char;

    k = 0;
    for (y = panel_row_min; y <= panel_row_max; y++) {	/* Top to bottom */
	k++;
	erase_line(k, 13);

	for (x = panel_col_min; x <= panel_col_max; x++) {	/* Left to right */
	    tmp_char = loc_symbol(y, x);
	    if (tmp_char != ' ')
		print(tmp_char, y, x);
	}
    }
}














/*
 * And now for something completely different...
 */





/*
 * We need to reset the view of things.			-CJS-
 */
void check_view(void)
{
    int        i, j;
    cave_type *c_ptr, *d_ptr;

    c_ptr = &cave[char_row][char_col];

    /* Check for new panel		   */
    if (get_panel(char_row, char_col, FALSE)) prt_map();

    /* Move the light source		   */
    move_light(char_row, char_col, char_row, char_col);

    /* A room of light should be lit.	 */
    if (c_ptr->fval == LIGHT_FLOOR) {
		if ((p_ptr->flags.blind < 1) && !c_ptr->pl) light_room(char_row, char_col);
    }

    /* In doorway of light-room?		   */
    else if (c_ptr->lr && (p_ptr->flags.blind < 1)) {
	for (i = (char_row - 1); i <= (char_row + 1); i++)
	    for (j = (char_col - 1); j <= (char_col + 1); j++) {
		d_ptr = &cave[i][j];
		if ((d_ptr->fval == LIGHT_FLOOR) && !d_ptr->pl) light_room(i, j);
	    }
    }
}


/*
 * Illuminate any room containing the given location.
 */
void light_room(int y, int x)
{
    register cave_type *c_ptr;
    register monster_type  *m_ptr;

    c_ptr = &cave[y][x];
    if (!c_ptr->pl && c_ptr->lr) {
	c_ptr->pl = TRUE;
	m_ptr = &m_list[c_ptr->cptr];

/* Monsters that are intelligent wake up all the time; non-MINDLESS monsters wake
 * up 1/3 the time, and MINDLESS monsters wake up 1/10 the time -CWS
 */
	if ((r_list[m_ptr->mptr].cdefense & MF2_INTELLIGENT) ||
	    (!(r_list[m_ptr->mptr].cdefense & MF2_MINDLESS) && (randint(3) == 1)) ||
	    (randint(10) == 1))
	    m_ptr->csleep = 0;

	if (c_ptr->fval == NT_DARK_FLOOR)
	    c_ptr->fval = NT_LIGHT_FLOOR;
	else if (c_ptr->fval == DARK_FLOOR)
	    c_ptr->fval = LIGHT_FLOOR;
#ifdef MSDOS
	lite_spot(y, x);	   /* this does all that; plus color-safe -CFT */
#else
	if ((y - panel_row_prt) < 23 && (y - panel_row_prt) > 0 &&
	    (x - panel_col_prt) > 12 && (x - panel_col_prt) < 80)
	    print(loc_symbol(y, x), y, x);
#endif
    
	if (c_ptr->fval < MIN_CLOSED_SPACE) {
	    c_ptr = &cave[y + 1][x];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x);
	    
	    c_ptr = &cave[y - 1][x];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x);

	    c_ptr = &cave[y][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y, x + 1);
	    
	    c_ptr = &cave[y][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y, x - 1);
	    
	    c_ptr = &cave[y + 1][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x + 1);
	    
	    c_ptr = &cave[y - 1][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x - 1);
	    
	    c_ptr = &cave[y - 1][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x + 1);
	    
	    c_ptr = &cave[y + 1][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x - 1);
	}
    }     
}

/*
 * Darken all rooms containing the given location
 */
void darken_room(int y, int x)
{
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];
    if (c_ptr->pl && c_ptr->lr) {
	c_ptr->tl = FALSE;
	if (c_ptr->fval == NT_LIGHT_FLOOR)
	    c_ptr->fval = NT_DARK_FLOOR;
	else if (c_ptr->fval == LIGHT_FLOOR)
	    c_ptr->fval = DARK_FLOOR;
#ifdef MSDOS
	if (panel_contains(y, x))
	    if (c_ptr->fval < MIN_CLOSED_SPACE) {
#else
	if ((y - panel_row_prt) < 23 && (y - panel_row_prt) > 0 &&
	    (x - panel_col_prt) > 12 && (x - panel_col_prt) < 80)
	    if (c_ptr->fval < MIN_CLOSED_SPACE) {
#endif
		c_ptr->pl = FALSE;
		darken_room(y + 1, x);
		darken_room(y - 1, x);
		darken_room(y, x + 1);
		darken_room(y, x - 1);
		darken_room(y + 1, x + 1);
		darken_room(y - 1, x - 1);
		darken_room(y - 1, x + 1);
		darken_room(y + 1, x - 1);
	    }
	print(loc_symbol(y, x), y, x);
    }
}




/*
 * Light up the dungeon -RAK-
 */
void wizard_light(int light)
{
    register cave_type *c_ptr;
    register int        k, l, i, j;
    int                 flag;

    if (!light) {
	if (cave[char_row][char_col].pl)
	    flag = FALSE;
	else
	    flag = TRUE;
    } else {
	flag = (light > 0) ? 1 : 0;
    }
    for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	    if (cave[i][j].fval <= MAX_CAVE_FLOOR)
		for (k = i - 1; k <= i + 1; k++)
		    for (l = j - 1; l <= j + 1; l++) {
			c_ptr = &cave[k][l];
			c_ptr->pl = flag;
			if (!flag)
			    c_ptr->fm = FALSE;
		    }
    prt_map();
}

