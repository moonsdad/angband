/* File: misc1.c */

/* Purpose: misc utility and initialization code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#ifndef FSCALE
#define FSCALE (1<<8)
#endif

#if defined(Pyramid) || defined(NeXT) || defined(sun) || \
defined(NCR3K) || defined(linux) || defined(ibm032) || defined (__osf__)
#include <sys/time.h>
#else
#include <time.h>
#endif

#if !defined(GEMDOS) && !defined(MAC)
#ifndef VMS
#include <sys/types.h>
#else
#include <types.h>
#endif
#endif

#include <stdio.h>
#include "constant.h"
#include "monster.h"
#include "angband.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#else
#include "string.h"
#endif
#else
#include <strings.h>
#endif

typedef struct statstime {
    int                 cp_time[4];
    int                 dk_xfer[4];
    unsigned int        v_pgpgin;
    unsigned int        v_pgpgout;
    unsigned int        v_pswpin;
    unsigned int        v_pswpout;
    unsigned int        v_intr;
    int                 if_ipackets;
    int                 if_ierrors;
    int                 if_opackets;
    int                 if_oerrors;
    int                 if_collisions;
    unsigned int        v_swtch;
    long                avenrun[3];
    struct timeval      boottime;
    struct timeval      curtime;

} statstime;


/* Lets do all prototypes correctly.... -CWS */
#ifndef NO_LINT_ARGS
#ifdef __STDC__
static int   test_place(int, int);
static char *cap(char *);
static void  magic_ammo(inven_type *, int, int, int, int, int);
#else
static int   test_place();
static char *cap();
static void  magic_ammo();
#endif
static void compact_objects();
#endif

extern int peek;
extern int rating;


/*
 * Gets a new random seed for the random number generator 
 * Hack -- saves seeds for the town layout and object colors
 */
void init_seeds(void)
{
    /* Allocate some RNG arrays */
    old_state = (char *) malloc(256); /* excellent R.N.G. */
    dummy_state = (char *) malloc(8); /* simple R.N.G. */

    /* if malloc choked on 264 bytes, we're dead anyways */
    if (!old_state || !dummy_state) {
	puts("\nError initializing; unable to malloc space for RNG arrays...\n");
	exit(2);
    }

    /* is 'unix' a std define for unix system?  I thought UNIX is more common? */
    /* This may need to be changed.  It's fine for PCs, anyways... -CFT */

#ifdef unix

    /* Grab a random seed from the clock & PID... */
    (void) initstate(time(NULL), dummy_state, 8);
    (void) initstate(((getpid() << 1) * (time(NULL) >> 3)), old_state, 256);

#else

    /* ...else just grab a random seed from the clock. -CWS */
    (void) initstate(time(NULL), dummy_state, 8);
    (void)initstate(random(), old_state, 256);

#endif /* unix */

    /* Hack -- Extract seeds for the town layout and object colors */
    town_seed = random();
    randes_seed = random();
}


/*
 * change to different random number generator state
 * Hack -- used to keep consistent object colors and town layout
 */
void set_seed(u32b seed)
{
    setstate(dummy_state);
    srandom((seed % 2147483646L) + 1);
}


/*
 * restore the normal random generator state
 */
void reset_seed(void)
{
    (void)setstate(old_state);
}


#if !defined(time_t)
#define time_t long
#endif

/*
 * Check the day-time strings to see if open		-RAK-	
 */
int check_time(void)
{
#ifdef CHECKHOURS
    time_t              c;
    register struct tm *tp;

#ifndef __MINT__
    struct statstime    st;
#endif

    c = time((time_t *)0);
    tp = localtime(&c);
    if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') {
	return FALSE;
    }

#if !(defined(__MINT__) || defined(NCR3K) || defined(linux) || \
      defined(__386BSD__) || defined (__osf__))

    else {
	if (!rstat("localhost", &st)) {
	    if (((int)((double)st.avenrun[2] / (double)FSCALE)) >= (int)LOAD) {
		return FALSE;
	    }
	}
    }

#endif /* MINT, etc */

#endif /* CHECKHOURS - [cjh] */

    return TRUE;
}

#ifdef RI
/*
 * Generates a random integer x where 1<=X<=MAXVAL	-RAK-	 
 */
int randint(int maxval)
{
    register long randval;

    if (maxval < 1)
	return 1;
    randval = random();
    return ((randval % maxval) + 1);
}

#endif

/*
 * Generates a random integer number of NORMAL distribution -RAK- 
 *
 * XXX This should probably be retested just for paranoia's sake.
 *
 * There has been some suspicion that this function actually uses
 * a standard deviation of about 1.25 times the one given. XXX XXX
 */
int randnor(int mean, int stand)
{
    register int tmp, offset, low, iindex, high;

    tmp = randint(MAX_SHORT);

    /* off scale, assign random value between 4 and 5 times SD */
    if (tmp == MAX_SHORT) {

	offset = 4 * stand + randint(stand);

	/* one half are negative */
	if (randint(2) == 1) offset = (-offset);

	return (mean + offset);
    }


    /* binary search normal normal_table to get index that matches tmp */
    low = 0;
    iindex = NORMAL_TABLE_SIZE >> 1;
    high = NORMAL_TABLE_SIZE;

    /* this takes up to 8 iterations */
    while (TRUE) {

	if ((normal_table[iindex] == tmp) || (high == (low + 1))) {
	    break;
	}

	if (normal_table[iindex] > tmp) {
	    high = iindex;
	    iindex = low + ((iindex - low) >> 1);
	}
	else {
	    low = iindex;
	    iindex = iindex + ((high - iindex) >> 1);
	}
    }

    /* might end up one below target, check that here */
    if (normal_table[iindex] < tmp) iindex = iindex + 1;

    /* normal_table is based on SD of 64, so adjust the index value here, */
    /* round the half way case up */
    offset = ((stand * iindex) + (NORMAL_TABLE_SD >> 1)) / NORMAL_TABLE_SD;


    /* one half should be negative */
    if (randint(2) == 1) offset = (-offset);

    return (mean + offset);
}


/*
 * Returns position of first set bit (and clears that bit) 
 */
int bit_pos(u32b *test)
{
    register int    i;
    register u32b mask = 0x1L;
    
    /* Scan the input */
    for (i = 0; i < sizeof(*test) * 8; i++) {

	/* Test and clear */
	if (*test & mask) {
	    *test &= ~mask;
	    return (i);
	}

	/* Next! */
	mask <<= 0x1L;
    }

    /* no one bits found */
    return (-1);
}


/*
 * Calculates current boundaries
 */
void panel_bounds()
{
    panel_row_min = panel_row * (SCREEN_HEIGHT / 2);
    panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
    panel_row_prt = panel_row_min - 1;
    panel_col_min = panel_col * (SCREEN_WIDTH / 2);
    panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
    panel_col_prt = panel_col_min - 13;
}


/*
 * Given an row (y) and col (x), this routine detects when a move
 * off the screen has occurred and figures new borders. -RAK-
 *
 * Force forcses the panel bounds to be recalculated, useful for 'W'here. 
 */
int get_panel(int y, int x, int update)
{
    register int prow, pcol;
    register int panel;

    prow = panel_row;
    pcol = panel_col;

    if (force || (y < panel_row_min + 2) || (y > panel_row_max - 2)) {
	prow = ((y - SCREEN_HEIGHT / 4) / (SCREEN_HEIGHT / 2));
	if (prow > max_panel_rows) prow = max_panel_rows;
	else if (prow < 0) prow = 0;
    }

    if (force || (x < panel_col_min + 3) || (x > panel_col_max - 3)) {
	pcol = ((x - SCREEN_WIDTH / 4) / (SCREEN_WIDTH / 2));
	if (pcol > max_panel_cols) pcol = max_panel_cols;
	else if (pcol < 0) pcol = 0;
    }

    if ((prow != panel_row) || (pcol != panel_col)) {
	panel_row = prow;
	panel_col = pcol;
	panel_bounds();
	panel = TRUE;

    /* stop movement if any */
	if (find_bound)
	    end_find();
    } else
	panel = FALSE;
    return (panel);
}


/* 
 * Distance between two points				-RAK-
 */
int distance(int y1, int x1, int y2, int x2)
{
    register int dy, dx;

    dy = y1 - y2;
    if (dy < 0) dy = (-dy);
    dx = x1 - x2;
    if (dx < 0) dx = (-dx);

    return ((((dy + dx) << 1) - (dy > dx ? dx : dy)) >> 1);
}


/* Checks points north, south, east, and west for a wall -RAK-	 */
/*
 * note that y,x is always in_bounds(), i.e. 0 < y < cur_height-1, and 0 < x
 * < cur_width-1	 
 */
int next_to_walls(int y, int x)
{
    register int        i;
    register cave_type *c_ptr;

    i = 0;
    c_ptr = &cave[y - 1][x];
    if (c_ptr->fval >= MIN_CAVE_WALL)
	i++;
    c_ptr = &cave[y + 1][x];
    if (c_ptr->fval >= MIN_CAVE_WALL)
	i++;
    c_ptr = &cave[y][x - 1];
    if (c_ptr->fval >= MIN_CAVE_WALL)
	i++;
    c_ptr = &cave[y][x + 1];
    if (c_ptr->fval >= MIN_CAVE_WALL)
	i++;

    return (i);
}


/* Checks all adjacent spots for corridors		-RAK-	 */
/*
 * note that y, x is always in_bounds(), hence no need to check that j, k are
 * in_bounds(), even if they are 0 or cur_x-1 is still works 
 */
int next_to_corr(int y, int x)
{
    register int        k, j, i;
    register cave_type *c_ptr;

    i = 0;
    for (j = y - 1; j <= (y + 1); j++)
	for (k = x - 1; k <= (x + 1); k++) {
	    c_ptr = &cave[j][k];
	/* should fail if there is already a door present */
	    if (c_ptr->fval == CORR_FLOOR
	    && (c_ptr->tptr == 0 || t_list[c_ptr->tptr].tval < TV_MIN_DOORS))
		i++;
	}
    return (i);
}



/*
 * Generates damage for "2d6" style dice rolls
 */
int damroll(int num, int sides)
{
    register int i, sum = 0;
    for (i = 0; i < num; i++) sum += randint(sides);
    return (sum);
}


/* 
 * Old "array" format
 */
int pdamroll(byte *array)
{
    return damroll((int)array[0], (int)array[1]);
}


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

/* Adjacent? */
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
    } else if (deltaY == 0) {
	register int        px;	   /* x position -- loop variable	 */

	if (deltaX < 0) {
	    tmp = fromX;
	    fromX = toX;
	    toX = tmp;
	}
	for (px = fromX + 1; px < toX; px++)
	    if (cave[fromY][px].fval >= MIN_CLOSED_SPACE)
		return FALSE;
	return TRUE;
    }

/* handle Knightlike shapes -CWS */

    if (MY_ABS(deltaX) == 1) {
	if (deltaY == 2) {
	    if (cave[fromY + 1][fromX].fval <= MAX_OPEN_SPACE)
		return TRUE;
	} else if (deltaY == (-2)) {
	    if (cave[fromY - 1][fromX].fval <= MAX_OPEN_SPACE)
		return TRUE;
	}
    } else if (MY_ABS(deltaY) == 1) {
	if (deltaX == 2) {
	    if (cave[fromY][fromX + 1].fval <= MAX_OPEN_SPACE)
		return TRUE;
	} else if (deltaX == (-2)) {
	    if (cave[fromY][fromX - 1].fval <= MAX_OPEN_SPACE)
		return TRUE;
	}
    }

/*
 * Now, we've eliminated all the degenerate cases. In the computations below,
 * dy (or dx) and m are multiplied by a scale factor, scale = abs(deltaX *
 * deltaY * 2), so that we can use integer arithmetic. 
 */

    {
	register int        px,	   /* x position			 */
	                    p_y,   /* y position			 */
	                    scale2;/* above scale factor / 2		 */
	int                 scale, /* above scale factor		 */
	                    xSign, /* sign of deltaX			 */
	                    ySign, /* sign of deltaY			 */
	                    m;	   /* slope or 1/slope of LOS		 */

	scale2 = MY_ABS(deltaX * deltaY);
	scale = scale2 << 1;
	xSign = (deltaX < 0) ? -1 : 1;
	ySign = (deltaY < 0) ? -1 : 1;

    /*
     * Travel from one end of the line to the other, oriented along the
     * longer axis. 
     */

	if (MY_ABS(deltaX) >= MY_ABS(deltaY)) {
	    register int        dy;/* "fractional" y position	 */

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
	    } else
		p_y = fromY;

	    while (toX - px) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
		    return FALSE;

		dy += m;
		if (dy < scale2)
		    px += xSign;
		else if (dy > scale2) {
		    p_y += ySign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
			return FALSE;
		    px += xSign;
		    dy -= scale;
		} else {
		/*
		 * This is the case, dy == scale2, where the LOS exactly
		 * meets the corner of a tile. 
		 */
		    px += xSign;
		    p_y += ySign;
		    dy -= scale;
		}
	    }
	    return TRUE;
	} else {
	    register int        dx;/* "fractional" x position	 */

	    dx = deltaX * deltaX;
	    m = dx << 1;

	    p_y = fromY + ySign;
	    if (dx == scale2) {
		px = fromX + xSign;
		dx -= scale;
	    } else
		px = fromX;

	    while (toY - p_y) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
		    return FALSE;
		dx += m;
		if (dx < scale2)
		    p_y += ySign;
		else if (dx > scale2) {
		    px += xSign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
			return FALSE;
		    p_y += ySign;
		    dx -= scale;
		} else {
		    px += xSign;
		    p_y += ySign;
		    dx -= scale;
		}
	    }
	    return TRUE;
	}
    }
}


/*
 * Returns symbol for given row, column			-RAK-	 
 */
unsigned char loc_symbol(int y, int x)
{
    register cave_type    *cave_ptr;
    register struct flags *f_ptr;

    cave_ptr = &cave[y][x];
    f_ptr = &py.flags;

    if ((cave_ptr->cptr == 1) && (!find_flag || find_prself))
	return '@';
    if (f_ptr->status & PY_BLIND)
	return ' ';
    if ((f_ptr->image > 0) && (randint(12) == 1))
	return randint(95) + 31;
    if ((cave_ptr->cptr > 1) && (m_list[cave_ptr->cptr].ml))
	return c_list[m_list[cave_ptr->cptr].mptr].cchar;
    if (!cave_ptr->pl && !cave_ptr->tl && !cave_ptr->fm)
	return ' ';
    if ((cave_ptr->tptr != 0)
	&& (t_list[cave_ptr->tptr].tval != TV_INVIS_TRAP))
	return t_list[cave_ptr->tptr].tchar;
    if (cave_ptr->fval <= MAX_CAVE_FLOOR)
	return '.';
    if (cave_ptr->fval == GRANITE_WALL || cave_ptr->fval == BOUNDARY_WALL
	|| highlight_seams == FALSE) {
#ifdef MSDOS
	return wallsym;
#else
#ifndef ATARIST_MWC
	return '#';
#else
	return (unsigned char)240;
#endif
#endif
    } else   /* Originally set highlight bit, but that is not portable,
	      * now use the percent sign instead. */
	return '%';
}


/*
 * Tests a spot for light or field mark status		-RAK-	 
 */
int test_light(int y, int x)
{
    register cave_type *cave_ptr;

    cave_ptr = &cave[y][x];
    if (cave_ptr->pl || cave_ptr->tl || cave_ptr->fm)
	return (TRUE);
    else
	return (FALSE);
}


/*
 * Prints the map of the dungeon			-RAK-	 
 */
void prt_map()
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
 * Compact monsters					-RAK-	 
 *
 * Return TRUE if any monsters were deleted, FALSE if could not delete any
 * monsters. 
 */
int compact_monsters()
{
    register int           i;
    int                    cur_dis, delete_any;
    register monster_type *mon_ptr;

    msg_print("Compacting monsters...");

    cur_dis = 66;
    delete_any = FALSE;
    do {
	for (i = mfptr - 1; i >= MIN_MONIX; i--) {
	    mon_ptr = &m_list[i];
	    if ((cur_dis < mon_ptr->cdis) && (randint(3) == 1)) {
	    /* Don't compact Melkor! */
		if (c_list[mon_ptr->mptr].cmove & CM_WIN)
		/* do nothing */
		    ;

	    /* in case this is called from within creatures(), this is a
	     * horrible hack, the m_list/creatures() code needs to be
	     * rewritten 
	     */
		else if (hack_monptr < i) {
		    delete_monster(i);
		    delete_any = TRUE;
		} else

		/* fix1_delete_monster() does not decrement mfptr, so don't
		 * set delete_any if this was called 
		 */
		    fix1_delete_monster(i);
	    }
	}
	if (!delete_any) {
	    cur_dis -= 6;
	/* can't do anything else but abort, if can't delete any monsters */
	    if (cur_dis < 0)
		return FALSE;
	}
    }
    while (!delete_any);
    return TRUE;
}


/*
 * Add to the players food time				-RAK-	 
 */
void add_food(int num)
{
    register struct flags *p_ptr;
    register int           extra, penalty;

    p_ptr = &py.flags;
    if (p_ptr->food < 0) p_ptr->food = 0;
    p_ptr->food += num;
    /* overflow check */
    if (num > 0 && p_ptr->food <= 0) p_ptr->food = 32000;

    if (p_ptr->food > PLAYER_FOOD_MAX) {

	msg_print("You are bloated from overeating. ");

    /*
     * Calculate how much of num is responsible for the bloating. Give the
     * player food credit for 1/50, and slow him for that many turns also.  
     */
	extra = p_ptr->food - PLAYER_FOOD_MAX;
	if (extra > num) extra = num;
	penalty = extra / 50;

	p_ptr->slow += penalty;
	if (extra == num) p_ptr->food = p_ptr->food - num + penalty;
	else p_ptr->food = PLAYER_FOOD_MAX + penalty;
    }

    else if (p_ptr->food > PLAYER_FOOD_FULL) {
	msg_print("You are full. ");
    }
}


