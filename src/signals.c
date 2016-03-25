/* File: signals.c */

/* Purpose: signal handlers */

/*
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/*
 * This signal package was brought to you by		-JEW- 
 * Completely rewritten by				-CJS-
 * Rewritten again by					-BEN-
 * One more time by Jeff Collins (Jan. 1995).
 */

/* must include before externs.h, because that uses SIGTSTP */
#include <signal.h>

#include "angband.h"

/*
 * We need a simple method to correctly refer to the signal handler
 */
#ifdef __MINT__
# define signal_handler ((__Sigfunc)(signal_handler_proc))
# define suspend_handler ((__Sigfunc)(suspend_handler_proc))
#else
# define signal_handler (signal_handler_proc)
# define suspend_handler (suspend_handler_proc)
#endif


/*
 * suspend_handler_proc
 *
 * Note that the "raise()" function is not always available
 */
static void suspend_handler_proc(int sig)
{

#ifdef SIGSTOP

    /* Flush the term */
    Term_fresh();

    /* Suspend the "Term" */
    Term_xtra(TERM_XTRA_LEVEL, TERM_LEVEL_HARD_SHUT);

    /* Suspend ourself */
    (void)kill(0, SIGSTOP);

    /* Resume the "Term" */
    Term_xtra(TERM_XTRA_LEVEL, TERM_LEVEL_HARD_OPEN);

    /* Redraw the term */
    Term_redraw();

    /* Flush the term */
    Term_fresh();

#endif

    /* Have to restore handler. */
    (void)signal(sig, suspend_handler);
}



/*
 * signal_handler_proc - Handle signals
 *
 * The most important aspect of this is to catch SIGINT and SIGQUIT and
 * allow the user to rethink.  We don't want to kill a character because
 * of a stupid typo.
 */

static void signal_handler_proc(int sig)
{
    int simple = FALSE;

#if !defined(MACINTOSH) && !defined(_Windows)

    /* Ignore all second signals */
    (void)signal(sig, SIG_IGN);

/* Allow player to think twice. Wizard may force a core dump. */
#ifdef SIGINT
    /* Simple "Ctrl-C" interupt */
    if (sig == SIGINT) simple = TRUE;
#endif

#ifdef SIGQUIT
    /* Simple "Quit" key */
    if (sig == SIGQUIT) simple = TRUE;
#endif

    /* Handle simple interrupt */
    if (simple) {

	/* Only treat real characters specially */
	if (!death && !character_saved && character_generated) {

	    /* Save the screen */
	    save_screen();

	    /* Hack -- Allow player to think twice. */
	    if (!get_check(total_winner ?
			   "Do you want to retire?" :
			   "Really commit *Suicide*?")) {

		/* Restore the screen */
		restore_screen();

		/* Disturb and clear */
		Term_fresh();

		/* Disturb */
		disturb(1, 0);

		/* Restore handler for later. */
		(void)signal(sig, signal_handler);

		/* OK. We don't quit. */
		return;
	    }

	    /* Restore the screen */
	    restore_screen();

	    /* Death */
	    (void)strcpy(died_from, "Interrupting");
	}
	else {
	    (void)strcpy(died_from, "Abortion");
	}

	/* Interrupted */
	prt("Interrupt!", 0, 0);

	/* Suicide */
	death = TRUE;

	/* Save and exit */
	exit_game();

	/* Just in case */
	quit("interrupted");
    }

#endif /* !MACINTOSH */

    /* Die. */
    prt("OH NO!!!!!!  A gruesome software bug LEAPS out at you!", 21, 0);

    /* Try to save anyway */
    if (!death && !character_saved && character_generated) {

	/* Try a panic save */
	panic_save = 1;
	prt("Your guardian angel is trying to save you.", 22, 0);
	prt("", 23, 0);

	/* Attempt to save */
	(void)sprintf(died_from, "(panic save %d)", sig);
	if (save_player()) quit("panic save succeeded");

	/* Oops */
	(void)strcpy(died_from, "software bug");
	death = TRUE;
	turn = 0;
    }
    else {
	death = TRUE;
	prt("There is NO defense!", 22, 0);
	prt("", 23, 0);

	/* Low level access -- Quietly save the memory anyway. */
	(void)_save_player(savefile);
    }

    /* Shut down the terminal XXX XXX */
    term_nuke(term_screen);

#ifdef SET_UID
    /* generate a core dump if necessary */
    (void)signal(sig, SIG_DFL);
    (void)kill(getpid(), sig);
    (void)sleep(5);
#endif

    /* Quit anyway */
    quit(NULL);
}

/*
 * signals_ignore_tstp - Ignore SIGTSTP signals.
 */
void signals_ignore_tstp(void)
{
#ifdef SIGTSTP
    (void)signal(SIGTSTP, SIG_IGN);
#endif
}

/*
 * signals_handle_tstp - Handle SIGTSTP (keyboard suspend)
 */
void signals_handle_tstp(void)
{
#ifdef SIGTSTP
    /* Tell "suspend" to suspend */
    (void)signal(SIGTSTP, suspend_handler);
#endif
}

/*
 * Prepare to handle the relevant signals
 */
void signals_init()
{

#ifdef SIGHUP
    /* Ignore HANGUP, and let the EOF code take care of this case. */
    (void)signal(SIGHUP, SIG_IGN);
#endif

#ifdef SIGTSTP
    /* Hack -- suspend gracefully */
    (void)signal(SIGTSTP, suspend_handler);
#endif

#ifdef SIGINT
    /* Catch the basic "Ctrl-C" interupt */
    (void)signal(SIGINT, signal_handler);
#endif

#ifdef SIGQUIT
    (void)signal(SIGQUIT, signal_handler);
#endif

#ifdef SIGFPE
    (void)signal(SIGFPE, signal_handler);
#endif

#ifdef SIGILL
    (void)signal(SIGILL, signal_handler);
#endif

#ifdef SIGTRAP
    (void)signal(SIGTRAP, signal_handler);
#endif

#ifdef SIGIOT
    (void)signal(SIGIOT, signal_handler);
#endif

#ifdef SIGKILL
    (void)signal(SIGKILL, signal_handler);
#endif

#ifdef SIGBUS
    (void)signal(SIGBUS, signal_handler);
#endif

#ifdef SIGSEGV
    (void)signal(SIGSEGV, signal_handler);
#endif

#ifdef SIGTERM
    (void)signal(SIGTERM, signal_handler);
#endif

#ifdef SIGPIPE
    (void)signal(SIGPIPE, signal_handler);
#endif

#ifdef SIGEMT			   /* in BSD systems */
    (void)signal(SIGEMT, signal_handler);
#endif

#ifdef SIGDANGER		   /* in SYSV systems */
    (void)signal(SIGDANGER, signal_handler);
#endif

#ifdef SIGSYS
    (void)signal(SIGSYS, signal_handler);
#endif

#ifdef SIGXCPU   /* BSD */
    (void)signal(SIGXCPU, signal_handler);
#endif

#ifdef SIGPWR   /* SYSV */
    (void)signal(SIGPWR, signal_handler);
#endif
}

