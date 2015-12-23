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
 * occur if deltaX and deltaY exceed 90.
 */
int los(int fromY, int fromX, int toY, int toX)
{
    register int tmp, deltaX, deltaY;

    deltaX = toX - fromX;
    deltaY = toY - fromY;

    /* Handle Adjacent (or identical) grids */
    if ((deltaX < 2) && (deltaX > -2) && (deltaY < 2) && (deltaY > -2))
	return TRUE;

    /* Handle the cases where deltaX or deltaY == 0. */
    if (deltaX == 0) {
	register int        p_y;   /* y position -- loop variable	 */

	if (deltaY < 0) {
	    tmp = fromY;
	    fromY = toY;
	    toY = tmp;
	}
	for (p_y = fromY + 1; p_y < toY; p_y++)
	    if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE)
		return FALSE;
	return TRUE;
    }
	else if (deltaY == 0) {
	register int        px;	   /* x position -- loop variable	 */

	if (deltaX < 0) {
	    tmp = fromX;
	    fromX = toX;
	    toX = tmp;
	}
	for (px = fromX + 1; px < toX; px++) {
	    if (cave[fromY][px].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	return TRUE;
    }


    /* Handle Knightlike shapes -CWS */
    if (MY_ABS(deltaX) == 1) {
	if (deltaY == 2) {
	    if (cave[fromY + 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (deltaY == (-2)) {
	    if (cave[fromY - 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }
    else if (MY_ABS(deltaY) == 1) {
	if (deltaX == 2) {
	    if (cave[fromY][fromX + 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (deltaX == (-2)) {
	    if (cave[fromY][fromX - 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }


/*
 * Now, we've eliminated all the degenerate cases. In the computations below,
 * dy (or dx) and m are multiplied by a scale factor, scale = abs(deltaX *
 * deltaY * 2), so that we can use integer arithmetic. 
 */

    {
	register int        scale,	/* a scale factor		 */
			    scale2;	/* above scale factor / 2	*/
	int        px,	   /* x position			 */
			   p_y;   /* y position			 */

	int		    xSign,	/* sign of deltaX		 */
			    ySign,	/* sign of deltaY		 */
			    m;		/* slope or 1/slope of LOS	 */

	scale2 = MY_ABS(deltaX * deltaY);
	scale = scale2 << 1;

	xSign = (deltaX < 0) ? -1 : 1;
	ySign = (deltaY < 0) ? -1 : 1;


	/* Travel from one end of the line to the other, */
	/* oriented along the longer axis. */

	if (MY_ABS(deltaX) >= MY_ABS(deltaY)) {

	    register int        dy;  /* "fractional" y position	 */

	/*
	 * We start at the border between the first and second tiles, where
	 * the y offset = .5 * slope.  Remember the scale factor.  We have: 
	 *
	 * m = deltaY / deltaX * 2 * (deltaY * deltaX) = 2 * deltaY * deltaY. 
	 */

	    dy = deltaY * deltaY;
	    m = dy << 1;
	    px = fromX + xSign;

	    /* Consider the special case where slope == 1. */
	    if (dy == scale2) {
		p_y = fromY + ySign;
		dy -= scale;
	    }
	    else {
		p_y = fromY;
	    }

	    while (toX - px) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		dy += m;
		if (dy < scale2) {
		    px += xSign;
		}
		else if (dy > scale2) {
		    p_y += ySign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dy -= scale;
		    px += xSign;
		}
		else {
		/* Note: This is the case, (dy == scale2), where */
		/* the LOS exactly meets the corner of a tile. */
		    p_y += ySign;
		    dy -= scale;
		    px += xSign;
		}
	    }
	    return TRUE;
	}

	else {
	
	    register int        dx;	/* "fractional" x position	 */

	    dx = deltaX * deltaX;
	    m = dx << 1;

	    p_y = fromY + ySign;
	    if (dx == scale2) {
		px = fromX + xSign;
		dx -= scale;
	    }
	    else {
		px = fromX;
	    }

	    while (toY - p_y) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		dx += m;
		if (dx < scale2) {
		    p_y += ySign;
		}
		else if (dx > scale2) {
		    px += xSign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dx -= scale;
		    p_y += ySign;
		}
		else {
		    px += xSign;
		    dx -= scale;
		    p_y += ySign;
		}
	    }
	    return TRUE;
	}
    }
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
 * Returns true if player has no light			-RAK-
 */
int no_light(void)
{
    register cave_type *c_ptr;

    c_ptr = &cave[char_row][char_col];
    if (!c_ptr->tl && !c_ptr->pl) return TRUE;
    return FALSE;
}





/*
 * Moves the cursor to a given interpolated y, x position	-RAK-	 
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
 * Lights up given location				-RAK-
 */
void lite_spot(int y, int x)
{
    if (panel_contains(y, x)) print(loc_symbol(y, x), y, x);
}


/*
 * Prints the map of the dungeon			-RAK-
 */
void prt_map(void)
{
    register int           i, j, k;
    register unsigned char tmp_char;

    k = 0;
    for (i = panel_row_min; i <= panel_row_max; i++) {	/* Top to bottom */
	k++;
	erase_line(k, 13);

	for (j = panel_col_min; j <= panel_col_max; j++) {	/* Left to right */
	    tmp_char = loc_symbol(i, j);
	    if (tmp_char != ' ')
		print(tmp_char, i, j);
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
		if ((py.flags.blind < 1) && !c_ptr->pl) light_room(char_row, char_col);
    }

    /* In doorway of light-room?		   */
    else if (c_ptr->lr && (py.flags.blind < 1)) {
	for (i = (char_row - 1); i <= (char_row + 1); i++)
	    for (j = (char_col - 1); j <= (char_col + 1); j++) {
		d_ptr = &cave[i][j];
		if ((d_ptr->fval == LIGHT_FLOOR) && !d_ptr->pl) light_room(i, j);
	    }
    }
}