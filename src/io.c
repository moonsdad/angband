/* File: io.c */

/* Purpose: mid-level I/O (uses term.c) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */


#include "angband.h"




/* 
 * Flush the screen, make a noise
 */
void bell()
{
    /* Flush the output */
    Term_fresh();

    /* Make a bell noise (if allowed) */
    if (ring_bell) Term_bell();

    /* Flush the input */
    flush();
}



/*
 * Move the cursor to a given y, x position
 */
void move_cursor(int row, int col)
{
    Term_gotoxy(col, row);
}








/*
 * Flush all input chars in the buffer -RAK-
 */
void flush(void)
{
    /* Forget old keypresses */
    Term_flush();
}




/*
 * Get a keypress from the user. 
 * Returns a single character input from the terminal.	This silently -CJS-
 * consumes ^R to redraw the screen and reset the terminal, so that this
 * operation can always be performed at any input prompt.  inkey() never
 * returns ^R.	 
 */
char inkey(void)
#ifdef MACINTOSH
/* The Mac does not need ^R, so it just consumes it */
/* This routine does nothing special with direction keys */
/* Just returns their keypad ascii value (e.g. '0'-'9') */
/* Compare with inkeydir() below */
{
    char ch;
    int  dir;
    int  shift_flag, ctrl_flag;

    Term_fresh();
    command_count = 0;

    do {
	macgetkey(&ch, FALSE);
    } while (ch == CTRL('R'));

    dir = extractdir(ch, &shift_flag, &ctrl_flag);
    if (dir != -1)
	ch = '0' + dir;

    return (ch);
}
#else
{
    int i;

    Term_fresh();			   /* Dump IO buffer		 */
    command_count = 0;		   /* Just to be safe -CJS- */
    while (TRUE) {
#ifdef MSDOS
	i = msdos_getch();
#else
	i = getch();
#endif

    /* some machines may not sign extend. */
	if (i == EOF) {
	    eof_flag++;
	/*
	 * avoid infinite loops while trying to call inkey() for a -more-
	 * prompt. 
	 */
	    msg_flag = FALSE;

	    (void)refresh();
	    if (!character_generated || character_saved)
		exit_game();
	    disturb(1, 0);
	    if (eof_flag > 100) {
	    /* just in case, to make sure that the process eventually dies */
		panic_save = 1;
		(void)strcpy(died_from, "(end of input: panic saved)");
		if (!save_player()) {
		    (void)strcpy(died_from, "panic: unexpected eof");
		    death = TRUE;
		}
		exit_game();
	    }
	    return ESCAPE;
	}
	if (i != CTRL('R'))
	    return (char)i;
	(void)wrefresh(curscr);
	moriaterm();
    }
}
#endif


#ifdef MACINTOSH
char inkeydir()
/* The Mac does not need ^R, so it just consumes it */
/* This routine translates the direction keys in rogue-like mode */
/* Compare with inkeydir() below */
{
    char        ch;
    int         dir;
    int         shift_flag, ctrl_flag;
    static char tab[9] = {
	'b', 'j', 'n',
	'h', '.', 'l',
	'y', 'k', 'u'
    };
    static char shifttab[9] = {
	'B', 'J', 'N',
	'H', '.', 'L',
	'Y', 'K', 'U'
    };
    static char ctrltab[9] = {
	CTRL('B'), CTRL('J'), CTRL('N'),
	CTRL('H'), '.', CTRL('L'),
	CTRL('Y'), CTRL('K'), CTRL('U')
    };

    Term_fresh();
    command_count = 0;

    do {
	macgetkey(&ch, FALSE);
    } while (ch == CTRL('R'));

    dir = extractdir(ch, &shift_flag, &ctrl_flag);

    if (dir != -1) {
	if (!rogue_like_commands) {
	    ch = '0' + dir;
	} else {
	    if (ctrl_flag)
		ch = ctrltab[dir - 1];
	    else if (shift_flag)
		ch = shifttab[dir - 1];
	    else
		ch = tab[dir - 1];
	}
    }
    return (ch);
}
#endif





/*
 * Output a message to top line of screen.
 *
 * These messages are kept for later reference.	
 */
void msg_print(cptr str_buff)
{
    char   in_char;
    static len = 0;


    /* Old messages need verification */
    if (msg_flag) {

	if (str_buff && (len + strlen(str_buff)) > 72) {
	/* ensure that the complete -more- message is visible. */
	    if (len > 73)
		len = 73;
	    put_str(" -more-", MSG_LINE, len);
	/* let sigint handler know that we are waiting for a space */
	    wait_for_more = 1;
	    do {
		in_char = inkey();
	    } while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') &&
		     (in_char != '\r') && (!quick_messages));
	    len = 0;
	    wait_for_more = 0;
	    (void)move(MSG_LINE, 0);
	    clrtoeol();

	/* Make the null string a special case.  -CJS- */
	    if (str_buff) {
		put_str(str_buff, MSG_LINE, 0);
		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';
		len = strlen(str_buff) + 1;
		msg_flag = TRUE;
	    } else {
		len = 0;
		msg_flag = FALSE;
	    }
	} else {
	    if (!str_buff) {
		if (len > 73)
		    len = 73;
		put_str(" -more-", MSG_LINE, len);
	    /* let sigint handler know that we are waiting for a space */
		wait_for_more = 1;
		do {
		    in_char = inkey();
		} while ((in_char != ' ') && (in_char != ESCAPE)
			 && (in_char != '\n') && (in_char != '\r') && (!quick_messages));
		wait_for_more = 0;
		len = 0;
		(void)move(MSG_LINE, 0);
		clrtoeol();
		msg_flag = FALSE;
	    } else {
		put_str(str_buff, MSG_LINE, len);
		len += strlen(str_buff) + 1;
		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';
		msg_flag = TRUE;
	    }
	}
    } else {
	(void)move(MSG_LINE, 0);
	clrtoeol();
	if (str_buff) {
	    put_str(str_buff, MSG_LINE, 0);
	    command_count = 0;
	    len = strlen(str_buff) + 1;
	    last_msg++;
	    if (last_msg >= MAX_SAVE_MSG)
		last_msg = 0;
	    (void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
	    old_msg[last_msg][VTYPESIZ - 1] = '\0';
	    msg_flag = TRUE;
	} else {
	    msg_flag = FALSE;
	    len = 0;
	}
    }
}



/* Outputs a char to a given interpolated y, x position	-RAK-	 */
/* sign bit of a character used to indicate standout mode. -CJS */
void print(int ch, int row, int col)
{
    row -= panel_row_prt;	   /* Real co-ords convert to screen positions */
    col -= panel_col_prt;
#if 0
    if (ch & 0x80)
	standout();
#endif
    if (mvaddch(row, col, ch) == ERR) {
	vtype               tmp_str;

    /* clear msg_flag to avoid problems with unflushed messages */
	msg_flag = 0;
	(void)sprintf(tmp_str, "error in print, row = %d col = %d\n", row, col);
	prt(tmp_str, 0, 0);
	bell();
    /* wait so user can see error */
	(void)sleep(2);
	abort();
    }
#if 0
    if (ch & 0x80)
	standend();
#endif
}



/*
 * Erase a line (flush msg_print first)
 */
void erase_line(int row, int col)
{
    if (msg_flag && (row == MSG_LINE)) msg_print(NULL);

    Term_erase(col, row, 80-1, row);
}

/*
 * Erase the screen (flush msg_print first)
 */
void clear_screen(void)
{
    if (msg_flag) msg_print(NULL);

    Term_clear();
}

/*
 * Clear part of the screen
 */
void clear_from(int row)
{
    if (msg_flag && (row <= MSG_LINE)) msg_print(NULL);

    Term_erase(0, row, 80-1, 24-1);
}




/*
 * Move to a given location, and without clearing the line,
 * Print a string, using a color (never multi-hued)
 */
void c_put_str(byte attr, cptr str, int row, int col)
{
    if (msg_flag && (row <= MSG_LINE)) msg_print(NULL);

#ifdef USE_COLOR
    if (!use_color) attr = TERM_WHITE;
#else
    attr = TERM_WHITE;
#endif

    Term_putstr(col, row, -1, attr, str);
}

void put_str(cptr str, int row, int col)
{
    c_put_str(TERM_WHITE, str, row, col);
}


/*
 * Outputs a line to a given y, x position		-RAK-	
 * Clear a line at a given location, and, at the same location,
 * Print a string, using a real attribute
 * Hack -- Be sure to flush msg_print if necessary.
 */
void c_prt(byte attr, cptr str, int row, int col)
{
    int x, y;

#ifdef USE_COLOR
    if (!use_color) attr = TERM_WHITE;
#else
    attr = TERM_WHITE;
#endif

    if (msg_flag && (row == MSG_LINE)) msg_print(NULL);

    /* Position the cursor */
    Term_gotoxy(col, row);

    /* Dump the text */
    Term_addstr(-1, attr, str);

    /* Clear the rest of the line */
    if ((Term_locate(&x, &y) == 0) && (y == row)) Term_erase(x, y, 80-1, y);
}

void prt(cptr str, int row, int col)
{
    c_prt(TERM_WHITE, str, row, col);
}





/*
 * Let the player verify a choice.  -CJS-
 * Refuse to accept anything but y/n/Y/N/Escape
 * Return TRUE on "yes", else FALSE
 */
int get_check(cptr prompt)
{
    int res, x;

    /* Hack -- no prompt? */
    if (!prompt) prompt = "Yes or no?";

    /* Display the prompt (and clear the line) */
    prt(prompt, 0, 0);

    /* See how long the prompt is */
    x = strlen(prompt) + 1;

    /* Do NOT wrap */
    if (x > 74) x = 74;

    /* Ask the question */
    Term_putstr(x, 0, -1, TERM_WHITE, "[y/n]");

    /* Get an acceptable answer */
    while (1) {
	res = inkey();
	if (quick_messages) break;
	if (res == ESCAPE) break;
	if (strchr("YyNn", res)) break;
	bell();
    }

    /* Erase the prompt */
    erase_line(0, 0);

    /* Extract the answer */
    return (strchr("Yy", res) ? TRUE : FALSE);
}


/*
 * Prompts (optional), then gets and stores a keypress
 * Returns TRUE unless the character is "Escape"
 */
int get_com(cptr prompt, char *command)
{
    if (prompt) prt(prompt, 0, 0);

    *command = inkey();

    if (prompt) erase_line(MSG_LINE, 0);

    return (*command != ESCAPE);
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

/*
 * Gets a string terminated by <RETURN>, and return TRUE.
 * Function returns false if <ESCAPE> is input
 */
int get_string(char *buf, int row, int col, int len)
{
    register int i, x1, x2;
    char        *p;
    int          flag, aborted;

    aborted = FALSE;
    flag = FALSE;
    (void)move(row, col);
    for (i = len; i > 0; i--)
	(void)addch(' ');
    (void)move(row, col);

    /* Find the box bounds */
    x1 = col;
    x2 = x1 + len - 1;
    if (x2 >= 80) {
	len = 80 - x1;
	x2 = 80 - 1;
    }
    p = buf;
    do {
	i = inkey();

	switch (i) {

	  case ESCAPE:
	    aborted = TRUE;
	    break;
	  case CTRL('J'):
	  case CTRL('M'):
	    flag = TRUE;
	    break;
	  case DELETE:
	  case CTRL('H'):
	    if (col > x1) {
		col--;
		put_str(" ", row, col);
		move_cursor(row, col);
		*--p = '\0';
	    }
	    break;

	  default:
	    if (!isprint(i) || col > x2) {
		bell();
	    }
	    else {
#ifdef MACINTOSH
		DSetScreenCursor(col, row);
		DWriteScreenCharAttr((char)i, ATTR_NORMAL);
#else
		use_value2          mvaddch(row, col, (char)i);

#endif
		*p++ = i;
		col++;
	    }
	    break;
	}
    } while ((!flag) && (!aborted));
    if (aborted) return (FALSE);

    /* Remove trailing blanks */
    while ((p > buf) && (p[-1] == ' ')) p--;

    /* Terminate it */
    *p = '\0';

    /* Return the result */
    return (TRUE);
}


/*
 * Ask for a string, at the current cursor location.
 * Return "Was a legal answer entered?"
 *
 * Might be even better to give this routine a "prompt"
 * and to ask the questions at (0,0) with the prompt...
 */
int askfor(char *buf, int len)
{
    int x, y;
    Term_locate(&x, &y);
    return (get_string(buf, y, x, len));
}



/*
 * Pauses for user response before returning		-RAK-	 
 */
void pause_line(int prt_line)
{
    int i;
    erase_line(prt_line, 0);
    put_str("[Press any key to continue]", prt_line, 23);
    i = inkey();
    erase_line(prt_line, 0);
}


/* Pauses for user response before returning		-RAK-	 */
/* NOTE: Delay is for players trying to roll up "perfect"	 */
/* characters.  Make them wait a bit.			 */
void pause_exit(int prt_line, int delay)
{
    char dummy;

    prt("[Press any key to continue, or Q to exit.]", prt_line, 10);
    dummy = inkey();
    if (dummy == 'Q') {
	erase_line(prt_line, 0);
#ifndef MSDOS			   /* PCs are slow enough as is  -dgk */
	if (delay > 0)
	    (void)sleep((unsigned)delay);
#else
    /* prevent message about delay unused */
	dummy = delay;
#endif
#ifdef MACINTOSH
	enablefilemenu(FALSE);
	exit_game();
	enablefilemenu(TRUE);
#else
	exit_game();
#endif
    }
    erase_line(prt_line, 0);
}

/*
 * Save and restore the screen -- no flushing
 */

void save_screen()
{
    if (msg_flag) msg_print(NULL);

    Term_save();
}

void restore_screen()
{
    if (msg_flag) msg_print(NULL);

    Term_load();
}




/* definitions used by screen_map() */
/* index into border character array */
#define TL 0			   /* top left */
#define TR 1
#define BL 2
#define BR 3
#define HE 4			   /* horizontal edge */
#define VE 5

/* character set to use */
#ifdef MSDOS
# ifdef ANSI
#   define CH(x)	(ansi ? screen_border[0][x] : screen_border[1][x])
# else
#   define CH(x)	(screen_border[1][x])
# endif
#else
#   define CH(x)	(screen_border[0][x])
#endif

/* Display highest priority object in the RATIO by RATIO area */
#define	RATIO 3

void screen_map()
{
    register int i, j;
    static byte screen_border[2][6] = {
    {'+', '+', '+', '+', '-', '|'},	/* normal chars */
    {201, 187, 200, 188, 205, 186}	/* graphics chars */
    };
    byte map[MAX_WIDTH / RATIO + 1];
    byte tmp;
    int   priority[256];
    int   row, orow, col, myrow = 0, mycol = 0;

#ifndef MACINTOSH
    char  prntscrnbuf[80];

#endif

    for (i = 0; i < 256; i++)
	priority[i] = 0;
    priority['<'] = 5;
    priority['>'] = 5;
    priority['@'] = 10;
#ifdef MSDOS
    priority[wallsym] = (-5);
    priority[floorsym] = (-10);
    priority['±'] = (-1);
#else
    priority['#'] = (-5);
    priority['.'] = (-10);
    priority['x'] = (-1);
#endif
    priority['\''] = (-3);
    priority[' '] = (-15);

    save_screen();
    clear_screen();
#ifdef MACINTOSH
    DSetScreenCursor(0, 0);
    DWriteScreenCharAttr(CH(TL), ATTR_NORMAL);
    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	DWriteScreenCharAttr(CH(HE), ATTR_NORMAL);
    DWriteScreenCharAttr(CH(TR), ATTR_NORMAL);
#else
    use_value2          mvaddch(0, 0, CH(TL));

    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	(void)addch(CH(HE));
    (void)addch(CH(TR));
#endif
    orow = (-1);
    map[MAX_WIDTH / RATIO] = '\0';
    for (i = 0; i < MAX_HEIGHT; i++) {
	row = i / RATIO;
	if (row != orow) {
	    if (orow >= 0) {
#ifdef MACINTOSH
		DSetScreenCursor(0, orow + 1);
		DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
		DWriteScreenString(map);
		DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
#else
	    /* can not use mvprintw() on ibmpc, because PC-Curses is horribly
	     * written, and mvprintw() causes the fp emulation library to be
	     * linked with PC-Moria, makes the program 10K bigger 
	     */
		(void)sprintf(prntscrnbuf, "%c%s%c",
			      CH(VE), map, CH(VE));
		use_value2          mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
	    }
	    for (j = 0; j < MAX_WIDTH / RATIO; j++)
		map[j] = ' ';
	    orow = row;
	}
	for (j = 0; j < MAX_WIDTH; j++) {
	    col = j / RATIO;
	    tmp = loc_symbol(i, j);
	    if (priority[map[col]] < priority[tmp])
		map[col] = tmp;
	    if (map[col] == '@') {
		mycol = col + 1;   /* account for border */
		myrow = row + 1;
	    }
	}
    }
    if (orow >= 0) {
#ifdef MACINTOSH
	DSetScreenCursor(0, orow + 1);
	DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
	DWriteScreenString(map);
	DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
#else
	(void)sprintf(prntscrnbuf, "%c%s%c",
		      CH(VE), map, CH(VE));
	use_value2          mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
    }
#ifdef MACINTOSH
    DSetScreenCursor(0, orow + 2);
    DWriteScreenCharAttr(CH(BL), ATTR_NORMAL);
    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	DWriteScreenCharAttr(CH(HE), ATTR_NORMAL);
    DWriteScreenCharAttr(CH(BR), ATTR_NORMAL);
#else
    use_value2          mvaddch(orow + 2, 0, CH(BL));

    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	(void)addch(CH(HE));
    (void)addch(CH(BR));
#endif

#ifdef MACINTOSH
    DSetScreenCursor(23, 23);
    DWriteScreenStringAttr("Hit any key to continue", ATTR_NORMAL);
    if (mycol > 0)
	DSetScreenCursor(mycol, myrow);
#else
    use_value2          mvaddstr(23, 23, "Hit any key to continue");

    if (mycol > 0)
	(void)move(myrow, mycol);
#endif
    (void)inkey();
    restore_screen();
}


/* Print a message so as not to interrupt a counted command. -CJS- */
void count_msg_print(cptr p)
{
    int i;

    i = command_count;
    msg_print(p);
    command_count = i;
}


