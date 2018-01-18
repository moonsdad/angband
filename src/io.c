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
 * Convert a decimal to a single digit octal number
 */
static char octify(uint i)
{
    if (i < 8) return ('0' + i);
    return ('0');
}

/*
 * Convert a decimal to a single digit hex number
 */
static char hexify(uint i)
{
    if (i < 10) return ('0' + i);
    if (i < 16) return ('A' + i - 10);
    return ('0');
}


/*
 * Convert a hex-digit into a decimal
 */
static int deoct(char c)
{
    return (c - '0');
}

/*
 * Convert a hex-digit into a decimal
 */
static int dehex(char c)
{
    return ((c>='a') ? (10+c-'a') : (c>='A') ? (10+c-'A') : (c-'0'));
}


/*
 * Hack -- convert a printable string into real ascii
 *
 * I have no clue if this function correctly handles, for example,
 * parsing "\xFF" into a (signed) char.  Whoever thought of making
 * the "sign" of a "char" undefined is a complete moron.  Oh well.
 */
void text_to_ascii(char *buf, cptr str)
{
    char *s = buf;

    /* Analyze the "ascii" string */
    while (*str) {

	/* Backslash codes */
	if (*str == '\\') {

	    /* Skip the backslash */
	    str++;

	    /* Hex-mode XXX */
	    if (*str == 'x') {
		*s = 16 * dehex(*++str);
		*s++ += dehex(*++str);
	    }

	    /* Hack -- simple way to specify "backslash" */
	    else if (*str == '\\') {
		*s++ = '\\';
	    }
	    
	    /* Hack -- simple way to specify "caret" */
	    else if (*str == '^') {
		*s++ = '^';
	    }
	    
	    /* Hack -- simple way to specify "space" */
	    else if (*str == 's') {
		*s++ = ' ';
	    }
	    
	    /* Hack -- simple way to specify Escape */
	    else if (*str == 'e') {
		*s++ = ESCAPE;
	    }

	    /* Backspace */
	    else if (*str == 'b') {
		*s++ = '\b';
	    }
	    
	    /* Newline */
	    else if (*str == 'n') {
		*s++ = '\n';
	    }

	    /* Return */
	    else if (*str == 'r') {
		*s++ = '\r';
	    }

	    /* Tab */
	    else if (*str == 't') {
		*s++ = '\t';
	    }

	    /* Octal-mode */
	    else if (*str == '0') {
		*s = 8 * deoct(*++str);
		*s++ += deoct(*++str);
	    }

	    /* Octal-mode */
	    else if (*str == '1') {
		*s = 64 + 8 * deoct(*++str);
		*s++ += deoct(*++str);
	    }

	    /* Octal-mode */
	    else if (*str == '2') {
		*s = 64 * 2 + 8 * deoct(*++str);
		*s++ += deoct(*++str);
	    }

	    /* Octal-mode */
	    else if (*str == '3') {
		*s = 64 * 3 + 8 * deoct(*++str);
		*s++ += deoct(*++str);
	    }

	    /* Skip the final char */
	    str++;
	}

	/* Normal Control codes */
	else if (*str == '^') {
	    str++;
	    *s++ = (*str++ & 037);
	}

	/* Normal chars */
	else {
	    *s++ = *str++;
	}
    }

    /* Terminate */
    *s = '\0';
}


/*
 * Hack -- convert a string into a printable form
 */
void ascii_to_text(char *buf, cptr str)
{
    char *s = buf;

    /* Analyze the "ascii" string */
    while (*str) {

	uint i = *str++;
	
	if (i == ESCAPE) {
	    *s++ = '\\';
	    *s++ = 'e';
	}
	else if (i == ' ') {
	    *s++ = '\\';
	    *s++ = 's';
	}
	else if (i == '\b') {
	    *s++ = '\\';
	    *s++ = 'b';
	}
	else if (i == '\t') {
	    *s++ = '\\';
	    *s++ = 't';
	}
	else if (i == '\n') {
	    *s++ = '\\';
	    *s++ = 'n';
	}
	else if (i == '\r') {
	    *s++ = '\\';
	    *s++ = 'r';
	}
	else if (i == '^') {
	    *s++ = '\\';
	    *s++ = '^';
	}
	else if (i == '\\') {
	    *s++ = '\\';
	    *s++ = '\\';
	}
	else if (i < 32) {
	    *s++ = '^';
	    *s++ = i + 64;
	}
	else if (i < 127) {
	    *s++ = i;
	}
	else if (i < 64) {
	    *s++ = '\\';
	    *s++ = '0';
	    *s++ = octify(i / 8);
	    *s++ = octify(i % 8);
	}
	else {
	    *s++ = '\\';
	    *s++ = 'x';
	    *s++ = hexify(i / 16);
	    *s++ = hexify(i % 16);
	}
    }

    /* Terminate */
    *s = '\0';
}







/*
 * Help get a keypress.
 *
 * Returns a single character input from the terminal.	This silently -CJS-
 * consumes ^R to redraw the screen and reset the terminal, so that this
 * operation can always be performed at any input prompt.  inkey() never
 * returns ^R.	 
 */
static char inkey_aux(void)
{
    int i;

    /* Wait for a new keypress from the current term */
    i = Term_inkey();

    return (i);
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
 *
 */
char inkey(void)
{
    int i;
    
    /* Flush the output */
    Term_fresh(); /* Dump IO buffer */

    command_count = 0;		   /* Just to be safe -CJS- */

    /* Get a keypress */
    for (i = 0; !i; i = inkey_aux());


    /* Return the keypress */
    return (i);
}




/*
 * Get a "more" message (on the message line)
 * Then erase the entire message line
 * Leave the cursor on the message line.
 */
void msg_more(int x)
{
    char   in_char;

    /* ensure that the complete -more- message is visible. */
    if (x > 73) x = 73;

    /* Print a message */
    put_str(" -more-", MSG_LINE, x);

	/* let sigint handler know that we are waiting for a space */
	    wait_for_more = 1;

    /* Get an acceptable keypress */
    do {
		in_char = inkey();
	} while ((in_char != ' ') && (in_char != ESCAPE)
	&& (in_char != '\n') &&(in_char != '\r') && (!quick_messages));
	    wait_for_more = 0;

    /* Note -- Do *NOT* call erase_line() */
    (void)move(MSG_LINE, 0);
    clrtoeol();
}



/*
 * Output a message to top line of screen.
 *
 * These messages are kept for later reference.	
 */
void msg_print(cptr msg)
{
    static len = 0;


    /* Old messages need verification */
    if (msg_flag) {

	/* Special case: flush */
	if (!msg) {

	    /* Wait for it... */
	    msg_more(len);

	    /* Forget it */
	    len = 0;
	    msg_flag = FALSE;
	}

	/* We must start a new line */
	else if ((len + strlen(msg)) > 72) {

	    /* Wait for it */
	    msg_more(len);

	    /* Display the new message */
	    put_str(msg, MSG_LINE, 0);

		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], msg, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';

	    /* Let other messages share the line */
	    len = strlen(msg) + 1;
	    msg_flag = TRUE;

	}

	/* The message fits */
	else {

	    /* Display the new message */
	    put_str(msg, MSG_LINE, len);

	    /* Let other messages share the line */
	    len += strlen(msg) + 1;
		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], msg, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';
	    msg_flag = TRUE;
	    
	}
    }


    /* Ignore old messages */
    else {

	/* Make the null string a special case.  -CJS- */
	if (!msg) {
	    /* Forget the message */
	    len = 0;
	    msg_flag = FALSE;
	}

	/* Display the message */
	else {

	    /* Clear the line */
	    (void)move(MSG_LINE, 0);
	    clrtoeol();

	    /* Display it */
	    put_str(msg, MSG_LINE, 0);

	    /* Let other messages share the line */
	    command_count = 0;
	    len = strlen(msg) + 1;
	    last_msg++;
	    if (last_msg >= MAX_SAVE_MSG)
		last_msg = 0;
	    (void)strncpy(old_msg[last_msg], msg, VTYPESIZ);
	    old_msg[last_msg][VTYPESIZ - 1] = '\0';
	    msg_flag = TRUE;
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


/*
 * Gets a string terminated by <RETURN>, and return TRUE.
 * Function returns false if <ESCAPE> is input.
 * Hack -- force legal col and len values
 */
int get_string(char *buf, int row, int col, int len)
{
    register int i, k, x1, x2;
    int done;


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

    /* Erase the "answer box" and place the cursor */
    Term_erase(x1, row, x2, row);

    /* Assume no answer (yet) */
    buf[0] = '\0';

    /* Process input */    
    for (k = 0, done = 0; !done; ) {

	i = inkey();

	switch (i) {

	  case ESCAPE:
	    return (FALSE);
	  case CTRL('J'):
	  case CTRL('M'):
	    done = TRUE;
	    break;
	  case CTRL('H'):
	  case DELETE:
	    if (k > 0) {
		buf[--k] = '\0';
		Term_erase(x1 + k, row, x2, row);
	    }
	    break;

	  default:
	    if ((k < len) && (isprint(i))) {
		Term_putch(x1 + k, row, TERM_WHITE, i);
		buf[k++] = i;
		buf[k] = '\0';
	    }
	    else {
		bell();
	    }
	    break;
	}
    }

    /* Remove trailing blanks */
    while ((k > 0) && (buf[k-1] == ' ')) k--;

    /* Terminate it */
    buf[k] = '\0';

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




